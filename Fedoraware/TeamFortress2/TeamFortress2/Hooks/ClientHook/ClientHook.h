#pragma once

#include "../../SDK/SDK.h"

namespace ClientHook
{
	inline SEOHook::VTable Table;

	namespace PreEntity
	{
		const int index = 5;
		using fn = void(__thiscall*)(CBaseClientDLL*, const char*);
		void __stdcall Hook(const char* szMapName);
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
		void __stdcall Hook(EClientFrameStage frameStage);
	}

	namespace DispatchUserMessage
	{
		const int index = 36;
		using fn = bool(__thiscall*)(CBaseClientDLL*, UserMessageType type, bf_read& msgData);
		bool __stdcall Hook(UserMessageType type, bf_read& msgData);
	}

	namespace DoPrecipitation
	{
		inline SEOHook::Func Func;
		using fn = void(__thiscall*)(void*, void*);
		void __fastcall Hook(void* ecx, void* edx);
	}

	namespace CHud__FindElement
	{
		//E8 ? ? ? ? C3 53
		inline SEOHook::Func Func;
		using fn = void(__thiscall*)(void*, void*, const char*);
		void __fastcall Hook(void* ecx, void* edx, const char* String2);
	}


}
