#include "../Hooks.h"

MAKE_HOOK(CSkyboxView_Enable3dSkyboxFog, S::CSkyboxView_Enable3dSkyboxFog(), void, __fastcall, void* ecx, void* edx)
{
	if (Vars::Visuals::Fog::DisableFog.Value)
	{
		return;
	}
	if (!Vars::Visuals::Fog::CustomFog.Value)
	{
		return Hook.Original<FN>()(ecx, edx);
	}
	auto pRenderContext = I::MaterialSystem->GetRenderContext();
	if (pRenderContext)
	{
		if (const auto& pLocal = g_EntityCache.GetLocal())
		{
			float scale = 1.f;
			float fSkybox3dScale = (float)*(int*)(pLocal + 3860);
			if (fSkybox3dScale > 0.0f)
			{
				scale = 1.f / fSkybox3dScale;
			}
			using namespace Vars::Visuals;
			pRenderContext->FogMode(MATERIAL_FOG_LINEAR);

			float blend[3] = { Color::TOFLOAT(Fog::FogColorSkybox.Value.r), Color::TOFLOAT(Fog::FogColorSkybox.Value.g), Color::TOFLOAT(Fog::FogColorSkybox.Value.b) };
			pRenderContext->FogColor3fv(blend);
			pRenderContext->FogStart(Fog::FogStartSkybox.Value * scale);
			pRenderContext->FogEnd(Fog::FogEndSkybox.Value * scale);
			pRenderContext->FogMaxDensity(Fog::FogDensitySkybox.Value);
		}
	}
}