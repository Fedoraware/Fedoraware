#include "Auto.h"

#include "../Vars.h"

#include "AutoShoot/AutoShoot.h"
#include "AutoStab/AutoStab.h"
#include "AutoDetonate/AutoDetonate.h"
#include "AutoBlast/AutoBlast.h"
#include "AutoUber/AutoUber.h"

bool CAuto::ShouldRun(CBaseEntity* pLocal)
{
	/*
	if (!Vars::Triggerbot::Global::Active.m_Var || !g_AutoGlobal.IsKeyDown()) // this is bad because i say so
		return false;
	*/

	// if triggerbot is active and we havent set a key its clear we want to trigger all the time, forcing keybinds is madness (especially when it's not done @ AimbotGlobal.cpp)
	if (!Vars::Triggerbot::Global::Active.m_Var || (!g_AutoGlobal.IsKeyDown() && Vars::Triggerbot::Global::TriggerKey.
		m_Var)) { return false; }

	if (I::EngineVGui->IsGameUIVisible() || I::Surface->IsCursorVisible())
	{
		return false;
	}

	return true;
}

void CAuto::Run(CUserCmd* pCmd)
{
	if (Vars::Triggerbot::Stab::Disguise.m_Var && g_AutoStab.m_bShouldDisguise)
	{
		I::Engine->ClientCmd_Unrestricted(_("lastdisguise"));
	}

	g_GlobalInfo.m_bAutoBackstabRunning = false;
	g_AutoStab.m_bShouldDisguise = false;

	const auto pLocal = g_EntityCache.m_pLocal;
	const auto pWeapon = g_EntityCache.m_pLocalWeapon;

	if (pLocal && pWeapon)
	{
		if (ShouldRun(pLocal))
		{
			g_AutoShoot.Run(pLocal, pWeapon, pCmd);
			g_AutoStab.Run(pLocal, pWeapon, pCmd);
			g_AutoDetonate.Run(pLocal, pWeapon, pCmd);
			g_AutoAirblast.Run(pLocal, pWeapon, pCmd);
			g_AutoUber.Run(pLocal, pWeapon, pCmd);
		}
	}
}