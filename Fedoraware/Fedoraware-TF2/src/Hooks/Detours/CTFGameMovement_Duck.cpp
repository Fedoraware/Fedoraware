#include "../Hooks.h"

MAKE_HOOK(CTFGameMovement_Duck, S::CTFGameMovement_Duck(), void, __cdecl, void* ecx, void* edx)
{
	if (!I::CTFGameMovement)
	{
		I::CTFGameMovement = ecx;
	}

	Hook.Original<FN>()(ecx, edx);
}