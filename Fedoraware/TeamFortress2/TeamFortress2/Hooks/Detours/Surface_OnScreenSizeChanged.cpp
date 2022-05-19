#include "../Hooks.h"

MAKE_HOOK(Surface_OnScreenSizeChanged, Utils::GetVFuncPtr(g_Interfaces.Surface, 111), void, __fastcall,
		  void* ecx, void* edx, int nOldWidth, int nOldHeight)
{
	Hook.Original<FN>()(ecx, edx, nOldWidth, nOldHeight);

	g_ScreenSize.Update();
	g_Draw.ReloadFonts();
}