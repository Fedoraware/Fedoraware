#include "../Hooks.h"

#include "../../Features/Misc/Misc.h"

MAKE_HOOK(CL_Move, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC ? 83 3D ? ? ? ? 02 0F 8C ? ? 00 00 E8 ? ? ? 00 84 C0"), void, __cdecl,
	float accumulated_extra_samples, bool bFinalTick)
{
	static auto oClMove = Hook.Original<FN>();

	const auto pLocal = g_EntityCache.GetLocal();

	static KeyHelper tpKey{ &Vars::Misc::CL_Move::TeleportKey.Value };
	static KeyHelper rechargeKey{ &Vars::Misc::CL_Move::RechargeKey.Value };

	if (!Vars::Misc::CL_Move::Enabled.Value)	// return the normal CL_Move if we don't want to manipulate it.
	{
		G::ShiftedTicks = 0;					// cough
		return oClMove(accumulated_extra_samples, bFinalTick);
	}

	while (G::ShiftedTicks > Vars::Misc::CL_Move::DTTicks.Value)	//	get rid of ticks we aren't going to use.
	{
		G::ShiftedTicks --;
		oClMove(accumulated_extra_samples, (G::ShiftedTicks == Vars::Misc::CL_Move::DTTicks.Value));
	}

	// pSpeedhack
	if (Vars::Misc::CL_Move::SEnabled.Value)
	{
		int SpeedTicks{ 0 };
		int SpeedTicksDesired = Vars::Misc::CL_Move::SFactor.Value;
		G::ShiftedTicks = 0;

		while (SpeedTicks < SpeedTicksDesired)
		{
			SpeedTicks++;
			oClMove(accumulated_extra_samples, (SpeedTicks == (SpeedTicksDesired)));
		}
	}

	static bool streaming = false;
// Teleport
	if ((tpKey.Down() || streaming) && G::ShiftedTicks > 0 && !G::Recharging && !G::RechargeQueued)
	{
		G::Teleporting = true;
		oClMove(accumulated_extra_samples, false);
		switch (Vars::Misc::CL_Move::TeleportMode.Value)
		{
			case 0:
			{	// plain
				while (G::ShiftedTicks > 0)
				{
					oClMove(0, G::ShiftedTicks == 1);
					G::ShiftedTicks--;
				}
				break;
			}
			case 1:
			{	// smooth
				if (G::ShiftedTicks)
				{
					oClMove(0, G::ShiftedTicks == 1);
					G::ShiftedTicks--;
				}

				break;
			}
			case 2:
			{ // weird
				streaming = true;
				static int wishSpeed = 2;
				int speed = 0;
				while (speed < wishSpeed && G::ShiftedTicks)
				{
					oClMove(0, G::ShiftedTicks == 1);
					G::ShiftedTicks--;
					speed++;
				}
				streaming = G::ShiftedTicks;
			}
		}
		G::Teleporting = false;
		return;
	}

	// Prepare for a recharge (Is a recharge queued?)
	if (G::RechargeQueued && !G::IsChoking)
	{
		// probably perfect method of waiting to ensure we don't mess with fakelag
		G::RechargeQueued = false; // see relevant code @clientmodehook
		G::Recharging = true;
	}

	// Recharge ticks
	else if (G::Recharging && (G::ShiftedTicks < Vars::Misc::CL_Move::DTTicks.Value))
	{
		G::ForceSendPacket = true; // force uninterrupted connection with server
		G::ShiftedTicks++; // add ticks to tick counter
		G::WaitForShift = round(1.f / I::GlobalVars->interval_per_tick) - Vars::Misc::CL_Move::DTTicks.Value; // set wait condition (genius)
		return; // this recharges
	}

	// Queue a recharge if the recharge key was pressed
	else if (rechargeKey.Down() && !G::RechargeQueued && (G::ShiftedTicks < Vars::Misc::CL_Move::DTTicks.Value))
	{
		// queue recharge
		G::ForceSendPacket = true;
		G::RechargeQueued = true;
	}

	// We're unable to recharge
	else
	{
		G::Recharging = false;
	}

	oClMove(accumulated_extra_samples, (G::ShouldShift && !G::WaitForShift) ? true : bFinalTick);

	if (!pLocal)
	{
		G::ShiftedTicks = 0; // we do not have charge if we do not exist
		return;
	}

	if (G::WaitForShift)
	{
		G::WaitForShift--;
		return;
	}

	if (G::LastUserCmd) {
		if (G::IsAttacking || (G::CurWeaponType == EWeaponType::MELEE && G::LastUserCmd->buttons & IN_ATTACK))
		{
			G::ShouldShift = Vars::Misc::CL_Move::NotInAir.Value ? pLocal->IsOnGround() : true;
		}
	}

	// Should we shift?
	if (G::ShouldShift && !G::WaitForShift)
	{
		if (// 0 - On key
			(Vars::Misc::CL_Move::DTMode.Value == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.Value)) ||

			// 1 - Always
			(Vars::Misc::CL_Move::DTMode.Value == 1) ||

			// 2 - Disable on key 
			(Vars::Misc::CL_Move::DTMode.Value == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.Value)))
		{
			while (G::ShiftedTicks > 0)
			{
				oClMove(0, (G::ShiftedTicks == 1));
				G::ShiftedTicks--;
			}

			I::EngineClient->FireEvents();
			G::WaitForShift = DT_WAIT_CALLS;
		}
		G::ShouldShift = false;
	}
}