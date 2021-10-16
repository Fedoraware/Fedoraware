#include "InputHook.h"
#include "../../Features/Crits/Crits.h"

CUserCmd* __stdcall InputHook::GetUserCmd::Hook(int sequence_number)
{
	CUserCmd* pCommand = Table.Original<fn>(index)(g_Interfaces.Input, sequence_number);

	if (pCommand == nullptr)
		return pCommand;

	if (!Vars::Crits::Active.m_Var) {
		return pCommand;
	}

	int oldcmd;
	if (g_Crits.new_command_number.find(pCommand->command_number) != g_Crits.new_command_number.end())
	{
		oldcmd = pCommand->command_number;
		pCommand->command_number = g_Crits.new_command_number[pCommand->command_number];
		pCommand->random_seed = MD5_PseudoRandom(unsigned(pCommand->command_number)) & 0x7fffffff;
		g_Crits.new_command_number.erase(g_Crits.new_command_number.find(oldcmd));
		g_Interfaces.ClientState->lastoutgoingcommand = pCommand->command_number - 1;
		if (g_Interfaces.Engine->GetNetChannelInfo())
			g_Interfaces.Engine->GetNetChannelInfo()->m_nOutSequenceNr = pCommand->command_number - 1;
	}

	return pCommand;

	//return Table.Original<fn>(index)(g_Interfaces.Input, sequence_number);
}
