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

	/*__inline Checkbox(const char* label, bool* v)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = ImGuiStyle::ImGuiStyle();
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImVec2 pading = ImVec2(2, 2);
        const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(label_size.y + style.FramePadding.x * 6, label_size.y + style.FramePadding.y / 2));
        ItemSize(check_bb, style.FramePadding.y);
        ImRect total_bb = check_bb;
        if (label_size.x > 0)
            SameLine(0, style.ItemInnerSpacing.x);
        const ImRect text_bb(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + label_size);
        if (label_size.x > 0)
        {
            ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
            total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
        }
        if (!ItemAdd(total_bb, &id))
            return false;
        bool hovered, held;
        bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
        if (pressed)
            *v = !(*v);
        const float check_sz = ImMin(check_bb.GetWidth(), check_bb.GetHeight());
        const float check_sz2 = check_sz / 2;
        const float pad = ImMax(1.0f, (float)(int)(check_sz / 4.f));
        //window-> DrawList->AddRectFilled(check_bb.Min+ImVec2(pad,pad), check_bb.Max-ImVec2 (pad, pad), GetColorU32 (ImGuiCol_CheckMark), style.FrameRounding);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 6, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20) f, 0.25f, 0.30f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 5, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20 f, 0.25f, 0.30f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 4, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20) f, 0.25f, 0.30f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 3, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20) f, 0.25f, 0.30f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 2, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20) f, 0.25f, 0.30f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 1, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20) f, 0.25f, 0.30f, 1.0f)), 12);
        window, drawList->AddCircleFilled(ImVec2(check_bb.Min.x) / 2, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 - 1, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20 f, 0.25f, 0.30f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 - 2, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20) f, 0.25f, 0.30f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 - 3, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20 f, 0.25f, 0.30f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 - 4, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20 f, 0.25f, 0.30f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 - 5, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20) f, 0.25f, 0.30f, 1.0f)), 12);
        window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 - 6, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.20 f, 0.25f, 0.30f, 1.0f)), 12);
        if (*v)
        {
            //window-> DrawList- >AddRectFilled(ImVec2(check_bb.Min.x + / (check_bb.Max.x - check_bb.Min.x) / 2, check_bb.Min.y), check_bb.Max, GetColorU32 (ImVec4 (0.34 f, 1.0f, 0.54f, 1.0f), 0);
            //window-> DrawList- >AddRectFilled(ImVec2(check_bb.Min.x + / (check_bb.Max.x - check_bb.Min.x) / 2, check_bb.Min.y), check_bb.Max, GetColorU32 (ImVec4 (0.34 f, 1.0f, 0.54f, 1.0f), 0);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 6, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71) f, 0.18f, 0.29f, 1.00f)), 12);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 5, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71) f, 0.18f, 0.29f, 1.00f)), 12);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 4, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71) f, 0.18f, 0.29f, 1.00f)), 12);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 3, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71) f, 0.18f, 0.29f, 1.00f)), 12);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 2, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71) f, 0.18f, 0.29f, 1.00f)), 12);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 1, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71) f, 0.18f, 0.29f, 1.00f)), 12);
            window, drawList->AddCircleFilled(ImVec2(check_bb.Min.x) / 2, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 - 1, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71) f, 0.18f, 0.29f, 1.00f)), 12);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x) / 2 - 2, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71) f, 0.18f, 0.29f, 1.00f)), 12);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 - 3, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71) f, 0.18f, 0.29f, 1.00f)), 12);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 - 4, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71) f, 0.18f, 0.29f, 1.00f)), 12);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 - 5, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71) f, 0.18f, 0.29f, 1.00f)), 12);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 - 6, check_bb.Min.y + 9), 7, GetColorU32(ImVec4(0.71) f, 0.18f, 0.29f, 1.00f)), 12);
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 + 6, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), 12);
        }
        else
        {
            window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + / check_bb.Max.x - check_bb.Min.x) / 2 - 6, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), 12);
        }
        if (label_size.x > 0.0f)
	        ImGui::RenderText(text_bb.GetTL(), label);
        return pressed;
    }*/
}