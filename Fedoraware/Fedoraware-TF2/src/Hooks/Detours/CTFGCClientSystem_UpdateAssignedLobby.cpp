#include "../Hooks.h"

// Unsure if this really returns a char (bool)

MAKE_HOOK(CTFGCClientSystem_UpdateAssignedLobby, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 28 53 8B D9 56 57 89 5D F0 8B 8B ? ? ? ? 85 C9 0F 84 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 77"), char, __fastcall,
		  void* ecx, void* edx)
{
	char cReturn = Hook.Original<FN>()(ecx, edx);
	if (ecx)
	{
		*((bool*)ecx + 1440) = false;
	}
	return cReturn;
}