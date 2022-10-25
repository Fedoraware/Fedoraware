#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Resolver/Resolver.h"
#include "../../Features/Misc/Misc.h"

MAKE_HOOK(BaseClientDLL_Shutdown, Utils::GetVFuncPtr(I::BaseClientDLL, 7), void, __fastcall,
		  void* ecx, void* edx)
{
	Hook.Original<FN>()(ecx, edx);
	g_EntityCache.Clear();
	F::Visuals.rain.Cleanup();
	G::DormantPlayerESP.clear();
	//F::Resolver.ResolveData.clear();
	G::ChokeMap.clear();
}