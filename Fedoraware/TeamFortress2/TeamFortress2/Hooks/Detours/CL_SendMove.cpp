#include "../Hooks.h"

MAKE_HOOK(CL_SendMove, g_Pattern.Find(L"engine.dll", L"55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 8D"), void, __cdecl,
		  void* ecx, void* edx)
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
		{
			g_Interfaces.ClientState->m_NetChannel->m_nChokedPackets -= extraCommands;
		}
		GetVFunc<bool(__thiscall*)(PVOID, INetMessage* msg, bool, bool)>(g_Interfaces.ClientState->m_NetChannel, 37)(g_Interfaces.ClientState->m_NetChannel, &moveMsg, false, false);
	}
}