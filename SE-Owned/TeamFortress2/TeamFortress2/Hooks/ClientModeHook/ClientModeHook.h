#pragma once

#include "../../SDK/SDK.h"

namespace ClientModeHook
{
	inline SEOHook::VTable Table;

	namespace OverrideView
	{
		const int index = 16;
		using fn = void(__thiscall*)(CClientModeShared*, CViewSetup*);
		void __stdcall Hook(CViewSetup* pView);
	}

	namespace ShouldDrawViewModel
	{
		const int index = 24;
		using fn = bool(__thiscall*)(CClientModeShared*);
		bool __stdcall Hook();
	}

	namespace CreateMove
	{
		const int index = 21;
		using fn = bool(__thiscall*)(CClientModeShared*, float, CUserCmd*);
		bool __stdcall Hook(float input_sample_frametime, CUserCmd* pCmd);
	}

	namespace DoPostScreenSpaceEffects
	{
		const int index = 39;
		using fn = bool(__thiscall*)(CClientModeShared*, const CViewSetup*);
		bool __stdcall Hook(const CViewSetup* pSetup);
	}

}