#pragma once

#include "../../SDK/SDK.h"

namespace EngineVGuiHook
{
	inline SEOHook::VTable Table;

	namespace Paint
	{
		using fn = void(__thiscall *)(CEngineVGui *, int);
		constexpr int index = 13;
		void __stdcall Hook(int mode);
	}
}