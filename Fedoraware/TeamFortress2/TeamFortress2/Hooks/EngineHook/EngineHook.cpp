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

	static ConVar* sv_maxusrcmdprocessticks = g_Interfaces.CVars->FindVar("sv_maxusrcmdprocessticks");
	if (!sv_maxusrcmdprocessticks) {
		return oClMove(accumulated_extra_samples, bFinalTick);
	}
	if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var)) {
		g_GlobalInfo.m_bForceSendPacket = true; // the more failsafes the safer u r from failure right
		g_GlobalInfo.m_bRecharging = true;
	}
	if (g_GlobalInfo.m_bRecharging && g_GlobalInfo.m_nShifted < sv_maxusrcmdprocessticks->GetInt()) {
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

	if (g_GlobalInfo.m_bShouldShift && !g_GlobalInfo.m_nWaitForShift) {
		if (
			(Vars::Misc::CL_Move::DTMode.m_Var == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) ||	// 0 - On key
			(Vars::Misc::CL_Move::DTMode.m_Var == 1) ||																	// 1 - Always
			(Vars::Misc::CL_Move::DTMode.m_Var == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)))		// 2 - Disable on key 
		{
			int ticksShifted = 0;
			
			//while (g_GlobalInfo.m_nShifted != 0) { // equals -1 like a bawss
			while (g_GlobalInfo.m_nShifted > 0) {
				ticksShifted++;
				oClMove(accumulated_extra_samples, g_GlobalInfo.m_nShifted == 1);
				g_GlobalInfo.m_nShifted--;
				g_GlobalInfo.m_bForceSendPacket = true; // make sure we dont fakelag while shifting (should not even be an issue but whatever)

				if (ticksShifted == Vars::Misc::CL_Move::DTTicks.m_Var) {
					g_GlobalInfo.m_bShouldShift = false; // this is retarded and should be done differently
					g_GlobalInfo.m_nWaitForShift = DT_WAIT_CALLS;
					break;
				}
			}
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
