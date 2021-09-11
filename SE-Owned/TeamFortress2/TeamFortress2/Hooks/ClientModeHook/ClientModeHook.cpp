#include "ClientModeHook.h"

#include "../../Features/Menu/Menu.h"
#include "../../Features/Prediction/Prediction.h"
#include "../../Features/Aimbot/Aimbot.h"
#include "../../Features/Auto/Auto.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AntiHack/AntiAim.h"


#include "../../Features/Vars.h"
#include "../../Features/Menu/Menu.h"
#include "../../SDK/Timer.h"

void __stdcall ClientModeHook::OverrideView::Hook(CViewSetup* pView)
{
	Table.Original<fn>(index)(g_Interfaces.ClientMode, pView);
	g_Visuals.FOV(pView);
	//g_Visuals.OverrideWorldTextures();
}

bool __stdcall ClientModeHook::ShouldDrawViewModel::Hook()
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (pLocal->IsScoped() && Vars::Visuals::RemoveScope.m_Var && Vars::Visuals::RemoveZoom.m_Var && !g_Interfaces.Input->CAM_IsThirdPerson())
			return true;
	}

	return Table.Original<fn>(index)(g_Interfaces.ClientMode);
}

Timer AntiAfkTimer{  };
int last_buttons{ 0 };

static void updateAntiAfk(CUserCmd *pCmd)
{
	if (pCmd->buttons != last_buttons) {
		AntiAfkTimer.update();
		last_buttons = pCmd->buttons;
	}
	else {
		if (g_ConVars.afkTimer->GetInt() != 0 && AntiAfkTimer.check(g_ConVars.afkTimer->GetInt() * 60 * 1000 - 10000)) {
			bool flip = false;
			pCmd->buttons |= flip ? IN_FORWARD : IN_BACK;
			flip = !flip;
			if (AntiAfkTimer.check(g_ConVars.afkTimer->GetInt() * 60 * 1000 + 1000))
			{
				AntiAfkTimer.update();
			}
		}
		last_buttons = pCmd->buttons;
	}
}

