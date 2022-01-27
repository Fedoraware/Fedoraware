#include "GameMovementHook.h"

void __stdcall GameMovement::ProcessMovement::Hook(CBaseEntity* pPlayer, CMoveData* pMove)
{
	fn OriginalFn = Table.Original<fn>(index);

	OriginalFn(g_Interfaces.GameMovement, pPlayer, pMove);
}
