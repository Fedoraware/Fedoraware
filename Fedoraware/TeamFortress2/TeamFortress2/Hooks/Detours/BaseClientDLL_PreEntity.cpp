#include "../Hooks.h"

MAKE_HOOK(BaseClientDLL_PreEntity, Utils::GetVFuncPtr(I::Client, 5), void, __fastcall,
		  void* ecx, void* edx, const char* szMapName)
{
	Hook.Original<FN>()(ecx, edx, szMapName);
}