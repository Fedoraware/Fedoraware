#include "ConVars.h"

void CConVars::Init()
{
	cl_interp = g_Interfaces.CVars->FindVar(_("cl_interp"));
	cl_sidespeed = g_Interfaces.CVars->FindVar(_("cl_sidespeed"));
	cl_interp_ratio = g_Interfaces.CVars->FindVar(_("cl_interp_ratio"));
	cl_updaterate = g_Interfaces.CVars->FindVar(_("cl_updaterate"));
	tf_crit_cap = g_Interfaces.CVars->FindVar(_("tf_weapon_criticals_bucket_cap"));
	tf_crit_bottom = g_Interfaces.CVars->FindVar(_("tf_weapon_criticals_bucket_bottom"));
	tf_crit_default = g_Interfaces.CVars->FindVar(_("tf_weapon_criticals_bucket_default"));
	sv_gravity = g_Interfaces.CVars->FindVar(_("sv_gravity"));
	cl_flipviewmodels = g_Interfaces.CVars->FindVar(_("cl_flipviewmodels"));
	sv_maxunlag = g_Interfaces.CVars->FindVar(_("sv_maxunlag"));
	tf_enable_glows_after_respawn = g_Interfaces.CVars->FindVar(_("tf_enable_glows_after_respawn"));
	glow_outline_effect_enable = g_Interfaces.CVars->FindVar(_("glow_outline_effect_enable"));
	r_drawspecificstaticprop = g_Interfaces.CVars->FindVar(_("r_drawspecificstaticprop"));
	sv_namechange_cooldown_seconds = g_Interfaces.CVars->FindVar(_("sv_namechange_cooldown_seconds"));
	sv_cheats = g_Interfaces.CVars->FindVar(_("sv_cheats"));
	afkTimer = g_Interfaces.CVars->FindVar(_("mp_idlemaxtime"));

	ConCommandBase* cmdBase = g_Interfaces.CVars->GetCommands();
	while (cmdBase != nullptr) {
		constexpr int FCVAR_HIDDEN = (int)EConVarFlags::FCVAR_HIDDEN;
		constexpr int FCVAR_DEVELOPMENT_ONLY = (int)EConVarFlags::FCVAR_DEVELOPMENT_ONLY;
		cmdBase->m_nFlags &= ~( FCVAR_HIDDEN | FCVAR_DEVELOPMENT_ONLY );

		if (ConVar* convar = reinterpret_cast<ConVar*>(cmdBase)) {
			if (convar->GetName() == "name") {
				convar->m_fnChangeCallback = 0;
				convar->InternalSetValue(0);
				convar->m_fnChangeCallback = 0;
			}
			cvarMap[FNV1A::HashConst(convar->GetName())] = convar;

		}

		cmdBase = cmdBase->m_pNext;
	}
}