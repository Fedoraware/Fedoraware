#include "../Hooks.h"

MAKE_HOOK(CInterpolatedVarArrayBase__Extrapolate, g_Pattern.Find(L"client.dll", L"55 8B EC 8B 45 0C 8B D1 F3 0F 10 05 ? ? ? ? 56"), void, __fastcall,
	void* ecx, void* edx, void* pOut, void* pOld, void* pNew, float flDestinationTime, float flMaxExtrapolationAmount)
{
	return Hook.Original<FN>()(ecx, edx, pOut, pOld, pNew, flDestinationTime, flMaxExtrapolationAmount);
}
