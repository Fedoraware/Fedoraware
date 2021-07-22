#pragma once

#include "../../SDK/SDK.h"

namespace SurfaceHook
{
	inline SEOHook::VTable Table;

	namespace OnScreenSizeChanged
	{
		const int index = 111;
		using fn = void(__thiscall *)(CSurface *, int, int);
		void __stdcall Hook(int OldWidht, int OldHeight);
	}

	namespace LockCursor
	{
		const int index = 62;
		using fn = void(__thiscall *)(CSurface *);
		void __stdcall Hook();
	}
}