#include "EngineHook.h"
#include "../../Features/Vars.h"
#include "../../Features/Misc/Misc.h"

void __cdecl EngineHook::CL_Move::Hook(float accumulated_extra_samples, bool bFinalTick)
{
	static auto oClMove = Func.Original<fn>();

	if (!Vars::Misc::CL_Move::Enabled.m_Var)
	{
		g_GlobalInfo.m_nShifted = 0;
		return oClMove(accumulated_extra_samples, bFinalTick);
	}

	if (g_GlobalInfo.m_nShifted > Vars::Misc::CL_Move::DTTicks.m_Var) { g_GlobalInfo.m_nShifted -= 1; oClMove(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == Vars::Misc::CL_Move::DTTicks.m_Var + 1)); } // pCode

	// pSpeedhack
	if (Vars::Misc::CL_Move::SEnabled.m_Var)
	{
		int SpeedTicks{ 0 };
		int SpeedTicksDesired = Vars::Misc::CL_Move::SFactor.m_Var;
		g_GlobalInfo.m_nShifted = 0;

		while (SpeedTicks < SpeedTicksDesired)
		{
			SpeedTicks++;
			oClMove(accumulated_extra_samples, (SpeedTicks == (SpeedTicksDesired)));
		}
	}

	auto pLocal = g_EntityCache.m_pLocal;

	if (GetAsyncKeyState(Vars::Misc::CL_Move::TeleportKey.m_Var) && g_GlobalInfo.m_nShifted && !g_GlobalInfo.
		m_bRecharging)
	{
		while (g_GlobalInfo.m_nShifted > 0)
		{
			oClMove(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == 1));
			g_GlobalInfo.m_nShifted--;
		}
		return;
	}

	if (g_GlobalInfo.m_bRechargeQueued && !g_GlobalInfo.m_bChoking)
	{
		// probably perfect method of waiting to ensure we don't mess with fakelag
		g_GlobalInfo.m_bRechargeQueued = false; // see relevant code @clientmodehook
		g_GlobalInfo.m_bRecharging = true;
	}
	else if (g_GlobalInfo.m_bRecharging && (g_GlobalInfo.m_nShifted < Vars::Misc::CL_Move::DTTicks.m_Var))
	{
		g_GlobalInfo.m_bForceSendPacket = true; // force uninterrupted connection with server
		g_GlobalInfo.m_nShifted++; // add ticks to tick counter
		g_GlobalInfo.m_nWaitForShift = DT_WAIT_CALLS + 1; // set wait condition
		return; // this recharges
	}
	else if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var))
	{
		// queue recharge
		g_GlobalInfo.m_bForceSendPacket = true;
		g_GlobalInfo.m_bRechargeQueued = true;
	}
	else
	{
		g_GlobalInfo.m_bRecharging = false; // if we are unable to recharge, don't
	}

	oClMove(accumulated_extra_samples,
		(g_GlobalInfo.m_bShouldShift && !g_GlobalInfo.m_nWaitForShift) ? true : bFinalTick);

	if (g_GlobalInfo.m_nWaitForShift)
	{
		g_GlobalInfo.m_nWaitForShift--;
		return;
	}

	if (g_GlobalInfo.lateUserCmd != nullptr)
	{
		// Shift if attacking normally
		if (Vars::Misc::CL_Move::NotInAir.m_Var) {
			if (pLocal) {
				if (pLocal->IsOnGround()) {
					g_GlobalInfo.m_bShouldShift = g_GlobalInfo.m_bShouldShift
						? true
						: g_GlobalInfo.lateUserCmd->buttons & IN_ATTACK;
				}
				else {
					g_GlobalInfo.m_bShouldShift = false;
				}
			}
		}
		else {
			g_GlobalInfo.m_bShouldShift = g_GlobalInfo.m_bShouldShift
				? true
				: g_GlobalInfo.lateUserCmd->buttons & IN_ATTACK;
		}
	}


	if (!pLocal)
	{
		g_GlobalInfo.m_nShifted = 0; // we do not have charge if we do not exist
		return;
	}

	if (g_GlobalInfo.m_bShouldShift && !g_GlobalInfo.m_nWaitForShift)
	{
		if (
			(Vars::Misc::CL_Move::DTMode.m_Var == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) ||
			// 0 - On key
			(Vars::Misc::CL_Move::DTMode.m_Var == 1) || // 1 - Always
			(Vars::Misc::CL_Move::DTMode.m_Var == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)))
			// 2 - Disable on key 
		{
			while (g_GlobalInfo.m_nShifted > 0)
			{
				oClMove(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == 1));
				g_GlobalInfo.m_nShifted--;
				//g_GlobalInfo.m_bForceSendPacket = true;
			}
			g_Interfaces.Engine->FireEvents();
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
	for (int to = nextcommandnr - numcmds + 1; to <= nextcommandnr; to++)
	{
		const bool isnewcmd = to >= nextcommandnr - moveMsg.m_nNewCommands + 1;
		bOK = bOK && g_Interfaces.Client->WriteUsercmdDeltaToBuffer(&moveMsg.m_DataOut, from, to, isnewcmd);
		from = to;
	}

	if (bOK)
	{
		if (extraCommands)
			g_Interfaces.ClientState->m_NetChannel->m_nChokedPackets -= extraCommands;
		GetVFunc<bool(__thiscall*)(PVOID, INetMessage* msg, bool, bool)>(g_Interfaces.ClientState->m_NetChannel, 37)(
			g_Interfaces.ClientState->m_NetChannel, &moveMsg, false, false);
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
}

int __cdecl EngineHook::Q_stricmp::Hook(const char* str1, const char* str2)
{
	static auto originalFn = Func.Original<fn>();

	static std::map<void*, bool> calls = {};

	if (str2 == "name") {
		auto retaddress = _ReturnAddress();
		if (calls.find(retaddress) == calls.end()) {
			calls[retaddress] = true;
			printf("%p\n", retaddress);
		}
	}

	return Func.Original<fn>()(str1, str2);
}

void __cdecl EngineHook::UpdateNameFromSteamID::Hook(IConVar* pConvar, CSteamID* pSteamID)
{
	//static bool setOnce = false;
	if (pConvar && pSteamID) {
		//if (!setOnce) {
		//	Func.Original<fn>()(pConvar, pSteamID);
		//	setOnce = true;
		//}
	}
}


void __cdecl EngineHook::CL_NameCvarChanged::Hook(IConVar* pConvar)
{
	Func.Original<fn>()(pConvar);
	if (auto name = g_Interfaces.CVars->FindVar("name")) {
		g_Interfaces.CVars->ConsolePrintf("[FeD] Name set to: %s\n", name->GetString());
	}
}
