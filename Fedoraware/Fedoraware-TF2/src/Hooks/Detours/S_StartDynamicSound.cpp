#include "../Hooks.h"
#include "../../Features/Visuals/Visuals.h"

MAKE_HOOK(S_StartDynamicSound, S::S_StartDynamicSound(), int, __cdecl, StartSoundParams_t& params)
{
	F::Visuals.ManualNetwork(params);
	return Hook.Original<FN>()(params);
}