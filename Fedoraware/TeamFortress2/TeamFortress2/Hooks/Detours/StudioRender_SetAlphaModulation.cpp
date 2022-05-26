#include "../Hooks.h"

MAKE_HOOK(StudioRender_SetAlphaModulation, Utils::GetVFuncPtr(I::StudioRender, 28), void, __fastcall,
		  void* ecx, void* edx, float flAlpha)
{
	Hook.Original<FN>()(ecx, edx, G::DrawingStaticProps ? Color::TOFLOAT(Colors::StaticPropModulation.a) : flAlpha);
}