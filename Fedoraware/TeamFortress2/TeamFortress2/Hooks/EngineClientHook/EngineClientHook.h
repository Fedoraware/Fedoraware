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

	namespace ClientCmd_Unrestricted
	{
		const int index = 106;
		inline SEOHook::Func Func;
		using fn = void(__fastcall*)(void*, void*, const char*);
		void __fastcall Hook(void* ecx, void* edx, const char* szCmdString);
	}
}
