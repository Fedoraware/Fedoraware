#include "../Hooks.h"

DWORD CL_FireEventsAddress()
{
	DWORD dwCLFireEvents = g_Pattern.Find(L"engine.dll", L"E8 ? ? ? ? 83 EC 10") + 0x1;
	return *(PDWORD)dwCLFireEvents + dwCLFireEvents + 4;
}

MAKE_HOOK(CL_FireEvents, CL_FireEventsAddress(), float, __fastcall,
		  void* ecx, void* edx)
{
	static DWORD dwGetTime = g_Pattern.Find(L"engine.dll", L"D9 43 ? DF F1");

	if (reinterpret_cast<DWORD>(_ReturnAddress()) == (dwGetTime))
	{
		return FLT_MAX;
	}

	return Hook.Original<FN>()(ecx, edx);
}