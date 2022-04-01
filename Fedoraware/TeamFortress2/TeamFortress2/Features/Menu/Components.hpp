#pragma once

namespace ImGui
{
	__inline bool SidebarButton(const char* label, bool active = false)
	{
		if (active) { PushStyleColor(ImGuiCol_Button, ImColor(38, 38, 38).Value); }
		const bool pressed = Button(label, { GetWindowSize().x - 2 * GetStyle().WindowPadding.x, 44.f });
		if (active) { PopStyleColor(); }
		return pressed;
	}

    /* Container for feature groups */
	__inline bool BeginContainer(const char* str_id)
	{
		PushStyleColor(ImGuiCol_Text, ImColor(13, 13, 13).Value);
		const bool open = CollapsingHeader(str_id, ImGuiTreeNodeFlags_DefaultOpen);
		PopStyleColor();
		return open;
	}

    // Source: https://github.com/ocornut/imgui/issues/1537#issuecomment-355569554
	__inline void ToggleButton(const char* str_id, bool* v)
    {
	    const auto p = GetCursorScreenPos();
        auto* drawList = GetWindowDrawList();
	    const auto style = GetStyle();

        const float height = GetFrameHeight();
        const float width = height * 1.8f;
        const float radius = height * 0.50f;
        const ImVec2 labelSize = CalcTextSize(str_id, nullptr, true);

        InvisibleButton(str_id, ImVec2(width + style.ItemInnerSpacing.x + labelSize.x, height));
        if (IsItemClicked()) { *v = !*v; }

        float t = *v ? 1.0f : 0.0f;

        ImGuiContext& g = *GImGui;
        constexpr float ANIM_SPEED = 0.08f;
        if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
        {
	        const float tAnim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
            t = *v ? (tAnim) : (1.0f - tAnim);
        }

        const ImU32 colBg = IsItemHovered() ? ImColor(60, 60, 60) : ImColor(50, 50, 50);
        const ImU32 colCircle = (*v) ? g_Menu.Accent : ImColor(180, 180, 180);

        drawList->AddRectFilled(p, ImVec2(p.x + width, p.y + height), colBg, height * 0.5f);
        drawList->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, colCircle);
        drawList->AddText({ p.x + width + style.ItemInnerSpacing.x, p.y + (height / 2 - labelSize.y / 2) }, ImColor(255, 255, 255), str_id);
    }
}
