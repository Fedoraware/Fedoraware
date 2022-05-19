#include "../Hooks.h"

MAKE_HOOK(StudioRender_SetAlphaModulation, Utils::GetVFuncPtr(g_Interfaces.StudioRender, 28), void, __fastcall,
		  void* ecx, void* edx, float flAlpha)
{
	Hook.Original<FN>()(ecx, edx, g_GlobalInfo.m_bDrawingStaticProps ? Color::TOFLOAT(Colors::StaticPropModulation.a) : flAlpha);
}