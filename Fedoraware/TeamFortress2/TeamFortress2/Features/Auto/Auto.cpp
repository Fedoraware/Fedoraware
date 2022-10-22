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
	if (!Vars::Triggerbot::Global::Active.m_Var || !F::AutoGlobal.IsKeyDown()) // this is bad because i say so
		return false;
	*/

	// if triggerbot is active and we havent set a key its clear we want to trigger all the time, forcing keybinds is madness (especially when it's not done @ AimbotGlobal.cpp)
	if (!Vars::Triggerbot::Global::Active.Value || (!F::AutoGlobal.IsKeyDown() && Vars::Triggerbot::Global::TriggerKey.
		Value))
	{
		return false;
	}

	if (I::EngineVGui->IsGameUIVisible() || I::VGuiSurface->IsCursorVisible())
	{
		return false;
	}

	if (G::ShouldShift) { return false; }

	return true;
}

void CAuto::Run(CUserCmd* pCmd)
{
	if (Vars::Triggerbot::Stab::Disguise.Value && F::AutoStab.m_bShouldDisguise)
	{
		I::EngineClient->ClientCmd_Unrestricted("lastdisguise");
	}

	G::AutoBackstabRunning = false;
	F::AutoStab.m_bShouldDisguise = false;

	const auto pLocal = g_EntityCache.GetLocal();
	const auto pWeapon = g_EntityCache.GetWeapon();

	if (pLocal && pWeapon)
	{
		if (ShouldRun(pLocal))
		{
			F::AutoShoot.Run(pLocal, pWeapon, pCmd);
			F::AutoStab.Run(pLocal, pWeapon, pCmd);
			F::AutoDetonate.Run(pLocal, pWeapon, pCmd);
			F::AutoAirblast.Run(pLocal, pWeapon, pCmd);
			F::AutoUber.Run(pLocal, pWeapon, pCmd);
		}
	}
}