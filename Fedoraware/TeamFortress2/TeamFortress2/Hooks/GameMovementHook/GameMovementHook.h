#pragma once
#include "../Hooks.h"

namespace GameMovement {
	inline SEOHook::VTable Table;
	namespace ProcessMovement {
		const int index = 1;
		using fn = void(__thiscall*)(CGameMovement*, CBaseEntity*, CMoveData*);
		void __stdcall Hook(CBaseEntity* pPlayer, CMoveData* pMove);
	}
}