#include "../Hooks.h"

MAKE_HOOK(GetClientInterpAmount, S::GetClientInterpAmount(), float, __cdecl)
{
	return (Vars::Misc::DisableInterpolation.Value && !(I::EngineClient->IsTakingScreenshot() && Vars::Visuals::CleanScreenshots.Value)) ? 0.f : Hook.Original<FN>()();
}
