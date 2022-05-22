#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Resolver/Resolver.h"

MAKE_HOOK(BaseClientDLL_Shutdown, Utils::GetVFuncPtr(I::Client, 7), void, __fastcall,
		  void* ecx, void* edx)
{
	Hook.Original<FN>()(ecx, edx);
	g_EntityCache.Clear();
	g_Visuals.rain.Cleanup();
	g_GlobalInfo.partyPlayerESP.clear();
	g_Resolver.ResolveData.clear();
	g_GlobalInfo.chokeMap.clear();
}