#pragma once

#include "../Hooks.h"

namespace Ragdoll
{
	inline SEOHook::Func Func;
	void Init();
	using fn = void(__fastcall*)(void*, void*);
	void __fastcall Hook(void* ecx, void* edx);
}
