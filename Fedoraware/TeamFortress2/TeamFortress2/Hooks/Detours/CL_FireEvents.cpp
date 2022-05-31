#include "../Hooks.h"

MAKE_HOOK(CL_FireEvents, g_Pattern.Find( L"engine.dll", L"55 8B EC 51 80 B9 14 4B" ), float, __fastcall,
		  void* ecx, void* edx)
{
	static DWORD dwGetTime = g_Pattern.Find(L"engine.dll", L"D9 43 ? DF F1");

	if (reinterpret_cast<DWORD>(_ReturnAddress()) == (dwGetTime))
	{
		return FLT_MAX;
	}

	return Hook.Original<FN>()(ecx, edx);
}