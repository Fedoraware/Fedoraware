//
//  imgui_color_gradient.cpp
//  imgui extension
//
//  Created by David Gallardo on 11/06/16.


#include "imgui_color_gradient.h"
#include "imgui_internal.h"

static constexpr float GRADIENT_BAR_WIDGET_HEIGHT = 25;
static constexpr float GRADIENT_BAR_EDITOR_HEIGHT = 40;
static constexpr float GRADIENT_MARK_DELETE_DIFFY = 40;

ImGradient::ImGradient()
{
	AddMark(0.0f, ImColor(0.0f, 0.0f, 0.0f));
	AddMark(1.0f, ImColor(1.0f, 1.0f, 1.0f));
}

ImGradient::~ImGradient() { for (const ImGradientMark* mark : Marks) { delete mark; } }

void ImGradient::AddMark(float position, const ImColor color)
{
	position = ImClamp(position, 0.0f, 1.0f);
	const auto newMark = new ImGradientMark();
	newMark->Position = position;
	newMark->Color = color.Value;

	Marks.push_back(newMark);

	RefreshCache();
}

void ImGradient::RemoveMark(ImGradientMark* mark)
{
	Marks.remove(mark);
	RefreshCache();
}

void ImGradient::ClearMarks()
{
	Marks.clear();
}

void ImGradient::GetColorAt(float position, ImVec4* color) const
{
	position = ImClamp(position, 0.0f, 1.0f);
	const int cachePos = (position * 255);
	color->x = CachedValues[cachePos].x;
	color->y = CachedValues[cachePos].y;
	color->z = CachedValues[cachePos].z;
	color->w = CachedValues[cachePos].w;
}

void ImGradient::ComputeColorAt(float position, ImVec4* color) const
{
	position = ImClamp(position, 0.0f, 1.0f);

	ImGradientMark* lower = nullptr;
	ImGradientMark* upper = nullptr;

	for (ImGradientMark* mark : Marks)
	{
		if (mark->Position < position) { if (!lower || lower->Position < mark->Position) { lower = mark; } }

		if (mark->Position >= position) { if (!upper || upper->Position > mark->Position) { upper = mark; } }
	}

	if (upper && !lower) { lower = upper; }
	else if (!upper && lower) { upper = lower; }
	else if (!lower && !upper)
	{
		color->x = color->y = color->z = color->w = 0;
		return;
	}

	if (upper == lower)
	{
		color->x = upper->Color.x;
		color->y = upper->Color.y;
		color->z = upper->Color.z;
		color->w = upper->Color.w;
	}
	else
	{
		const float distance = upper->Position - lower->Position;
		const float delta = (position - lower->Position) / distance;

		// Lerp
		color->x = ((1.0f - delta) * lower->Color.x) + ((delta) * upper->Color.x);
		color->y = ((1.0f - delta) * lower->Color.y) + ((delta) * upper->Color.y);
		color->z = ((1.0f - delta) * lower->Color.z) + ((delta) * upper->Color.z);
		color->w = ((1.0f - delta) * lower->Color.w) + ((delta) * upper->Color.w);
	}
}

void ImGradient::RefreshCache()
{
	Marks.sort([](const ImGradientMark* a, const ImGradientMark* b) { return a->Position < b->Position; });

	for (int i = 0; i < 256; ++i) { ComputeColorAt(i / 255.0f, &CachedValues[i]); }
}


namespace ImGui
{
	void GradientRect(ImGradient* gradient,
	                  const ImVec2& bar_pos,
	                  float maxWidth,
	                  float height)
	{
		const auto drawList = GetWindowDrawList();
		const ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems) { return; }

		ImVec4 colorA = {1, 1, 1, 1};
		ImVec4 colorB = {1, 1, 1, 1};
		float prevX = bar_pos.x;
		const float barBottom = bar_pos.y + height;
		ImGradientMark* prevMark = nullptr;

		if (gradient->GetMarks().empty())
		{
			drawList->AddRectFilled(ImVec2(bar_pos.x, bar_pos.y),
			                        ImVec2(bar_pos.x + maxWidth, barBottom),
			                        ImColor(255, 255, 255, 255));
		}

		ImU32 colorAU32 = 0;
		ImU32 colorBU32 = 0;

		for (const auto mark : gradient->GetMarks())
		{
			const float from = prevX;
			const float to = prevX = bar_pos.x + mark->Position * maxWidth;

			if (prevMark == nullptr)
			{
				colorA.x = mark->Color.x;
				colorA.y = mark->Color.y;
				colorA.z = mark->Color.z;
				colorA.w = mark->Color.w;
			}
			else
			{
				colorA.x = prevMark->Color.x;
				colorA.y = prevMark->Color.y;
				colorA.z = prevMark->Color.z;
				colorA.w = prevMark->Color.w;
			}

			colorB.x = mark->Color.x;
			colorB.y = mark->Color.y;
			colorB.z = mark->Color.z;
			colorB.w = mark->Color.w;

			colorAU32 = ColorConvertFloat4ToU32(colorA);
			colorBU32 = ColorConvertFloat4ToU32(colorB);

			if (mark->Position > 0.f)
			{
				drawList->AddRectFilledMultiColor(ImVec2(from, bar_pos.y),
				                                  ImVec2(to, barBottom),
				                                  colorAU32, colorBU32, colorBU32, colorAU32);
			}

			prevMark = mark;
		}

