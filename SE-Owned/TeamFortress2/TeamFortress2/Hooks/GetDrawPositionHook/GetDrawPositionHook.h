#pragma once
#include "../../SDK/SDK.h"

namespace GetDrawPositionHook
{
	inline SEOHook::Func Func;
	using fn = void(__cdecl*)(float*, float*, bool*, Vec3);
	void __cdecl Hook(float* pX, float* pY, bool* pbBehindCamera, Vec3 angleCrosshairOffset);
}