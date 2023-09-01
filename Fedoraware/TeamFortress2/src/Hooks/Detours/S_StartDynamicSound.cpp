#include "../Hooks.h"
#include "../../Features/Visuals/Visuals.h"

MAKE_HOOK(S_StartDynamicSound, g_Pattern.Find(L"engine.dll", L"55 8B EC 8B 0D ? ? ? ? 81 EC ? ? ? ? 85 C9"), int, __cdecl,
		  StartSoundParams_t& params)
{
	F::Visuals.ManualNetwork(params);
	return Hook.Original<FN>()(params);
}