#include "../Hooks.h"

MAKE_HOOK(CInventoryManager_ShowItemsPickedUp, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 18 80 7D 08 00"), bool, __fastcall,
		  void* ecx, void* edx, bool bForce, bool bReturnToGame, bool bNoPanel)
{
	Hook.Original<FN>()(ecx, edx, true, true, true);
	return false;
}