#include "../Hooks.h"

MAKE_HOOK(CTFPartyClient_OnInQueueChanged, g_Pattern.Find(L"client.dll", L"55 8B EC 56 8B 75 08 57 56 8B F9 E8 ? ? ? ? 83 C4 04 84 C0 74 38 83 FE FF 74 33 8B 57 38 33 C0 85 D2 7E 0F 8B 4F 2C"), void, __fastcall,
		  void* ecx, void* edx, int iMatchGroup)
{
	Hook.Original<FN>()(ecx, edx, iMatchGroup);
}