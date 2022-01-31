#include "DrawStaticPropsHook.h"
#include "../../Features/Vars.h"

bool bDrawing = false;

void __fastcall DrawStaticPropsHook::Hook(void* ecx, void* edx, IClientRenderable** pProps, int count, bool bShadowDepth, bool drawVCollideWireframe)
{
	bDrawing = true;
	if (Vars::Visuals::PropWireframe.m_Var) { drawVCollideWireframe = true; } // wireframes look kind of bad but it's an option that's kind of alright ig.
	Func.Original<fn>()(ecx, edx, pProps, count, bShadowDepth, drawVCollideWireframe);
	bDrawing = false;
}

void __fastcall SetColorModulationHook::Hook(void* ecx, void* edx, float const* pColor)
{
	float custom[3] = {
		Color::TOFLOAT(Colors::StaticPropModulation.r),
		Color::TOFLOAT(Colors::StaticPropModulation.g),
		Color::TOFLOAT(Colors::StaticPropModulation.b)
	};

	bool bShouldUseCustom = Vars::Visuals::WorldModulation.m_Var && bDrawing;
	Func.Original<fn>()(ecx, edx, bShouldUseCustom ? custom : pColor);
}

void __fastcall SetAlphaModulationHook::Hook(void* ecx, void* edx, float alpha)
{ // its better to get alpha from the var that alr exists than to make another just for alpha
	Func.Original<fn>()(ecx, edx, bDrawing ? Color::TOFLOAT(Colors::StaticPropModulation.a) : alpha);
}