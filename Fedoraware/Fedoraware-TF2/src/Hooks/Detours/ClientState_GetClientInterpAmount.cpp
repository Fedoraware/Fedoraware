#include "../Hooks.h"
#include "../../Features/Vars.h"

MAKE_HOOK(ClientState_GetClientInterpAmount, S::ClientState_GetClientInterpAmount(), float, __fastcall, void* ecx, void* edx)
{
	const float retVal = Hook.Original<FN>()(ecx, edx);
	G::LerpTime = retVal;
	return Vars::Misc::DisableInterpolation.Value ? 0.f : retVal;
}
