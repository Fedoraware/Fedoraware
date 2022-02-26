#pragma once

#include "../Hooks.h"

namespace MaterialSystemHook
{
	namespace CMaterial {
		namespace Uncache {
			inline SEOHook::Func Hook;
			using fn = void(__thiscall*)(void*,  bool);
			void __fastcall Func(void* ecx, void* edx, bool bPreserveVars);
			void Init();
		}
	}
}