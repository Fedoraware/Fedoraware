#pragma once

#include "../../SDK/SDK.h"
#include "../ClientModeHook/ClientModeHook.h"
namespace EngineHook
{
	namespace CL_Move
	{
		inline SEOHook::Func Func;

		using fn = void(__cdecl*)(float, bool);
		void __cdecl Hook(float accumulated_extra_samples, bool bFinalTick);
	}

	namespace CL_SendMove
	{
		inline SEOHook::Func Func;

		using fn = void(__cdecl*)(void*, void*);
		void __cdecl Hook(void* ecx, void* edx);
	}

	namespace CL_FireEvents
	{
		inline SEOHook::Func Func;
		using fn = float(__fastcall*)(void*, void*);
		float __fastcall Hook(void* ecx, void* edx);
	}
}