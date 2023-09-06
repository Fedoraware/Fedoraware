#include "../Hooks.h"

//Credits: myzarfin/spook953

namespace S
{
	MAKE_SIGNATURE(DisplayDamageFeedback, CLIENT_DLL, "E8 ? ? ? ? F3 0F 10 45 ? 83 C4 18 0F 2F 05 ? ? ? ? 8B 75 10", 0x5);
}

MAKE_HOOK(UTIL_TraceLine, S::UTIL_TraceLine(), void, __cdecl,
		  void* ecx, void* edx, Vector* vecAbsStart, Vector* vecAbsEnd, unsigned int mask, CTraceFilter* pFilter, CGameTrace* ptr)
{
	static auto dwDisplayDamageFeedback = S::DisplayDamageFeedback();

	if (reinterpret_cast<DWORD>(_ReturnAddress()) == dwDisplayDamageFeedback && pFilter)
	{
		*reinterpret_cast<float*>(reinterpret_cast<DWORD>(pFilter) + 0x2C) = 1.0f;
		return;
	}

	return Hook.Original<FN>()(ecx, edx, vecAbsStart, vecAbsEnd, mask, pFilter, ptr);
}
