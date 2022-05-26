#include "../Hooks.h"

#include "../../Features/Prediction/Prediction.h"
#include "../../Features/Aimbot/Aimbot.h"
#include "../../Features/Auto/Auto.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AntiHack/AntiAim.h"
#include "../../Features/AntiHack/FakeLag/FakeLagHandler.h"
#include "../../Features/Backtrack/Backtrack.h"
#include "../../Features/Visuals/FakeAngleManager/FakeAng.h"
#include "../../Features/Camera/CameraWindow.h"
#include "../../Features/CritHack/CritHack.h"
#include "../../Features/Fedworking/Fedworking.h"
#include "../../Features/Resolver/Resolver.h"
#include "../../Features/AntiHack/CheaterDetection/CheaterDetection.h"
#include "../../Features/Followbot/Followbot.h"
#include "../../Features/Vars.h"
#include "../../Features/PlayerResource/PlayerResource.h"

static void UpdateAntiAFK(CUserCmd* pCmd)
{
	if (Vars::Misc::AntiAFK.m_Var && g_ConVars.afkTimer->GetInt() != 0)
	{
		static float last_time = 0.0f;
		static int buttones = 0;

		if (pCmd->command_number % 2)
		{
			pCmd->buttons |= 1 << 27;
		}
	}
}

