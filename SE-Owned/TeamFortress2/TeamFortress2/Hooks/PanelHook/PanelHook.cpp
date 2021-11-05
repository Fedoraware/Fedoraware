#include "PanelHook.h"

#include "../../Features/Visuals/Visuals.h"

void __stdcall PanelHook::PaintTraverse::Hook(unsigned int vgui_panel, bool force_repaint, bool allow_force)
{
	if (g_Visuals.RemoveScope(vgui_panel))
		return;

	Table.Original<fn>(index)(g_Interfaces.Panel, vgui_panel, force_repaint, allow_force);
}