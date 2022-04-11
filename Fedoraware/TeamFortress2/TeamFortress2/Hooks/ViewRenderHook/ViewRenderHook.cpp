#include "ViewRenderHook.h"
#include "../../Features/Chams/Chams.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Camera/CameraWindow.h"

void CustomFogSetup() {
	if (static auto fog_enable = g_Interfaces.CVars->FindVar("fog_enable"); fog_enable) {
		fog_enable->SetValue(Vars::Visuals::Fog::CustomFog.m_Var);
	}
	if (static auto fog_enableskybox = g_Interfaces.CVars->FindVar("fog_enableskybox"); fog_enableskybox) {
		fog_enableskybox->SetValue(Vars::Visuals::Fog::CustomFog.m_Var);
	}
	if (static auto fog_override = g_Interfaces.CVars->FindVar("fog_override"); fog_override) {
		fog_override->SetValue(Vars::Visuals::Fog::CustomFog.m_Var);
	}
	if (static auto fog_density = g_Interfaces.CVars->FindVar("fog_maxdensity"); fog_density) {
		fog_density->SetValue(Vars::Visuals::Fog::FogDensity.m_Var);
	}
	if (static auto fog_color = g_Interfaces.CVars->FindVar("fog_color"); fog_color) {
		fog_color->SetValue(std::string("").
			append(std::to_string(Vars::Visuals::Fog::FogColor.r)).
			append(" ").
			append(std::to_string(Vars::Visuals::Fog::FogColor.g)).
			append(" ").
			append(std::to_string(Vars::Visuals::Fog::FogColor.b)).
			append(" ").c_str());
	}
	if (static auto fog_start = g_Interfaces.CVars->FindVar("fog_start"); fog_start) {
		fog_start->SetValue(Vars::Visuals::Fog::FogStart.m_Var);
	}
	if (static auto fog_end = g_Interfaces.CVars->FindVar("fog_end"); fog_end) {
		fog_end->SetValue(Vars::Visuals::Fog::FogEnd.m_Var);
	}
	if (static auto fog_density = g_Interfaces.CVars->FindVar("fog_maxdensityskybox"); fog_density) {
		fog_density->SetValue(Vars::Visuals::Fog::FogDensitySkybox.m_Var);
	}
	if (static auto fog_colorskybox = g_Interfaces.CVars->FindVar("fog_colorskybox"); fog_colorskybox) {
		fog_colorskybox->SetValue(std::string("").
			append(std::to_string(Vars::Visuals::Fog::FogColorSkybox.r)).
			append(" ").
			append(std::to_string(Vars::Visuals::Fog::FogColorSkybox.g)).
			append(" ").
			append(std::to_string(Vars::Visuals::Fog::FogColorSkybox.b)).
			append(" ").c_str());
	}
	if (static auto fog_start = g_Interfaces.CVars->FindVar("fog_startskybox"); fog_start) {
		fog_start->SetValue(Vars::Visuals::Fog::FogStartSkybox.m_Var);
	}
	if (static auto fog_end = g_Interfaces.CVars->FindVar("fog_endskybox"); fog_end) {
		fog_end->SetValue(Vars::Visuals::Fog::FogEndSkybox.m_Var);
	}
}

void __fastcall ViewRenderHook::LevelInit::Hook(void* ecx, void* edx)
{
	CustomFogSetup();
	g_Visuals.StoreMaterialHandles();
	g_Visuals.OverrideWorldTextures();
	Func.Original<fn>()(ecx);
	g_Visuals.ModulateWorld();
}

void __fastcall ViewRenderHook::LevelShutdown::Hook(void* ecx, void* edx)
{
	g_Visuals.ClearMaterialHandles();
	Func.Original<fn>()(ecx);
}

void __fastcall ViewRenderHook::RenderView::Hook(void* ecx, void* edx, const CViewSetup& view, ClearFlags_t nClearFlags, RenderViewInfo_t whatToDraw)
{
	Func.Original<fn>()(ecx, view, nClearFlags, whatToDraw);
	g_CameraWindow.RenderView(ecx, view);
}
