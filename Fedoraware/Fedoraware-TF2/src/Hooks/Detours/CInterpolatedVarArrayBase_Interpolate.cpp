#include "../Hooks.h"

//	it's a template.
MAKE_HOOK(CInterpolatedVarArrayBase_Interpolate, S::CInterpolatedVarArrayBase_Interpolate(), int, __fastcall,
		  void* ecx, void* edx, float currentTime, float interpolation_amount)
{
	if (Vars::Misc::DisableInterpolation.Value) { return 1; }
	return Hook.Original<FN>()(ecx, edx, currentTime, interpolation_amount);
}
