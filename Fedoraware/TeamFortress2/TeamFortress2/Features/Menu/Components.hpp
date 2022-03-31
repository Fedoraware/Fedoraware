#pragma once
#include "ImGui/imgui_impl_dx9.h"

namespace Cmp
{
	__inline bool SidebarButton(const char* label, bool active = false)
	{
		if (active) { ImGui::PushStyleColor(ImGuiCol_Button, ImColor(38, 38, 38).Value); }
		const bool pressed = ImGui::Button(label, { ImGui::GetWindowSize().x - 2 * ImGui::GetStyle().WindowPadding.x, 44.f });
		if (active) { ImGui::PopStyleColor(); }
		return pressed;
	}
	
}