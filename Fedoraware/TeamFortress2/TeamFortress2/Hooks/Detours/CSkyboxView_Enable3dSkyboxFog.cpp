#include "../Hooks.h"

MAKE_HOOK(CSkyboxView_Enable3dSkyboxFog, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 10 57 E8 ? ? ? ? 8B F8 85 FF 74 57 8B 0D ? ? ? ? 56 8B 11 FF 92 ? ? ? ? 8B F0 85 F6 74 07"), void, __fastcall,
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

			float blend[3] = { Color::TOFLOAT(Fog::FogColorSkybox.r), Color::TOFLOAT(Fog::FogColorSkybox.g), Color::TOFLOAT(Fog::FogColorSkybox.b) };
			pRenderContext->FogColor3fv(blend);
			pRenderContext->FogStart(Fog::FogStartSkybox.Value * scale);
			pRenderContext->FogEnd(Fog::FogEndSkybox.Value * scale);
			pRenderContext->FogMaxDensity(Fog::FogDensitySkybox.Value);
		}
	}
}