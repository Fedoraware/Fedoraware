#include "ConVars.h"

void CConVars::Init()
{
	cl_interp = I::Cvar->FindVar("cl_interp");
	cl_sidespeed = I::Cvar->FindVar("cl_sidespeed");
	cl_interp_ratio = I::Cvar->FindVar("cl_interp_ratio");
	cl_updaterate = I::Cvar->FindVar("cl_updaterate");
	tf_crit_cap = I::Cvar->FindVar("tf_weapon_criticals_bucket_cap");
	tf_crit_bottom = I::Cvar->FindVar("tf_weapon_criticals_bucket_bottom");
	tf_crit_default = I::Cvar->FindVar("tf_weapon_criticals_bucket_default");
	sv_gravity = I::Cvar->FindVar("sv_gravity");
	cl_flipviewmodels = I::Cvar->FindVar("cl_flipviewmodels");
	sv_maxunlag = I::Cvar->FindVar("sv_maxunlag");
	tf_enable_glows_after_respawn = I::Cvar->FindVar("tf_enable_glows_after_respawn");
	glow_outline_effect_enable = I::Cvar->FindVar("glow_outline_effect_enable");
	r_drawspecificstaticprop = I::Cvar->FindVar("r_drawspecificstaticprop");
	sv_namechange_cooldown_seconds = I::Cvar->FindVar("sv_namechange_cooldown_seconds");
	afkTimer = I::Cvar->FindVar("mp_idlemaxtime");
	sv_maxusrcmdprocessticks = I::Cvar->FindVar("sv_maxusrcmdprocessticks");
	sv_maxusrcmdprocessticks_holdaim = I::Cvar->FindVar("sv_maxusrcmdprocessticks_holdaim");

	ConCommandBase* cmdBase = I::Cvar->GetCommands();
	while (cmdBase != nullptr)
	{
		constexpr int FCVAR_HIDDEN = (int)EConVarFlags::FCVAR_HIDDEN;
		constexpr int FCVAR_DEVELOPMENT_ONLY = (int)EConVarFlags::FCVAR_DEVELOPMENT_ONLY;
		constexpr int FCVAR_CHEAT = (int)EConVarFlags::FCVAR_CHEAT;
		constexpr int FCVAR_NOT_CONNECTED = (int)EConVarFlags::FCVAR_NOT_CONNECTED;
		cmdBase->m_nFlags &= ~(FCVAR_HIDDEN | FCVAR_DEVELOPMENT_ONLY | FCVAR_CHEAT | FCVAR_NOT_CONNECTED);


		if (ConVar* convar = reinterpret_cast<ConVar*>(cmdBase))
		{
			if (convar->GetName() == "name")
			{
				convar->m_fnChangeCallback = 0;
				convar->InternalSetValue(0);
				convar->m_fnChangeCallback = 0;
			}
			cvarMap[FNV1A::HashConst(convar->GetName())] = convar;

		}

		cmdBase = cmdBase->m_pNext;
	}
}

ConVar* CConVars::FindVar(const char* cvarname)
{
	if (!cvarMap[FNV1A::HashConst(cvarname)])
	{
		cvarMap[FNV1A::HashConst(cvarname)] = I::Cvar->FindVar(cvarname);
	}
	return cvarMap[FNV1A::HashConst(cvarname)];
}