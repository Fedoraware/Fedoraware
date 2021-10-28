#pragma once

#include "../../SDK/SDK.h"

namespace ViewRenderHook
{
	inline SEOHook::VTable Table;

	namespace LevelInit
	{
		const int index = 1;
		using fn = void(__stdcall*)(IViewRender*);
		void __stdcall Hook();
	}

	namespace LevelShutdown
	{
		const int index = 2;
		using fn = void(__stdcall*)(IViewRender*);
		void __stdcall Hook();
	}
}