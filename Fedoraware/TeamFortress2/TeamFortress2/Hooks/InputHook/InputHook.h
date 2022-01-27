#pragma once
#include "../../SDK/SDK.h"

namespace InputHook {
	inline SEOHook::VTable Table;
	namespace GetUserCmd {
		const int index = 8;
		using fn = CUserCmd * (__thiscall*)(IInput*, int);
		CUserCmd* __stdcall Hook(int sequence_number);
	}
}