#include "../Hooks.h"

MAKE_HOOK(CSkyboxView_Enable3dSkyboxFog, g_Pattern.E8(L"client.dll", L"E8 ? ? ? ? 8B 87 ? ? ? ? 8D 77 48"), void, __fastcall,
		  void* ecx, void* edx)
{
	if (Vars::Visuals::Fog::DisableFog.Value)
	{
		return;
	}
	if (!Vars::Visuals::Fog::CustomFog.Value)
	{
		return Hook.Original<FN>()(ecx, edx);
	}
	auto pRenderContext = I::MatSystem->GetRenderContext();
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
			pRenderContext->FogColor3fv(Color::TOBLEND(Fog::FogColorSkybox));
			pRenderContext->FogStart(Fog::FogStartSkybox.Value * scale);
			pRenderContext->FogEnd(Fog::FogEndSkybox.Value * scale);
			pRenderContext->FogMaxDensity(Fog::FogDensitySkybox.Value);
		}
	}
}