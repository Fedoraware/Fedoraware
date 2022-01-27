#pragma once
#include "../BaseEntity/BaseEntity.h"

struct ConVars_t
{
	ConVar* cl_interp = nullptr;
	ConVar* cl_sidespeed = nullptr;
	ConVar* cl_interp_ratio = nullptr;
	ConVar* cl_updaterate = nullptr;
	ConVar* tf_crit_cap = nullptr;
	ConVar* tf_crit_bottom = nullptr;
	ConVar* tf_crit_default = nullptr;
	ConVar* sv_gravity = nullptr;
	ConVar* cl_flipviewmodels = nullptr;
	ConVar* sv_maxunlag = nullptr;
	ConVar* tf_enable_glows_after_respawn = nullptr;
	ConVar* glow_outline_effect_enable = nullptr;
	ConVar* r_drawspecificstaticprop = nullptr;
	ConVar* sv_namechange_cooldown_seconds = nullptr;
	ConVar* afkTimer = nullptr;

	void Init();
};

inline ConVars_t g_ConVars;