bool __stdcall ClientModeHook::CreateMove::Hook(float input_sample_frametime, CUserCmd* pCmd)
{
	g_GlobalInfo.m_bSilentTime = false;
	g_GlobalInfo.m_bAttacking = false;

	fn OriginalFn = Table.Original<fn>(index);

	if (!pCmd || !pCmd->command_number)
		return OriginalFn(g_Interfaces.ClientMode, input_sample_frametime, pCmd);

	if (OriginalFn(g_Interfaces.ClientMode, input_sample_frametime, pCmd))
		g_Interfaces.Prediction->SetLocalViewAngles(pCmd->viewangles);

	uintptr_t _bp; __asm mov _bp, ebp;
	bool* pSendPacket = (bool*)(***(uintptr_t***)_bp - 0x1);


	int nOldFlags = 0;
	Vec3 vOldAngles = pCmd->viewangles;
	float fOldSide = pCmd->sidemove;
	float fOldForward = pCmd->forwardmove;

	//Bless your beautiful soul reestart/kris

	auto AntiWarp = [](CUserCmd* cmd) -> void
	{
		int shiftcheck = g_GlobalInfo.m_nShifted; //grab shifted.

		if (shiftcheck < 19)
		{
			if (shiftcheck < 5)
			{
				cmd->forwardmove = -cmd->forwardmove;
				cmd->sidemove = -cmd->sidemove;
			}
			else
			{
				cmd->forwardmove = 0;
				cmd->sidemove = 0;
			}
		}
		else {
			g_GlobalInfo.fast_stop = false;
		}
	};

	if (g_GlobalInfo.fast_stop) {
		AntiWarp(pCmd);
	}


	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
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
		}
	}

	if (Vars::Misc::AntiAFK.m_Var) {
		updateAntiAfk(pCmd);
	}

	if (Vars::Misc::Roll.m_Var && pCmd->buttons & IN_DUCK) {

		Vec3 ang = vOldAngles;
		float v = fOldForward;
		static bool fake = false;

		if (std::abs(v) > 0.0f) {
			ang.z = 90.0f;
			pCmd->sidemove = 0.0f;

			if ((pCmd->buttons & IN_FORWARD) && !(pCmd->buttons & IN_ATTACK)) {
				if ((Vars::Misc::Roll.m_Var == 2 && !fake) || !(Vars::Misc::Roll.m_Var != 2)) {
					ang.y = ang.y + 180.0f;
				}
				v = -1.0f * v;
			}

			g_GlobalInfo.m_bRollExploiting = true;
		}

		if (Vars::Misc::Roll.m_Var == 2) {
			*pSendPacket = fake;
			fake = !fake;
		}
		pCmd->forwardmove = v;
		pCmd->viewangles = ang;
	}

	g_Misc.Run(pCmd);
	g_EnginePrediction.Start(pCmd);
	{
		g_Aimbot.Run(pCmd);
		g_Auto.Run(pCmd);
		g_AntiAim.Run(pCmd, pSendPacket);
		g_Misc.EdgeJump(pCmd, nOldFlags);
	}
	g_EnginePrediction.End(pCmd);
	g_Misc.AutoRocketJump(pCmd);
	g_GlobalInfo.m_vViewAngles = pCmd->viewangles;

	//fakelag

	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon) {
			if (pLocal->IsAlive())
			{
				auto netchan = g_Interfaces.Engine->GetNetChannelInfo();

				if ((Vars::Misc::CL_Move::Fakelag.m_Var && netchan->m_nChokedPackets < (int)Vars::Misc::CL_Move::FakelagValue.m_Var) || pWeapon->CanShoot(pLocal) && (pCmd->buttons & IN_ATTACK)) {
					if (Vars::Misc::CL_Move::FakelagOnKey.m_Var) {
						if (GetAsyncKeyState(Vars::Misc::CL_Move::FakelagKey.m_Var)) {
							*pSendPacket = false;
						}
					}
					else {
						*pSendPacket = false;
					}
				}
				else {
					*pSendPacket = true;
				}
			}
		}
	}


	if (Vars::Misc::TauntSlide.m_Var)
	{
		if (const auto &pLocal = g_EntityCache.m_pLocal)
		{
			if (pLocal->IsTaunting())
			{
				if (Vars::Misc::TauntControl.m_Var)
					pCmd->viewangles.x = (pCmd->buttons & IN_BACK) ? 91.0f : (pCmd->buttons & IN_FORWARD) ? 0.0f : 90.0f;

				return false;
			}
		}
	}

	static bool bWasSet = false;

	if (g_GlobalInfo.m_bSilentTime) {
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

	//failsafe
	{
		static int nChoked = 0;

		if (!*pSendPacket)
			nChoked++;

		else nChoked = 0;

		if (nChoked > 14)
			*pSendPacket = true;
	}
	if (static_cast<int>(g_Misc.strings.size()) > 0) {
		g_GlobalInfo.tickCounter++;

		if (g_GlobalInfo.tickCounter > Vars::Visuals::despawnTime.m_Var) {
			g_GlobalInfo.tickCounter = 0;

			g_Misc.strings.pop_back();
		}
	}
	else {
		g_GlobalInfo.tickCounter = 0;
	}

	return g_GlobalInfo.m_bSilentTime
		|| g_GlobalInfo.m_bAAActive
		|| g_GlobalInfo.m_bHitscanSilentActive
		|| g_GlobalInfo.m_bProjectileSilentActive
		|| g_GlobalInfo.m_bRollExploiting
		//|| ShouldNoPush()
		? false : OriginalFn(g_Interfaces.ClientMode, input_sample_frametime, pCmd);
}

#include "../../Features/Glow/Glow.h"
#include "../../Features/Chams/Chams.h"

bool __stdcall ClientModeHook::DoPostScreenSpaceEffects::Hook(const CViewSetup *pSetup)
{
	g_Chams.Render();
	g_Glow.Render();
	return Table.Original<fn>(index)(g_Interfaces.ClientMode, pSetup);
}