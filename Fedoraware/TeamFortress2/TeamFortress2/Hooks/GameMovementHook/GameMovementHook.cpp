#include "GameMovementHook.h"

void __stdcall GameMovement::ProcessMovement::Hook(CBaseEntity* pPlayer, CMoveData* pMove)
{
	auto OriginalFn = Table.Original<fn>(index);

	OriginalFn(g_Interfaces.GameMovement, pPlayer, pMove);
}

void __cdecl GameMovement::Duck::Hook(void* ecx, void* edx)
{
	if (!g_Interfaces.CTFGameMovement)
	{
		g_Interfaces.CTFGameMovement = ecx;
	}

	Func.Original<fn>()(ecx, edx);
}

void GameMovement::Duck::Init()
{
	auto FN = reinterpret_cast<fn>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 20 53 56 8B D9 57"));
	Func.Hook(FN, Hook);
}
