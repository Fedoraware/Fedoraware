#include "ClientModeHook.h"

#include "../../Features/Menu/Menu.h"
#include "../../Features/Prediction/Prediction.h"
#include "../../Features/Aimbot/Aimbot.h"
#include "../../Features/Auto/Auto.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AntiHack/AntiAim.h"
#include "../../Features/Crits/Crits.h"
#include "../../Features/Backtrack/Backtrack.h"
#include "../../Features/Visuals/FakeAngleManager/FakeAng.h"


#include "../../Features/Vars.h"

#include "../../Features/PlayerResource/PlayerResource.h"

void AngleVectors2(const QAngle& angles, Vector* forward)
{
	float sp, sy, cp, cy;

	Math::SinCos(DEG2RAD(angles.x), &sy, &cy);
	Math::SinCos(DEG2RAD(angles.y), &sp, &cp);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

/*void FastStop(CUserCmd* pCmd)
{
	if (g_EntityCache.m_pLocal)
	{
		// Get velocity
		Vector vel = g_EntityCache.m_pLocal->GetVelocity();
		//velocity::EstimateAbsVelocity(RAW_ENT(LOCAL_E), vel);

		static auto sv_friction = g_Interfaces.CVars->FindVar("sv_friction");
		static auto sv_stopspeed = g_Interfaces.CVars->FindVar("sv_stopspeed");

		auto speed = vel.Length2D();
		auto friction = sv_friction->GetFloat() * *reinterpret_cast<float*>(g_EntityCache.m_pLocal + 0x12b8);
		auto control = (speed < sv_stopspeed->GetFloat()) ? sv_stopspeed->GetFloat() : speed;
		auto drop = control * friction * g_Interfaces.GlobalVars->interval_per_tick;

		if (speed > drop - 1.0f)
		{
			Vector velocity = vel;
			Vector direction;
			Math::VectorAngles(vel, direction);
			float speed = velocity.Length();

			direction.y = pCmd->viewangles.y - direction.y;

			Vector forward;
			AngleVectors2(direction, &forward);
			Vector negated_direction = forward * -speed;

			pCmd->forwardmove = negated_direction.x;
			pCmd->sidemove = negated_direction.y;
		}
		else
		{
			pCmd->forwardmove = pCmd->sidemove = 0.0f;
		}
	}
}*/

void __stdcall ClientModeHook::OverrideView::Hook(CViewSetup* pView)
{
	Table.Original<fn>(index)(g_Interfaces.ClientMode, pView);
	g_Visuals.FOV(pView);
	g_Visuals.ThirdPerson(pView);
}

bool __stdcall ClientModeHook::ShouldDrawViewModel::Hook()
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (pLocal->IsScoped() && Vars::Visuals::RemoveScope.m_Var && Vars::Visuals::RemoveZoom.m_Var && !g_Interfaces.
			Input->CAM_IsThirdPerson())
			return true;
	}

	return Table.Original<fn>(index)(g_Interfaces.ClientMode);
}

static void updateAntiAfk(CUserCmd* pCmd)
{
	if (Vars::Misc::AntiAFK.m_Var && g_ConVars.afkTimer->GetInt() != 0)
	{
		static float last_time = 0.0f;
		static int buttones = 0;

		if (g_Interfaces.GlobalVars->curtime - last_time > 20)
		{
			pCmd->buttons &= IN_FORWARD;
			pCmd->buttons &= IN_JUMP;
			last_time = g_Interfaces.GlobalVars->curtime;
		}
	}
}

inline Vector ComputeMove(CUserCmd* pCmd, CBaseEntity* pLocal, Vec3& a, Vec3& b)
{
	Vec3 diff = (b - a);
	if (diff.Length() == 0.0f)
		return Vec3(0.0f, 0.0f, 0.0f);
	const float x = diff.x;
	const float y = diff.y;
	Vec3 vsilent(x, y, 0);
	Vec3 ang;
	Math::VectorAngles(vsilent, ang);
	float yaw = DEG2RAD(ang.y - pCmd->viewangles.y);
	float pitch = DEG2RAD(ang.x - pCmd->viewangles.x);
	Vec3 move = {cos(yaw) * 450.0f, -sin(yaw) * 450.0f, -cos(pitch) * 450.0f};

	// Only apply upmove in water
	if (!(g_Interfaces.EngineTrace->GetPointContents(pLocal->GetEyePosition()) & CONTENTS_WATER))
		move.z = pCmd->upmove;
	return move;
}

