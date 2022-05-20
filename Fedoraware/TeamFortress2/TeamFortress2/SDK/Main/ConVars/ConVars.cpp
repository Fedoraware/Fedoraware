#include "ConVars.h"

void CConVars::Init()
{
	cl_interp = I::CVars->FindVar(_("cl_interp"));
	cl_sidespeed = I::CVars->FindVar(_("cl_sidespeed"));
	cl_interp_ratio = I::CVars->FindVar(_("cl_interp_ratio"));
	cl_updaterate = I::CVars->FindVar(_("cl_updaterate"));
	tf_crit_cap = I::CVars->FindVar(_("tf_weapon_criticals_bucket_cap"));
	tf_crit_bottom = I::CVars->FindVar(_("tf_weapon_criticals_bucket_bottom"));
	tf_crit_default = I::CVars->FindVar(_("tf_weapon_criticals_bucket_default"));
	sv_gravity = I::CVars->FindVar(_("sv_gravity"));
	cl_flipviewmodels = I::CVars->FindVar(_("cl_flipviewmodels"));
	sv_maxunlag = I::CVars->FindVar(_("sv_maxunlag"));
	tf_enable_glows_after_respawn = I::CVars->FindVar(_("tf_enable_glows_after_respawn"));
	glow_outline_effect_enable = I::CVars->FindVar(_("glow_outline_effect_enable"));
	r_drawspecificstaticprop = I::CVars->FindVar(_("r_drawspecificstaticprop"));
	sv_namechange_cooldown_seconds = I::CVars->FindVar(_("sv_namechange_cooldown_seconds"));
	afkTimer = I::CVars->FindVar(_("mp_idlemaxtime"));

	ConCommandBase* cmdBase = I::CVars->GetCommands();
	while (cmdBase != nullptr) {
		constexpr int FCVAR_HIDDEN = (int)EConVarFlags::FCVAR_HIDDEN;
		constexpr int FCVAR_DEVELOPMENT_ONLY = (int)EConVarFlags::FCVAR_DEVELOPMENT_ONLY;
		constexpr int FCVAR_CHEAT = (int)EConVarFlags::FCVAR_CHEAT;
		constexpr int FCVAR_NOT_CONNECTED = (int)EConVarFlags::FCVAR_NOT_CONNECTED;
		cmdBase->m_nFlags &= ~(FCVAR_HIDDEN | FCVAR_DEVELOPMENT_ONLY | FCVAR_CHEAT | FCVAR_NOT_CONNECTED);


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