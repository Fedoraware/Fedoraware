#pragma once

#include "../../SDK/SDK.h"

namespace PerformScreenSpaceEffects
{
	inline SEOHook::Func Func;
	using fn = void(__stdcall*)(int x, int y, int w, int h);
	void __stdcall Hook(int x, int y, int w, int h);
}