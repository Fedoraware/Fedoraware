#pragma once
#include "../../SDK/SDK.h"

namespace EventMangerHook
{
	inline SEOHook::VTable Table;

	namespace FireEventClientSide
	{
		const int index = 8;
		using fn = bool(__thiscall*)(CGameEventManager*, CGameEvent* pEvent);
		bool __stdcall Hook(CGameEvent* pEvent);
	}
}