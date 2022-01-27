#pragma once

#include "../../SDK/SDK.h"

namespace EngineClientHook
{
	inline SEOHook::VTable Table;

	namespace IsPlayingTimeDemo
	{
		const int index = 78;
		using fn = bool(__thiscall*)(CEngineClient*);
		bool __stdcall Hook();
	}
}