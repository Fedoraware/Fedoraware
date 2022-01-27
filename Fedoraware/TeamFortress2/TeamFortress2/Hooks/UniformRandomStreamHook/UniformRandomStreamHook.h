#pragma once

#include "../../SDK/SDK.h"

namespace UniformRandomStreamHook
{
	inline SEOHook::VTable Table;

	namespace RandInt
	{
		const int index = 2;
		using fn = int(__thiscall*)(IUniformRandomStream*, int, int);
		int __stdcall Hook(int iMinVal, int iMaxVal);
	}
}