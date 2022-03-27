#include "InputHook.h"

inline CUserCmd* GetCmds()
{
	return *(CUserCmd**)((uintptr_t)g_Interfaces.Input + 0xFC);
}

CUserCmd* __stdcall InputHook::GetUserCmd::Hook(int sequence_number)
{
	return &GetCmds()[sequence_number % 90];
}
