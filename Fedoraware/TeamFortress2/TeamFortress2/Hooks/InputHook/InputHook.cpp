#include "InputHook.h"

CUserCmd* __stdcall InputHook::GetUserCmd::Hook(int sequence_number)
{
	auto cmds = *(CUserCmd**)(g_Interfaces.Input + 0xFC);
	return &cmds[sequence_number % 90];
}
