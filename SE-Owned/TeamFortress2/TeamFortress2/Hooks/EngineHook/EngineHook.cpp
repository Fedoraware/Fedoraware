#include "EngineHook.h"
#include "../../Features/Vars.h"
#include "../../Features/Misc/Misc.h"

void __cdecl EngineHook::CL_Move::Hook(float accumulated_extra_samples, bool bFinalTick)
{
	static auto oClMove = Func.Original<fn>();
	if (!Vars::Misc::CL_Move::Enabled.m_Var) {
		return oClMove(accumulated_extra_samples, bFinalTick);
	}

	auto pLocal = g_EntityCache.m_pLocal;

	if (Vars::Misc::CL_Move::TeleportKey.m_Var && (GetAsyncKeyState(Vars::Misc::CL_Move::TeleportKey.m_Var)) && g_GlobalInfo.m_nShifted >= g_GlobalInfo.dtTicks) {
		while (g_GlobalInfo.m_nShifted != 0) {
			g_GlobalInfo.m_nShifted--;
			oClMove(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == 1));
		}

		return;
	}



	if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var)) {
		g_GlobalInfo.m_bRecharging = true;
	}
	if (g_GlobalInfo.m_bRecharging && g_GlobalInfo.m_nShifted < g_GlobalInfo.dtTicks) {
		g_GlobalInfo.m_nShifted++;
		g_GlobalInfo.m_nWaitForShift = DT_WAIT_CALLS;	
		return; // Don't move
	}
	else {
		g_GlobalInfo.m_bRecharging = false;
	}

	oClMove(accumulated_extra_samples, (g_GlobalInfo.m_bShouldShift && !g_GlobalInfo.m_nWaitForShift) ? true : bFinalTick);

	if (g_GlobalInfo.m_nWaitForShift) {
		g_GlobalInfo.m_nWaitForShift--;
		return;
	}

	if (g_GlobalInfo.lateUserCmd != nullptr) {
		// Shift if attacking normally
		g_GlobalInfo.m_bShouldShift = g_GlobalInfo.m_bShouldShift ? true : g_GlobalInfo.lateUserCmd->buttons & IN_ATTACK;
	}



	if (!pLocal) {
		return;
	}

	if (g_GlobalInfo.m_bShouldShift) {
		if (
			(Vars::Misc::CL_Move::DTMode.m_Var == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) ||	// 0 - On key
			(Vars::Misc::CL_Move::DTMode.m_Var == 1) ||																	// 1 - Always
			(Vars::Misc::CL_Move::DTMode.m_Var == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)))		// 2 - Disable on key 
		{
			g_GlobalInfo.fast_stop = true;
			while (g_GlobalInfo.m_nShifted) {
				if (Vars::Misc::CL_Move::NotInAir.m_Var) {
					if (pLocal->IsOnGround()) {
						g_GlobalInfo.m_nShifted--;
						oClMove(accumulated_extra_samples, g_GlobalInfo.m_nShifted == 1);
					}
					else {
						return;
					}
				}
				else {
					g_GlobalInfo.m_nShifted--;
					oClMove(accumulated_extra_samples, g_GlobalInfo.m_nShifted == 1);
				}
			}
			g_GlobalInfo.fast_stop = false;
		}
		g_GlobalInfo.m_bShouldShift = false;
	}

}

//void __cdecl EngineHook::CL_Move::Hook(float accumulated_extra_samples, bool bFinalTick)
//{
//	if (!Vars::Misc::CL_Move::Enabled.m_Var) {
//		return Func.Original<fn>()(accumulated_extra_samples, bFinalTick);
//	}
//
//	// Warp
//	if (Vars::Misc::CL_Move::TeleportKey.m_Var && (GetAsyncKeyState(Vars::Misc::CL_Move::TeleportKey.m_Var)) && g_GlobalInfo.m_nShifted) 
//	{
//		while (g_GlobalInfo.m_nShifted < MAX_NEW_COMMANDS_HEAVY)
//		{
//			g_GlobalInfo.m_nShifted--;
//			Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == 1));
//		}
//
//		return;
//	}
//
//
//	// Recharge
//	if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var)) {
//		g_GlobalInfo.m_bRecharging = true;
//	}
//	if (g_GlobalInfo.m_bRecharging && g_GlobalInfo.m_nShifted <= g_GlobalInfo.dtTicks )
//	{
//		g_GlobalInfo.m_nShifted++;
//		g_GlobalInfo.m_nWaitForShift = DT_WAIT_CALLS;
//		return;
//	}
//	else {
//		g_GlobalInfo.m_bRecharging = false;
//	}
//
//
//	// Yeah
//	Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_bShouldShift && !g_GlobalInfo.m_nWaitForShift) ? true : bFinalTick);
//
//
//	// Count down wait for dt
//	if (g_GlobalInfo.m_nWaitForShift)
//	{
//		g_GlobalInfo.m_nWaitForShift--;
//		return;
//	}
//	if (g_GlobalInfo.lateUserCmd != nullptr) {
//		g_GlobalInfo.m_bShouldShift = g_GlobalInfo.m_bShouldShift ? true : g_GlobalInfo.lateUserCmd->buttons & IN_ATTACK;
//	}
//
//	const auto& pLocal = g_EntityCache.m_pLocal;
//	
//
//	// Doubletap
//	if (g_GlobalInfo.m_bShouldShift)
//	{	
//		if (!pLocal)
//			return;
//		if 
//		(
//			(Vars::Misc::CL_Move::DTMode.m_Var == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) ||	// 0 - On key
//			(Vars::Misc::CL_Move::DTMode.m_Var == 1) ||																	// 1 - Always
//			(Vars::Misc::CL_Move::DTMode.m_Var == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var))		// 2 - Disable on key
//		) 
//		{ 
//			g_GlobalInfo.fast_stop = true;
//			while (g_GlobalInfo.m_nShifted)
//			{
//				if (Vars::Misc::CL_Move::NotInAir.m_Var && pLocal->IsOnGround()) {
//					Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == 1));
//					g_GlobalInfo.m_nShifted--;
//				}
//				else {
//					Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == 1));
//					g_GlobalInfo.m_nShifted--;
//				}
//			}
//			g_GlobalInfo.fast_stop = false;
//		}
//		g_GlobalInfo.m_bShouldShift = false;
//	}
//}
