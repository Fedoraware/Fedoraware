#include "../Hooks.h"

MAKE_HOOK(BaseClientDLL_PreEntity, Utils::GetVFuncPtr(g_Interfaces.Client, 5), void, __fastcall,
		  void* ecx, void* edx, const char* szMapName)
{
	Hook.Original<FN>()(ecx, edx, szMapName);
}