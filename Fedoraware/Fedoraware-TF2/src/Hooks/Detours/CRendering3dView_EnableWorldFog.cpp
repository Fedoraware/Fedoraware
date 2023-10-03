#include "../Hooks.h"

MAKE_HOOK(CRendering3dView_EnableWorldFog, S::CRendering3dView_EnableWorldFog(), void, __cdecl)
{
	if (I::EngineClient->IsTakingScreenshot() && Vars::Visuals::CleanScreenshots.Value) { return Hook.Original<FN>()(); }
	if (Vars::Visuals::Fog::DisableFog.Value)
	{
		return;
	}
	if (!Vars::Visuals::Fog::CustomFog.Value)
	{
		return Hook.Original<FN>()();
	}

	const auto pRenderContext = I::MaterialSystem->GetRenderContext();
	if (pRenderContext)
	{
		using namespace Vars::Visuals;
		pRenderContext->FogMode(MATERIAL_FOG_LINEAR);
		float blend[3] = { Color::TOFLOAT(Fog::FogColor.Value.r), Color::TOFLOAT(Fog::FogColor.Value.g), Color::TOFLOAT(Fog::FogColor.Value.b) };
		pRenderContext->FogColor3fv(blend);
		pRenderContext->FogStart(Fog::FogStart.Value);
		pRenderContext->FogEnd(Fog::FogEnd.Value);
		pRenderContext->FogMaxDensity(Fog::FogDensity.Value);
	}
}