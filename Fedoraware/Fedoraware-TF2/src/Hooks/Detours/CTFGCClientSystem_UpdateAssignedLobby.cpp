#include "../Hooks.h"

// Unsure if this really returns a char (bool)

MAKE_HOOK(CTFGCClientSystem_UpdateAssignedLobby, S::CTFGCClientSystem_UpdateAssignedLobby(), char, __fastcall,
		  void* ecx, void* edx)
{
	char cReturn = Hook.Original<FN>()(ecx, edx);
	if (ecx)
	{
		*((bool*)ecx + 1440) = false;
	}
	return cReturn;
}