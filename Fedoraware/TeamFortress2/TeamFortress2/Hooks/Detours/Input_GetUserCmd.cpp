#include "../Hooks.h"

inline CUserCmd* GetCmds()
{
	return *reinterpret_cast<CUserCmd**>(reinterpret_cast<uintptr_t>(I::Input) + 0xFC);
}

MAKE_HOOK(Input_GetUserCmd, Utils::GetVFuncPtr(I::Input, 8), CUserCmd*, __fastcall,
		  void* ecx, void* edx, int sequence_number)
{
	return &GetCmds()[sequence_number % 90];
}