// Function for when you want to goto a vector
inline void WalkTo(CUserCmd* pCmd, CBaseEntity* pLocal, Vec3& a, Vec3& b, float scale)
{
	// Calculate how to get to a vector
	auto result = ComputeMove(pCmd, pLocal, a, b);
	// Push our move to usercmd
	pCmd->forwardmove = result.x * scale;
	pCmd->sidemove = result.y * scale;
	pCmd->upmove = result.z * scale;
}

void FastStop(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static Vec3 vStartOrigin = {};
	static Vec3 vStartVel = {};
	static int nShiftTick = 0;
	if (pLocal && pLocal->IsAlive())
	{
		if (g_GlobalInfo.m_bShouldShift && g_GlobalInfo.m_nShifted > 0 && Vars::Misc::CL_Move::AntiWarp.m_Var)
		{
			if (vStartOrigin.IsZero())
			{
				vStartOrigin = pLocal->GetVecOrigin();
			}

			if (vStartVel.IsZero())
			{
				vStartVel = pLocal->GetVecVelocity();
			}

			pCmd->forwardmove = 0.0f;
			pCmd->sidemove = 0.0f;

			Vec3 vPredicted = vStartOrigin + (vStartVel *
				TICKS_TO_TIME(8 - nShiftTick));
			Vec3 vPredictedMax = vStartOrigin + (vStartVel * TICKS_TO_TIME(8));

			float flScale = Math::RemapValClamped(vPredicted.DistTo(vStartOrigin), 0.0f,
			                                      vPredictedMax.DistTo(vStartOrigin) * 1.27f, 1.0f, 0.0f);
			float flScaleScale = Math::RemapValClamped(vStartVel.Length2D(), 0.f, 520.f, 0.f, 1.f);
			WalkTo(pCmd, pLocal, vPredictedMax, vStartOrigin, flScale * flScaleScale);

			nShiftTick++;
		}
		else
		{
			vStartOrigin = Vec3(0, 0, 0);
			vStartVel = Vec3(0, 0, 0);
			nShiftTick = 0;
		}
	}
}

#include "../../Features/Aimbot/MovementSimulation/MovementSimulation.h"

