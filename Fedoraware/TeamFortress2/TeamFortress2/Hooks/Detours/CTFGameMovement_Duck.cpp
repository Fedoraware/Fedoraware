#include "../Hooks.h"

MAKE_HOOK(CTFGameMovement_Duck, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 20 53 56 8B D9 57"), void, __cdecl,
		  void* ecx, void* edx)
{
	if (!I::CTFGameMovement)
	{
		I::CTFGameMovement = ecx;
	}

	Hook.Original<FN>()(ecx, edx);
}