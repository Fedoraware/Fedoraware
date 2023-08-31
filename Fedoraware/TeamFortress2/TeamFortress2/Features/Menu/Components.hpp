#pragma once
#include <ranges>
#include <ImGui/imgui_internal.h>

inline std::bitset<255> VALID_KEYS;

namespace ImGui
{
	/* Color_t to ImVec4 */
	inline ImVec4 ColorToVec(Color_t color)
	{
		return { Color::TOFLOAT(color.r), Color::TOFLOAT(color.g), Color::TOFLOAT(color.b), Color::TOFLOAT(color.a) };
	}

	/* ImVec4 to Color_t */
	inline Color_t VecToColor(const ImVec4& color)
	{
		return {
			static_cast<byte>(color.x * 256.0f > 255 ? 255 : color.x * 256.0f),
			static_cast<byte>(color.y * 256.0f > 255 ? 255 : color.y * 256.0f),
			static_cast<byte>(color.z * 256.0f > 255 ? 255 : color.z * 256.0f),
			static_cast<byte>(color.w * 256.0f > 255 ? 255 : color.w * 256.0f)
		};
	}

	__inline void HelpMarker(const char* desc)
	{
		if (IsItemHovered())
		{
			if (Vars::Menu::ModernDesign)
			{
				SetTooltip("%s", desc);
			}
			else
			{
				F::Menu.FeatureHint = desc;
			}
		}
	}

	__inline bool IconButton(const char* icon)
	{
		PushFont(F::Menu.IconFont);
		TextUnformatted(icon);
		const bool pressed = IsItemClicked();
		PopFont();
		return pressed;
	}

	__inline void SectionTitle(const char* title, float yOffset = 0)
	{
		Dummy({ 0, yOffset });
		PushFont(F::Menu.SectionFont);
		const ImVec2 titleSize = CalcTextSize(title);
		SetCursorPosX((GetWindowSize().x - titleSize.x) * .5f);
		Text(title);
		PopFont();

		const auto widgetPos = GetCursorScreenPos();
		GradientRect(&F::Menu.MainGradient, { widgetPos.x, widgetPos.y - 2 }, GetColumnWidth(), 3);
	}

	__inline bool TableColumnChild(const char* str_id)
	{
		TableNextColumn();
		float contentHeight = GetWindowHeight() - (F::Menu.TabHeight + GetStyle().ItemInnerSpacing.y);
		return BeginChild(str_id, { GetColumnWidth(), contentHeight }, !Vars::Menu::ModernDesign);
	}

	__inline bool SidebarButton(const char* label, bool active = false)
	{
		if (active) { PushStyleColor(ImGuiCol_Button, ImColor(38, 38, 38).Value); }
		const bool pressed = Button(label, { GetWindowSize().x - 2 * GetStyle().WindowPadding.x, 44.f });
		if (active) { PopStyleColor(); }
		return pressed;
	}

	__inline bool TabButton(const char* label, bool active = false)
	{
		TableNextColumn();
		if (active) { PushStyleColor(ImGuiCol_Button, GetColorU32(ImGuiCol_ButtonActive)); }
		const bool pressed = Button(label, { GetColumnWidth(), F::Menu.TabHeight });
		if (active)
		{
			PopStyleColor();

			const auto widgetPos = GetCursorScreenPos();
			GradientRect(&F::Menu.TabGradient, { widgetPos.x, widgetPos.y - 3 }, GetColumnWidth(), 3);
		}
		return pressed;
	}

