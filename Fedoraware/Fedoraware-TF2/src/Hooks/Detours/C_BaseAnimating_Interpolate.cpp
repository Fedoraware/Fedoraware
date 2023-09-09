#include "../Hooks.h"

//	xref C_BaseEntity_BaseInterpolatePart1
MAKE_HOOK(C_BaseAnimating_Interpolate, S::CBaseAnimating_Interpolate(), bool, __fastcall, void* ecx, void* edx, float currentTime)
{
	return Vars::Misc::DisableInterpolation.Value ? true : Hook.Original<FN>()(ecx, edx, currentTime);
}
