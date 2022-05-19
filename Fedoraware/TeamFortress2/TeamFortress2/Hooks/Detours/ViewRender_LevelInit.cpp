#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"

void CustomFogSetup()
{
	if (static auto fog_enable = I::CVars->FindVar("fog_enable"); fog_enable)
	{
		fog_enable->SetValue(Vars::Visuals::Fog::CustomFog.m_Var);
	}
	if (static auto fog_enableskybox = I::CVars->FindVar("fog_enableskybox"); fog_enableskybox)
	{
		fog_enableskybox->SetValue(Vars::Visuals::Fog::CustomFog.m_Var);
	}
	if (static auto fog_override = I::CVars->FindVar("fog_override"); fog_override)
	{
		fog_override->SetValue(Vars::Visuals::Fog::CustomFog.m_Var);
	}
	if (static auto fog_density = I::CVars->FindVar("fog_maxdensity"); fog_density)
	{
		fog_density->SetValue(Vars::Visuals::Fog::FogDensity.m_Var);
	}
	if (static auto fog_color = I::CVars->FindVar("fog_color"); fog_color)
	{
		fog_color->SetValue(std::string("").
							append(std::to_string(Vars::Visuals::Fog::FogColor.r)).
							append(" ").
							append(std::to_string(Vars::Visuals::Fog::FogColor.g)).
							append(" ").
							append(std::to_string(Vars::Visuals::Fog::FogColor.b)).
							append(" ").c_str());
	}
	if (static auto fog_start = I::CVars->FindVar("fog_start"); fog_start)
	{
		fog_start->SetValue(Vars::Visuals::Fog::FogStart.m_Var);
	}
	if (static auto fog_end = I::CVars->FindVar("fog_end"); fog_end)
	{
		fog_end->SetValue(Vars::Visuals::Fog::FogEnd.m_Var);
	}
	if (static auto fog_density = I::CVars->FindVar("fog_maxdensityskybox"); fog_density)
	{
		fog_density->SetValue(Vars::Visuals::Fog::FogDensitySkybox.m_Var);
	}
	if (static auto fog_colorskybox = I::CVars->FindVar("fog_colorskybox"); fog_colorskybox)
	{
		fog_colorskybox->SetValue(std::string("").
								  append(std::to_string(Vars::Visuals::Fog::FogColorSkybox.r)).
								  append(" ").
								  append(std::to_string(Vars::Visuals::Fog::FogColorSkybox.g)).
								  append(" ").
								  append(std::to_string(Vars::Visuals::Fog::FogColorSkybox.b)).
								  append(" ").c_str());
	}
	if (static auto fog_start = I::CVars->FindVar("fog_startskybox"); fog_start)
	{
		fog_start->SetValue(Vars::Visuals::Fog::FogStartSkybox.m_Var);
	}
	if (static auto fog_end = I::CVars->FindVar("fog_endskybox"); fog_end)
	{
		fog_end->SetValue(Vars::Visuals::Fog::FogEndSkybox.m_Var);
	}
}

MAKE_HOOK(ViewRender_LevelInit, Utils::GetVFuncPtr(I::ViewRender, 1), void, __fastcall,
		  void* ecx, void* edx)
{
	CustomFogSetup();
	g_Visuals.StoreMaterialHandles();
	g_Visuals.OverrideWorldTextures();
	Hook.Original<FN>()(ecx, edx);
	g_Visuals.ModulateWorld();
}