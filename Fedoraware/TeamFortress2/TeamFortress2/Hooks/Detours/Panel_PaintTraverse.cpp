#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Camera/CameraWindow.h"

MAKE_HOOK(Panel_PaintTraverse, Utils::GetVFuncPtr(I::Panel, 41), void, __stdcall,
		  unsigned int vgui_panel, bool force_repaint, bool allow_force)
{
	g_CameraWindow.Draw();
	if (g_Visuals.RemoveScope(vgui_panel)) { return; }

	// Forgive me father for I have sinned
	Hook.Original<void(__thiscall*)(CPanel*, unsigned int, bool, bool)>()(I::Panel, vgui_panel, force_repaint, allow_force);
}