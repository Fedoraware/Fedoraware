#include "TickHandler.h"

void CTickshiftHandler::Speedhack(CUserCmd* pCmd){	//	called from CreateMove
	bSpeedhack = Vars::Misc::CL_Move::SEnabled.Value;
	if (!bSpeedhack) { return; }
	bTeleport = false; bRecharge = false; bDoubletap = false;
}

void CTickshiftHandler::Recharge(CUserCmd* pCmd){	//	called from CreateMove
	static KeyHelper kRecharge{ &Vars::Misc::CL_Move::RechargeKey.Value };
	bRecharge = (((!bTeleport && !bDoubletap && !bSpeedhack) && (kRecharge.Down()) || G::RechargeQueued) || bRecharge) && iAvailableTicks < Vars::Misc::CL_Move::DTTicks.Value;
	G::ShouldStop = (bRecharge && Vars::Misc::CL_Move::StopMovement.Value) || G::ShouldStop;
}

void CTickshiftHandler::Teleport(CUserCmd* pCmd){	//	called from CreateMove
	static KeyHelper kTeleport{ &Vars::Misc::CL_Move::TeleportKey.Value };
	bTeleport = (((!bRecharge && !bDoubletap && !bSpeedhack) && kTeleport.Down()) || bTeleport) && iAvailableTicks;
}

void CTickshiftHandler::Doubletap(CUserCmd* pCmd, CBaseEntity* pLocal){	//	called from CreateMove
	static KeyHelper kDoubletap{ &Vars::Misc::CL_Move::DoubletapKey.Value };
	if (bTeleport || bRecharge || bSpeedhack || (iAvailableTicks < Vars::Misc::CL_Move::DTTicks.Value)) { return; }
	if (G::WaitForShift && Vars::Misc::CL_Move::WaitForDT.Value) { return; }
	if (G::ShouldShift || !pCmd) { return; }
	switch (Vars::Misc::CL_Move::DTMode.Value){
	case 0: { if (!kDoubletap.Down())	{ return; }	break; }	
	case 2: { if (kDoubletap.Down())	{ return; }	break; }	
	}

	if (G::IsAttacking || (G::CurWeaponType == EWeaponType::MELEE && pCmd->buttons & IN_ATTACK))
	{
		bDoubletap = G::ShouldShift = Vars::Misc::CL_Move::NotInAir.Value ? pLocal->OnSolid() : true;
	}
}

void CTickshiftHandler::CLMoveFunc(float accumulated_extra_samples, bool bFinalTick){
	const auto fCL_Move = g_HookManager.GetMapHooks()["CL_Move"];
	if (!fCL_Move){ return; }
	iAvailableTicks--;
	if (iAvailableTicks < 0) { return; }
	G::ShiftedTicks = iAvailableTicks;
	return fCL_Move->Original<void(__cdecl*)(float, bool)>()(accumulated_extra_samples, bFinalTick);
}

void CTickshiftHandler::CLMove(float accumulated_extra_samples, bool bFinalTick){
	G::ShiftedTicks = iAvailableTicks;	//	put this above incremenet to prevent jittering

	while (iAvailableTicks > std::max(Vars::Misc::CL_Move::DTTicks.Value, 22)) { CLMoveFunc(accumulated_extra_samples, iAvailableTicks == Vars::Misc::CL_Move::DTTicks.Value); }	//	skim any excess ticks

	iAvailableTicks++;	//	since we now have full control over CL_Move, increment.
	if (iAvailableTicks <= 0) { iAvailableTicks = 0; return; }	//	ruhroh

	if (G::WaitForShift) { G::WaitForShift--; }

	if (bTeleport){
		const int iWishTicks = (Vars::Misc::CL_Move::TeleportMode.Value ? 2 : iAvailableTicks);
		for (int i = 0; i < iWishTicks; i++) { CLMoveFunc(accumulated_extra_samples, i == iWishTicks); } return;
	}
	if (bRecharge){
		if (iAvailableTicks <= Vars::Misc::CL_Move::DTTicks.Value) { return; }
		else { bRecharge = false; }
		G::WaitForShift = round(1.f / I::GlobalVars->interval_per_tick) - Vars::Misc::CL_Move::DTTicks.Value;
	}
	if (bDoubletap){
		while (iAvailableTicks){ CLMoveFunc(accumulated_extra_samples, iAvailableTicks == 1); } bDoubletap = false; return;
	}
	if (bSpeedhack){
		iAvailableTicks = 0;
		for (int i = 0; i < Vars::Misc::CL_Move::SFactor.Value; i++) { CLMoveFunc(accumulated_extra_samples, i == Vars::Misc::CL_Move::SFactor.Value); } return;
	}

	CLMoveFunc(accumulated_extra_samples, true); return;
}

void CTickshiftHandler::CreateMove(CUserCmd* pCmd){
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive()) { return; }
	if (!Vars::Misc::CL_Move::Enabled.Value) { return; }

	Recharge(pCmd);
	Teleport(pCmd);
	Doubletap(pCmd, pLocal);
	Speedhack(pCmd);
	 
	G::ShouldShift = bDoubletap; G::Teleporting = bTeleport; G::Recharging = bRecharge;
}