bool __stdcall ClientModeHook::CreateMove::Hook(float input_sample_frametime, CUserCmd* pCmd)
{
	g_GlobalInfo.m_bSilentTime = false;
	g_GlobalInfo.m_bAttacking = false;

	auto OriginalFn = Table.Original<fn>(index);

	if (!pCmd || !pCmd->command_number)
		return OriginalFn(g_Interfaces.ClientMode, input_sample_frametime, pCmd);

	if (OriginalFn(g_Interfaces.ClientMode, input_sample_frametime, pCmd))
		g_Interfaces.Prediction->SetLocalViewAngles(pCmd->viewangles);

	static ConVar* engine_no_focus_sleep = g_Interfaces.CVars->FindVar("engine_no_focus_sleep");
	if (engine_no_focus_sleep && engine_no_focus_sleep->GetInt() >= 1)
	{ engine_no_focus_sleep->SetValue(0); } // stop lagging my audio when I alt-tab lmao

	uintptr_t _bp;
	__asm mov _bp, ebp;
	auto pSendPacket = (bool*)(***(uintptr_t***)_bp - 0x1);

	int nOldFlags = 0;
	Vec3 vOldAngles = pCmd->viewangles;
	float fOldSide = pCmd->sidemove;
	float fOldForward = pCmd->forwardmove;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		// Freecam
		if (Vars::Visuals::FreecamKey.m_Var && GetAsyncKeyState(Vars::Visuals::FreecamKey.m_Var) & 0x8000) {
			if (g_GlobalInfo.m_bFreecamActive == false) {
				g_GlobalInfo.m_vFreecamPos = pLocal->GetVecOrigin();
				g_GlobalInfo.m_bFreecamActive = true;
			}

			const Vec3 viewAngles = g_Interfaces.Engine->GetViewAngles();
			Vec3 fcMove;

			fcMove.x = cosf(DEG2RAD(viewAngles.y)) * cosf(DEG2RAD(viewAngles.x));
			fcMove.y = sinf(DEG2RAD(viewAngles.y)) * cosf(DEG2RAD(viewAngles.x));
			fcMove.z = sinf(DEG2RAD(viewAngles.x));

			if (pCmd->buttons & IN_FORWARD) {
				g_GlobalInfo.m_vFreecamPos.x += Vars::Visuals::FreecamSpeed.m_Var * fcMove.x;
				g_GlobalInfo.m_vFreecamPos.y += Vars::Visuals::FreecamSpeed.m_Var * fcMove.y;
				g_GlobalInfo.m_vFreecamPos.z -= Vars::Visuals::FreecamSpeed.m_Var * fcMove.z;
			}

			if (pCmd->buttons & IN_BACK) {
				g_GlobalInfo.m_vFreecamPos.x -= Vars::Visuals::FreecamSpeed.m_Var * fcMove.x;
				g_GlobalInfo.m_vFreecamPos.y -= Vars::Visuals::FreecamSpeed.m_Var * fcMove.y;
				g_GlobalInfo.m_vFreecamPos.z += Vars::Visuals::FreecamSpeed.m_Var * fcMove.z;
			}

			pCmd->buttons = 0;
			pCmd->forwardmove = 0;
			pCmd->sidemove = 0;
			pCmd->upmove = 0;
		} else {
			g_GlobalInfo.m_bFreecamActive = false;
		}
		
		nOldFlags = pLocal->GetFlags();

		if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
		{
			const int nItemDefIndex = pWeapon->GetItemDefIndex();

			if (g_GlobalInfo.m_nCurItemDefIndex != nItemDefIndex || !pWeapon->GetClip1())
				g_GlobalInfo.m_nWaitForShift = DT_WAIT_CALLS;

			g_GlobalInfo.m_nCurItemDefIndex = nItemDefIndex;
			g_GlobalInfo.m_bWeaponCanHeadShot = pWeapon->CanWeaponHeadShot();
			g_GlobalInfo.m_bWeaponCanAttack = pWeapon->CanShoot(pLocal);
			g_GlobalInfo.m_bWeaponCanSecondaryAttack = pWeapon->CanSecondaryAttack(pLocal);
			g_GlobalInfo.m_WeaponType = Utils::GetWeaponType(pWeapon);

			if (pWeapon->GetSlot() != SLOT_MELEE)
			{
				if (pWeapon->IsInReload())
					g_GlobalInfo.m_bWeaponCanAttack = true;

				if (g_GlobalInfo.m_nCurItemDefIndex != Soldier_m_TheBeggarsBazooka)
				{
					if (pWeapon->GetClip1() == 0)
						g_GlobalInfo.m_bWeaponCanAttack = false;
				}
			}

			if (Vars::Misc::RageRetry.m_Var)
			{
				if (pLocal->IsAlive() && pLocal->GetHealth() <= (pLocal->GetMaxHealth() * (Vars::Misc::RageRetryHealth.
					m_Var / 100.f)))
				{
					g_Interfaces.Engine->ClientCmd_Unrestricted("retry");
				}
			}
		}
	}

	updateAntiAfk(pCmd);

	if (Vars::Misc::CL_Move::RechargeWhileDead.m_Var)
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			if (!pLocal->IsAlive() && g_GlobalInfo.m_nShifted)
			{
				g_GlobalInfo.m_bRecharging = true;
			}
		}
	}

	if (Vars::Misc::CL_Move::AutoRecharge.m_Var)
	{
		if (g_GlobalInfo.m_nShifted && !g_GlobalInfo.m_bShouldShift)
		{
			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				if (pLocal->GetVecVelocity().Length2D() < 5.0f && !(pCmd->buttons == 0))
				{
					g_GlobalInfo.m_bRecharging = true;
				}
			}
		}
	}

	if (Vars::Misc::Roll.m_Var && pCmd->buttons & IN_DUCK)
	{
		Vec3 ang = vOldAngles;
		float v = fOldForward;
		static bool fake = false;

		if (std::abs(v) > 0.0f)
		{
			ang.z = 90.0f;
			pCmd->sidemove = 0.0f;

			if ((pCmd->buttons & IN_FORWARD) && !(pCmd->buttons & IN_ATTACK))
			{
				if ((Vars::Misc::Roll.m_Var == 2 && !fake) || !(Vars::Misc::Roll.m_Var != 2))
				{
					ang.y = ang.y + 180.0f;
				}
				v = -1.0f * v;
			}

			g_GlobalInfo.m_bRollExploiting = true;
		}

		if (Vars::Misc::Roll.m_Var == 2)
		{
			*pSendPacket = fake;
			fake = !fake;
		}
		pCmd->forwardmove = v;
		pCmd->viewangles = ang;
	}

	g_PR->Update();

	g_Misc.Run(pCmd);
	
	g_Crits.Tick(pCmd);

	g_EnginePrediction.Start(pCmd);
	{
		g_Aimbot.Run(pCmd);
		g_Backtrack.Run(pCmd);
		g_Auto.Run(pCmd);
		g_AntiAim.Run(pCmd, pSendPacket);
		g_Misc.EdgeJump(pCmd, nOldFlags);
		FastStop(pCmd, g_EntityCache.m_pLocal);
	}
	g_EnginePrediction.End(pCmd);
	g_Misc.AutoRocketJump(pCmd);

	g_GlobalInfo.m_vViewAngles = pCmd->viewangles;

	static int nChoked = 0;

	if (g_GlobalInfo.m_bShouldShift)
	{
		*pSendPacket = g_GlobalInfo.m_nShifted == 1;

		if (!*pSendPacket)
			nChoked++;

		else nChoked = 0;

		if (nChoked > 21)
			*pSendPacket = true;
	}
	else
	{
		nChoked = 0;
		const auto& pLocal = g_EntityCache.m_pLocal;
		static int chockedPackets = 0;
		static int chockValue = 0;
		if (pLocal && pLocal->IsAlive() && (Vars::Misc::CL_Move::FakelagMode.m_Var > 0) && (Vars::Misc::CL_Move::FakelagMode
			.m_Var != 1 || (GetAsyncKeyState(Vars::Misc::CL_Move::FakelagKey.m_Var) && Vars::Misc::CL_Move::FakelagOnKey
				.
				m_Var) || !Vars::Misc::CL_Move::FakelagOnKey.m_Var) && (Vars::Misc::CL_Move::FakelagMode.m_Var != 3 || (
					abs(pLocal->GetVelocity().x) + abs(pLocal->GetVelocity().y) + abs(pLocal->GetVelocity().z)) > 20.0f) && !
			g_GlobalInfo.m_bForceSendPacket)
		{
			if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
			{
				if (!(pWeapon->CanShoot(pLocal) && (pCmd->buttons & IN_ATTACK)) &&
					!g_GlobalInfo.m_bRecharging &&
					!g_GlobalInfo.m_nShifted &&
					!g_GlobalInfo.m_bShouldShift &&
					!g_GlobalInfo.m_bRechargeQueued)
				{
					chockedPackets++;

					if (chockedPackets > chockValue)
					{
						if (Vars::Misc::CL_Move::FakelagMode.m_Var == 2)
						{
							chockValue = (rand() % (Vars::Misc::CL_Move::FakelagMax.m_Var - Vars::Misc::CL_Move::FakelagMin.
								m_Var)) + Vars::Misc::CL_Move::FakelagMin.m_Var;
						}
						else { chockValue = Vars::Misc::CL_Move::FakelagValue.m_Var; }
						*pSendPacket = true;
						g_GlobalInfo.m_bChoking = false;
						chockedPackets = 0;
						g_FakeAng.Run(pCmd);
					}
					else
					{
						*pSendPacket = false;
						g_GlobalInfo.m_bChoking = true;
					}
				}
				else
				{
					g_FakeAng.Run(pCmd);
					*pSendPacket = true;
					g_GlobalInfo.m_bChoking = false;
				}
			}
		}
		else if (chockedPackets > 0)	// failsafe
		{
			*pSendPacket = true;
			chockedPackets = 0;
			g_GlobalInfo.m_bChoking = false;
		}
		else { g_GlobalInfo.m_bChoking = false; }
	}

	//	TODO: make this p
	/*auto AntiWarp = [](CUserCmd* cmd) -> void
	{
		if (g_GlobalInfo.m_bShouldShift && g_GlobalInfo.m_nShifted)
		{
			cmd->sidemove = (- (cmd->sidemove) * (static_cast<float>(g_GlobalInfo.m_nShifted) / Vars::Misc::CL_Move::DTTicks.m_Var)) * 0.95f;
			cmd->forwardmove = (-(cmd->forwardmove) * (static_cast<float>(g_GlobalInfo.m_nShifted) / Vars::Misc::CL_Move::DTTicks.m_Var)) * 0.95f;
		}
	};

	if (!Vars::Misc::CL_Move::AntiWarp.m_Var)
	{
		AntiWarp(pCmd);
	}*/


	if (Vars::Misc::TauntSlide.m_Var)
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			if (pLocal->IsTaunting())
			{
				if (Vars::Misc::TauntControl.m_Var)
					pCmd->viewangles.x = (pCmd->buttons & IN_BACK)
						                     ? 91.0f
						                     : (pCmd->buttons & IN_FORWARD)
						                     ? 0.0f
						                     : 90.0f;

				return false;
			}
		}
	}

	static bool bWasSet = false;

	if (g_GlobalInfo.m_bSilentTime)
	{
		*pSendPacket = false;
		bWasSet = true;
	}

	else
	{
		if (bWasSet)
		{
			*pSendPacket = true;
			pCmd->viewangles = vOldAngles;
			pCmd->sidemove = fOldSide;
			pCmd->forwardmove = fOldForward;
			bWasSet = false;
		}
	}

	if (static_cast<int>(g_Misc.strings.size()) > 0)
	{
		g_GlobalInfo.gNotifCounter++;

		if (g_GlobalInfo.gNotifCounter > Vars::Visuals::despawnTime.m_Var)
		{
			g_GlobalInfo.gNotifCounter = 0;

			g_Misc.strings.pop_back();
		}
	}
	else
	{
		g_GlobalInfo.gNotifCounter = 0;
	}

	g_GlobalInfo.vEyeAngDelay++; // Used for the return delay in the viewmodel aimbot
	g_GlobalInfo.lateUserCmd = pCmd;
	if (g_GlobalInfo.m_bForceSendPacket)
	{
		*pSendPacket = true;
		g_GlobalInfo.m_bForceSendPacket = false;
	} // if we are trying to force update do this lol
	else if (g_GlobalInfo.m_bForceChokePacket)
	{
		*pSendPacket = false;
		g_GlobalInfo.m_bForceChokePacket = false;
	} // check after force send to prevent timing out possibly

	//if (pCmd->buttons & IN_ATTACK && g_GlobalInfo.m_bShouldShift && !g_GlobalInfo.m_nWaitForShift)
	//{
	//	if (
	//		(Vars::Misc::CL_Move::DTMode.m_Var == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) ||
	//		// 0 - On key
	//		(Vars::Misc::CL_Move::DTMode.m_Var == 1) || // 1 - Always
	//		(Vars::Misc::CL_Move::DTMode.m_Var == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var))) {
	//		g_GlobalInfo.m_bShouldShift = true;
	//	}
	//}

	return g_GlobalInfo.m_bSilentTime
	       || g_GlobalInfo.m_bAAActive
	       || g_GlobalInfo.m_bHitscanSilentActive
	       || g_GlobalInfo.m_bProjectileSilentActive
	       || g_GlobalInfo.m_bRollExploiting
		       ? false
		       : OriginalFn(g_Interfaces.ClientMode, input_sample_frametime, pCmd);
}