	__inline bool InputKeybind(const char* label, CVar<int>& output, bool bAllowNone = true, bool bAllowSpecial = false)
	{
		// Init the valid keys bitset
		static std::once_flag initFlag;
		std::call_once(initFlag, [] {
			for (short i = 0x00; i <= 0x07; i++) { VALID_KEYS[i] = true; }
			for (short i = 0x30; i <= 0x5A; i++) { VALID_KEYS[i] = true; } // 0 - 9
			for (short i = 0x41; i <= 0x5A; i++) { VALID_KEYS[i] = true; } // A - Z

			VALID_KEYS[VK_SHIFT] = true;
			VALID_KEYS[VK_CONTROL] = true;
			VALID_KEYS[VK_MENU] = true;
			VALID_KEYS[VK_PRIOR] = true;
			VALID_KEYS[VK_NEXT] = true;
			VALID_KEYS[VK_END] = true;
			VALID_KEYS[VK_HOME] = true;
			VALID_KEYS[VK_LSHIFT] = true;
			VALID_KEYS[VK_RSHIFT] = true;
			VALID_KEYS[VK_DELETE] = true;
			VALID_KEYS[VK_BACK] = true;
		});

		auto VK2STR = [&](const int key) -> const char*
		{
			switch (key)
			{
				case VK_LBUTTON: return "LMB";
				case VK_RBUTTON: return "RMB";
				case VK_MBUTTON: return "MMB";
				case VK_XBUTTON1: return "Mouse4";
				case VK_XBUTTON2: return "Mouse5";
				case VK_BACK: return "Back";
				case VK_SPACE: return "Space";
				case 0x0: return "None";
				case VK_A: return "A";
				case VK_B: return "B";
				case VK_C: return "C";
				case VK_D: return "D";
				case VK_E: return "E";
				case VK_F: return "F";
				case VK_G: return "G";
				case VK_H: return "H";
				case VK_I: return "I";
				case VK_J: return "J";
				case VK_K: return "K";
				case VK_L: return "L";
				case VK_M: return "M";
				case VK_N: return "N";
				case VK_O: return "O";
				case VK_P: return "P";
				case VK_Q: return "Q";
				case VK_R: return "R";
				case VK_S: return "S";
				case VK_T: return "T";
				case VK_U: return "U";
				case VK_V: return "V";
				case VK_W: return "W";
				case VK_X: return "X";
				case VK_Y: return "Y";
				case VK_Z: return "Z";
				case VK_0: return "0";
				case VK_1: return "1";
				case VK_2: return "2";
				case VK_3: return "3";
				case VK_4: return "4";
				case VK_5: return "5";
				case VK_6: return "6";
				case VK_7: return "7";
				case VK_8: return "8";
				case VK_9: return "9";
				case VK_ESCAPE: return "Escape";
				case VK_SHIFT: return "Shift";
				case VK_LSHIFT: return "LShift";
				case VK_RSHIFT: return "RShift";
				case VK_CONTROL: return "Control";
				case VK_MENU: return "LAlt";
				case VK_PRIOR: return "Page Up";
				case VK_NEXT: return "Page Down";
				case VK_INSERT: return "Insert";
				case VK_DELETE: return "Delete";
				case VK_HOME: return "Home";
				case VK_END: return "End";
				case VK_F9: return "F9";
				case VK_F10: return "F10";
				case VK_F11: return "F11";
				case VK_F12: return "F12";
				default: break;
			}

			CHAR buffer[16] = {};
			const int result = GetKeyNameTextA(MapVirtualKeyW(key, MAPVK_VK_TO_VSC) << 16, buffer, 16);
			if (result != 0)
			{
				return buffer;
			}

			return nullptr;
		};

		const auto id = GetID(label);
		PushID(label);

		if (GetActiveID() == id)
		{
			// Active
			SetNextItemAllowOverlap();
			Button("...", ImVec2(100, 0));

			if (!IsItemHovered() && IsMouseClicked(ImGuiMouseButton_Left))
			{
				// Keybinding canceled
				ClearActiveID();
			}
			else if (IsKeyPressed(ImGuiKey_Escape) && bAllowNone)
			{
				// Keybinding escaped
				output.Value = 0;
				ClearActiveID();
			}
			else
			{
				SetActiveID(id, GetCurrentWindow());

				// Get the pressed key
				for (short key = 0; key < 255; key++)
				{
					if (!(GetAsyncKeyState(key) & 0x8000)) { continue; }
					if (!VALID_KEYS[key]) { continue; }
					if (!bAllowSpecial && (key == VK_INSERT || key == VK_F9 || key == VK_F10 || key == VK_F12)) { continue; }

					output.Value = key;
					ClearActiveID();
					break;
				}
			}
		}
		else
		{
			// Inactive
			const char* keyName = VK2STR(output.Value);
			if (Button(keyName ? keyName : "Unknown", ImVec2(100, 0)))
			{
				SetActiveID(id, GetCurrentWindow());
			}
		}

		SameLine();
		Text("%s", label);

		PopID();
		return true;
	}

	/* Combobox for custom materials */
	__inline bool MaterialCombo(const char* label, std::string* current_mat, ImGuiComboFlags flags = 0)
	{
		bool active = false;
		PushItemWidth(F::Menu.ItemWidth);
		if (BeginCombo(label, current_mat->c_str(), flags))
		{
			for (const auto& name : F::MaterialEditor.MaterialMap | std::views::keys)
			{
				if (Selectable(name.c_str(), name == *current_mat))
				{
					*current_mat = name;
					active = true;
				}
			}

			EndCombo();
		}
		PopItemWidth();

		return active;
	}

	/* Combobox with multiple selectable items */
	__inline void MultiCombo(std::vector<const char*> titles, const std::vector<bool*>& options,
							 const std::string& comboName)
	{
		if (titles.size() != options.size()) { return; }

		std::string preview = "<None>##";
		for (size_t i = 0; i < options.size(); i++)
		{
			if (*options[i])
			{
				if (preview == "<None>##") { preview = ""; }
				preview += titles[i];
				preview.append(", ");
			}
		}
		preview.pop_back(); preview.pop_back(); // This is a stupid but easy way to remove the last comma

		PushItemWidth(F::Menu.ItemWidth);
		if (BeginCombo(comboName.c_str(), preview.c_str()))
		{
			for (size_t i = 0; i < titles.size(); i++)
			{
				Selectable((*options[i]) ? std::format("+ {}", titles[i]).c_str() : titles[i], options[i], ImGuiSelectableFlags_DontClosePopups);
			}

			EndCombo();
		}
		PopItemWidth();
	}

