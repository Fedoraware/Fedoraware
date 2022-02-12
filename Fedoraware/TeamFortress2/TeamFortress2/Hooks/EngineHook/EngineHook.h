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

	namespace CL_ReadPackets
	{
		inline SEOHook::Func Func;
		using fn = void(__cdecl*)(bool bFinalTick);
		void __cdecl Hook(bool bFinalTick);
	}
}

class CPingReducer
{
public:

	struct ReadPacketsStore
	{
		float Frametime = 0.0f;
		float CsFrametime = 0.0f;
		float Curtime = 0.0f;
		int   TickCount = 0;

		void Setup();
		void Apply() const;
	};

	ReadPacketsStore OnReadPacket;

	bool ShouldCallReadPackets();

	void FixInputDelay(bool a);

	using ReadPacketFn = void(__cdecl*)(bool);
	ReadPacketFn oReadPackets;
};

inline CPingReducer g_PingReducer;