//	TODO: make this p
void FastStop(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static Vec3 vStartOrigin = {};
	static Vec3 vStartVel = {};
	static int nShiftTick = 0;
	if (pLocal && pLocal->IsAlive())
	{
		if (G::m_bShouldShift && G::m_nShifted > 0 && Vars::Misc::CL_Move::AntiWarp.m_Var && pLocal->GetVecVelocity().Length2D() > 10.f && pLocal->IsOnGround())
		{
			if (vStartOrigin.IsZero())
			{
				vStartOrigin = pLocal->GetVecOrigin();
			}

			if (vStartVel.IsZero())
			{
				vStartVel = pLocal->GetVecVelocity();
			}

			const Vec3 vPredicted = vStartOrigin + (vStartVel * TICKS_TO_TIME(3 - nShiftTick));
			Vec3 vPredictedMax = vStartOrigin + (vStartVel * TICKS_TO_TIME(3));

			const float flScale = Math::RemapValClamped(vPredicted.DistTo(vStartOrigin), 0.0f, vPredictedMax.DistTo(vStartOrigin) * 1.5, 1.2f, 0.0f);
			const float flScaleScale = Math::RemapValClamped(vStartVel.Length2D(), 0.0f, 540.0f, 0.5f, 2.0f);
			Utils::WalkTo(pCmd, pLocal, vPredictedMax, vStartOrigin, flScale * flScaleScale);

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


MAKE_HOOK(ClientModeShared_CreateMove, Utils::GetVFuncPtr(I::ClientMode, 21), bool, __fastcall,
		  void* ecx, void* edx, float input_sample_frametime, CUserCmd* pCmd)
{
	G::m_bSilentTime = false;
	G::m_bAttacking = false;
	G::m_bFakeShotPitch = false;

	if (!pCmd || !pCmd->command_number)
	{
		return Hook.Original<FN>()(ecx, edx, input_sample_frametime, pCmd);
	}

	if (Hook.Original<FN>()(ecx, edx, input_sample_frametime, pCmd))
	{
		I::Prediction->SetLocalViewAngles(pCmd->viewangles);
	}

	//static ConVar* engine_no_focus_sleep = I::CVars->FindVar("engine_no_focus_sleep");
	//if (engine_no_focus_sleep && engine_no_focus_sleep->GetInt() >= 1)
	//{
	//	engine_no_focus_sleep->SetValue(0);
	//} // stop lagging my audio when I alt-tab lmao

	uintptr_t _bp;
	__asm mov _bp, ebp;
	auto pSendPacket = (bool*)(***(uintptr_t***)_bp - 0x1);

	int nOldFlags = 0;
	Vec3 vOldAngles = pCmd->viewangles;
	float fOldSide = pCmd->sidemove;
	float fOldForward = pCmd->forwardmove;

	G::currentUserCmd = pCmd;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		nOldFlags = pLocal->GetFlags();

		if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
		{
			const int nItemDefIndex = pWeapon->GetItemDefIndex();

			if (G::m_nCurItemDefIndex != nItemDefIndex || !pWeapon->GetClip1())
				G::m_nWaitForShift = DT_WAIT_CALLS;

			G::m_nCurItemDefIndex = nItemDefIndex;
			G::m_bWeaponCanHeadShot = pWeapon->CanWeaponHeadShot();
			G::m_bWeaponCanAttack = pWeapon->CanShoot(pLocal);
			G::m_bWeaponCanSecondaryAttack = pWeapon->CanSecondaryAttack(pLocal);
			G::m_WeaponType = Utils::GetWeaponType(pWeapon);
			G::m_bAttacking = Utils::IsAttacking(pCmd, pWeapon);

			if (pWeapon->GetSlot() != SLOT_MELEE)
			{
				if (pWeapon->IsInReload())
					G::m_bWeaponCanAttack = true;

				if (G::m_nCurItemDefIndex != Soldier_m_TheBeggarsBazooka)
				{
					if (pWeapon->GetClip1() == 0)
						G::m_bWeaponCanAttack = false;
				}
			}

			if (Vars::Misc::RageRetry.m_Var)
			{
				if (pLocal->IsAlive() && pLocal->GetHealth() <= (pLocal->GetMaxHealth() * (Vars::Misc::RageRetryHealth.
					m_Var / 100.f)))
				{
					I::Engine->ClientCmd_Unrestricted("retry");
				}
			}
		}

		if (Vars::Misc::CL_Move::RechargeWhileDead.m_Var)
		{
			if (!pLocal->IsAlive() && G::m_nShifted)
			{
				G::m_bRecharging = true;
			}
		}

		if (Vars::Misc::CL_Move::AutoRecharge.m_Var)
		{
			if (G::m_nShifted && !G::m_bShouldShift)
			{
				if (pLocal->GetVecVelocity().Length2D() < 5.0f && !(pCmd->buttons == 0))
				{
					G::m_bRecharging = true;
				}
			}
		}
	}

	UpdateAntiAFK(pCmd);

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

			G::m_bRollExploiting = true;
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
	g_Fedworking.Run();
	g_CameraWindow.Update();
	g_BadActors.OnTick();

	g_EnginePrediction.Start(pCmd);
	{
		g_Aimbot.Run(pCmd);
		g_Backtrack.Run(pCmd);
		g_Auto.Run(pCmd);
		g_AntiAim.Run(pCmd, pSendPacket);
		g_Misc.EdgeJump(pCmd, nOldFlags);
	}
	g_EnginePrediction.End(pCmd);
	g_CritHack.Run(pCmd);

	FastStop(pCmd, g_EntityCache.m_pLocal);
	g_Misc.RunLate(pCmd);
	g_Resolver.Update(pCmd);
	g_Followbot.Run(pCmd);
	g_FLGHandler.onTick(pCmd, g_EntityCache.m_pLocal, pSendPacket);

	G::m_vViewAngles = pCmd->viewangles;

	static int nChoked = 0;
	if (G::m_bShouldShift)
	{
		*pSendPacket = G::m_nShifted == 1;

		if (!*pSendPacket) { nChoked++; }
		else { nChoked = 0; }
		if (nChoked > 21) { *pSendPacket = true; }
	}

	if (G::m_nShifted > 0)
	{
		g_FakeAng.DrawChams = false;
	}

	if (Vars::Misc::PartyCrasher.m_Var)
	{
		I::Engine->ClientCmd_Unrestricted("tf_party_chat \"FED@MA==\"");
	}

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (Vars::Misc::TauntSlide.m_Var)
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

		if (Vars::Debug::DebugInfo.m_Var)
		{

			static float cycledelta = 0.f;
			if (!*pSendPacket)
			{
				pLocal->m_bClientSideAnimation() = false;
				pLocal->m_flPlaybackRate() = 0.f;
				cycledelta += 0.02f;
			}
			else
			{
				pLocal->m_bClientSideAnimation() = true;
				pLocal->GetUpdateClientSideAnimation();
				pLocal->m_flPlaybackRate() = 1.f;

				pLocal->m_flCycle() += cycledelta;
				cycledelta = 0.f;
			}
		}
	}

	static bool bWasSet = false;

	if (G::m_bSilentTime)
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

	G::vEyeAngDelay++; // Used for the return delay in the viewmodel aimbot
	G::lateUserCmd = pCmd;
	if (G::m_bForceSendPacket)
	{
		*pSendPacket = true;
		G::m_bForceSendPacket = false;
	} // if we are trying to force update do this lol
	else if (G::m_bForceChokePacket)
	{
		*pSendPacket = false;
		G::m_bForceChokePacket = false;
	} // check after force send to prevent timing out possibly


	if (G::m_bSilentTime ||
		G::m_bAAActive ||
		G::m_bFakeShotPitch ||
		G::m_bHitscanSilentActive ||
		G::m_bAvoidingBackstab ||
		G::m_bProjectileSilentActive ||
		G::m_bRollExploiting)
	{
		return false;
	}

	return Hook.Original<FN>()(ecx, edx, input_sample_frametime, pCmd);
}