#include "../../Features/Glow/Glow.h"
#include "../../Features/Chams/Chams.h"

bool __stdcall ClientModeHook::DoPostScreenSpaceEffects::Hook(const CViewSetup* pSetup)
{
	g_Chams.Render();
	g_Glow.Render();
	return Table.Original<fn>(index)(g_Interfaces.ClientMode, pSetup);
}

void __stdcall ClientModeHook::InputCreateMove::Hook(int sequence_number, float input_sample_frametime, bool active)
{
}

void ClientModeHook::InputCreateMove::Init()
{
	auto FN = reinterpret_cast<fn>(Utils::GetVFuncPtr(g_Interfaces.Input, 3));
	Func.Hook(FN, Hook);
}

#include <map>
#include <intrin.h>
void __fastcall ClientModeHook::SetAbsVelocity::Hook(void* ecx, void* edx, const Vector& vecAbsVelocity)
{
	static DWORD dwC_BasePlayer_PostDataUpdate_SetAbsVelocityCall = g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? 8D 47 ? 39 05") - 0x3;

	if (reinterpret_cast<DWORD>(_ReturnAddress()) == dwC_BasePlayer_PostDataUpdate_SetAbsVelocityCall) {
		if (auto pBasePlayer = static_cast<CBaseEntity*>(ecx)) {
			if (g_GlobalInfo.velFixRecord.find(pBasePlayer) != g_GlobalInfo.velFixRecord.end()) {
				const auto& Record = g_GlobalInfo.velFixRecord[pBasePlayer];

				float flSimTimeDelta = pBasePlayer->GetSimulationTime() - Record.m_flSimulationTime;

				if (flSimTimeDelta > 0.0f) {
					Vec3 vOldOrigin = Record.m_vecOrigin;

					int nCurFlags = pBasePlayer->m_fFlags();
					int nOldFlags = Record.m_nFlags;

					if (!(nCurFlags & FL_ONGROUND) && !(nOldFlags & FL_ONGROUND))
					{
						bool bCorrected = false;

						if ((nCurFlags & FL_DUCKING) && !(nOldFlags & FL_DUCKING)) {
							vOldOrigin.z += 20.0f;
							bCorrected = true;
						}

						if (!(nCurFlags & FL_DUCKING) && (nOldFlags & FL_DUCKING)) {
							vOldOrigin.z -= 20.0f;
							bCorrected = true;
						}

						if (bCorrected)
						{
							Vec3 vNewVelocity = vecAbsVelocity;
							vNewVelocity.z = (pBasePlayer->m_vecOrigin().z - vOldOrigin.z) / flSimTimeDelta;
							Func.Original<fn>()(ecx, edx, vNewVelocity);
							return;
						}
					}
				}
			}
		}
	}

	Func.Original<fn>()(ecx, edx, vecAbsVelocity);
}

