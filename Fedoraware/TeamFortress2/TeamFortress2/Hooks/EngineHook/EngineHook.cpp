#include "EngineHook.h"
#include "../../Features/Vars.h"
#include "../../Features/Misc/Misc.h"

void __cdecl EngineHook::CL_Move::Hook(float accumulated_extra_samples, bool bFinalTick)
{
	static auto oClMove = Func.Original<fn>();

	if (!Vars::Misc::CL_Move::Enabled.m_Var) {
		return oClMove(accumulated_extra_samples, bFinalTick);
	}

	auto pLocal = g_EntityCache.m_pLocal; const auto& pWeapon = g_EntityCache.m_pLocalWeapon;

	if (Vars::Misc::CL_Move::TeleportKey.m_Var && (GetAsyncKeyState(Vars::Misc::CL_Move::TeleportKey.m_Var)) && g_GlobalInfo.m_nShifted >= g_GlobalInfo.dtTicks) {
		while (g_GlobalInfo.m_nShifted != 0) {
			g_GlobalInfo.m_nShifted--;
			oClMove(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == 1));
		}

		return;
	}

	// this check is useless because the cvar isn't used, and even if it was, this check achieves nothing
	/*
	static ConVar* sv_maxusrcmdprocessticks = g_Interfaces.CVars->FindVar("sv_maxusrcmdprocessticks");
	if (!sv_maxusrcmdprocessticks) {
		return oClMove(accumulated_extra_samples, bFinalTick);
	}
	*/ // ok so, if they have set the value to 0, leaving only a hardcoded restriction of like 46 or smthn on how many you can manipulate, this code right
	//		below me decides to charge, to a whopping, nothing.

	/*
	if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var) && !g_GlobalInfo.m_bChoking) { // ok ik this is advanced BUT hear me out
		g_GlobalInfo.m_bForceSendPacket = true; // the more failsafes the safer u r from failure right		// we do this so our cheat has 1 tick to turn off fakelag and game can account for it all that shih
		g_GlobalInfo.m_bRecharging = true;																	// and then while recharging we make sure we keep up connection with the server as well
	}
	else if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var)) {	// here we check to see if we tried and hadn't waited the 1 tick
		g_GlobalInfo.m_bForceSendPacket = true;									// and then we set it, next time we cycle around we won't hit this, probably
	}
	if (g_GlobalInfo.m_bRecharging && g_GlobalInfo.m_nShifted < Vars::Misc::CL_Move::DTTicks.m_Var) {
		g_GlobalInfo.m_bForceSendPacket = true;													// continue to ensure we keep up connection with the server while recharging
		g_GlobalInfo.m_nShifted++;								// while it is unlikely anybody would allow for a commit to choke packets while we are manipulating ticks, it is possible.
		g_GlobalInfo.m_nWaitForShift = DT_WAIT_CALLS;			//		so we will account for the possibility by ensuring that anybody trying to do so is almost certain to find this
		return; // Don't move									//				or be left with a half broken feature and a broken doubletap
	}
	else {
		g_GlobalInfo.m_bRecharging = false;
	}
	*/

	if (g_GlobalInfo.m_bRechargeQueued && !g_GlobalInfo.m_bChoking) {
		g_GlobalInfo.m_bRechargeQueued = false;
		g_GlobalInfo.m_bRecharging = true;
	}
	else if (g_GlobalInfo.m_bRecharging && (g_GlobalInfo.m_nShifted < Vars::Misc::CL_Move::DTTicks.m_Var)) {
		g_GlobalInfo.m_bForceSendPacket = true;				// force uninterrupted connection with server
		g_GlobalInfo.m_nShifted++;							// add ticks to tick counter
		g_GlobalInfo.m_nWaitForShift = DT_WAIT_CALLS + 1;	// set wait condition
		return;												// !CLMove
	}
	else if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var)) {
		g_GlobalInfo.m_bForceSendPacket = true;
		g_GlobalInfo.m_bRechargeQueued = true;
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

	if (g_GlobalInfo.m_bShouldShift && !g_GlobalInfo.m_nWaitForShift) {
		if (
			(Vars::Misc::CL_Move::DTMode.m_Var == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) ||	// 0 - On key
			(Vars::Misc::CL_Move::DTMode.m_Var == 1) ||																	// 1 - Always
			(Vars::Misc::CL_Move::DTMode.m_Var == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)))		// 2 - Disable on key 
		{
			//while (g_GlobalInfo.m_nShifted != 0) { // equals -1 like a bawss
			while (g_GlobalInfo.m_nShifted > 0) {
				oClMove(accumulated_extra_samples, g_GlobalInfo.m_nShifted == 1);
				g_GlobalInfo.m_nShifted--;
				g_GlobalInfo.m_bForceSendPacket = true;
				g_Interfaces.Engine->FireEvents();
			}
			g_Interfaces.Engine->FireEvents();
			g_GlobalInfo.m_bForceSendPacket = true;
			g_GlobalInfo.m_nWaitForShift = DT_WAIT_CALLS;
		}
		g_GlobalInfo.m_bShouldShift = false;
	}
}

void __cdecl EngineHook::CL_SendMove::Hook(void* ecx, void* edx)
{
	byte data[4000];

	const int nextcommandnr = g_Interfaces.ClientState->lastoutgoingcommand + g_Interfaces.ClientState->chokedcommands + 1;

	CLC_Move moveMsg;
	moveMsg.m_DataOut.StartWriting(data, sizeof(data));
	moveMsg.m_nNewCommands = std::clamp(1 + g_Interfaces.ClientState->chokedcommands, 0, 15);
	const int extraCommands = g_Interfaces.ClientState->chokedcommands + 1 - moveMsg.m_nNewCommands;
	const int backupCommands = std::max(2, extraCommands);
	moveMsg.m_nBackupCommands = std::clamp(backupCommands, 0, 7);

	const int numcmds = moveMsg.m_nNewCommands + moveMsg.m_nBackupCommands;

	int from = -1;
	bool bOK = true;
	for (int to = nextcommandnr - numcmds + 1; to <= nextcommandnr; to++) {
		const bool isnewcmd = to >= nextcommandnr - moveMsg.m_nNewCommands + 1;
		bOK = bOK && g_Interfaces.Client->WriteUsercmdDeltaToBuffer(&moveMsg.m_DataOut, from, to, isnewcmd);
		from = to;
	}

	if (bOK) {
		if (extraCommands)
			g_Interfaces.ClientState->m_NetChannel->m_nChokedPackets -= extraCommands;
		GetVFunc<bool(__thiscall*)(PVOID, INetMessage* msg, bool, bool)>(g_Interfaces.ClientState->m_NetChannel, 37)(g_Interfaces.ClientState->m_NetChannel, &moveMsg, false, false);
	}
	//static auto originalFn = Func.Original<fn>();
	//return originalFn(ecx, edx);
}

float __fastcall EngineHook::CL_FireEvents::Hook(void* ecx, void* edx)
{
	static auto originalFn = Func.Original<fn>();
	static DWORD dwGetTime = g_Pattern.Find(_(L"engine.dll"), _(L"D9 43 ? DF F1"));

	if (reinterpret_cast<DWORD>(_ReturnAddress()) == (dwGetTime))
		return FLT_MAX;

	return originalFn(ecx, edx);
}	// this shit fucking works