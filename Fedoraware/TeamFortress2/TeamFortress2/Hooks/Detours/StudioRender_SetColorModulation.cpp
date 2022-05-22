#include "../Hooks.h"

MAKE_HOOK(StudioRender_SetColorModulation, Utils::GetVFuncPtr(I::StudioRender, 27), void, __fastcall,
		  void* ecx, void* edx, const float* pColor)
{
	const float flCustomBlend[3] = {
		Color::TOFLOAT(Colors::StaticPropModulation.r),
		Color::TOFLOAT(Colors::StaticPropModulation.g),
		Color::TOFLOAT(Colors::StaticPropModulation.b)
	};

	const bool bShouldUseCustomBlend = Vars::Visuals::WorldModulation.m_Var && g_GlobalInfo.m_bDrawingStaticProps;
	Hook.Original<FN>()(ecx, edx, bShouldUseCustomBlend ? flCustomBlend : pColor);
}