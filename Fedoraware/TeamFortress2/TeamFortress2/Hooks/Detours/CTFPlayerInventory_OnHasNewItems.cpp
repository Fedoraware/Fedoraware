#include "../Hooks.h"

MAKE_HOOK(CTFPlayerInventory_OnHasNewItems, g_Pattern.Find(L"client.dll", L"8B 0D ? ? ? ? 85 C9 74 0E 8B 01 8B 80 ? ? ? ? FF D0 84 C0 75 50"), void, __cdecl)
{
	Hook.Original<FN>()();
}