		if (prevMark && prevMark->Position < 1.f)
		{
			drawList->AddRectFilledMultiColor(ImVec2(prevX, bar_pos.y),
			                                  ImVec2(bar_pos.x + maxWidth, barBottom),
			                                  colorBU32, colorBU32, colorBU32, colorBU32);
		}

		SetCursorScreenPos(ImVec2(bar_pos.x, bar_pos.y + height + 10.0f));
	}

	static void DrawGradientBar(ImGradient* gradient,
	                            const struct ImVec2& bar_pos,
	                            float maxWidth,
	                            float height)
	{
		ImVec4 colorA = {1, 1, 1, 1};
		ImVec4 colorB = {1, 1, 1, 1};
		float prevX = bar_pos.x;
		const float barBottom = bar_pos.y + height;
		ImGradientMark* prevMark = nullptr;
		ImDrawList* drawList = GetWindowDrawList();

		drawList->AddRectFilled(ImVec2(bar_pos.x - 2, bar_pos.y - 2),
		                         ImVec2(bar_pos.x + maxWidth + 2, barBottom + 2),
		                         IM_COL32(100, 100, 100, 255));

		if (gradient->GetMarks().empty())
		{
			drawList->AddRectFilled(ImVec2(bar_pos.x, bar_pos.y),
			                         ImVec2(bar_pos.x + maxWidth, barBottom),
			                         IM_COL32(255, 255, 255, 255));
		}

		ImU32 colorAU32 = 0;
		ImU32 colorBU32 = 0;

		for (const auto mark : gradient->GetMarks())
		{
			const float from = prevX;
			const float to = prevX = bar_pos.x + mark->Position * maxWidth;

			if (prevMark == nullptr)
			{
				colorA.x = mark->Color.x;
				colorA.y = mark->Color.y;
				colorA.z = mark->Color.z;
				colorA.w = mark->Color.w;
			}
			else
			{
				colorA.x = prevMark->Color.x;
				colorA.y = prevMark->Color.y;
				colorA.z = prevMark->Color.z;
				colorA.w = prevMark->Color.w;
			}

			colorB.x = mark->Color.x;
			colorB.y = mark->Color.y;
			colorB.z = mark->Color.z;
			colorB.w = mark->Color.w;

			colorAU32 = ColorConvertFloat4ToU32(colorA);
			colorBU32 = ColorConvertFloat4ToU32(colorB);

			if (mark->Position > 0.f)
			{
				drawList->AddRectFilledMultiColor(ImVec2(from, bar_pos.y),
				                                   ImVec2(to, barBottom),
				                                   colorAU32, colorBU32, colorBU32, colorAU32);
			}

			prevMark = mark;
		}

		if (prevMark && prevMark->Position < 1.f)
		{
			drawList->AddRectFilledMultiColor(ImVec2(prevX, bar_pos.y),
			                                   ImVec2(bar_pos.x + maxWidth, barBottom),
			                                   colorBU32, colorBU32, colorBU32, colorBU32);
		}

		SetCursorScreenPos(ImVec2(bar_pos.x, bar_pos.y + height + 10.0f));
	}

	static void DrawGradientMarks(ImGradient* gradient,
	                              ImGradientMark*& draggingMark,
	                              ImGradientMark*& selectedMark,
	                              const struct ImVec2& bar_pos,
	                              float maxWidth,
	                              float height)
	{
		ImVec4 colorA = {1, 1, 1, 1};
		ImVec4 colorB = {1, 1, 1, 1};
		const float barBottom = bar_pos.y + height;
		ImGradientMark* prevMark = nullptr;
		ImDrawList* drawList = GetWindowDrawList();
		ImU32 colorAU32 = 0;
		ImU32 colorBU32 = 0;

		for (const auto mark : gradient->GetMarks())
		{
			if (!selectedMark) { selectedMark = mark; }

			const float to = bar_pos.x + mark->Position * maxWidth;

			if (prevMark == nullptr)
			{
				colorA.x = mark->Color.x;
				colorA.y = mark->Color.y;
				colorA.z = mark->Color.z;
				colorA.w = mark->Color.w;
			}
			else
			{
				colorA.x = prevMark->Color.x;
				colorA.y = prevMark->Color.y;
				colorA.z = prevMark->Color.z;
				colorA.w = prevMark->Color.w;
			}

			colorB.x = mark->Color.x;
			colorB.y = mark->Color.y;
			colorB.z = mark->Color.z;
			colorB.w = mark->Color.w;

			colorAU32 = ColorConvertFloat4ToU32(colorA);
			colorBU32 = ColorConvertFloat4ToU32(colorB);

			drawList->AddTriangleFilled(ImVec2(to, bar_pos.y + (height - 6)),
			                             ImVec2(to - 6, barBottom),
			                             ImVec2(to + 6, barBottom), IM_COL32(100, 100, 100, 255));

			drawList->AddRectFilled(ImVec2(to - 6, barBottom),
			                         ImVec2(to + 6, bar_pos.y + (height + 12)),
			                         IM_COL32(100, 100, 100, 255), 1.0f, 1.0f);

			drawList->AddRectFilled(ImVec2(to - 5, bar_pos.y + (height + 1)),
			                         ImVec2(to + 5, bar_pos.y + (height + 11)),
			                         IM_COL32(0, 0, 0, 255), 1.0f, 1.0f);

			if (selectedMark == mark)
			{
				drawList->AddTriangleFilled(ImVec2(to, bar_pos.y + (height - 3)),
				                             ImVec2(to - 4, barBottom + 1),
				                             ImVec2(to + 4, barBottom + 1), IM_COL32(0, 255, 0, 255));

				drawList->AddRect(ImVec2(to - 5, bar_pos.y + (height + 1)),
				                   ImVec2(to + 5, bar_pos.y + (height + 11)),
				                   IM_COL32(0, 255, 0, 255), 1.0f, 1.0f);
			}

			drawList->AddRectFilledMultiColor(ImVec2(to - 3, bar_pos.y + (height + 3)),
			                                   ImVec2(to + 3, bar_pos.y + (height + 9)),
			                                   colorBU32, colorBU32, colorBU32, colorBU32);

			SetCursorScreenPos(ImVec2(to - 6, barBottom));
			InvisibleButton("mark", ImVec2(12, 12));

			if (IsItemHovered())
			{
				if (IsMouseClicked(0))
				{
					selectedMark = mark;
					draggingMark = mark;
				}
			}


			prevMark = mark;
		}

		SetCursorScreenPos(ImVec2(bar_pos.x, bar_pos.y + height + 20.0f));
	}

	bool GradientButton(ImGradient* gradient)
	{
		if (!gradient) { return false; }

		const ImVec2 widgetPos = GetCursorScreenPos();
		// ImDrawList* draw_list = GetWindowDrawList();

		const float maxWidth = ImMax(250.0f, GetContentRegionAvail().x - 100.0f);
		const bool clicked = InvisibleButton("gradient_bar", ImVec2(maxWidth, GRADIENT_BAR_WIDGET_HEIGHT));

		DrawGradientBar(gradient, widgetPos, maxWidth, GRADIENT_BAR_WIDGET_HEIGHT);

		return clicked;
	}

	bool GradientEditor(const char* label, ImGradient* gradient,
	                    ImGradientMark*& draggingMark,
	                    ImGradientMark*& selectedMark)
	{
		if (!gradient) { return false; }

		bool modified = false;

		ImVec2 barPos = GetCursorScreenPos();
		barPos.x += 10;
		const float maxWidth = GetContentRegionAvail().x - 20;
		const float barBottom = barPos.y + GRADIENT_BAR_EDITOR_HEIGHT;

		InvisibleButton("gradient_editor_bar", ImVec2(maxWidth, GRADIENT_BAR_EDITOR_HEIGHT));

		if (IsItemHovered() && IsMouseClicked(0))
		{
			const float pos = (GetIO().MousePos.x - barPos.x) / maxWidth;

			// float newMarkCol[4];
			ImVec4 newMarkCol;
			gradient->GetColorAt(pos, &newMarkCol);
			
			gradient->AddMark(pos, ImColor(newMarkCol.x, newMarkCol.y, newMarkCol.z, newMarkCol.w));
		}

		DrawGradientBar(gradient, barPos, maxWidth, GRADIENT_BAR_EDITOR_HEIGHT);
		DrawGradientMarks(gradient, draggingMark, selectedMark, barPos, maxWidth, GRADIENT_BAR_EDITOR_HEIGHT);

		if (!IsMouseDown(0) && draggingMark) { draggingMark = nullptr; }

		if (IsMouseDragging(0) && draggingMark)
		{
			const float increment = GetIO().MouseDelta.x / maxWidth;
			const bool insideZone = (GetIO().MousePos.x > barPos.x) &&
				(GetIO().MousePos.x < barPos.x + maxWidth);

			if (increment != 0.0f && insideZone)
			{
				draggingMark->Position += increment;
				draggingMark->Position = ImClamp(draggingMark->Position, 0.0f, 1.0f);
				gradient->RefreshCache();
				modified = true;
			}

			const float diffY = GetIO().MousePos.y - barBottom;

			if (diffY >= GRADIENT_MARK_DELETE_DIFFY)
			{
				gradient->RemoveMark(draggingMark);
				draggingMark = nullptr;
				selectedMark = nullptr;
				modified = true;
			}
		}

		if (!selectedMark && !gradient->GetMarks().empty()) { selectedMark = gradient->GetMarks().front(); }

		if (selectedMark)
		{
			const bool colorModified = ColorPicker3(label, reinterpret_cast<float*>(&selectedMark->Color));

			if (selectedMark && colorModified)
			{
				modified = true;
				gradient->RefreshCache();
			}
		}

		return modified;
	}
};