void ClientModeHook::SetAbsVelocity::Init()
{
	static DWORD dwLocation = g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC ? 56 57 8B 7D ? 8B F1 F3 0F"));
	//static DWORD dwFN = ((*(PDWORD)(dwLocation)) + dwLocation + 4);
	Func.Hook(reinterpret_cast<void*>(dwLocation), Hook);
}

void __fastcall ClientModeHook::StartMessageMode::Hook(CClientModeShared* ecx, void* edx, int iMessageTypeMode)
{
	if (g_Interfaces.GlobalVars->maxclients != -1) {
		if (const auto v4 = g_Interfaces.ClientMode->m_pChatElement) {
#ifdef _DEBUG
			using fn = void(__thiscall*)(CBaseHudChat*, int);
			static auto dwCBaseHudChat__StartMessageMode = g_Pattern.Find(L"client.dll", L"55 8B EC 8B 45 08 83 EC 0C 56 57 8B F9 68 ? ? ? ?");
			fn fnCBaseHudChat__StartMessageMode = reinterpret_cast<fn>(dwCBaseHudChat__StartMessageMode);
			if (fnCBaseHudChat__StartMessageMode) {
				fnCBaseHudChat__StartMessageMode(v4, iMessageTypeMode);
			}
#else
			v4->StartMessageMode(iMessageTypeMode);
#endif
		}
	}
}
