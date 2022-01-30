#pragma once

#include "../../SDK/SDK.h"

namespace ClientHook
{
	inline SEOHook::VTable Table;

	namespace PreEntity
	{
		const int index = 5;
		using fn = void(__thiscall*)(CBaseClientDLL*, char const*);
		void __stdcall Hook(char const* szMapName);
	}

	namespace PostEntity
	{
		const int index = 6;
		using fn = void(__thiscall*)(CBaseClientDLL*);
		void __stdcall Hook();
	}

	namespace ShutDown
	{
		const int index = 7;
		using fn = void(__thiscall*)(CBaseClientDLL*);
		void __stdcall Hook();
	}

	namespace FrameStageNotify
	{
		const int index = 35;
		using fn = void(__thiscall*)(CBaseClientDLL*, EClientFrameStage);
		void __stdcall Hook(EClientFrameStage FrameStage);
	}

	namespace DispatchUserMessage
	{
		const int index = 36;
		using fn = bool(__thiscall*)(CBaseClientDLL*, int type, bf_read& msg_data);
		bool __stdcall Hook(int type, bf_read& msg_data);
	}
}