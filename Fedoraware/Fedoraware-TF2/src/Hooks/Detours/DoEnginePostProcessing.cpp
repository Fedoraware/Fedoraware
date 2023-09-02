#include "../Hooks.h"

MAKE_HOOK(DoEnginePostProcessing, g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 33 DB 89 5D 8C 89 5D 90 8B 01 89 5D A4 89 5D A8 56 57 85 C0 74 3C"), void, __cdecl,
		  int x, int y, int w, int h, bool bFlashlightIsOn, bool bPostVGui)
{
	if (I::EngineClient->IsTakingScreenshot() && Vars::Visuals::CleanScreenshots.Value)
	{
		return Hook.Original<FN>()(x, y, w, h, bFlashlightIsOn, bPostVGui);
	}

	if (Vars::Visuals::DoPostProcessing.Value)
	{
		Hook.Original<FN>()(x, y, w, h, bFlashlightIsOn, bPostVGui);
	}
}