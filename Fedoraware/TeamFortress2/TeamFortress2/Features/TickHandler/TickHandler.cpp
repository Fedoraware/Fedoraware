#include "TickHandler.h"
#include "../../Hooks/HookManager.h"
#include "../../Hooks/Hooks.h"

void CTickshiftHandler::Speedhack(CUserCmd* pCmd)
{
	//	called from CreateMove
	bSpeedhack = Vars::Misc::CL_Move::SEnabled.Value;
	if (!bSpeedhack) { return; }
	bTeleport = false;
	bRecharge = false;
	bDoubletap = false;
}

void CTickshiftHandler::Recharge(CUserCmd* pCmd)
{
	//	called from CreateMove
	static KeyHelper kRecharge{ &Vars::Misc::CL_Move::RechargeKey.Value };
	bRecharge = (((!bTeleport && !bDoubletap && !bSpeedhack) && (kRecharge.Down()) || G::RechargeQueued) || bRecharge) && iAvailableTicks < Vars::Misc::CL_Move::DTTicks.Value;
	G::ShouldStop = (bRecharge && Vars::Misc::CL_Move::StopMovement.Value) || G::ShouldStop;
}

void CTickshiftHandler::Teleport(CUserCmd* pCmd)
{
	//	called from CreateMove
	static KeyHelper kTeleport{ &Vars::Misc::CL_Move::TeleportKey.Value };
	bTeleport = (((!bRecharge && !bDoubletap && !bSpeedhack) && kTeleport.Down()) || bTeleport) && iAvailableTicks;
}

void CTickshiftHandler::Doubletap(const CUserCmd* pCmd, CBaseEntity* pLocal)
{
	//	called from CreateMove
	static KeyHelper kDoubletap{ &Vars::Misc::CL_Move::DoubletapKey.Value };
	if (bTeleport || bRecharge || bSpeedhack || (iAvailableTicks < Vars::Misc::CL_Move::DTTicks.Value)) { return; }
	if (G::WaitForShift && Vars::Misc::CL_Move::WaitForDT.Value) { return; }
	if (G::ShouldShift || !pCmd) { return; }

	switch (Vars::Misc::CL_Move::DTMode.Value)
	{
		case 0:
		{
			if (!kDoubletap.Down()) { return; }
			break;
		}
		case 2:
		{
			if (kDoubletap.Down()) { return; }
			break;
		}
		case 3: { return; }
	}

	if (G::IsAttacking || (G::CurWeaponType == EWeaponType::MELEE && pCmd->buttons & IN_ATTACK))
	{
		bDoubletap = G::ShouldShift = Vars::Misc::CL_Move::NotInAir.Value ? pLocal->OnSolid() : true;
	}
}

bool CTickshiftHandler::MeleeDoubletapCheck(CBaseEntity* pLocal)
{
	static KeyHelper kDoubletap{ &Vars::Misc::CL_Move::DoubletapKey.Value };
	if (bTeleport || bRecharge || bSpeedhack || (iAvailableTicks < Vars::Misc::CL_Move::DTTicks.Value)) { return false; }
	if (G::WaitForShift && Vars::Misc::CL_Move::WaitForDT.Value) { return false; }
	if (G::ShouldShift) { return false; }
	switch (Vars::Misc::CL_Move::DTMode.Value)
	{
		case 0:
		{
			if (!kDoubletap.Down()) { return false; }
			break;
		}
		case 2:
		{
			if (kDoubletap.Down()) { return false; }
			break;
		}
		case 3: { return false; }
	}
	return Vars::Misc::CL_Move::NotInAir.Value ? pLocal->OnSolid() : true;
}

void CTickshiftHandler::CLMoveFunc(float accumulated_extra_samples, bool bFinalTick)
{
	static auto CL_Move = g_HookManager.GetMapHooks()["CL_Move"];
	if (!CL_Move)
	{
		CL_Move = g_HookManager.GetMapHooks()["CL_Move"];
		return;
	}

	iAvailableTicks--;
	if (iAvailableTicks < 0) { return; }
	G::ShiftedTicks = iAvailableTicks;
	if (G::WaitForShift > 0) { G::WaitForShift--; }

	return CL_Move->Original<void(__cdecl*)(float, bool)>()(accumulated_extra_samples, bFinalTick);
}

void CTickshiftHandler::CLMove(float accumulated_extra_samples, bool bFinalTick)
{
	G::ShiftedTicks = iAvailableTicks; //	put this above incremenet to prevent jittering
	while (iAvailableTicks > Vars::Misc::CL_Move::DTTicks.Value) { CLMoveFunc(accumulated_extra_samples, false); } //	skim any excess ticks

	iAvailableTicks++; //	since we now have full control over CL_Move, increment.
	if (iAvailableTicks <= 0)
	{
		iAvailableTicks = 0;
		return;
	} //	ruhroh

	if (!Vars::Misc::CL_Move::Enabled.Value || I::EngineClient->IsPlayingTimeDemo())
	{
		//	
		G::WaitForShift = G::ShiftedTicks = 0;
		while (iAvailableTicks > 1) { CLMoveFunc(accumulated_extra_samples, false); }
		return CLMoveFunc(accumulated_extra_samples, true);
	}

	if (bTeleport)
	{
		const int iWishTicks = (Vars::Misc::CL_Move::TeleportMode.Value ? std::min(std::min(Vars::Misc::CL_Move::TeleportFactor.Value, 2), iAvailableTicks) : iAvailableTicks);
		for (int i = 0; i < iWishTicks; i++) { CLMoveFunc(accumulated_extra_samples, i == iWishTicks); }
		return;
	}

	if (bRecharge)
	{
		if (iAvailableTicks <= Vars::Misc::CL_Move::DTTicks.Value) { return; }
		bRecharge = false;
		G::RechargeQueued = false;
		G::WaitForShift = iTickRate - Vars::Misc::CL_Move::DTTicks.Value;
	}

	if (bDoubletap)
	{
		while (iAvailableTicks) { CLMoveFunc(accumulated_extra_samples, iAvailableTicks == 1); }
		bDoubletap = false;
		return;
	}

	if (bSpeedhack)
	{
		iAvailableTicks = 0;
		for (int i = 0; i < Vars::Misc::CL_Move::SFactor.Value; i++) { CLMoveFunc(accumulated_extra_samples, i == Vars::Misc::CL_Move::SFactor.Value); }
		return;
	}

	if (I::GlobalVars->tickcount >= iNextPassiveTick && Vars::Misc::CL_Move::PassiveRecharge.Value && G::ShiftedTicks < Vars::Misc::CL_Move::DTTicks.Value)
	{
		iNextPassiveTick = I::GlobalVars->tickcount + (iTickRate / Vars::Misc::CL_Move::PassiveRecharge.Value);
		return;
	}

	return CLMoveFunc(accumulated_extra_samples, true);
}

void CTickshiftHandler::CreateMove(CUserCmd* pCmd)
{
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive()) { return; }
	if (!Vars::Misc::CL_Move::Enabled.Value) { return; }

	Recharge(pCmd);
	Teleport(pCmd);
	Doubletap(pCmd, pLocal);
	Speedhack(pCmd);

	G::ShouldShift = bDoubletap;
	G::Teleporting = bTeleport;
	G::Recharging = bRecharge;
}

void CTickshiftHandler::Reset()
{
	bSpeedhack = bDoubletap = bRecharge = bTeleport = false;
	iAvailableTicks = 0;
	iNextPassiveTick = 0;
	iTickRate = round(1.f / I::GlobalVars->interval_per_tick);
}
