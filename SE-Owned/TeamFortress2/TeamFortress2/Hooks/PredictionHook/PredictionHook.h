#pragma once

#include "../../SDK/SDK.h"

namespace PredictionHook
{
	inline SEOHook::VTable Table;

	namespace RunCommand
	{
		const int index = 17;
		using fn = void(__thiscall*)(CPrediction*, CBaseEntity*, CUserCmd*, CMoveHelper*);
		void __stdcall Hook(CBaseEntity* pEntity, CUserCmd* pCmd, CMoveHelper* pMoveHelper);
	}
}