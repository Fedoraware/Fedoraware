#include "../Hooks.h"

MAKE_HOOK(CTFPlayerInventory_GetMaxItemCount, g_Pattern.Find(L"client.dll", L"8B 49 68 56"), int, __fastcall,
		  void* ecx, void* edx)
{
	return 3000;
}