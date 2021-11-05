#pragma once

#include "../../SDK/SDK.h"

namespace PanelHook
{
	inline SEOHook::VTable Table;

	namespace PaintTraverse
	{
		const int index = 41;
		using fn = void(__thiscall*)(CPanel*, unsigned int, bool, bool);
		void __stdcall Hook(unsigned int vgui_panel, bool force_repaint, bool allow_force);
	}
}