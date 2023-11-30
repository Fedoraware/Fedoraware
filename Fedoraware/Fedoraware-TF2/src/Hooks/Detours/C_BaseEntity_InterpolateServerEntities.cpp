#include "../Hooks.h"

MAKE_HOOK(C_BaseEntity_InterpolateServerEntities, S::C_BaseEntity_InterpolateServerEntities(), void, __cdecl)
{
	if (Vars::Misc::DisableInterpolation.Value) {
		static ConVar* cl_extrapolate = I::Cvar->FindVar("cl_extrapolate");

		if (cl_extrapolate && cl_extrapolate->GetInt()) {
			cl_extrapolate->SetValue(0);
		}
	}

	return Hook.Original<FN>()();
}
