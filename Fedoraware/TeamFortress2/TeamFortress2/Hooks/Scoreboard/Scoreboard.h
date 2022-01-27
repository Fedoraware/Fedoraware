#pragma once
#pragma once

#include "../../SDK/SDK.h"
#include "../Hooks.h"
#include "../../Utils/MinHook/hook.h"

namespace Scoreboard
{
	inline SEOHook::Func Func;
	namespace KeyValues {
		using fn = void(__fastcall *)(void*, void*, const char*, int);
		void __fastcall Hook(void* ecx, void* edx, const char* keyName, int value);
		void Init();
	}
	namespace IsPlayerDominated {
		using fny = bool(__fastcall *)(void*, void*, int);
		bool __fastcall Hook(void* ecx, void* edx, int index);
		void Init();
	}
}