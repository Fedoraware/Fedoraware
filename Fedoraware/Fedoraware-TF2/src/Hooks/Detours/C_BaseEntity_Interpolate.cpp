#include "../Hooks.h"

//	xref C_BaseEntity_BaseInterpolatePart1
MAKE_HOOK(C_BaseEntity_Interpolate, S::CBaseEntity_Interpolate(), bool, __fastcall, void* ecx, void* edx, float currentTime)
{
	return Vars::Misc::DisableInterpolation.Value ? true : Hook.Original<FN>()(ecx, edx, currentTime);
}
