#include "../Hooks.h"

MAKE_HOOK(CRendering3dView_EnableWorldFog, g_Pattern.Find(L"client.dll", L"55 8B EC 8B 0D ? ? ? ? 83 EC 0C 8B 01 53 56 FF 90 ? ? ? ? 8B F0 85 F6 74 07 8B 06 8B CE FF 50 08"), void, __cdecl)
{
	if (Vars::Visuals::Fog::DisableFog.Value)
	{
		return;
	}
	if (!Vars::Visuals::Fog::CustomFog.Value)
	{
		return Hook.Original<FN>()();
	}
	auto pRenderContext = I::MatSystem->GetRenderContext();
	if (pRenderContext)
	{
		using namespace Vars::Visuals;
		pRenderContext->FogMode(MATERIAL_FOG_LINEAR);
		pRenderContext->FogColor3fv(Color::TOBLEND(Fog::FogColor));
		pRenderContext->FogStart(Fog::FogStart.Value);
		pRenderContext->FogEnd(Fog::FogEnd.Value);
		pRenderContext->FogMaxDensity(Fog::FogDensity.Value);
	}
}