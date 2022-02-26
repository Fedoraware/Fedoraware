#pragma once

#include "../../SDK/SDK.h"

namespace ViewRenderHook
{
	inline SEOHook::VTable Table;

	namespace LevelInit
	{
		const int index = 1;
		inline SEOHook::Func Func;
		using fn = void(__thiscall*)(void*);
		void __fastcall Hook(void* ecx, void* edx);
	}

	namespace LevelShutdown
	{
		const int index = 2;
		inline SEOHook::Func Func;
		using fn = void(__thiscall*)(void*);
		void __fastcall Hook(void* ecx, void* edx);
	}

	namespace RenderView
	{
		const int index = 6;
		inline SEOHook::Func Func;
		using fn = void(__thiscall*)(void*, const CViewSetup& view, int nClearFlags, int whatToDraw);
		void __fastcall Hook(void* ecx, void* edx, const CViewSetup& view, ClearFlags_t nClearFlags, RenderViewInfo_t whatToDraw);
	}
}
