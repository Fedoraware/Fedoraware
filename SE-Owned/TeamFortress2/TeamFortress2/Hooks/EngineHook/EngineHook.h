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

	//namespace CL_SendMove
	//{

	//}
}