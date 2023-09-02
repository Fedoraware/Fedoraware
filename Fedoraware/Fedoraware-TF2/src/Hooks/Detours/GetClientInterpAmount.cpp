#include "../Hooks.h"

MAKE_HOOK(GetClientInterpAmount, g_Pattern.Find(L"client.dll", L"55 8B EC A1 ? ? ? ? 83 EC 08 56 A8 01 75 22 8B 0D ? ? ? ? 83 C8 01 A3 ? ? ? ? 68 ? ? ? ? 8B 01 FF 50 34 8B F0 89 35 ? ? ? ? EB 06 8B 35 ? ? ? ? 85 F6 74 68 8B 06 8B CE 8B 40 3C FF D0 8B 0D"), float, __cdecl)
{
	return (Vars::Misc::DisableInterpolation.Value && !(I::EngineClient->IsTakingScreenshot() && Vars::Visuals::CleanScreenshots.Value)) ? 0.f : Hook.Original<FN>()();
}
