#pragma once
#include "../Hooks.h"

namespace IsLocalPlayerUsingVisionFilterFlags
{
	inline SEOHook::Func Hook;

	using fn = bool(__cdecl*)(int, bool);
	bool __cdecl Func(int nFlags, bool bWeaponsCheck);
	void Init();
}