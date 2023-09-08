#include "../Hooks.h"

MAKE_HOOK(CInventoryManager_ShowItemsPickedUp, S::CInventoryManager_ShowItemsPickedUp(), bool, __fastcall,
		  void* ecx, void* edx, bool bForce, bool bReturnToGame, bool bNoPanel)
{
	Hook.Original<FN>()(ecx, edx, true, true, true);
	return false;
}