	__inline void MultiFlags(std::vector<const char*> flagNames, const std::vector<int>& flagValues, int* flagVar, const std::string& comboName)
	{
		if (flagNames.size() != flagValues.size()) { return; }

		std::string preview = "<Default>##";
		if (*flagVar == 0)
		{
			preview = "<None>##";
		}
		else
		{
			for (size_t i = 0; i < flagValues.size(); i++)
			{
				if (*flagVar & flagValues[i])
				{
					if (preview == "<Default>##") { preview = ""; }
					preview += flagNames[i];
					preview.append(", ");
				}
			}
			preview.pop_back(); preview.pop_back();
		}

		PushItemWidth(F::Menu.ItemWidth);
		if (BeginCombo(comboName.c_str(), preview.c_str()))
		{
			for (size_t i = 0; i < flagNames.size(); i++)
			{
				const bool flagActive = *flagVar & flagValues[i];
				if (Selectable(flagActive ? std::format("+ {}", flagNames[i]).c_str() : flagNames[i], flagActive, ImGuiSelectableFlags_DontClosePopups))
				{
					if (flagActive)
					{
						*flagVar &= ~flagValues[i];
					}
					else
					{
						*flagVar |= flagValues[i];
					}
				}
			}

			EndCombo();
		}
		PopItemWidth();
	}

	__inline bool ColorPicker(const char* label, Color_t& color)
	{
		bool open = false;
		ImVec4 tempColor = ColorToVec(color);
		PushItemWidth(F::Menu.ItemWidth);
		if (ColorEdit4(label, &tempColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
		{
			color = VecToColor(tempColor);
			open = true;
		}
		PopItemWidth();
		HelpMarker(label);
		return open;
	}

	/* Inline color picker */
	__inline bool ColorPickerL(const char* label, Color_t& color, int num = 0)
	{
		SameLine(GetContentRegionMax().x - 20 - (num * 24));
		SetNextItemWidth(20);
		return ColorPicker(label, color);
	}

	__inline void TextCentered(const char* fmt)
	{
		const auto windowWidth = GetWindowSize().x;
		const auto textWidth = CalcTextSize(fmt).x;

		SetCursorPosX((windowWidth - textWidth) * 0.5f);
		Text("%s", fmt);
	}

	// Source: https://github.com/ocornut/imgui/issues/1537#issuecomment-355569554
	__inline bool ToggleButton(const char* label, bool* v)
	{
		const auto p = GetCursorScreenPos();
		auto* drawList = GetWindowDrawList();
		const auto& style = GetStyle();

		const float height = GetFrameHeight();
		const float width = height * 1.8f;
		const float radius = height * 0.50f;
		const ImVec2 labelSize = CalcTextSize(label, nullptr, true);

		InvisibleButton(label, ImVec2(width + style.ItemInnerSpacing.x + labelSize.x, height));
		if (IsItemClicked()) { *v = !*v; }

		float t = *v ? 1.0f : 0.0f;

		const ImGuiContext& g = *GImGui;
		constexpr float ANIM_SPEED = 0.08f;
		if (g.LastActiveId == g.CurrentWindow->GetID(label))
		{
			const float tAnim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (tAnim) : (1.0f - tAnim);
		}

		const ImU32 colBg = IsItemHovered() ? ImColor(60, 60, 60) : ImColor(50, 50, 50);
		const ImU32 colCircle = (*v) ? F::Menu.Accent : ImColor(180, 180, 180);

		drawList->AddRectFilled(p, ImVec2(p.x + width, p.y + height), colBg, height * 0.5f);
		drawList->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, colCircle);
		RenderText({ p.x + width + style.ItemInnerSpacing.x, p.y + (height / 2 - labelSize.y / 2) }, label);

		return *v;
	}

#pragma region Width Components
	__inline bool WCombo(const char* label, int* current_item, std::vector<const char*> items)
	{
		SetNextItemWidth(F::Menu.ItemWidth);
		return Combo(label, current_item, items.data(), items.size(), -1);
	}

	__inline bool WSlider(const char* label, float* v, float v_min, float v_max, const char* format = "%.2f", ImGuiSliderFlags flags = 0)
	{
		SetNextItemWidth(F::Menu.ItemWidth);
		return SliderFloat(label, v, v_min, v_max, format, flags);
	}

	__inline bool WSlider(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0)
	{
		SetNextItemWidth(F::Menu.ItemWidth);
		return SliderInt(label, v, v_min, v_max, format, flags);
	}

	__inline bool WInputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr)
	{
		SetNextItemWidth(F::Menu.ItemWidth);
		return InputText(label, str, flags, callback, user_data);
	}

	__inline bool WInputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL)
	{
		SetNextItemWidth(F::Menu.ItemWidth);
		return InputTextWithHint(label, hint, str, flags, callback, user_data);
	}

	__inline bool WInputInt(const char* label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0)
	{
		SetNextItemWidth(F::Menu.ItemWidth);
		return InputInt(label, v, step, step_fast, flags);
	}

	__inline bool WToggle(const char* label, bool* v)
	{
		bool result;
		if (Vars::Menu::ModernDesign)
		{
			result = ToggleButton(label, v);
		}
		else
		{
			result = Checkbox(label, v);
		}
		return result;
	}
#pragma endregion
}
