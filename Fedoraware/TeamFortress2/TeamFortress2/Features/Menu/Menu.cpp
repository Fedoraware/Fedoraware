#include "Menu.h"
#include "../Vars.h"
#include "../Visuals/Visuals.h"
#include "ImGui/imgui_internal.h"
#include "InputHelper/InputHelper.h"
#include "ConfigManager/ConfigManager.h"
#include "ImGui/imgui_stdlib.h"
#include <filesystem>
#include "../AttributeChanger/AttributeChanger.h"
#include "../Glow/Glow.h"
#include "../Chams/Chams.h"
#include "../PlayerList/PlayerList.h"
#include "ImGui/imgui_color_gradient.h"
ImFont* g_pImFontDefaultFont = nullptr;
ImFont* g_pImFontChineseFont = nullptr;
bool tooltips = true;
bool showFonts = false;
void AlignRight(int offset) {
	ImGui::SameLine(ImGui::GetContentRegionMax().x - offset);
	ImGui::SetNextItemWidth(offset);
}

static void HelpMarker(const char* desc)
{
	if (tooltips) {
		//ImGui::SameLine();
		//ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
}

int skyName = 0;

int unu1 = 0;
int unu2 = 0;

bool InputKeybind(const char* label, CVar<int>& output, bool bAllowNone = true)
{
	bool active = false;

	auto VK2STR = [&](const short key) -> const char* {
		switch (key) {
		case VK_LBUTTON: return "Mouse1";
		case VK_RBUTTON: return "Mouse2";
		case VK_MBUTTON: return "Mouse3";
		case VK_XBUTTON1: return "Mouse4";
		case VK_XBUTTON2: return "Mouse5";
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
		case VK_LSHIFT: return "Shift";
		case VK_RSHIFT: return "Shift";
		case VK_CONTROL: return "Control";
		case VK_MENU: return "LAlt";
		default: break;
		}

		WCHAR output[16] = { L"\0" };
		if (const int result = GetKeyNameTextW(MapVirtualKeyW(key, MAPVK_VK_TO_VSC) << 16, output, 16)) {
			char outputt[128];
			sprintf(outputt, "%ws", output);
			return outputt;
		}

		return "VK2STR_FAILED";
	};

	auto labell = VK2STR(output.m_Var);

	const auto id = ImGui::GetID(label);
	ImGui::PushID(label);

	if (ImGui::GetActiveID() == id) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonActive));
		ImGui::Button("...", ImVec2(100, 17));
		ImGui::PopStyleColor();

		static float time = g_Interfaces.Engine->Time();
		float elapsed = g_Interfaces.Engine->Time() - time;
		static CVar<int>* curr = nullptr, * prevv = curr;
		if (curr != prevv) {
			time = g_Interfaces.Engine->Time();
			prevv = curr;
		}

		if (curr == nullptr && elapsed > 0.1f) {
			for (short n = 0; n < 256; n++) {
				if ((n > 0x0 && n < 0x7) || (n > L'A' - 1 && n < L'Z' + 1) || (n > L'0' - 1 && n < L'9' + 1) || n == VK_LSHIFT || n == VK_RSHIFT || n == VK_SHIFT || n == VK_ESCAPE || n == VK_HOME || n == VK_CONTROL || n == VK_MENU) {
					if ((!ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[0])) {
						ImGui::ClearActiveID();
						break;
					}
					if (GetAsyncKeyState(n) & 0x8000)
					{
						if (n == VK_HOME || n == VK_INSERT) {
							break;
						}

						if (n == VK_ESCAPE && bAllowNone) {
							ImGui::ClearActiveID();
							output.m_Var = 0x0;
							break;
						}

						output.m_Var = n;
						ImGui::ClearActiveID();
						break;
					}
				} //loop
			}
		}

		if (curr != prevv) {
			time = g_Interfaces.Engine->Time();
			prevv = curr;
		}

		ImGui::GetCurrentContext()->ActiveIdAllowOverlap = true;
		if ((!ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[0]))
			ImGui::ClearActiveID();
	}
	else if (ImGui::Button(VK2STR(output.m_Var), ImVec2(50, 20))) {
		ImGui::SetActiveID(id, ImGui::GetCurrentWindow());
	}

	ImGui::SameLine();
	ImGui::TextUnformatted(label);
	ImGui::PopID();

	return true;
}

void combo(const char* label, int* current_item, const char* const* items, int items_count, int popup_max_height_in_items = -1) {
	ImGui::PushItemWidth(100);
	ImGui::Combo(label, current_item, items, items_count, popup_max_height_in_items);
	ImGui::PopItemWidth();
}

ImVec4 to_vec4(float r, float g, float b, float a)
{
	return ImVec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}

int SettingsTab = 0;

ImVec4 mColor(Color_t color) {
	return ImVec4(Color::TOFLOAT(color.r), Color::TOFLOAT(color.g), Color::TOFLOAT(color.b), Color::TOFLOAT(color.a));
}

Color_t vColor(ImVec4 color) {
	return {
		(byte)(color.x * 256.0f > 255 ? 255 : color.x * 256.0f),
		(byte)(color.y * 256.0f > 255 ? 255 : color.y * 256.0f),
		(byte)(color.z * 256.0f > 255 ? 255 : color.z * 256.0f),
		(byte)(color.w * 256.0f > 255 ? 255 : color.w * 256.0f)
	};
}

void ColorPicker(const char* label, Color_t& color) {
	ImVec4 FUCKOFF = mColor(color);
	ImGui::PushItemWidth(150);
	if (ImGui::ColorEdit4(label, &FUCKOFF.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
		color = vColor(FUCKOFF);
	}
	ImGui::PopItemWidth();
}

Color_t* vpColor(ImVec4 color) {
	Color_t col = { (byte)(color.x * 255.0f),
		(byte)(color.y * 255.0f),
		(byte)(color.z * 255.0f),
		(byte)(color.w * 255.0f) };
	return &col;
}

float* cColor(ImVec4 color, Color_t& out) {
	out = *vpColor(color);

	return &color.x;
}

// Redo the Nitro DT bar
//void CMenu::TextCenter(std::string text) {
//	ImGui::PushFont(DT);
//	float font_size = ImGui::CalcTextSize(text.c_str()).x;
//	//float font_size = ImGui::GetFontSize() * text.size() / 2;
//	ImGui::SameLine(
//		ImGui::GetWindowSize().x / 2 -
//		font_size + (font_size / 2)
//	);
//
//	ImGui::Text(text.c_str());
//	ImGui::PopFont();
//}



void CMenu::Run() {
	m_bReopened = false;

	static bool bOldOpen = m_bOpen;

	if (bOldOpen != m_bOpen)
	{
		bOldOpen = m_bOpen;

		if (m_bOpen)
			m_bReopened = true;
	}

	flTimeOnChange = 0.0f;

	if (Utils::IsGameWindowInFocus() && (GetAsyncKeyState(VK_HOME) & 1)) {
		flTimeOnChange = g_Interfaces.Engine->Time();
	}
	m_flFadeElapsed = g_Interfaces.Engine->Time() - flTimeOnChange;

	if (m_flFadeElapsed < m_flFadeDuration) {
		m_flFadeAlpha = Math::RemapValClamped(m_flFadeElapsed, 0.0f, m_flFadeDuration, !m_bOpen ? 1.0f : 0.0f, m_bOpen ? 1.0f : 0.0f);
		g_Interfaces.Surface->DrawSetAlphaMultiplier(m_flFadeAlpha);
	}

	g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);
}

//ImGui::SliderFloat("Aimbot FOV", &Vars::Aimbot::Global::AimFOV.m_Var, 0.f, 180.f, "%f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_)

#define WidthSlider(label, var, min, max, format, flagspower) \
ImGui::PushItemWidth(100); \
ImGui::SliderFloat(label, var, min, max, format, flagspower); \
ImGui::PopItemWidth()

void CMenu::Render(IDirect3DDevice9* pDevice) {
	static bool bInitImGui = false;
	static bool bColumnsWidthened = false;
	bool modified_custom_style = false;

	if (!config)
		return;

	if (!bInitImGui) {
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(FindWindowA(0, "Team Fortress 2"));
		ImGui_ImplDX9_Init(pDevice);

		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x0400, 0x044F, // Cyrillic
			0,
		};

		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowMinSize = ImVec2(708, 708);
		style.WindowPadding = ImVec2(0, 0);
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.WindowBorderSize = 0.f;
		style.PopupBorderSize = 1.f;
		style.FrameBorderSize = 0.f;
		style.FrameRounding = 0;
		style.ScrollbarSize = 3.f;

		auto fontConfig = ImFontConfig();
		fontConfig.OversampleH = 1;
		fontConfig.OversampleV = 1;
		fontConfig.PixelSnapH = true;

		VerdanaNormal = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdana.ttf", 14.0f, &fontConfig, io.Fonts->GetGlyphRangesCyrillic());
		VerdanaSmall = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdana.ttf", 12.0f, &fontConfig, io.Fonts->GetGlyphRangesCyrillic());
		VerdanaBold = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdanab.ttf", 18.0f, &fontConfig, io.Fonts->GetGlyphRangesCyrillic());

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.31f, 0.32f, 0.35f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.47f, 0.48f, 0.53f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.40f, 0.40f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.40f, 0.40f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.32f, 0.31f, 0.34f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.27f, 0.27f, 0.30f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.32f, 0.31f, 0.34f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.27f, 0.27f, 0.30f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.32f, 0.31f, 0.34f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.27f, 0.27f, 0.30f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.32f, 0.31f, 0.34f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 0.40f, 0.40f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.27f, 0.27f, 0.30f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.32f, 0.31f, 0.34f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.27f, 0.27f, 0.30f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(1.00f, 0.40f, 0.40f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		bInitImGui = true;

		//ImGui::SetColorEditOptions(ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_Uint8);
		//ImGuiStyle* style = &ImGui::GetStyle();
		//auto& io = ImGui::GetIO();
		//ImVec4* colors = ImGui::GetStyle().Colors;
		//colors[ImGuiCol_Text] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
		//colors[ImGuiCol_TextDisabled] = ImVec4(0.56f, 0.41f, 0.04f, 1.00f);
		//colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.05f, 0.08f, 1.00f);
		//colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		//colors[ImGuiCol_PopupBg] = ImVec4(0.04f, 0.05f, 0.08f, 1.00f);
		//colors[ImGuiCol_Border] = ImVec4(1.00f, 0.00f, 0.00f, 0.29f);
		//colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_FrameBg] = ImVec4(0.09f, 0.09f, 0.09f, 0.23f);
		//colors[ImGuiCol_FrameBgHovered] = ImVec4(0.04f, 0.17f, 0.21f, 0.54f);
		//colors[ImGuiCol_FrameBgActive] = ImVec4(0.13f, 0.48f, 0.65f, 1.00f);
		//colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.14f, 0.62f);
		//colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 0.61f);
		//colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.14f, 0.62f);
		//colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		//colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		//colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		//colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		//colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		//colors[ImGuiCol_CheckMark] = ImVec4(0.69f, 0.22f, 0.22f, 1.00f);
		//colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		//colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		//colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		//colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		//colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		//colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		//colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		//colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		//colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		//colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		//colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		//colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		//colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		//colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		//colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		//colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		//colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		//colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		//colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		//colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		//colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		//colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		//colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		//colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		//colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		//colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		//colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		//colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		//colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.10f, 0.20f);
		//style->WindowPadding = ImVec2(0.00f, 2.00f);
		//style->FramePadding = ImVec2(20.00f, 2.00f);
		//style->CellPadding = ImVec2(6.00f, 0.00f);
		//style->ItemSpacing = ImVec2(6.00f, 4.00f);
		//style->ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		//style->TouchExtraPadding = ImVec2(0.00f, 0.00f);
		//style->IndentSpacing = 0;
		//style->ScrollbarSize = 1;
		//style->GrabMinSize = 7;
		//style->WindowBorderSize = 0;
		//style->ChildBorderSize = 1;
		//style->PopupBorderSize = 1;
		//style->FrameBorderSize = 0;
		//style->TabBorderSize = 1;
		//style->WindowRounding = 0;
		//style->ChildRounding = 0;
		//style->FrameRounding = 0;
		//style->PopupRounding = 0;
		//style->ScrollbarRounding = 0;
		//style->GrabRounding = 0;
		//style->LogSliderDeadzone = 0;
		//style->TabRounding = 0;
		//style->WindowTitleAlign = ImVec2(1.0f, 0.5f);
		//style->SelectableTextAlign = ImVec2(0.0f, 0.5f);
		//style->ButtonTextAlign = ImVec2(0.0f, 0.5f);
		//style->DisplaySafeAreaPadding = ImVec2(0, 17);
		//auto m_font_config = ImFontConfig();
		//m_font_config.OversampleH = 1;
		//m_font_config.OversampleV = 1;
		//m_font_config.PixelSnapH = true;
		//drawList = ImGui::GetBackgroundDrawList();
		//Normal = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\tahomabd.ttf", 14.0f, &m_font_config, io.Fonts->GetGlyphRangesCyrillic());
		//DT = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\tahoma.ttf", 14.0f, &m_font_config, io.Fonts->GetGlyphRangesCyrillic());
	}

	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		g_Interfaces.Surface->SetCursorAlwaysVisible(g_Menu.m_bOpen = !g_Menu.m_bOpen);
		menuOpen = !menuOpen;
		g_Menu.flTimeOnChange = g_Interfaces.Engine->Time();

	}
	g_Menu.m_flFadeElapsed = g_Interfaces.Engine->Time() - g_Menu.flTimeOnChange;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	//if ((!g_Interfaces.EngineVGui->IsGameUIVisible() || g_Menu.m_bOpen) && Vars::Misc::CL_Move::DTBarStyle.m_Var == 2) {
	//	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.03, 0.03, 0.03, 0.3));
	//	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
	//	if (ImGui::Begin("Doubletap bar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | (g_Menu.m_bOpen ? 0 : ImGuiWindowFlags_NoDecoration) | (g_Menu.m_bOpen ? 0 : ImGuiWindowFlags_NoTitleBar)))
	//	{
	//		ImGui::SetWindowSize(ImVec2(180, 30));
	//		static std::string dtstring = "";
	//		if (const auto& pLocal = g_EntityCache.m_pLocal) {
	//			dtstring = "Doubletap (" + std::to_string(g_GlobalInfo.m_nShifted) + " / 24)";
	//			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.3f, 0.3f, 1.0f));
	//			if (g_GlobalInfo.m_nShifted >= Vars::Misc::CL_Move::DTTicks.m_Var) {
	//				if (!g_GlobalInfo.m_nWaitForShift) {
	//					ImGui::PopStyleColor();
	//					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.f, 0.3f, 1.0f));
	//				}
	//				else {
	//					ImGui::PopStyleColor();
	//					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.5f, 0.f, 1.0f));
	//				}
	//			}
	//			TextCenter(dtstring);
	//			ImGui::PopStyleColor();
	//		}
	//	}
	//	ImGui::End();
	//	ImGui::PopStyleColor(2);
	//}
	ImGui::GetIO().MouseDrawCursor = menuOpen;
	if (g_Menu.m_bOpen)
	{
		ImColor accent = ImColor(Color::TOFLOAT(Vars::Menu::Colors::MenuAccent.r), Color::TOFLOAT(Vars::Menu::Colors::MenuAccent.g), Color::TOFLOAT(Vars::Menu::Colors::MenuAccent.b));
		ImGui::GetStyle().Colors[ImGuiCol_CheckMark] = accent;
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = accent;
		ImGui::GetStyle().Colors[ImGuiCol_ResizeGrip] = accent;
		ImGui::GetStyle().Colors[ImGuiCol_TabActive] = accent;
		ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg] = accent;
		static ImGradient titlegradient;
		{
			ImColor titlebg = ImGui::GetStyle().Colors[ImGuiCol_TitleBg];
			titlegradient.m_marks.clear();
			titlegradient.addMark(0.0f, titlebg);
			titlegradient.addMark(0.32f, titlebg);
			titlegradient.addMark(0.5f, accent);
			titlegradient.addMark(0.68f, titlebg);
			titlegradient.addMark(0.9999998f, titlebg); // literally why
		}
		static ImGradient hover;
		{
			ImColor buttonhovered = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
			hover.m_marks.clear();
			hover.addMark(0.0f, buttonhovered);
			hover.addMark(0.32f, buttonhovered);
			hover.addMark(0.5f, ImColor(IM_COL32(255, 255, 255, 255)));
			hover.addMark(0.68f, buttonhovered);
			hover.addMark(0.9999998f, buttonhovered); // literally why
		}
		static ImGradient active;
		{
			ImColor buttonactive = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
			active.m_marks.clear();
			active.addMark(0.0f, buttonactive);
			active.addMark(0.32f, buttonactive);
			active.addMark(0.5f, ImColor(IM_COL32(255, 255, 255, 255)));
			active.addMark(0.68f, buttonactive);
			active.addMark(0.9999998f, buttonactive); // literally why
		}
		static ImGradient normal;
		{
			ImColor framenormal = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
			normal.m_marks.clear();
			normal.addMark(0.0f, framenormal);
			normal.addMark(0.32f, framenormal);
			normal.addMark(0.5f, accent/*ImColor(IM_COL32(255, 255, 255, 255))*/);
			normal.addMark(0.68f, framenormal);
			normal.addMark(0.9999998f, framenormal); // literally why
		}
		static int tab1 = 0;
		static int tab2 = 0;
		ImGui::GetStyle().WindowMinSize = ImVec2(708, 708);

		ImGui::PushFont(VerdanaBold); ImGui::PushStyleColor(ImGuiCol_Text, accent.Value); ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		ImGui::Begin("Fedoraware", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar); ImGui::PopFont(); ImGui::PopStyleColor(2); ImGui::PushFont(VerdanaNormal);
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImVec2 winSize = ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
			ImVec2 winPos = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
			const auto drawList = ImGui::GetWindowDrawList();
			const auto bgDrawList = ImGui::GetBackgroundDrawList();
			const auto fgDrawList = window->DrawList;
			const auto foregroundDrawList = ImGui::GetForegroundDrawList();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
			if (ImGui::BeginPopupContextWindow("Settings")) {
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ColorPicker("Menu accent", Vars::Menu::Colors::MenuAccent);
				ImGui::SameLine(); ImGui::Text("Menu accent");
				static std::wstring selected = {};
				int nConfig = 0;

				for (const auto& entry : std::filesystem::directory_iterator(g_CFG.m_sConfigPath)) {
					if (std::string(std::filesystem::path(entry).filename().string()).find(_(".fed")) == std::string_view::npos)
					{
						continue;
					}
					nConfig++;
				}

				if (nConfig < 100) {
					std::string output = {};

					ImGui::PushItemWidth(200);
					if (ImGui::InputTextWithHint("###configname", "New config name", &output, ImGuiInputTextFlags_EnterReturnsTrue)) {
						std::wstring outstring(output.begin(), output.end());
						if (!std::filesystem::exists(g_CFG.m_sConfigPath + L"\\" + outstring)) {
							g_CFG.Save(outstring.c_str());
						}
					}
					ImGui::PopItemWidth();
				}

				for (const auto& entry : std::filesystem::directory_iterator(g_CFG.m_sConfigPath)) {
					if (std::string(std::filesystem::path(entry).filename().string()).find(_(".fed")) == std::string_view::npos) {
						continue;
					}
					std::wstring s = entry.path().filename().wstring();
					s.erase(s.end() - 4, s.end());
					std::string configName(s.begin(), s.end());
					if (s == selected) {
						ImGuiStyle* style2 = &ImGui::GetStyle();
						ImVec4* colors2 = style2->Colors;
						ImVec4 buttonColor = colors2[ImGuiCol_Button];
						buttonColor.w *= 0.5;
						ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
						if (ImGui::Button(configName.c_str(), ImVec2(200, 20))) {
							selected = s;
						}
						ImGui::PopStyleColor();
					}
					else {
						if (ImGui::Button(configName.c_str(), ImVec2(200, 20))) {
							selected = s;
						}
					}
				}
				if (!selected.empty())
				{
					if (ImGui::Button("Save", ImVec2(61, 20))) {
						ImGui::OpenPopup("Save config?");
					}
					ImGui::SameLine();
					if (ImGui::Button("Load", ImVec2(61, 20))) {
						g_CFG.Load(selected.c_str());
						selected.clear();
					}
					ImGui::SameLine();
					if (ImGui::Button("Remove", ImVec2(62, 20))) {
						ImGui::OpenPopup("Remove config?");
					}
					// Save config dialog
					if (ImGui::BeginPopupModal("Save config?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
						ImGui::Text("Do you really want to override this config?\n\n");
						ImGui::Separator();
						if (ImGui::Button("Yes, override!", ImVec2(150, 0))) {
							g_CFG.Save(selected.c_str());
							selected.clear();
							ImGui::CloseCurrentPopup();
						}
						ImGui::SameLine();
						if (ImGui::Button("No", ImVec2(120, 0))) {
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
					// Delete config dialog
					if (ImGui::BeginPopupModal("Remove config?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
						ImGui::Text("Do you really want to delete this config?\n\n");
						ImGui::Separator();
						if (ImGui::Button("Yes, remove!", ImVec2(150, 0))) {
							g_CFG.Remove(selected.c_str());
							selected.clear();
							ImGui::CloseCurrentPopup();
						}
						ImGui::SameLine();
						if (ImGui::Button("No", ImVec2(150, 0))) {
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
				}

				ImGui::EndPopup();
			}
			else {
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}
			//if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			//	ImGui::BeginPopup("Settings");
			//	{
			//		ColorPicker("Menu accent", Vars::Menu::Colors::MenuAccent);
			//		ImGui::SameLine(); ImGui::Text("Menu accent");
			//		ImGui::EndPopup();
			//	}
			//}


			std::vector < Font_t > fonts;
			static const char* flags[]{ "None","Italic","Underline","Strikeout","Symbol",	"Antialias",	"Gaussian",	"Rotary",	"Dropshadow","Additive",	"Outline","Custom" };
			static int fontflags[]{ 0x000,0x001,0x002,0x004,0x008,0x010,0x020,0x040,0x080,0x100,0x200,0x400 };

			ImGui::GradientRect(foregroundDrawList, &titlegradient, { winPos.x, winPos.y }, winSize.x, 3);
			ImGui::Dummy(ImVec2(0, 7));

			const float half = winSize.x / 2;
			const float third = winSize.x / 3;
			const float quarter = winSize.x / 4;
			const float fifth = winSize.x / 5;
			const float sixth = winSize.x / 6;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.1f));
			{
				bool pushed1 = false;

				// Tab 1
				{
					if (tab1 == 0) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushed1 = true;
					}
					else {
						pushed1 = false;
					}
					if (ImGui::Button("Aimbot", ImVec2(quarter, 27))) {
						tab1 = 0;
					}
					if (tab1 == 0) {
						if (pushed1) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;

					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x, b1.y - 3 }, quarter, 3);
					}
					else if (tab1 == 0) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, quarter, 3);
					}
					ImGui::SameLine();
				}

				bool pushed2 = false;

				// Tab 2
				{
					if (tab1 == 1) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushed2 = true;
					}
					else {
						pushed2 = false;
					}
					if (ImGui::Button("Triggerbot", ImVec2(quarter, 27))) {
						tab1 = 1;
					}
					if (tab1 == 1) {
						if (pushed2) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b2 = window->DC.CursorPos;
					b2.x += quarter;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b2.x, b2.y - 3 }, quarter, 3);
					}
					else if (tab1 == 1) {
						ImGui::GradientRect(fgDrawList, &active, { b2.x, b2.y - 3 }, quarter, 3);
					}
					ImGui::SameLine();
				}

				bool pushed3 = false;

				// Tab 3
				{
					if (tab1 == 2) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushed3 = true;
					}
					else {
						pushed3 = false;
					}
					if (ImGui::Button("Visuals", ImVec2(quarter, 27))) {
						tab1 = 2;
					}
					if (tab1 == 2) {
						if (pushed3) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b3 = window->DC.CursorPos;
					b3.x += quarter * 2;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b3.x, b3.y - 3 }, quarter, 3);
					}
					else if (tab1 == 2) {
						ImGui::GradientRect(fgDrawList, &active, { b3.x, b3.y - 3 }, quarter, 3);
					}
					ImGui::SameLine();
				}

				bool pushed4 = false;

				// Tab4
				{
					if (tab1 == 3) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushed4 = true;
					}
					else {
						pushed4 = false;
					}
					if (ImGui::Button("Misc", ImVec2(quarter, 27))) {
						tab1 = 3;
					}
					if (tab1 == 3) {
						if (pushed4) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					b1.x += quarter * 3;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, quarter, 3);
					}
					else if (tab1 == 3) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, quarter, 3);
					}
					ImGui::SameLine();
				}
			}
			ImGui::Dummy(ImVec2(0, 0));

			if (tab1 == 2) {
				{
					bool pushedtab20 = false;
					if (tab2 == 0) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedtab20 = true;
					}
					else {
						pushedtab20 = false;
					}
					if (ImGui::Button("Players", ImVec2(sixth, 27))) {
						tab2 = 0;
					}
					if (tab2 == 0) {
						if (pushedtab20) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					//b1.x += fifth * 3;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, sixth, 3);
					}
					else if (tab2 == 0) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, sixth, 3);
					}
					ImGui::SameLine();
				}

				{
					bool pushedtab20 = false;
					if (tab2 == 1) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedtab20 = true;
					}
					else {
						pushedtab20 = false;
					}
					if (ImGui::Button("Buildings", ImVec2(sixth, 27))) {
						tab2 = 1;
					}
					if (tab2 == 1) {
						if (pushedtab20) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					b1.x += sixth;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, sixth, 3);
					}
					else if (tab2 == 1) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, sixth, 3);
					}
					ImGui::SameLine();
				}
				{
					bool pushedtab20 = false;
					if (tab2 == 2) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedtab20 = true;
					}
					else {
						pushedtab20 = false;
					}
					if (ImGui::Button("World", ImVec2(sixth, 27))) {
						tab2 = 2;
					}
					if (tab2 == 2) {
						if (pushedtab20) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					b1.x += sixth * 2;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, sixth, 3);
					}
					else if (tab2 == 2) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, sixth, 3);
					}
					ImGui::SameLine();
				}
				{
					bool pushedtab20 = false;
					if (tab2 == 3) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedtab20 = true;
					}
					else {
						pushedtab20 = false;
					}
					if (ImGui::Button("Fonts", ImVec2(sixth, 27))) {
						tab2 = 3;
					}
					if (tab2 == 3) {
						if (pushedtab20) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					b1.x += sixth * 3;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, sixth, 3);
					}
					else if (tab2 == 3) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, sixth, 3);
					}
					ImGui::SameLine();
				}
				{
					bool pushedtab20 = false;
					if (tab2 == 4) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedtab20 = true;
					}
					else {
						pushedtab20 = false;
					}
					if (ImGui::Button("Misc###MiscButtonTab", ImVec2(sixth, 27))) {
						tab2 = 4;
					}
					if (tab2 == 4) {
						if (pushedtab20) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					b1.x += sixth * 4;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, sixth, 3);
					}
					else if (tab2 == 4) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, sixth, 3);
					}
					ImGui::SameLine();
				}
				{
					bool pushedtab20 = false;
					if (tab2 == 5) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedtab20 = true;
					}
					else {
						pushedtab20 = false;
					}
					if (ImGui::Button("Radar###RadarButtonTab", ImVec2(sixth, 27))) {
						tab2 = 5;
					}
					if (tab2 == 5) {
						if (pushedtab20) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					b1.x += sixth * 5;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, sixth, 3);
					}
					else if (tab2 == 5) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, sixth, 3);
					}
					ImGui::SameLine();
				}
				ImGui::Dummy(ImVec2(0, 0));
			}
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();

			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
			ImGui::BeginChild("Outer", ImVec2(winSize.x, 0), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding);
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
				// (s.x / 2) - 16 = Fits two panels
				/*
				To fit three panels
				(s.x / 3) - 13
				(s.x / 3) - 13
				(s.x / 3) - 12
				*/
				ImGui::BeginChild("Feature 1", (tab1 == 2 && tab2 == 4) ? ImVec2((winSize.x / 2) - 16, 0) : ImVec2((winSize.x / 3) - 13, 0), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_HorizontalScrollbar);
				{
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
					if (tab1 == 0) {

						ImVec2 font_size = ImGui::CalcTextSize("Global");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Global");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Aimbot", &Vars::Aimbot::Global::Active.m_Var);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Target", Colors::Target);
						InputKeybind("Aimbot key", Vars::Aimbot::Global::AimKey);
						WidthSlider("Aimbot FoV", &Vars::Aimbot::Global::AimFOV.m_Var, 0.f, 180.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
						ImGui::Checkbox("Autoshoot###AimbotAutoshoot", &Vars::Aimbot::Global::AutoShoot.m_Var);
						ImGui::PushItemWidth(100);
						{
							std::vector<std::string> aimTargets;
							static bool aim_targetVariables[2]{ Vars::Aimbot::Global::AimPlayers.m_Var, Vars::Aimbot::Global::AimBuildings.m_Var };
							const char* aim_targetStrings[] = { "Players", "Buildings" }; static std::string aim_targetPreview = "PH";
							if (aim_targetPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
								aim_targetPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(aim_targetStrings); i++) {
									if (aim_targetVariables[i])
										aimTargets.push_back(aim_targetStrings[i]);
								}
								for (size_t i = 0; i < aimTargets.size(); i++)
								{
									if (aimTargets.size() == 1)
										aim_targetPreview += aimTargets.at(i);
									else if (!(i == aimTargets.size() - 1))
										aim_targetPreview += aimTargets.at(i) + ", ";
									else
										aim_targetPreview += aimTargets.at(i);
								}
							}
							if (ImGui::BeginCombo("Aim targets", aim_targetPreview.c_str()))
							{
								aim_targetPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(aim_targetStrings); i++)
								{
									ImGui::Selectable(aim_targetStrings[i], &aim_targetVariables[i]);
									if (aim_targetVariables[i])
										aimTargets.push_back(aim_targetStrings[i]);
								}
								for (size_t i = 0; i < aimTargets.size(); i++)
								{
									if (aimTargets.size() == 1)
										aim_targetPreview += aimTargets.at(i);
									else if (!(i == aimTargets.size() - 1))
										aim_targetPreview += aimTargets.at(i) + ", ";
									else
										aim_targetPreview += aimTargets.at(i);
								}
								ImGui::EndCombo();
							}
							for (size_t i = 0; i < IM_ARRAYSIZE(aim_targetVariables); i++) {
								if (aim_targetVariables[i]) {
									switch (i + 1) {
									case 1: { Vars::Aimbot::Global::AimPlayers.m_Var = true; break; }
									case 2: { Vars::Aimbot::Global::AimBuildings.m_Var = true; break; }
									}
								}
								else {
									switch (i + 1) {
									case 1: { Vars::Aimbot::Global::AimPlayers.m_Var = false; break; }
									case 2: { Vars::Aimbot::Global::AimBuildings.m_Var = false; break; }
									}
								}
							}
						} // Aim targets
						{
							std::vector<std::string> ignoreTargets;
							static bool ignore_targetVariables[4]{ Vars::Aimbot::Global::IgnoreInvlunerable.m_Var, Vars::Aimbot::Global::IgnoreCloaked.m_Var, Vars::Aimbot::Global::IgnoreFriends.m_Var, Vars::Aimbot::Global::IgnoreTaunting.m_Var };
							const char* ignore_targetStrings[] = { "Invulnerable", "Cloaked", "Friends", "Taunting" }; static std::string ignore_targetPreview = "PH";
							if (ignore_targetPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
								ignore_targetPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(ignore_targetStrings); i++) {
									if (ignore_targetVariables[i])
										ignoreTargets.push_back(ignore_targetStrings[i]);
								}
								for (size_t i = 0; i < ignoreTargets.size(); i++)
								{
									if (ignoreTargets.size() == 1)
										ignore_targetPreview += ignoreTargets.at(i);
									else if (!(i == ignoreTargets.size() - 1))
										ignore_targetPreview += ignoreTargets.at(i) + ", ";
									else
										ignore_targetPreview += ignoreTargets.at(i);
								}
							}
							if (ImGui::BeginCombo("Ignored targets###HitscanIgnoredTargets", ignore_targetPreview.c_str()))
							{
								ignore_targetPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(ignore_targetStrings); i++)
								{
									ImGui::Selectable(ignore_targetStrings[i], &ignore_targetVariables[i]);
									if (ignore_targetVariables[i])
										ignoreTargets.push_back(ignore_targetStrings[i]);
								}
								for (size_t i = 0; i < ignoreTargets.size(); i++)
								{
									if (ignoreTargets.size() == 1)
										ignore_targetPreview += ignoreTargets.at(i);
									else if (!(i == ignoreTargets.size() - 1))
										ignore_targetPreview += ignoreTargets.at(i) + ", ";
									else
										ignore_targetPreview += ignoreTargets.at(i);
								}
								ImGui::EndCombo();
							}
							for (size_t i = 0; i < IM_ARRAYSIZE(ignore_targetVariables); i++) {
								if (ignore_targetVariables[i]) {
									switch (i + 1) {
									case 1: { Vars::Aimbot::Global::IgnoreInvlunerable.m_Var = true; break; }
									case 2: { Vars::Aimbot::Global::IgnoreCloaked.m_Var = true; break; }
									case 3: { Vars::Aimbot::Global::IgnoreFriends.m_Var = true; break; }
									case 4: { Vars::Aimbot::Global::IgnoreTaunting.m_Var = true; break; }
									}
								}
								else {
									switch (i + 1) {
									case 1: { Vars::Aimbot::Global::IgnoreInvlunerable.m_Var = false; break; }
									case 2: { Vars::Aimbot::Global::IgnoreCloaked.m_Var = false; break; }
									case 3: { Vars::Aimbot::Global::IgnoreFriends.m_Var = false; break; }
									case 4: { Vars::Aimbot::Global::IgnoreTaunting.m_Var = false; break; }
									}
								}
							}
						} // Ignore targets
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Invulnerable colour", Colors::Invuln);
						ImGui::Dummy(ImVec2(0, 20));
						font_size = ImGui::CalcTextSize("Crits");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Crits");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 6;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Crit hack", &Vars::Crits::Active.m_Var);
						InputKeybind("Crit key", Vars::Crits::CritKey);
						ImGui::PopStyleVar();
					}
					else if (tab1 == 1) {
						ImVec2 font_size = ImGui::CalcTextSize("Global");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Global");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Triggerbot", &Vars::Triggerbot::Global::Active.m_Var);
						InputKeybind("Trigger key", Vars::Triggerbot::Global::TriggerKey);
						ImGui::PushItemWidth(100);
						{
							std::vector<std::string> ignoreTargets;
							static bool ignore_targetVariables[3]{ Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var, Vars::Aimbot::Global::IgnoreCloaked.m_Var, Vars::Aimbot::Global::IgnoreFriends.m_Var };
							const char* ignore_targetStrings[] = { "Invulnerable", "Cloaked", "Friends" }; static std::string ignore_targetPreview = "PH";
							if (ignore_targetPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
								ignore_targetPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(ignore_targetStrings); i++) {
									if (ignore_targetVariables[i])
										ignoreTargets.push_back(ignore_targetStrings[i]);
								}
								for (size_t i = 0; i < ignoreTargets.size(); i++)
								{
									if (ignoreTargets.size() == 1)
										ignore_targetPreview += ignoreTargets.at(i);
									else if (!(i == ignoreTargets.size() - 1))
										ignore_targetPreview += ignoreTargets.at(i) + ", ";
									else
										ignore_targetPreview += ignoreTargets.at(i);
								}
							}
							if (ImGui::BeginCombo("Ignored targets###TriggerbotIgnoredTargets", ignore_targetPreview.c_str()))
							{
								ignore_targetPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(ignore_targetStrings); i++)
								{
									ImGui::Selectable(ignore_targetStrings[i], &ignore_targetVariables[i]);
									if (ignore_targetVariables[i])
										ignoreTargets.push_back(ignore_targetStrings[i]);
								}
								for (size_t i = 0; i < ignoreTargets.size(); i++)
								{
									if (ignoreTargets.size() == 1)
										ignore_targetPreview += ignoreTargets.at(i);
									else if (!(i == ignoreTargets.size() - 1))
										ignore_targetPreview += ignoreTargets.at(i) + ", ";
									else
										ignore_targetPreview += ignoreTargets.at(i);
								}
								ImGui::EndCombo();
							}
							for (size_t i = 0; i < IM_ARRAYSIZE(ignore_targetVariables); i++) {
								if (ignore_targetVariables[i]) {
									switch (i + 1) {
									case 1: { Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var = true; break; }
									case 2: { Vars::Triggerbot::Global::IgnoreCloaked.m_Var = true; break; }
									case 3: { Vars::Triggerbot::Global::IgnoreFriends.m_Var = true; break; }
									}
								}
								else {
									switch (i + 1) {
									case 1: { Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var = false; break; }
									case 2: { Vars::Triggerbot::Global::IgnoreCloaked.m_Var = false; break; }
									case 3: { Vars::Triggerbot::Global::IgnoreFriends.m_Var = false; break; }
									}
								}
							}
						} // Ignore targets
						ImGui::PopItemWidth();
						ImGui::Dummy(ImVec2(0, 20));

						font_size = ImGui::CalcTextSize("Autoshoot");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("Autoshoot");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Autoshoot###AutoshootTrigger", &Vars::Triggerbot::Shoot::Active.m_Var);
						ImGui::PushItemWidth(100);
						{
							std::vector<std::string> aimTargets;
							static bool aim_targetVariables[2]{ Vars::Triggerbot::Shoot::TriggerPlayers.m_Var, Vars::Triggerbot::Shoot::TriggerBuildings.m_Var };
							const char* aim_targetStrings[] = { "Players", "Buildings" }; static std::string aim_targetPreview = "PH";
							if (aim_targetPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
								aim_targetPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(aim_targetStrings); i++) {
									if (aim_targetVariables[i])
										aimTargets.push_back(aim_targetStrings[i]);
								}
								for (size_t i = 0; i < aimTargets.size(); i++)
								{
									if (aimTargets.size() == 1)
										aim_targetPreview += aimTargets.at(i);
									else if (!(i == aimTargets.size() - 1))
										aim_targetPreview += aimTargets.at(i) + ", ";
									else
										aim_targetPreview += aimTargets.at(i);
								}
							}
							if (ImGui::BeginCombo("Trigger targets", aim_targetPreview.c_str()))
							{
								aim_targetPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(aim_targetStrings); i++)
								{
									ImGui::Selectable(aim_targetStrings[i], &aim_targetVariables[i]);
									if (aim_targetVariables[i])
										aimTargets.push_back(aim_targetStrings[i]);
								}
								for (size_t i = 0; i < aimTargets.size(); i++)
								{
									if (aimTargets.size() == 1)
										aim_targetPreview += aimTargets.at(i);
									else if (!(i == aimTargets.size() - 1))
										aim_targetPreview += aimTargets.at(i) + ", ";
									else
										aim_targetPreview += aimTargets.at(i);
								}
								ImGui::EndCombo();
							}
							for (size_t i = 0; i < IM_ARRAYSIZE(aim_targetVariables); i++) {
								if (aim_targetVariables[i]) {
									switch (i + 1) {
									case 1: { Vars::Aimbot::Global::AimPlayers.m_Var = true; break; }
									case 2: { Vars::Aimbot::Global::AimBuildings.m_Var = true; break; }
									}
								}
								else {
									switch (i + 1) {
									case 1: { Vars::Aimbot::Global::AimPlayers.m_Var = false; break; }
									case 2: { Vars::Aimbot::Global::AimBuildings.m_Var = false; break; }
									}
								}
							}
						} // Aim targets
						ImGui::PopItemWidth();
						ImGui::Checkbox("Head only", &Vars::Triggerbot::Shoot::HeadOnly.m_Var);
						ImGui::Checkbox("Wait for charge###TriggerbotWaitForCharge", &Vars::Triggerbot::Shoot::WaitForCharge.m_Var);
						WidthSlider("Head scale", &Vars::Triggerbot::Shoot::HeadScale.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
						ImGui::PopStyleVar();
					}
					else if (tab1 == 2 && tab2 == 0) {
						ImVec2 font_size = ImGui::CalcTextSize("ESP Main");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("ESP Main");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("ESP###EnableESP", &Vars::ESP::Main::Active.m_Var);
						ImGui::Checkbox("Outlined health bars", &Vars::ESP::Main::Outlinedbar.m_Var);
						ImGui::Checkbox("Relative colours", &Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
						if (Vars::ESP::Main::EnableTeamEnemyColors.m_Var) {
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
							ImGui::SetNextItemWidth(20);
							ColorPicker("Enemy###EnemyColorPicker", Colors::Enemy);
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
							ImGui::SetNextItemWidth(44);
							ColorPicker("Team###TeamColorPicker", Colors::Team);
						}
						else {
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
							ImGui::SetNextItemWidth(20);
							ColorPicker("RED Team###RedColorPicker", Colors::TeamRed);
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
							ImGui::SetNextItemWidth(44);
							ColorPicker("BLU Team###BluColorPicker", Colors::TeamBlu);
						}
						ImGui::Dummy(ImVec2(0, 20));

						font_size = ImGui::CalcTextSize("Player ESP");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("Player ESP");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Player ESP###EnablePlayerESP", &Vars::ESP::Players::Active.m_Var);
						ImGui::Checkbox("Name ESP###PlayerNameESP", &Vars::ESP::Players::Name.m_Var);
						ImGui::Checkbox("Name ESP box###PlayerNameESPBox", &Vars::ESP::Players::NameBox.m_Var);
						ImGui::Checkbox("Self ESP###SelfESP", &Vars::ESP::Players::ShowLocal.m_Var);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Local colour", Colors::Local);
						static const char* ignoreTeamArr[]{ "Off", "All", "Only friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore team###IgnoreTeamESPp", &Vars::ESP::Players::IgnoreTeammates.m_Var, ignoreTeamArr, IM_ARRAYSIZE(ignoreTeamArr)); ImGui::PopItemWidth();
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Friend colour", Colors::Friend);
						static const char* ignoreCloakArr[]{ "Off", "All", "Only enemies" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore cloaked###IgnoreCloakESPp", &Vars::ESP::Players::IgnoreCloaked.m_Var, ignoreCloakArr, IM_ARRAYSIZE(ignoreCloakArr)); ImGui::PopItemWidth();
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Cloaked colour", Colors::Cloak);
						static const char* espUberArr[]{ "Off", "Text", "Bar" }; ImGui::PushItemWidth(100); ImGui::Combo("Ubercharge###PlayerUber", &Vars::ESP::Players::Uber.m_Var, espUberArr, IM_ARRAYSIZE(espUberArr)); ImGui::PopItemWidth();
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Ubercharge colour", Colors::UberColor);
						static const char* classArr[]{ "Off", "Icon", "Text", "Both" }; ImGui::PushItemWidth(100); ImGui::Combo("Class###PlayerIconClass", &Vars::ESP::Players::Class.m_Var, classArr, IM_ARRAYSIZE(classArr)); ImGui::PopItemWidth();
						ImGui::Checkbox("Weapon icons", &Vars::ESP::Players::WeaponIcon.m_Var);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Invulnerable colour", Colors::WeaponIcon);
						ImGui::Checkbox("Health bar###ESPPlayerHealthBar", &Vars::ESP::Players::HealthBar.m_Var);
						ImGui::Checkbox("Health text###ESPPlayerHealthText", &Vars::ESP::Players::Health.m_Var);
						ImGui::Checkbox("Condition", &Vars::ESP::Players::Cond.m_Var);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Condition colour", Colors::Cond);
						ImGui::Checkbox("GUID", &Vars::ESP::Players::GUID.m_Var);
						static const char* boxArr[]{ "Off", "Bounding", "Cornered", "3D" }; ImGui::PushItemWidth(100); ImGui::Combo("Box###PlayerBoxESP", &Vars::ESP::Players::Box.m_Var, boxArr, IM_ARRAYSIZE(boxArr)); ImGui::PopItemWidth();
						static const char* bonesESP[]{ "Off", "Custom colour", "Health" }; ImGui::PushItemWidth(100); ImGui::Combo("Skeleton###PlayerSkellington", &Vars::ESP::Players::Bones.m_Var, bonesESP, IM_ARRAYSIZE(bonesESP)); ImGui::PopItemWidth(); HelpMarker("Will draw the bone structure of the player");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Skellington colour", Colors::Bones);
						ImGui::Checkbox("Lines###Playerlines", &Vars::ESP::Players::Lines.m_Var);
						ImGui::Checkbox("Dlights###PlayerDlights", &Vars::ESP::Players::Dlights.m_Var);
						WidthSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Players::DlightRadius.m_Var, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
						WidthSlider("ESP alpha###PlayerESPAlpha", &Vars::ESP::Players::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

						ImGui::PopStyleVar();
					}
					else if (tab1 == 2 && tab2 == 1) {
						ImVec2 font_size = ImGui::CalcTextSize("Building ESP");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Building ESP");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Building ESP###BuildinGESPSwioifas", &Vars::ESP::Buildings::Active.m_Var);
						ImGui::Checkbox("Ignore team###BuildingESPIgnoreTeammates", &Vars::ESP::Buildings::IgnoreTeammates.m_Var);
						ImGui::Checkbox("Name ESP###BuildingNameESP", &Vars::ESP::Buildings::Name.m_Var);
						ImGui::Checkbox("Name ESP box###BuildingNameESPBox", &Vars::ESP::Buildings::NameBox.m_Var);
						ImGui::Checkbox("Health bar###Buildinghelathbar", &Vars::ESP::Buildings::HealthBar.m_Var);
						ImGui::Checkbox("Health text###buildinghealth", &Vars::ESP::Buildings::Health.m_Var);
						ImGui::Checkbox("Building owner###Buildingowner", &Vars::ESP::Buildings::Owner.m_Var);
						ImGui::Checkbox("Building level###Buildinglevel", &Vars::ESP::Buildings::Level.m_Var);
						ImGui::Checkbox("Building condition###Buildingconditions", &Vars::ESP::Buildings::Cond.m_Var);
						ImGui::Checkbox("Lines###buildinglines", &Vars::ESP::Buildings::Lines.m_Var);
						static const char* boxArr[]{ "Off", "Bounding", "Cornered", "3D" }; ImGui::PushItemWidth(100); ImGui::Combo("Box###PBuildingBoxESP", &Vars::ESP::Buildings::Box.m_Var, boxArr, IM_ARRAYSIZE(boxArr)); ImGui::PopItemWidth();
						ImGui::Checkbox("Dlights###PlayerDlights", &Vars::ESP::Buildings::Dlights.m_Var);
						WidthSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Buildings::DlightRadius.m_Var, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
						WidthSlider("ESP alpha###BuildingESPAlpha", &Vars::ESP::Buildings::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
						ImGui::PopStyleVar();
					}
					else if (tab1 == 2 && tab2 == 2) {
						ImVec2 font_size = ImGui::CalcTextSize("World ESP");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("World ESP");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("World ESP###WorldESPActive", &Vars::ESP::World::Active.m_Var);
						ImGui::Checkbox("Health packs###WorldESPHealthPacks", &Vars::ESP::World::HealthText.m_Var);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Health pack colour", Colors::Health);
						ImGui::Checkbox("Ammo packs###WorldESPAmmoPacks", &Vars::ESP::World::AmmoText.m_Var);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Ammo pack colour", Colors::Ammo);
						WidthSlider("ESP alpha###WordlESPAlpha", &Vars::ESP::World::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
						ImGui::PopStyleVar();
					}
					else if (tab1 == 2 && tab2 == 3) {
						ImVec2 font_size = ImGui::CalcTextSize("ESP font");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("ESP font");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::PushItemWidth(100);
						ImGui::InputText("Font name###espfontname", &Vars::Fonts::FONT_ESP::szName);
						ImGui::InputInt("Font height###espfontheight", &Vars::Fonts::FONT_ESP::nTall.m_Var);
						ImGui::InputInt("Font weight###espfontweight", &Vars::Fonts::FONT_ESP::nWeight.m_Var);
						{
							static bool flagbools[12]{ 0,  0,  0, 0,  0, 0, 0,  0, 0, 0,0,0 };
							static std::string previewValue = "";
							std::vector < std::string > vec;
							if (ImGui::BeginCombo("Font flags###espfonttttttttttt", previewValue.c_str())) {
								previewValue = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++) {
									ImGui::Selectable(flags[i], &flagbools[i]);
									if (flagbools[i])
										vec.push_back(flags[i]);
								}
								for (size_t i = 0; i < vec.size(); i++) {
									if (vec.size() == 1)
										previewValue += vec.at(i);
									else if (!(i == vec.size() - 1))
										previewValue += vec.at(i) + ",";
									else
										previewValue += vec.at(i);
								}
								ImGui::EndCombo();
							}
							Vars::Fonts::FONT_ESP::nFlags.m_Var = 0;
							for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++) {
								if (flagbools[i]) {
									Vars::Fonts::FONT_ESP::nFlags.m_Var |= fontflags[i];
								}
							}
						}
						ImGui::PopItemWidth();

						ImGui::Dummy(ImVec2(0, 20));

						font_size = ImGui::CalcTextSize("Name font");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("Name font");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushItemWidth(100);
						ImGui::InputText("Font name###espfontnamename", &Vars::Fonts::FONT_ESP_NAME::szName);
						ImGui::InputInt("Font height###espfontnameheight", &Vars::Fonts::FONT_ESP_NAME::nTall.m_Var);
						ImGui::InputInt("Font weight###espfontnameweight", &Vars::Fonts::FONT_ESP_NAME::nWeight.m_Var);
						{
							static bool flagbools[12]{ 0,  0,  0, 0,  0, 0, 0,  0, 0, 0,0,0 };
							static std::string previewValue = "";
							std::vector < std::string > vec;
							if (ImGui::BeginCombo("Font flags###espfonnamettttttttttt", previewValue.c_str())) {
								previewValue = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++) {
									ImGui::Selectable(flags[i], &flagbools[i]);
									if (flagbools[i])
										vec.push_back(flags[i]);
								}
								for (size_t i = 0; i < vec.size(); i++) {
									if (vec.size() == 1)
										previewValue += vec.at(i);
									else if (!(i == vec.size() - 1))
										previewValue += vec.at(i) + ",";
									else
										previewValue += vec.at(i);
								}
								ImGui::EndCombo();
							}
							Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var = 0;
							for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++) {
								if (flagbools[i]) {
									Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var |= fontflags[i];
								}
							}
						}
						ImGui::PopItemWidth();
						ImGui::PopStyleVar();
					}
					else if (tab1 == 2 && tab2 == 4) {
						ImVec2 font_size = ImGui::CalcTextSize("Visual misc 1");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Visual misc 1");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::PushItemWidth(150); ImGui::SliderInt("Field of view", &Vars::Visuals::FieldOfView.m_Var, 70, 150, "%d"); ImGui::PopItemWidth(); HelpMarker("How many degrees of field of vision you would like");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Aimbot FOV circle", Colors::FOVCircle);
						const char* visionModifiers[]{ "Off", "Pyrovision", "Halloween", "Romevision" }; ImGui::PushItemWidth(150); ImGui::Combo("Vision modifiers", &Vars::Visuals::Vision.m_Var, visionModifiers, IM_ARRAYSIZE(visionModifiers)); ImGui::PopItemWidth(); HelpMarker("Vision modifiers");
						{
							ImGui::PushItemWidth(150);
							std::vector<std::string> modulationvec;
							static bool modulationFlags[]{ Vars::Visuals::WorldModulation.m_Var,Vars::Visuals::SkyModulation.m_Var,Vars::Visuals::PropWireframe.m_Var };
							const char* pmodulation[] = { "World", "Sky", "Prop Wireframe" }; static std::string modulationPreview = "PH";
							if (modulationPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
								modulationPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(pmodulation); i++) {
									if (modulationFlags[i])
										modulationvec.push_back(pmodulation[i]);
								}
								for (size_t i = 0; i < modulationvec.size(); i++)
								{
									if (modulationvec.size() == 1)
										modulationPreview += modulationvec.at(i);
									else if (!(i == modulationvec.size() - 1))
										modulationPreview += modulationvec.at(i) + ", ";
									else
										modulationPreview += modulationvec.at(i);
								}
							}
							if (ImGui::BeginCombo("World", modulationPreview.c_str()))
							{
								modulationPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(pmodulation); i++)
								{
									ImGui::Selectable(pmodulation[i], &modulationFlags[i]);
									if (modulationFlags[i])
										modulationvec.push_back(pmodulation[i]);
								}
								for (size_t i = 0; i < modulationvec.size(); i++)
								{
									if (modulationvec.size() == 1)
										modulationPreview += modulationvec.at(i);
									else if (!(i == modulationvec.size() - 1))
										modulationPreview += modulationvec.at(i) + ", ";
									else
										modulationPreview += modulationvec.at(i);
								}
								ImGui::EndCombo();
							}
							for (size_t i = 0; i < IM_ARRAYSIZE(modulationFlags); i++) {
								if (modulationFlags[i]) {
									switch (i + 1) {
									case 1: { Vars::Visuals::WorldModulation.m_Var = true; break; }
									case 2: { Vars::Visuals::SkyModulation.m_Var = true; break; }
									case 3: { Vars::Visuals::PropWireframe.m_Var = true; break; }
									}
								}
								else {
									switch (i + 1) {
									case 1: { Vars::Visuals::WorldModulation.m_Var = false; break; }
									case 2: { Vars::Visuals::SkyModulation.m_Var = false; break; }
									case 3: { Vars::Visuals::PropWireframe.m_Var = false; break; }
									}
								}
							}
						}
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("World modulation colour", Colors::WorldModulation);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
						ImGui::SetNextItemWidth(44);
						ColorPicker("Sky modulation colour", Colors::SkyModulation);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 68);
						ImGui::SetNextItemWidth(68);
						ColorPicker("Prop modulation colour", Colors::StaticPropModulation);
						{
							ImGui::PushItemWidth(150);
							std::vector<std::string> removalsvec;
							static bool removalFlags[6]{ Vars::Visuals::RemoveScope.m_Var,Vars::Visuals::RemoveZoom.m_Var,Vars::Visuals::RemoveDisguises.m_Var,Vars::Visuals::RemoveTaunts.m_Var,Vars::Misc::DisableInterpolation.m_Var,Vars::Visuals::RemovePunch.m_Var };
							const char* pRemovals[] = { "Scope", "Zoom", "Disguises", "Taunts", "Interpolation", "View Punch" }; static std::string removalsPreview = "PH";
							if (removalsPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
								removalsPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(pRemovals); i++) {
									if (removalFlags[i])
										removalsvec.push_back(pRemovals[i]);
								}
								for (size_t i = 0; i < removalsvec.size(); i++)
								{
									if (removalsvec.size() == 1)
										removalsPreview += removalsvec.at(i);
									else if (!(i == removalsvec.size() - 1))
										removalsPreview += removalsvec.at(i) + ", ";
									else
										removalsPreview += removalsvec.at(i);
								}
							}
							if (ImGui::BeginCombo("Removals", removalsPreview.c_str()))
							{
								removalsPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(pRemovals); i++)
								{
									ImGui::Selectable(pRemovals[i], &removalFlags[i]);
									if (removalFlags[i])
										removalsvec.push_back(pRemovals[i]);
								}
								for (size_t i = 0; i < removalsvec.size(); i++)
								{
									if (removalsvec.size() == 1)
										removalsPreview += removalsvec.at(i);
									else if (!(i == removalsvec.size() - 1))
										removalsPreview += removalsvec.at(i) + ", ";
									else
										removalsPreview += removalsvec.at(i);
								}
								ImGui::EndCombo();
							}
							for (size_t i = 0; i < IM_ARRAYSIZE(removalFlags); i++) {
								if (removalFlags[i]) {
									switch (i + 1) {
									case 1: { Vars::Visuals::RemoveScope.m_Var = true; break; }
									case 2: { Vars::Visuals::RemoveZoom.m_Var = true; break; }
									case 3: { Vars::Visuals::RemoveDisguises.m_Var = true; break; }
									case 4: { Vars::Visuals::RemoveTaunts.m_Var = true; break; }
									case 5: { Vars::Misc::DisableInterpolation.m_Var = true; break; }
									case 6: { Vars::Visuals::RemovePunch.m_Var = true; break; }
									}
								}
								else {
									switch (i + 1) {
									case 1: { Vars::Visuals::RemoveScope.m_Var = false; break; }
									case 2: { Vars::Visuals::RemoveZoom.m_Var = false; break; }
									case 3: { Vars::Visuals::RemoveDisguises.m_Var = false; break; }
									case 4: { Vars::Visuals::RemoveTaunts.m_Var = false; break; }
									case 5: { Vars::Misc::DisableInterpolation.m_Var = false; break; }
									case 6: { Vars::Visuals::RemovePunch.m_Var = false; break; }
									}
								}
							}
						}

						{
							ImGui::PushItemWidth(150);
							std::vector<std::string> predictionsvec;
							static bool predictionFlags[]{ Vars::Visuals::CrosshairAimPos.m_Var,Vars::Visuals::AimPosSquare.m_Var,Vars::Visuals::BulletTracer.m_Var,Vars::Visuals::AimbotViewmodel.m_Var };
							const char* pPredictions[] = { "Aimbot Crosshair", "Render Proj Line", "Bullet Tracers", "Viewmodel Aimbot" }; static std::string predictionsPreview = "PH";
							if (predictionsPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
								predictionsPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(pPredictions); i++) {
									if (predictionFlags[i])
										predictionsvec.push_back(pPredictions[i]);
								}
								for (size_t i = 0; i < predictionsvec.size(); i++)
								{
									if (predictionsvec.size() == 1)
										predictionsPreview += predictionsvec.at(i);
									else if (!(i == predictionsvec.size() - 1))
										predictionsPreview += predictionsvec.at(i) + ", ";
									else
										predictionsPreview += predictionsvec.at(i);
								}
							}
							if (ImGui::BeginCombo("Prediction", predictionsPreview.c_str()))
							{
								predictionsPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(pPredictions); i++)
								{
									ImGui::Selectable(pPredictions[i], &predictionFlags[i]);
									if (predictionFlags[i])
										predictionsvec.push_back(pPredictions[i]);
								}
								for (size_t i = 0; i < predictionsvec.size(); i++)
								{
									if (predictionsvec.size() == 1)
										predictionsPreview += predictionsvec.at(i);
									else if (!(i == predictionsvec.size() - 1))
										predictionsPreview += predictionsvec.at(i) + ", ";
									else
										predictionsPreview += predictionsvec.at(i);
								}
								ImGui::EndCombo();
							} // i got tired of trying better ways so this is new method fr*ck you
							for (size_t i = 0; i < IM_ARRAYSIZE(predictionFlags); i++) {
								if (predictionFlags[i]) {
									switch (i + 1) {
									case 1: { Vars::Visuals::CrosshairAimPos.m_Var = true; break; }
									case 2: { Vars::Visuals::AimPosSquare.m_Var = true; break; }
									case 3: { Vars::Visuals::BulletTracer.m_Var = true; break; }
									case 4: { Vars::Visuals::AimbotViewmodel.m_Var = true; break; }
									}
								}
								else {
									switch (i + 1) {
									case 1: { Vars::Visuals::CrosshairAimPos.m_Var = false; break; }
									case 2: { Vars::Visuals::AimPosSquare.m_Var = false; break; }
									case 3: { Vars::Visuals::BulletTracer.m_Var = false; break; }
									case 4: { Vars::Visuals::AimbotViewmodel.m_Var = false; break; }
									}
								}
							}
						}
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Bullet tracer colour", Colors::BulletTracer);
						static const char* bullettracers[]{ "Off", "Machina", "C.A.P.P.E.R", "Short Circuit", "Merasmus ZAP", "Merasmus ZAP Beam 2", "Big Nasty", "Distortion Trail", "Black Ink", "Custom" }; ImGui::PushItemWidth(100); ImGui::Combo("Particle tracer", &Vars::Visuals::ParticleTracer.m_Var, bullettracers, IM_ARRAYSIZE(bullettracers)); ImGui::PopItemWidth();
						if (Vars::Visuals::ParticleTracer.m_Var == 9) {
							ImGui::PushItemWidth(150); ImGui::InputText("Custom Tracer", &Vars::Visuals::ParticleName); ImGui::PopItemWidth(); HelpMarker("If you want to use a custom particle tracer");
						}
						if (Vars::Visuals::BulletTracer.m_Var) {
							ImGui::Checkbox("Rainbow tracers", &Vars::Visuals::BulletTracerRainbow.m_Var); HelpMarker("Bullet tracer color will be dictated by a changing color");
						}
						if (Vars::Visuals::RemoveScope.m_Var) {
							ImGui::Checkbox("Noscope lines", &Vars::Visuals::ScopeLines.m_Var); HelpMarker("Will draw a custom overlay");
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
							ImGui::SetNextItemWidth(20);
							ColorPicker("Noscope lines 1", Colors::NoscopeLines1);
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
							ImGui::SetNextItemWidth(44);
							ColorPicker("Noscope lines 2", Colors::NoscopeLines2);
						}
						ImGui::Checkbox("Draw Hitboxes", &Vars::Aimbot::Global::showHitboxes.m_Var); HelpMarker("Shows client hitboxes for enemies once they are attacked (not bbox)");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Hitbox matrix colour", Colors::bonecolor);
						ImGui::Checkbox("Clear Hitboxes", &Vars::Aimbot::Global::clearPreviousHitbox.m_Var); HelpMarker("Removes previous drawn hitboxes to mitigate clutter");
						ImGui::PushItemWidth(150); ImGui::SliderInt("Hitbox Draw Time", &Vars::Aimbot::Global::hitboxTime.m_Var, 1, 5); HelpMarker("Removes previous drawn hitboxes after n seconds");

						ImGui::Checkbox("Chat info", &Vars::Visuals::ChatInfo.m_Var);
						const char* specModes[]{ "Off", "Draggable", "Static", "Static + Avatars" }; ImGui::PushItemWidth(100); ImGui::Combo("Spectator list", &Vars::Visuals::SpectatorList.m_Var, specModes, IM_ARRAYSIZE(specModes)); ImGui::PopItemWidth();
						ImGui::Dummy(ImVec2(0, 20));

						font_size = ImGui::CalcTextSize("Viewmodel offset");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("Viewmodel offset");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushItemWidth(150); ImGui::SliderInt("VM Off X", &Vars::Visuals::VMOffX.m_Var, -90, 90);
						ImGui::PushItemWidth(150); ImGui::SliderInt("VM Off Y", &Vars::Visuals::VMOffY.m_Var, -90, 90);
						ImGui::PushItemWidth(150); ImGui::SliderInt("VM Off Z", &Vars::Visuals::VMOffZ.m_Var, -90, 90);
						ImGui::PushItemWidth(150); ImGui::SliderInt("VM Roll", &Vars::Visuals::VMRoll.m_Var, -180, 180);

						ImGui::Dummy(ImVec2(0, 20));
						font_size = ImGui::CalcTextSize("DT indicator");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("DT indicator");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						const char* dtBarStyles[]{ "Off", "Default", "Nitro", "Rijin" }; ImGui::PushItemWidth(150); ImGui::Combo("DT indicator style", &Vars::Misc::CL_Move::DTBarStyle.m_Var, dtBarStyles, IM_ARRAYSIZE(dtBarStyles)); ImGui::PopItemWidth(); HelpMarker("Which style to do the bar style");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("DT charging left###dtcg2", Colors::DtChargingLeft);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
						ImGui::SetNextItemWidth(44);
						ColorPicker("DT charging right###dtcg1", Colors::DtChargingRight);
						if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 3) {
							ImGui::PushItemWidth(150); ImGui::SliderInt("DT Bar Height", &Vars::Misc::CL_Move::DTBarScaleY.m_Var, 1, 25);
							ImGui::PopItemWidth(); ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
							ImGui::SetNextItemWidth(20);
							ColorPicker("DT charged left###dtcl1", Colors::DtChargedLeft);
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
							ImGui::SetNextItemWidth(44);
							ColorPicker("DT charged right###dtcr1", Colors::DtChargedRight);
							ImGui::PushItemWidth(150); ImGui::SliderInt("DT Bar Width", &Vars::Misc::CL_Move::DTBarScaleX.m_Var, 100, 1000);
							ImGui::PopItemWidth();
						}
						else {
							ImGui::PushItemWidth(150); ImGui::SliderInt("DT bar height", &Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var, 1, 30);
							ImGui::PopItemWidth(); ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
							ImGui::SetNextItemWidth(20);
							ColorPicker("DT charged left###dtcl2", Colors::DtChargedLeft);
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
							ImGui::SetNextItemWidth(44);
							ColorPicker("DT charged right###dtcr2", Colors::DtChargedRight);
							ImGui::PushItemWidth(150); ImGui::SliderInt("DT bar width", &Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var, 1, 30);
							ImGui::PopItemWidth();
						}
						
						const char* unuEffects[]{
							"None",
							"Hot",
							"Isotope",
							"Cool",
							"Energy orb"
						};
						const char* unuEffects2[]{
							"None",
							"Hot",
							"Isotope",
							"Cool",
							"Energy orb"
						};
						ImGui::Dummy(ImVec2(0, 20));
						font_size = ImGui::CalcTextSize("Attribute changer");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("Attribute changer");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushItemWidth(150);
						if (ImGui::Combo("Unusual effect 1", &unu1, unuEffects, IM_ARRAYSIZE(unuEffects))) {
							switch (unu1) {
							case 0:
								Vars::Visuals::Skins::Particle.m_Var = 0;
								break;
							case 1:
								Vars::Visuals::Skins::Particle.m_Var = 701;
								break;
							case 2:
								Vars::Visuals::Skins::Particle.m_Var = 702;
								break;
							case 3:
								Vars::Visuals::Skins::Particle.m_Var = 703;
								break;
							case 4:
								Vars::Visuals::Skins::Particle.m_Var = 704;
								break;
							default:
								break;
							}
						}
						ImGui::PopItemWidth();
						HelpMarker("The first unusual effect to be applied to the weapon");
						ImGui::PushItemWidth(150);
						if (ImGui::Combo("Unusual effect 2", &unu2, unuEffects2, IM_ARRAYSIZE(unuEffects2))) {
							switch (unu2) {
							case 0:
								Vars::Visuals::Skins::Effect.m_Var = 0;
								break;
							case 1:
								Vars::Visuals::Skins::Effect.m_Var = 701;
								break;
							case 2:
								Vars::Visuals::Skins::Effect.m_Var = 702;
								break;
							case 3:
								Vars::Visuals::Skins::Effect.m_Var = 703;
								break;
							case 4:
								Vars::Visuals::Skins::Effect.m_Var = 704;
								break;
							default:
								break;
							}
						}
						ImGui::PopItemWidth();
						HelpMarker("The second unusual effect to be applied to the weapon");
						const char* sheens[]{
							"None",
							"Team shine",
							"Deadly daffodil",
							"Manndarin",
							"Mean green",
							"Agonizing emerald",
							"Villainous violet",
							"Hot rod"
						};
						ImGui::PushItemWidth(150);
						ImGui::Combo("Sheen", &Vars::Visuals::Skins::Sheen.m_Var, sheens, IM_ARRAYSIZE(sheens));
						ImGui::PopItemWidth();
						HelpMarker("Which sheen to apply to the weapon");
						ImGui::Checkbox("Style override", &Vars::Visuals::Skins::Override.m_Var);
						if (ImGui::Button("Apply", ImVec2(45, 20))) {
							g_AttributeChanger.m_bSet = true;
						}
						ImGui::SameLine();
						if (ImGui::Button("Save", ImVec2(45, 20))) {
							g_AttributeChanger.m_bSave = true;
						}
						ImGui::SameLine();
						if (ImGui::Button("Load", ImVec2(44, 20))) {
							g_AttributeChanger.m_bLoad = true;
						}

						ImGui::PopStyleVar();
					}
					else if (tab1 == 2 && tab2 == 5) {
						ImVec2 font_size = ImGui::CalcTextSize("Main");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Main");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Active###RadarActive", &Vars::Radar::Main::Active.m_Var);
						ImGui::PushItemWidth(100); ImGui::SliderInt("Radar size", &Vars::Radar::Main::Size.m_Var, 20, 200); ImGui::PopItemWidth();
						ImGui::PushItemWidth(100); ImGui::SliderInt("Radar range", &Vars::Radar::Main::Range.m_Var, 50, 3000, "%d"); ImGui::PopItemWidth();
						ImGui::PushItemWidth(100); ImGui::SliderInt("Radar background alpha", &Vars::Radar::Main::BackAlpha.m_Var, 0, 255, "%d"); ImGui::PopItemWidth();
						ImGui::PushItemWidth(100); ImGui::SliderInt("Radar lines alpha", &Vars::Radar::Main::LineAlpha.m_Var, 0, 255, "%d"); ImGui::PopItemWidth();

						ImGui::Dummy(ImVec2(0, 20));
						font_size = ImGui::CalcTextSize("Players");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Players");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						const char* iconPlayersRadar[]{ "Scoreboard", "Portraits", "Avatar" }; ImGui::PushItemWidth(100); ImGui::Combo("Icon###radari", &Vars::Radar::Players::IconType.m_Var, iconPlayersRadar, IM_ARRAYSIZE(iconPlayersRadar)); ImGui::PopItemWidth();
						const char* backgroundpradar[]{ "Off", "Rectangle", "Texture" }; ImGui::PushItemWidth(100); ImGui::Combo("Background###radarb", &Vars::Radar::Players::BackGroundType.m_Var, backgroundpradar, IM_ARRAYSIZE(backgroundpradar)); ImGui::PopItemWidth();
						ImGui::Checkbox("Outline###radaro", &Vars::Radar::Players::Outline.m_Var); HelpMarker("Will put an outline on players on the radar");
						static const char* ignoreTeammatespRadar[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###radarplayersteam", &Vars::Radar::Players::IgnoreTeam.m_Var, ignoreTeammatespRadar, IM_ARRAYSIZE(ignoreTeammatespRadar)); ImGui::PopItemWidth();
						static const char* ignoreCloakedpRadar[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore cloaked###radarplayerscloaked", &Vars::Radar::Players::IgnoreCloaked.m_Var, ignoreCloakedpRadar, IM_ARRAYSIZE(ignoreCloakedpRadar)); ImGui::PopItemWidth();
						ImGui::Checkbox("Health bar###radarhealt", &Vars::Radar::Players::Health.m_Var);
						ImGui::PushItemWidth(100); ImGui::SliderInt("Icon size###playersizeiconradar", &Vars::Radar::Players::IconSize.m_Var, 12, 30, "%d"); ImGui::PopItemWidth();
						ImGui::PopStyleVar();
					}
					else if (tab1 == 3) {
						ImVec2 font_size = ImGui::CalcTextSize("Movement");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Movement");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("No push", &Vars::Misc::NoPush.m_Var);
						ImGui::Checkbox("Bunnyhop", &Vars::Misc::AutoJump.m_Var);
						const char* autoStrafeModes[]{ "Off", "Normal", "Directional" }; ImGui::PushItemWidth(100); ImGui::Combo("Autostrafe", &Vars::Misc::AutoStrafe.m_Var, autoStrafeModes, IM_ARRAYSIZE(autoStrafeModes)); ImGui::PopItemWidth();
						ImGui::Checkbox("Edge jump", &Vars::Misc::EdgeJump.m_Var);
						InputKeybind("Edge jump key", Vars::Misc::EdgeJumpKey, false);
						ImGui::Checkbox("Auto rocket jump", &Vars::Misc::AutoRocketJump.m_Var);
						ImGui::Checkbox("Anti-AFK", &Vars::Misc::AntiAFK.m_Var);
						ImGui::Checkbox("Taunt slide", &Vars::Misc::TauntSlide.m_Var);
						ImGui::Checkbox("Taunt control", &Vars::Misc::TauntControl.m_Var);
						const char* rollModes[]{ "Off", "Backwards", "Fake forward" }; ImGui::PushItemWidth(100); ImGui::Combo("Crouch speed", &Vars::Misc::Roll.m_Var, rollModes, IM_ARRAYSIZE(rollModes)); ImGui::PopItemWidth(); HelpMarker("Allows you to go at normal walking speed when crouching (affects many things, use with caution)");

						ImGui::Dummy(ImVec2(0, 20));
						font_size = ImGui::CalcTextSize("Misc 1");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Misc 1");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Noisemaker spam", &Vars::Misc::NoisemakerSpam.m_Var);
						const char* spamModes[]{ "Off", "Fedoraware", "Lmaobox", "Cathook" }; ImGui::PushItemWidth(100); ImGui::Combo("Chat spam", &Vars::Misc::ChatSpam.m_Var, spamModes, IM_ARRAYSIZE(spamModes)); ImGui::PopItemWidth();
						ImGui::Checkbox("Chat censor", &Vars::Misc::ChatCensor.m_Var);
						ImGui::Checkbox("Rage retry", &Vars::Misc::RageRetry.m_Var);
						ImGui::PushItemWidth(100); ImGui::SliderInt("Rage Retry health", &Vars::Misc::RageRetryHealth.m_Var, 1, 99, "%d%%");
						ImGui::Checkbox("Cat identify", &Vars::Misc::BeCat.m_Var);
						ImGui::Checkbox("Force sv_cheats", &Vars::Misc::CheatsBypass.m_Var);
						ImGui::Checkbox("MvM instant respawn", &Vars::Misc::MVMRes.m_Var);
						ImGui::Checkbox("Vote revealer", &Vars::Misc::VoteRevealer.m_Var);
						ImGui::Checkbox("Log votes to party", &Vars::Misc::VotesInChat.m_Var);
						ImGui::PopStyleVar();
					}
					ImGui::EndChild();
				}
				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
				ImGui::BeginChild("Feature 2", (tab1 == 2 && tab2 == 4) ? ImVec2((winSize.x / 2) - 16, 0) : ImVec2((winSize.x / 3) - 13, 0), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_HorizontalScrollbar);
				{
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
					if (tab1 == 0) {
						ImVec2 font_size = ImGui::CalcTextSize("Hitscan");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Hitscan");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						static const char* sortMethodArr[]{ "FOV", "Distance", }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###HitscanSortMethod", &Vars::Aimbot::Hitscan::SortMethod.m_Var, sortMethodArr, IM_ARRAYSIZE(sortMethodArr)); ImGui::PopItemWidth();
						static const char* aimMethodArr[]{ "Plain", "Smooth", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###HitscanAimMethod", &Vars::Aimbot::Hitscan::AimMethod.m_Var, aimMethodArr, IM_ARRAYSIZE(aimMethodArr)); ImGui::PopItemWidth();
						static const char* aimHitboxArr[]{ "Head", "Body", "Auto" }; ImGui::PushItemWidth(100); ImGui::Combo("Hitbox###HitscanHitbox", &Vars::Aimbot::Hitscan::AimHitbox.m_Var, aimHitboxArr, IM_ARRAYSIZE(aimHitboxArr)); ImGui::PopItemWidth();

						WidthSlider("Smooth factor###HitscanSmoothing", &Vars::Aimbot::Hitscan::SmoothingAmount.m_Var, 0.f, 20.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
						ImGui::PushItemWidth(100);
						{
							std::vector<std::string> hitscan_mpTargets;
							static bool hitscan_mpVariables[3]{ Vars::Aimbot::Hitscan::ScanHitboxes.m_Var, Vars::Aimbot::Hitscan::ScanHead.m_Var, Vars::Aimbot::Hitscan::ScanBuildings.m_Var };
							const char* hitscan_mpStrings[] = { "Body", "Head", "Buildings" }; static std::string hitscan_mpPreview = "PH";
							if (hitscan_mpPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
								hitscan_mpPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(hitscan_mpStrings); i++) {
									if (hitscan_mpVariables[i])
										hitscan_mpTargets.push_back(hitscan_mpStrings[i]);
								}
								for (size_t i = 0; i < hitscan_mpTargets.size(); i++)
								{
									if (hitscan_mpTargets.size() == 1)
										hitscan_mpPreview += hitscan_mpTargets.at(i);
									else if (!(i == hitscan_mpTargets.size() - 1))
										hitscan_mpPreview += hitscan_mpTargets.at(i) + ", ";
									else
										hitscan_mpPreview += hitscan_mpTargets.at(i);
								}
							}
							if (ImGui::BeginCombo("Aim targets", hitscan_mpPreview.c_str()))
							{
								hitscan_mpPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(hitscan_mpStrings); i++)
								{
									ImGui::Selectable(hitscan_mpStrings[i], &hitscan_mpVariables[i]);
									if (hitscan_mpVariables[i])
										hitscan_mpTargets.push_back(hitscan_mpStrings[i]);
								}
								for (size_t i = 0; i < hitscan_mpTargets.size(); i++)
								{
									if (hitscan_mpTargets.size() == 1)
										hitscan_mpPreview += hitscan_mpTargets.at(i);
									else if (!(i == hitscan_mpTargets.size() - 1))
										hitscan_mpPreview += hitscan_mpTargets.at(i) + ", ";
									else
										hitscan_mpPreview += hitscan_mpTargets.at(i);
								}
								ImGui::EndCombo();
							}
							for (size_t i = 0; i < IM_ARRAYSIZE(hitscan_mpVariables); i++) {
								if (hitscan_mpVariables[i]) {
									switch (i + 1) {
									case 1: { Vars::Aimbot::Hitscan::ScanHitboxes.m_Var = true; break; }
									case 2: { Vars::Aimbot::Hitscan::ScanHead.m_Var = true; break; }
									case 3: { Vars::Aimbot::Hitscan::ScanBuildings.m_Var = true; break; }
									}
								}
								else {
									switch (i + 1) {
									case 1: { Vars::Aimbot::Hitscan::ScanHitboxes.m_Var = false; break; }
									case 2: { Vars::Aimbot::Hitscan::ScanHead.m_Var = false; break; }
									case 3: { Vars::Aimbot::Hitscan::ScanBuildings.m_Var = false; break; }
									}
								}
							}
						}
						ImGui::PopItemWidth();
						ImGui::Checkbox("Wait for headshot", &Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var);
						ImGui::Checkbox("Wait for charge", &Vars::Aimbot::Hitscan::WaitForCharge.m_Var);
						ImGui::Checkbox("Spectated smooth", &Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var);
						ImGui::Checkbox("Scoped only", &Vars::Aimbot::Hitscan::ScopedOnly.m_Var);
						ImGui::Checkbox("Automatically scope", &Vars::Aimbot::Hitscan::AutoScope.m_Var);
						ImGui::Checkbox("Automatically rev", &Vars::Aimbot::Hitscan::AutoRev.m_Var);
						ImGui::Checkbox("Bodyaim if lethal", &Vars::Aimbot::Global::BAimLethal.m_Var);
						ImGui::PopStyleVar();
					}
					else if (tab1 == 1) {
						ImVec2 font_size = ImGui::CalcTextSize("Autostab");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Autostab");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Autostab###TriggerAutostab", &Vars::Triggerbot::Stab::Active.m_Var);
						ImGui::Checkbox("Rage mode", &Vars::Triggerbot::Stab::RageMode.m_Var);
						ImGui::Checkbox("Silent", &Vars::Triggerbot::Stab::Silent.m_Var);
						ImGui::Checkbox("Disguise on kill", &Vars::Triggerbot::Stab::Disguise.m_Var);
						ImGui::Checkbox("Ignore razorback", &Vars::Triggerbot::Stab::IgnRazor.m_Var);
						WidthSlider("Range###StabRange", &Vars::Triggerbot::Stab::Range.m_Var, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

						ImGui::Dummy(ImVec2(0, 20));

						font_size = ImGui::CalcTextSize("Autodetonate");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("Autodetonate");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Autodetonate###Triggerautodet", &Vars::Triggerbot::Detonate::Active.m_Var);
						ImGui::Checkbox("Explode stickies", &Vars::Triggerbot::Detonate::Stickies.m_Var);
						ImGui::Checkbox("Detonator flares", &Vars::Triggerbot::Detonate::Flares.m_Var);
						WidthSlider("Radius scale###Triggerdetradius", &Vars::Triggerbot::Detonate::RadiusScale.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

						ImGui::PopStyleVar();
					}
					else if (tab1 == 2 && tab2 == 1) {
						ImVec2 font_size = ImGui::CalcTextSize("Building chams");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Building chams");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Building chams###BuildingChamsBox", &Vars::Chams::Buildings::Active.m_Var);
						ImGui::Checkbox("Ignore team buildings", &Vars::Chams::Buildings::IgnoreTeammates.m_Var);
						//static const char* ignoreTeamArr[]{ "Off", "All", "Only friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore team###IgnoreTeamChamsb", &Vars::Chams::Buildings::IgnoreTeammates.m_Var, ignoreTeamArr, IM_ARRAYSIZE(ignoreTeamArr)); ImGui::PopItemWidth();
						if (g_Interfaces.CVars->FindVar("mat_phong")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_specular")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_bumpmap")->GetInt() == 1) {
							static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel" }; ImGui::PushItemWidth(100); ImGui::Combo("Building material", &Vars::Chams::Buildings::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth();
						}
						else {
							static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur" }; ImGui::PushItemWidth(100); ImGui::Combo("Building material", &Vars::Chams::Buildings::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth();
						}
						ImGui::Checkbox("Ignore Z###BuildingChamsIgnoreZ", &Vars::Chams::Players::IgnoreZ.m_Var);
						WidthSlider("Building chams alpha", &Vars::Chams::Buildings::Alpha.m_Var, 0.0f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
						ImGui::PopStyleVar();
					}
					else if (tab1 == 2 && tab2 == 0) {
						ImVec2 font_size = ImGui::CalcTextSize("Chams main");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Chams main");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Chams###ChamsMasterSwitch", &Vars::Chams::Main::Active.m_Var);
						ImGui::Dummy(ImVec2(0, 20));

						font_size = ImGui::CalcTextSize("Player chams");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("Player chams");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Player chams###PlayerChamsBox", &Vars::Chams::Players::Active.m_Var);
						ImGui::Checkbox("Self chams###PlayerChamsBox", &Vars::Chams::Players::ShowLocal.m_Var);
						static const char* ignoreTeamArr[]{ "Off", "All", "Only friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore team###IgnoreTeamChamsp", &Vars::Chams::Players::IgnoreTeammates.m_Var, ignoreTeamArr, IM_ARRAYSIZE(ignoreTeamArr)); ImGui::PopItemWidth();
						ImGui::Checkbox("Wearable chams###PlayerWearableChams", &Vars::Chams::Players::Wearables.m_Var);
						ImGui::Checkbox("Weapon chams###PlayerWearableChams", &Vars::Chams::Players::Weapons.m_Var);
						if (g_Interfaces.CVars->FindVar("mat_phong")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_specular")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_bumpmap")->GetInt() == 1) {
							static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel" }; ImGui::PushItemWidth(100); ImGui::Combo("Player material", &Vars::Chams::Players::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth();
						}
						else {
							static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur" }; ImGui::PushItemWidth(100); ImGui::Combo("Player material", &Vars::Chams::Players::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth();
						}
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Fresnel base colour###fresnelbasecolour", Colors::FresnelBase);
						ImGui::Checkbox("Ignore Z###PlayerChamsIgnoreZ", &Vars::Chams::Players::IgnoreZ.m_Var);
						WidthSlider("Player chams alpha", &Vars::Chams::Players::Alpha.m_Var, 0.0f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
						ImGui::Dummy(ImVec2(0, 20));

						font_size = ImGui::CalcTextSize("DME chams");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("DME chams");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("DME chams###dmeactive", &Vars::Chams::DME::Active.m_Var);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Weapon colour", Colors::Weapon);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
						ImGui::SetNextItemWidth(44);
						ColorPicker("Hand colour", Colors::Hands);
						static const char* handsMaterial[]{
							"Original",
							"Shaded",
							"Shiny",
							"Flat",
							"Wireframe shaded",
							"Wireframe shiny",
							"Wireframe flat",
							"Fresnel",
							"Brick"
						};
						ImGui::PushItemWidth(100);
						ImGui::Combo("Hand material", &Vars::Chams::DME::Hands.m_Var, handsMaterial, IM_ARRAYSIZE(handsMaterial));
						ImGui::PopItemWidth();


						ImGui::PushItemWidth(100); ImGui::SliderFloat("Hand alpha", &Vars::Chams::DME::HandsAlpha.m_Var, 0.0f, 1.0f, "%.2f"); ImGui::PopItemWidth();
						ImGui::Checkbox("Hand glow overlay", &Vars::Chams::DME::HandsGlowOverlay.m_Var);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Hand glow colour", Colors::HandsOverlay);
						ImGui::PushItemWidth(100); ImGui::SliderInt("Hand glow boost", &Vars::Chams::DME::HandsRimMultiplier.m_Var, 1, 100, "%d"); ImGui::PopItemWidth();
						static const char* weaponMaterial[]{
							"Original",
							"Shaded",
							"Shiny",
							"Flat",
							"Wireframe shaded",
							"Wireframe shiny",
							"Wireframe flat",
							"Fresnel",
							"Brick"
						};
						ImGui::PushItemWidth(100);
						ImGui::Combo("Weapon material", &Vars::Chams::DME::Weapon.m_Var, weaponMaterial, IM_ARRAYSIZE(weaponMaterial));
						ImGui::PopItemWidth();
						HelpMarker("What material to put on your viewmodels weapon");
						ImGui::PushItemWidth(100); ImGui::SliderFloat("Weapon alpha", &Vars::Chams::DME::WeaponAlpha.m_Var, 0.0f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque your viewmodel arms will appear");
						ImGui::Checkbox("Weapon glow overlay", &Vars::Chams::DME::WeaponGlowOverlay.m_Var); HelpMarker("Will place a second glow-like material overlayed on top of the original material");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Weapon glow colour", Colors::WeaponOverlay);
						ImGui::PushItemWidth(100); ImGui::SliderInt("Weapon glow boost", &Vars::Chams::DME::WeaponRimMultiplier.m_Var, 1, 100, "%d"); ImGui::PopItemWidth(); HelpMarker("How much the glow effect will be boosted by");

						ImGui::PopStyleVar();
					}
					else if (tab1 == 2 && tab2 == 2) {
						ImVec2 font_size = ImGui::CalcTextSize("World chams");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("World chams");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("World chams###woldchamsbut", &Vars::Chams::World::Active.m_Var);
						ImGui::Checkbox("Health packs###WorldchamsHealthPacks", &Vars::Chams::World::Health.m_Var);
						ImGui::Checkbox("Ammo packs###Worldchamsammopacks", &Vars::Chams::World::Ammo.m_Var);
						ImGui::Checkbox("Projectiles###Worldchamsprojectiles", &Vars::Chams::World::Ammo.m_Var);
						if (g_Interfaces.CVars->FindVar("mat_phong")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_specular")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_bumpmap")->GetInt() == 1) {
							static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel" }; ImGui::PushItemWidth(100); ImGui::Combo("Player material", &Vars::Chams::World::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth();
						}
						else {
							static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur" }; ImGui::PushItemWidth(100); ImGui::Combo("Player material", &Vars::Chams::World::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth();
						}
						ImGui::Checkbox("Ignore Z###worldignorez", &Vars::Chams::World::IgnoreZ.m_Var);
						WidthSlider("ESP alpha###WordlESPAlpha", &Vars::Chams::World::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
						ImGui::PopStyleVar();
					}
					else if (tab1 == 2 && tab2 == 3) {
						ImVec2 font_size = ImGui::CalcTextSize("Condition font");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Condition font");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::PushItemWidth(100);
						ImGui::InputText("Font name###espfontcondname", &Vars::Fonts::FONT_ESP_COND::szName);
						ImGui::InputInt("Font height###espfontcondheight", &Vars::Fonts::FONT_ESP_COND::nTall.m_Var);
						ImGui::InputInt("Font weight###espfontcondweight", &Vars::Fonts::FONT_ESP_COND::nWeight.m_Var);
						{
							static bool flagbools[12]{
								0,
								0,
								0,
								0,
								0,
								0,
								0,
								0,
								0,
								0,
								0,
								0
							};
							static std::string previewValue = "";
							std::vector < std::string > vec;
							if (ImGui::BeginCombo("Font flags###espfoncondttttttttttt", previewValue.c_str())) {
								previewValue = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++) {
									ImGui::Selectable(flags[i], &flagbools[i]);
									if (flagbools[i])
										vec.push_back(flags[i]);
								}
								for (size_t i = 0; i < vec.size(); i++) {
									if (vec.size() == 1)
										previewValue += vec.at(i);
									else if (!(i == vec.size() - 1))
										previewValue += vec.at(i) + ",";
									else
										previewValue += vec.at(i);
								}
								ImGui::EndCombo();
							}
							Vars::Fonts::FONT_ESP_COND::nFlags.m_Var = 0;
							for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++) {
								if (flagbools[i]) {
									Vars::Fonts::FONT_ESP_COND::nFlags.m_Var |= fontflags[i];
								}
							}
						}
						ImGui::PopItemWidth();
						ImGui::Dummy(ImVec2(0, 20));

						font_size = ImGui::CalcTextSize("Pickup font");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("Pickup font");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushItemWidth(100);
						ImGui::InputText("Font name###espfontpickupsname", &Vars::Fonts::FONT_ESP_PICKUPS::szName);
						ImGui::InputInt("Font height###espfontpickupsheight", &Vars::Fonts::FONT_ESP_PICKUPS::nTall.m_Var);
						ImGui::InputInt("Font weight###espfontpickupsweight", &Vars::Fonts::FONT_ESP_PICKUPS::nWeight.m_Var);
						{
							static bool flagbools[12]{
								0,
								0,
								0,
								0,
								0,
								0,
								0,
								0,
								0,
								0,
								0,
								0
							};
							static std::string previewValue = "";
							std::vector < std::string > vec;
							if (ImGui::BeginCombo("Font flags###espfonpickupsttttttttttt", previewValue.c_str())) {
								previewValue = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++) {
									ImGui::Selectable(flags[i], &flagbools[i]);
									if (flagbools[i])
										vec.push_back(flags[i]);
								}
								for (size_t i = 0; i < vec.size(); i++) {
									if (vec.size() == 1)
										previewValue += vec.at(i);
									else if (!(i == vec.size() - 1))
										previewValue += vec.at(i) + ",";
									else
										previewValue += vec.at(i);
								}
								ImGui::EndCombo();
							}
							Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var = 0;
							for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++) {
								if (flagbools[i]) {
									Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var |= fontflags[i];
								}
							}
						}
						ImGui::PopItemWidth();
						ImGui::PopStyleVar();
					}
					else if (tab1 == 2 && tab2 == 4) {
						ImVec2 font_size = ImGui::CalcTextSize("Visual misc 2");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Visual misc 2");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						const char* skyNames[] = {
							"Custom",
							"sky_tf2_04",
							"sky_upward",
							"sky_dustbowl_01",
							"sky_goldrush_01",
							"sky_granary_01",
							"sky_well_01",
							"sky_gravel_01",
							"sky_badlands_01",
							"sky_hydro_01",
							"sky_night_01",
							"sky_nightfall_01",
							"sky_trainyard_01",
							"sky_stormfront_01",
							"sky_morningsnow_01",
							"sky_alpinestorm_01",
							"sky_harvest_01",
							"sky_harvest_night_01",
							"sky_halloween",
							"sky_halloween_night_01",
							"sky_halloween_night2014_01",
							"sky_island_01",
							"sky_rainbow_01"
						};
						ImGui::Checkbox("Skybox changer", &Vars::Visuals::SkyboxChanger.m_Var);
						ImGui::PushItemWidth(150); ImGui::Combo("Skybox", &Vars::Skybox::SkyboxNum, skyNames, IM_ARRAYSIZE(skyNames), 6);  ImGui::PopItemWidth();
						ImGui::PushItemWidth(150); ImGui::InputText("Custom skybox", &Vars::Skybox::SkyboxName); ImGui::PopItemWidth();
						ImGui::Checkbox("World Textures Override", &Vars::Visuals::OverrideWorldTextures.m_Var);
						const char* logModes[]{ "Off", "Chat", "Text" }; ImGui::PushItemWidth(150); ImGui::Combo("Damage logger", &Vars::Visuals::damageLogger.m_Var, logModes, IM_ARRAYSIZE(logModes)); ImGui::PopItemWidth();
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Background colour###dmgloggerbg", Colors::DmgLoggerBackground);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
						ImGui::SetNextItemWidth(44);
						ColorPicker("Outline colour###dmgloggerout", Colors::DmgLoggerOutline);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 68);
						ImGui::SetNextItemWidth(68);
						ColorPicker("Text colour###dmgloggertext", Colors::DmgLoggerText);
						ImGui::PushItemWidth(150); ImGui::SliderFloat("Damage logger time", &Vars::Visuals::despawnTime.m_Var, 0.5f, 10.f, "%.1f"); ImGui::PopItemWidth();
						ImGui::Checkbox("Bypass pure", &Vars::Misc::BypassPure.m_Var);
						ImGui::Checkbox("Medal flip", &Vars::Misc::MedalFlip.m_Var);
						ImGui::Dummy(ImVec2(0, 20));


						font_size = ImGui::CalcTextSize("Thirdperson");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("Thirdperson");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Thirdperson", &Vars::Visuals::ThirdPerson.m_Var);
						InputKeybind("Thirdperson key", Vars::Visuals::ThirdPersonKey);
						ImGui::Checkbox("Thirdperson show real angles", &Vars::Visuals::ThirdPersonSilentAngles.m_Var);
						ImGui::Checkbox("Thirdperson instant yaw", &Vars::Visuals::ThirdPersonInstantYaw.m_Var);
						ImGui::Checkbox("Thirdperson show server hitboxes", &Vars::Visuals::ThirdPersonServerHitbox.m_Var);
						ImGui::Checkbox("Show fakelag", &Vars::Misc::CL_Move::FakelagIndicator.m_Var);
						ImGui::Dummy(ImVec2(0, 20));


						font_size = ImGui::CalcTextSize("Out of FOV arrows");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("Out of FOV arrows");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Active###fovar", &Vars::Visuals::OutOfFOVArrows.m_Var);
						ImGui::Checkbox("Outline arrows###OutlinedArrows", &Vars::Visuals::OutOfFOVArrowsOutline.m_Var); HelpMarker("16 missed calls");
						ImGui::PushItemWidth(150); ImGui::SliderFloat("Arrow length", &Vars::Visuals::ArrowLength.m_Var, 5.f, 50.f, "%.2f"); ImGui::PopItemWidth();
						ImGui::PushItemWidth(150); ImGui::SliderFloat("Arrow angle", &Vars::Visuals::ArrowAngle.m_Var, 5.f, 180.f, "%.2f"); ImGui::PopItemWidth();
						//ImGui::PushItemWidth(100); ImGui::SliderFloat("Arrow range", &Vars::Visuals::ScreenRange.m_Var, 1.1f, 4.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How far on the screen the arrows will go");
						ImGui::PushItemWidth(150); ImGui::SliderFloat("Max distance", &Vars::Visuals::MaxDist.m_Var, 0.f, 4000.f, "%.2f"); ImGui::PopItemWidth();
						ImGui::PushItemWidth(150); ImGui::SliderFloat("Min distance", &Vars::Visuals::MinDist.m_Var, 0.f, 1000.f, "%.2f"); ImGui::PopItemWidth();
						ImGui::Dummy(ImVec2(0, 20));

						font_size = ImGui::CalcTextSize("Spy warning");
						ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
						ImGui::Text("Spy warning");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Active###spywarn", &Vars::Visuals::SpyWarning.m_Var);
						ImGui::Checkbox("Voice command###spywarn1", &Vars::Visuals::SpyWarningAnnounce.m_Var);
						ImGui::Checkbox("Visible only###spywarn2", &Vars::Visuals::SpyWarningVisibleOnly.m_Var);
						ImGui::Checkbox("Ignore friends###spywarn3", &Vars::Visuals::SpyWarningIgnoreFriends.m_Var);
						const char* spyWmodes[]{ "Arrow", "Flash" }; ImGui::PushItemWidth(150); ImGui::Combo("Warning style", &Vars::Visuals::SpyWarningStyle.m_Var, spyWmodes, IM_ARRAYSIZE(spyWmodes)); ImGui::PopItemWidth();


						ImGui::PopStyleVar();
					}
					else if (tab1 == 2 && tab2 == 5) {
						ImVec2 font_size = ImGui::CalcTextSize("Buildings");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Buildings");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

						ImGui::Checkbox("Active###radarbuildingsa", &Vars::Radar::Buildings::Active.m_Var);
						ImGui::Checkbox("Outline###radarbuildingsao", &Vars::Radar::Buildings::Outline.m_Var);
						ImGui::Checkbox("Ignore team###radarbuildingsb", &Vars::Radar::Buildings::IgnoreTeam.m_Var);
						ImGui::Checkbox("Health bar###radarbuildingsc", &Vars::Radar::Buildings::Health.m_Var);
						ImGui::PushItemWidth(100); ImGui::SliderInt("Icon size###buildingsizeiconradar", &Vars::Radar::Buildings::IconSize.m_Var, 12, 30, "%d"); ImGui::PopItemWidth();
						ImGui::PopStyleVar();
					}
					else if (tab1 == 3) {
						ImVec2 font_size = ImGui::CalcTextSize("Tickbase exploits");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Tickbase exploits");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Doubletap", &Vars::Misc::CL_Move::Enabled.m_Var);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("DT bar outline colour", Colors::DtOutline);
						InputKeybind("Recharge key", Vars::Misc::CL_Move::RechargeKey);
						InputKeybind("Teleport key", Vars::Misc::CL_Move::TeleportKey);
						if (Vars::Misc::CL_Move::DTMode.m_Var == 0 || Vars::Misc::CL_Move::DTMode.m_Var == 2)
							InputKeybind("Doubletap key", Vars::Misc::CL_Move::DoubletapKey);
						{
							ImGui::PushItemWidth(100);
							std::vector<std::string> dtvec;
							static bool dtFlags[]{ Vars::Misc::CL_Move::RechargeWhileDead.m_Var,Vars::Misc::CL_Move::AutoRecharge.m_Var,Vars::Misc::CL_Move::WaitForDT.m_Var,Vars::Misc::CL_Move::AntiWarp.m_Var,Vars::Misc::CL_Move::NotInAir.m_Var };
							const char* pDt[] = { "Recharge While Dead", "Auto Recharge", "Wait for DT", "Anti-warp", "Avoid airborne"}; static std::string dtPreview = "PH";
							if (dtPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
								dtPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(pDt); i++) {
									if (dtFlags[i])
										dtvec.push_back(pDt[i]);
								}
								for (size_t i = 0; i < dtvec.size(); i++)
								{
									if (dtvec.size() == 1)
										dtPreview += dtvec.at(i);
									else if (!(i == dtvec.size() - 1))
										dtPreview += dtvec.at(i) + ", ";
									else
										dtPreview += dtvec.at(i);
								}
							}
							if (ImGui::BeginCombo("Options", dtPreview.c_str()))
							{
								dtPreview = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(pDt); i++)
								{
									ImGui::Selectable(pDt[i], &dtFlags[i]);
									if (dtFlags[i])
										dtvec.push_back(pDt[i]);
								}
								for (size_t i = 0; i < dtvec.size(); i++)
								{
									if (dtvec.size() == 1)
										dtPreview += dtvec.at(i);
									else if (!(i == dtvec.size() - 1))
										dtPreview += dtvec.at(i) + ", ";
									else
										dtPreview += dtvec.at(i);
								}
								ImGui::EndCombo();
							} // i got tired of trying better ways so this is new method fr*ck you
							for (size_t i = 0; i < IM_ARRAYSIZE(dtFlags); i++) {
								if (dtFlags[i]) {
									switch (i + 1) {
									case 1: { Vars::Misc::CL_Move::RechargeWhileDead.m_Var = true; break; }
									case 2: { Vars::Misc::CL_Move::AutoRecharge.m_Var = true; break; }
									case 3: { Vars::Misc::CL_Move::WaitForDT.m_Var = true; break; }
									case 4: { Vars::Misc::CL_Move::AntiWarp.m_Var = true; break; }
									case 5: { Vars::Misc::CL_Move::NotInAir.m_Var = true; break; }
									}
								}
								else {
									switch (i + 1) {
									case 1: { Vars::Misc::CL_Move::RechargeWhileDead.m_Var = false; break; }
									case 2: { Vars::Misc::CL_Move::AutoRecharge.m_Var = false; break; }
									case 3: { Vars::Misc::CL_Move::WaitForDT.m_Var = false; break; }
									case 4: { Vars::Misc::CL_Move::AntiWarp.m_Var = false; break; }
									case 5: { Vars::Misc::CL_Move::NotInAir.m_Var = false; break; }
									}
								}
							}
						}
						const char* dtModes[]{ "On key", "Always", "Disable on key", "Disabled" }; ImGui::PushItemWidth(100); ImGui::Combo("DT Mode", &Vars::Misc::CL_Move::DTMode.m_Var, dtModes, IM_ARRAYSIZE(dtModes)); ImGui::PopItemWidth();
						ImGui::PushItemWidth(100); ImGui::SliderInt("Ticks to shift", &Vars::Misc::CL_Move::DTTicks.m_Var, 1, 24, "%d");
						ImGui::Checkbox("SpeedHack", &Vars::Misc::CL_Move::SEnabled.m_Var); HelpMarker("Speedhack Master Switch");
						if (Vars::Misc::CL_Move::SEnabled.m_Var)
							ImGui::SliderInt("SpeedHack Factor", &Vars::Misc::CL_Move::SFactor.m_Var, 1, 66, "%d"); HelpMarker("High values are not recommended");

						ImGui::Dummy(ImVec2(0, 20));
						font_size = ImGui::CalcTextSize("HvH");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("HvH");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Anti-aim", &Vars::AntiHack::AntiAim::Active.m_Var);
						const char* pitch[]{ "None", "Up", "Down", "Fake up", "Fake down", "Random" }; ImGui::PushItemWidth(100); ImGui::Combo("Pitch", &Vars::AntiHack::AntiAim::Pitch.m_Var, pitch, IM_ARRAYSIZE(pitch)); ImGui::PopItemWidth();
						const char* realYaw[]{ "None", "Left", "Right", "Backwards", "Random", "Spin", "Edge" }; ImGui::PushItemWidth(100); ImGui::Combo("Real yaw", &Vars::AntiHack::AntiAim::YawReal.m_Var, realYaw, IM_ARRAYSIZE(realYaw)); ImGui::PopItemWidth();
						const char* fakeYaw[]{ "None", "Left", "Right", "Backwards", "Random", "Spin", "Edge" }; ImGui::PushItemWidth(100); ImGui::Combo("Fake yaw", &Vars::AntiHack::AntiAim::YawFake.m_Var, fakeYaw, IM_ARRAYSIZE(fakeYaw)); ImGui::PopItemWidth();
						if (Vars::AntiHack::AntiAim::YawFake.m_Var == 5 || Vars::AntiHack::AntiAim::YawReal.m_Var == 5)
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Spin Speed", &Vars::AntiHack::AntiAim::SpinSpeed.m_Var, -30.f, 30.f, "%.1f", 0); ImGui::PopItemWidth();
						ImGui::Checkbox("Resolver", &Vars::AntiHack::Resolver::Resolver.m_Var);
						const char* flgModes[]{ "None", "Plain", "Random", "Velocity Based" }; ImGui::PushItemWidth(100); ImGui::Combo("Fake Lag", &Vars::Misc::CL_Move::FakelagMode.m_Var, flgModes, IM_ARRAYSIZE(flgModes)); ImGui::PopItemWidth(); HelpMarker("Controls how fakelag will be controlled.");
						if (Vars::Misc::CL_Move::FakelagMode.m_Var > 0) {
							Vars::Misc::CL_Move::Fakelag.m_Var = true;
						}
						else {
							Vars::Misc::CL_Move::Fakelag.m_Var = false;
						}

						ImGui::Dummy(ImVec2(0, 20));
						font_size = ImGui::CalcTextSize("Fakelag");
						ImGui::SameLine(
							ImGui::GetWindowSize().x / 2 -
							font_size.x + (font_size.x / 2)
						);
						ImGui::Text("Fakelag");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						if (Vars::Misc::CL_Move::FakelagMode.m_Var == 1 || Vars::Misc::CL_Move::FakelagMode.m_Var == 3) {
							ImGui::PushItemWidth(100); ImGui::SliderInt("Fakelag value", &Vars::Misc::CL_Move::FakelagValue.m_Var, 1, 22, "%d"); ImGui::PopItemWidth(); HelpMarker("How much lag you should fake(?)");
							if (Vars::Misc::CL_Move::FakelagMode.m_Var == 1) {
								ImGui::Checkbox("Fakelag on key", &Vars::Misc::CL_Move::FakelagOnKey.m_Var);
								if (Vars::Misc::CL_Move::FakelagOnKey.m_Var)
									InputKeybind("Fakelag key", Vars::Misc::CL_Move::FakelagKey);
							}
						}
						if (Vars::Misc::CL_Move::FakelagMode.m_Var == 2) {
							ImGui::PushItemWidth(100); ImGui::SliderInt("Random max", &Vars::Misc::CL_Move::FakelagMax.m_Var, Vars::Misc::CL_Move::FakelagMin.m_Var + 1, 22, "%d"); ImGui::PopItemWidth();
							ImGui::PushItemWidth(100); ImGui::SliderInt("Random min", &Vars::Misc::CL_Move::FakelagMin.m_Var, 1, Vars::Misc::CL_Move::FakelagMax.m_Var - 1, "%d"); ImGui::PopItemWidth();
						}
						//ImGui::Checkbox("Fakelag Indicator", &Vars::Misc::CL_Move::FakelagIndicator.m_Var); HelpMarker("Shows your fakelag position in thirdperson");
						// this already exists in visuals misc near the thirdperson
						ImGui::PopStyleVar();
					}
					ImGui::EndChild();
				}
				if (!(tab1 == 2 && tab2 == 4)) {
					ImGui::SameLine(); //
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
					ImGui::BeginChild("Feature 3", ImVec2((winSize.x / 3) - 12, 0), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_HorizontalScrollbar);
					{
						ImGui::PopStyleVar();
						ImGui::PopStyleVar();
						ImGui::PopStyleVar();
						if (tab1 == 0) {
							ImVec2 font_size = ImGui::CalcTextSize("Projectile");
							//float font_size = ImGui::GetFontSize() * text.size() / 2;
							ImGui::SameLine(
								ImGui::GetWindowSize().x / 2 -
								font_size.x + (font_size.x / 2)
							);
							ImGui::Text("Projectile");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("Performance mode", &Vars::Aimbot::Projectile::PerformanceMode.m_Var);
							{
								static const char* sortMethodArr[]{ "FOV", "Distance", }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###ProjectileSortMethod", &Vars::Aimbot::Projectile::SortMethod.m_Var, sortMethodArr, IM_ARRAYSIZE(sortMethodArr)); ImGui::PopItemWidth();
								static const char* aimMethodArr[]{ "Plain", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###ProjectileAimMethod", &Vars::Aimbot::Projectile::AimMethod.m_Var, aimMethodArr, IM_ARRAYSIZE(aimMethodArr)); ImGui::PopItemWidth();
								static const char* aimHitboxArr[]{ "Body", "Feet", "Auto" }; ImGui::PushItemWidth(100); ImGui::Combo("Hitbox###ProjectileHitbox", &Vars::Aimbot::Projectile::AimPosition.m_Var, aimHitboxArr, IM_ARRAYSIZE(aimHitboxArr)); ImGui::PopItemWidth();
							}
							ImGui::Checkbox("Feet aim on ground", &Vars::Aimbot::Projectile::FeetAimIfOnGround.m_Var);
							ImGui::Checkbox("Custom huntsman Z-Adjust", &Vars::Aimbot::Projectile::FeetAimIfOnGround.m_Var);
							WidthSlider("Value###ZAdjustValue", &Vars::Aimbot::Projectile::ZAdjustAmount.m_Var, 0.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
							ImGui::Dummy(ImVec2(0, 20));
							font_size = ImGui::CalcTextSize("Melee");
							ImGui::SameLine(
								ImGui::GetWindowSize().x / 2 -
								font_size.x + (font_size.x / 2)
							);
							ImGui::Text("Melee");
							widget_pos = ImGui::GetCursorScreenPos();

							widget_pos.y -= 6;
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							{
								static const char* sortMethodArr[]{ "FOV", "Distance", }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###MeleeSortMethod", &Vars::Aimbot::Melee::SortMethod.m_Var, sortMethodArr, IM_ARRAYSIZE(sortMethodArr)); ImGui::PopItemWidth();
								static const char* aimMethodArr[]{ "Plain", "Smooth", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###MeleeAimMethod", &Vars::Aimbot::Melee::AimMethod.m_Var, aimMethodArr, IM_ARRAYSIZE(aimMethodArr)); ImGui::PopItemWidth();
							}
							WidthSlider("Smooth factor###MeleeSmoothing", &Vars::Aimbot::Melee::SmoothingAmount.m_Var, 0.f, 20.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
							ImGui::Checkbox("Range check", &Vars::Aimbot::Melee::RangeCheck.m_Var);
							ImGui::Checkbox("Swing prediction", &Vars::Aimbot::Melee::PredictSwing.m_Var);
							ImGui::Checkbox("Whip teammates", &Vars::Aimbot::Melee::WhipTeam.m_Var);
							ImGui::PopStyleVar();
						}
						else if (tab1 == 1) {
							ImVec2 font_size = ImGui::CalcTextSize("Autoblast");
							ImGui::SameLine(
								ImGui::GetWindowSize().x / 2 -
								font_size.x + (font_size.x / 2)
							);
							ImGui::Text("Autoblast");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("Autoblast###Triggreairblast", &Vars::Triggerbot::Blast::Active.m_Var);
							ImGui::Checkbox("Rage airblast", &Vars::Triggerbot::Blast::Rage.m_Var);
							ImGui::Checkbox("Silent###triggerblastsilent", &Vars::Triggerbot::Blast::Silent.m_Var);

							ImGui::Dummy(ImVec2(0, 20));

							font_size = ImGui::CalcTextSize("Autouber");
							ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
							ImGui::Text("Autouber");
							widget_pos = ImGui::GetCursorScreenPos();
							widget_pos.y -= 4;
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::Checkbox("Autouber###Triggeruber", &Vars::Triggerbot::Uber::Active.m_Var);
							ImGui::Checkbox("Only uber friends", &Vars::Triggerbot::Uber::OnlyFriends.m_Var);
							ImGui::Checkbox("Preserve self", &Vars::Triggerbot::Uber::PopLocal.m_Var);
							ImGui::Checkbox("Vaccinator resistances", &Vars::Triggerbot::Uber::AutoVacc.m_Var);
							WidthSlider("Health left (%)###TriggerUberHealthLeft", &Vars::Triggerbot::Uber::HealthLeft.m_Var, 1.f, 99.f, "%.0f%%", 1.0f);

							ImGui::PopStyleVar();
						}
						else if (tab1 == 2 && tab2 == 0) {
							ImVec2 font_size = ImGui::CalcTextSize("Glow main");
							ImGui::SameLine(
								ImGui::GetWindowSize().x / 2 -
								font_size.x + (font_size.x / 2)
							);
							ImGui::Text("Glow main");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("Glow", &Vars::Glow::Main::Active.m_Var);
							ImGui::PushItemWidth(100); ImGui::SliderInt("Glow scale", &Vars::Glow::Main::Scale.m_Var, 1, 10, "%d", ImGuiSliderFlags_AlwaysClamp); ImGui::PopItemWidth();
							ImGui::Checkbox("Wireframe glow", &Vars::Glow::Main::Wireframe.m_Var);
							ImGui::Dummy(ImVec2(0, 20));

							font_size = ImGui::CalcTextSize("Player glow");
							ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
							ImGui::Text("Player glow");
							widget_pos = ImGui::GetCursorScreenPos();
							widget_pos.y -= 4;
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::Checkbox("Player glow###PlayerGlowButton", &Vars::Glow::Players::Active.m_Var);
							ImGui::Checkbox("Self glow###SelfGlow", &Vars::Glow::Players::ShowLocal.m_Var);
							ImGui::Checkbox("Self rainbow glow###SelfGlowRainbow", &Vars::Glow::Players::LocalRainbow.m_Var); HelpMarker("Homosapien");
							static const char* ignoreTeamArr[]{ "Off", "All", "Only friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore team###IgnoreTeamGlowp", &Vars::Glow::Players::IgnoreTeammates.m_Var, ignoreTeamArr, IM_ARRAYSIZE(ignoreTeamArr)); ImGui::PopItemWidth();
							ImGui::Checkbox("Wearable glow###PlayerWearableChams", &Vars::Glow::Players::Wearables.m_Var);
							ImGui::Checkbox("Weapon glow###PlayerWearableChams", &Vars::Glow::Players::Weapons.m_Var);
							WidthSlider("Glow alpha###PlayerGlowAlpha", &Vars::Glow::Players::Alpha.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
							static const char* colourArr[]{ "Team", "Health" }; ImGui::PushItemWidth(100); ImGui::Combo("Glow colour###GlowColour", &Vars::Glow::Players::Color.m_Var, colourArr, IM_ARRAYSIZE(colourArr)); ImGui::PopItemWidth();

							ImGui::PopStyleVar();
						}
						else if (tab1 == 2 && tab2 == 1) {
							ImVec2 font_size = ImGui::CalcTextSize("Building glow");
							ImGui::SameLine(
								ImGui::GetWindowSize().x / 2 -
								font_size.x + (font_size.x / 2)
							);
							ImGui::Text("Building glow");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("Building glow###BuildiongGlowButton", &Vars::Glow::Buildings::Active.m_Var);
							ImGui::Checkbox("Ignore team buildings###buildingglowignoreteams", &Vars::Glow::Buildings::IgnoreTeammates.m_Var);
							WidthSlider("Glow alpha###BuildingGlowAlpha", &Vars::Glow::Buildings::Alpha.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
							static const char* colourArr[]{ "Team", "Health" }; ImGui::PushItemWidth(100); ImGui::Combo("Glow colour###GlowColourBuildings", &Vars::Glow::Buildings::Color.m_Var, colourArr, IM_ARRAYSIZE(colourArr)); ImGui::PopItemWidth();

							ImGui::PopStyleVar();
						}
						else if (tab1 == 2 && tab2 == 2) {
							ImVec2 font_size = ImGui::CalcTextSize("World glow");
							ImGui::SameLine(
								ImGui::GetWindowSize().x / 2 -
								font_size.x + (font_size.x / 2)
							);
							ImGui::Text("World glow");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("World glow###Worldglowbutton", &Vars::Glow::World::Active.m_Var);
							ImGui::Checkbox("Health packs###worldhealthpackglow", &Vars::Glow::World::Health.m_Var);
							ImGui::Checkbox("Health packs###worldammopackglow", &Vars::Glow::World::Ammo.m_Var);
							static const char* ignoreTeamArr[]{ "Off", "All", "Only enemies" }; ImGui::PushItemWidth(100); ImGui::Combo("Projectile glow###teamprojectileglow", &Vars::Glow::World::Projectiles.m_Var, ignoreTeamArr, IM_ARRAYSIZE(ignoreTeamArr)); ImGui::PopItemWidth();
							WidthSlider("Glow alpha###WorldGlowAlpha", &Vars::Glow::World::Alpha.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

							ImGui::PopStyleVar();
						}
						else if (tab1 == 2 && tab2 == 3) {
							ImVec2 font_size = ImGui::CalcTextSize("Menu font");
							ImGui::SameLine(
								ImGui::GetWindowSize().x / 2 -
								font_size.x + (font_size.x / 2)
							);
							ImGui::Text("Menu font");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::PushItemWidth(100);
							ImGui::InputText("Font name###espfontnamenameneby", &Vars::Fonts::FONT_MENU::szName);
							ImGui::InputInt("Font height###espfontnameheightafsdfads", &Vars::Fonts::FONT_MENU::nTall.m_Var);
							ImGui::InputInt("Font weight###espfontnameweightasfdafsd", &Vars::Fonts::FONT_MENU::nWeight.m_Var);
							{
								static bool flagbools[12]{
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0
								};
								static std::string previewValue = "";
								std::vector < std::string > vec;
								if (ImGui::BeginCombo("Font flags###espfonnametttttttttttafsafds", previewValue.c_str())) {
									previewValue = "";
									for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++) {
										ImGui::Selectable(flags[i], &flagbools[i]);
										if (flagbools[i])
											vec.push_back(flags[i]);
									}
									for (size_t i = 0; i < vec.size(); i++) {
										if (vec.size() == 1)
											previewValue += vec.at(i);
										else if (!(i == vec.size() - 1))
											previewValue += vec.at(i) + ",";
										else
											previewValue += vec.at(i);
									}
									ImGui::EndCombo();
								}
								Vars::Fonts::FONT_MENU::nFlags.m_Var = 0;
								for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++) {
									if (flagbools[i]) {
										Vars::Fonts::FONT_MENU::nFlags.m_Var |= fontflags[i];
									}
								}
							}
							ImGui::PopItemWidth();
							ImGui::Dummy(ImVec2(0, 20));

							font_size = ImGui::CalcTextSize("Indicator font");
							ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size.x + (font_size.x / 2));
							ImGui::Text("Indicator font");
							widget_pos = ImGui::GetCursorScreenPos();
							widget_pos.y -= 4;
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushItemWidth(100);
							ImGui::InputText("Font name###espfontindicatorname", &Vars::Fonts::FONT_INDICATORS::szName);
							ImGui::InputInt("Font height###espfontindicatorheight", &Vars::Fonts::FONT_INDICATORS::nTall.m_Var);
							ImGui::InputInt("Font weight###espfontindicatorweight", &Vars::Fonts::FONT_INDICATORS::nWeight.m_Var);
							{
								static bool flagbools[12]{
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0
								};
								static std::string previewValue = "";
								std::vector < std::string > vec;
								if (ImGui::BeginCombo("Font flags###espfoncondttttttttttt", previewValue.c_str())) {
									previewValue = "";
									for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++) {
										ImGui::Selectable(flags[i], &flagbools[i]);
										if (flagbools[i])
											vec.push_back(flags[i]);
									}
									for (size_t i = 0; i < vec.size(); i++) {
										if (vec.size() == 1)
											previewValue += vec.at(i);
										else if (!(i == vec.size() - 1))
											previewValue += vec.at(i) + ",";
										else
											previewValue += vec.at(i);
									}
									ImGui::EndCombo();
								}
								Vars::Fonts::FONT_INDICATORS::nFlags.m_Var = 0;
								for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++) {
									if (flagbools[i]) {
										Vars::Fonts::FONT_INDICATORS::nFlags.m_Var |= fontflags[i];
									}
								}
							}
							ImGui::PopItemWidth();
							if (ImGui::Button("Apply settings###fontapply")) {
								Font_t fontEsp = {
									0x0,
									Vars::Fonts::FONT_ESP::szName.c_str(),
									Vars::Fonts::FONT_ESP::nTall.m_Var,
									Vars::Fonts::FONT_ESP::nWeight.m_Var,
									Vars::Fonts::FONT_ESP::nFlags.m_Var
								};
								Font_t fontEspName = {
									0x0,
									Vars::Fonts::FONT_ESP_NAME::szName.c_str(),
									Vars::Fonts::FONT_ESP_NAME::nTall.m_Var,
									Vars::Fonts::FONT_ESP_NAME::nWeight.m_Var,
									Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var
								};
								Font_t fontEspCond = {
									0x0,
									Vars::Fonts::FONT_ESP_COND::szName.c_str(),
									Vars::Fonts::FONT_ESP_COND::nTall.m_Var,
									Vars::Fonts::FONT_ESP_COND::nWeight.m_Var,
									Vars::Fonts::FONT_ESP_COND::nFlags.m_Var
								};
								Font_t fontIndicator = {
									0x0,
									Vars::Fonts::FONT_INDICATORS::szName.c_str(),
									Vars::Fonts::FONT_INDICATORS::nTall.m_Var,
									Vars::Fonts::FONT_INDICATORS::nWeight.m_Var,
									Vars::Fonts::FONT_INDICATORS::nFlags.m_Var
								};
								Font_t fontEspPickups = {
									0x0,
									Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(),
									Vars::Fonts::FONT_ESP_PICKUPS::nTall.m_Var,
									Vars::Fonts::FONT_ESP_PICKUPS::nWeight.m_Var,
									Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var
								};
								Font_t fontMenu = {
									0x0,
									Vars::Fonts::FONT_MENU::szName.c_str(),
									Vars::Fonts::FONT_MENU::nTall.m_Var,
									Vars::Fonts::FONT_MENU::nWeight.m_Var,
									Vars::Fonts::FONT_MENU::nFlags.m_Var
								};
								std::vector < Font_t > fonts = {
									fontEsp,
									fontEspName,
									fontEspCond,
									fontEspPickups,
									fontMenu,
									fontIndicator,
									{ 0x0, "Verdana", 18, 800, FONTFLAG_ANTIALIAS},
								};
								g_Draw.RemakeFonts(fonts);
							}
							ImGui::PopStyleVar();
						}
						else if (tab1 == 2 && tab2 == 5) {
							ImVec2 font_size = ImGui::CalcTextSize("World");
							ImGui::SameLine(
								ImGui::GetWindowSize().x / 2 -
								font_size.x + (font_size.x / 2)
							);
							ImGui::Text("World");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("Active###radarworldd", &Vars::Radar::World::Active.m_Var);
							ImGui::Checkbox("Health###radarworldda", &Vars::Radar::World::Active.m_Var);
							ImGui::Checkbox("Ammo###radarworlddb", &Vars::Radar::World::Active.m_Var);
							ImGui::PushItemWidth(100); ImGui::SliderInt("Icon size###worldsizeiconradar", &Vars::Radar::World::IconSize.m_Var, 12, 30, "%d"); ImGui::PopItemWidth();

							ImGui::PopStyleVar();
						}

						else if (tab1 == 3) {
							ImVec2 font_size = ImGui::CalcTextSize("Discord RPC");
							ImGui::SameLine(
								ImGui::GetWindowSize().x / 2 -
								font_size.x + (font_size.x / 2)
							);
							ImGui::Text("Discord RPC");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("Discord RPC", &Vars::Misc::Discord::EnableRPC.m_Var);
							ImGui::Checkbox("Include map", &Vars::Misc::Discord::IncludeMap.m_Var);
							ImGui::Checkbox("Include class", &Vars::Misc::Discord::IncludeClass.m_Var);
							ImGui::Checkbox("Include timestamp", &Vars::Misc::Discord::IncludeTimestamp.m_Var);
							const char* imagething[]{ "Big fedora + Small TF2", "Small TF2 + Big fedora" }; ImGui::PushItemWidth(100); ImGui::Combo("Image Options", &Vars::Misc::Discord::WhatImagesShouldBeUsed.m_Var, imagething, IM_ARRAYSIZE(imagething)); ImGui::PopItemWidth();

							ImGui::Dummy(ImVec2(0, 20));
							font_size = ImGui::CalcTextSize("Steam RPC");
							ImGui::SameLine(
								ImGui::GetWindowSize().x / 2 -
								font_size.x + (font_size.x / 2)
							);
							ImGui::Text("Steam RPC");
							widget_pos = ImGui::GetCursorScreenPos();
							widget_pos.y -= 4;
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::Checkbox("Steam RPC", &Vars::Misc::Steam::EnableRPC.m_Var); HelpMarker("Enable Steam Rich Presence");
							const char* matchGroups[]{ "Special Event", "MvM Mann Up", "Competitive", "Casual", "MvM Boot Camp" }; ImGui::PushItemWidth(100); ImGui::Combo("Match group", &Vars::Misc::Steam::MatchGroup.m_Var, matchGroups, IM_ARRAYSIZE(matchGroups)); ImGui::PopItemWidth(); HelpMarker("Which match group should be used?");
							ImGui::Checkbox("Override in menu", &Vars::Misc::Steam::OverrideMenu.m_Var); HelpMarker("Override match group to \"Main Menu\" when in main menu");
							const char* mapTexts[]{ "Fedoraware", "Figoraware", "Meowhook.club", "Rathook.cc", "Nitro.tf", "custom" }; ImGui::PushItemWidth(100); ImGui::Combo("Map text", &Vars::Misc::Steam::MapText.m_Var, mapTexts, IM_ARRAYSIZE(mapTexts)); ImGui::PopItemWidth(); HelpMarker("Which map text should be used?");
							ImGui::PushItemWidth(100); ImGui::InputText("Custom map text", &Vars::Misc::Steam::CustomText); ImGui::PopItemWidth(); HelpMarker("For when \"custom\" is selcted in \"Map text\". Sets custom map text.");
							ImGui::PushItemWidth(100); ImGui::InputInt("Group size", &Vars::Misc::Steam::GroupSize.m_Var); HelpMarker("Sets party size"); ImGui::PopItemWidth();

							ImGui::Dummy(ImVec2(0, 20));
							font_size = ImGui::CalcTextSize("Utilities");
							ImGui::SameLine(
								ImGui::GetWindowSize().x / 2 -
								font_size.x + (font_size.x / 2)
							);
							ImGui::Text("Utilities");
							widget_pos = ImGui::GetCursorScreenPos();
							widget_pos.y -= 4;
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							
							if (ImGui::Button("Toggle playerlist", ImVec2(150, 20)))
								g_PlayerList.showWindow = !g_PlayerList.showWindow;
							if (ImGui::Button("Full update", ImVec2(150, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("cl_fullupdate");
							if (ImGui::Button("Reload HUD", ImVec2(150, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("hud_reloadscheme");
							if (ImGui::Button("Restart sound", ImVec2(150, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("snd_restart");
							if (ImGui::Button("Stop sound", ImVec2(150, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("stopsound");
							if (ImGui::Button("Status", ImVec2(150, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("status");
							if (ImGui::Button("Ping", ImVec2(150, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("ping");
							if (ImGui::Button("Retry", ImVec2(150, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("retry");
							if (ImGui::Button("Exit", ImVec2(150, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("exit");
							if (ImGui::Button("Console", ImVec2(150, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("showconsole");
							if (ImGui::Button("Demo playback", ImVec2(150, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("demoui");
							if (ImGui::Button("Demo trackbar", ImVec2(150, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("demoui2");
							if (ImGui::Button("Itemtest", ImVec2(150, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("itemtest");
							
							ImGui::PopStyleVar();
						}
						ImGui::EndChild();
					}

				}
				ImGui::EndChild();
			}
		}
		ImGui::PopFont();
		if (g_PlayerList.showWindow)
		{
			g_PlayerList.Render();
		}
		ImGui::End(); 
		//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.00f));
		//if (ImGui::Begin("Fedoraware", nullptr, ImGuiWindowFlags_NoCollapse))
		//{
		//	//ImGui::PopStyleColor();
		//	ImGui::SetWindowSize(ImVec2(1020, 600), ImGuiCond_Once);
		//
		//	ImGui::Columns(2);
		//	ImGui::SetColumnOffset(1, 140);
		//	{
		//		static ImVec4 active = to_vec4(30, 30, 30, 255);
		//		static ImVec4 inactive = to_vec4(30, 30, 30, 255);
		//		if (ImGui::Button("Aimbot", ImVec2(140 - 15, 29))) {
		//			SettingsTab = 0;
		//		}
		//		if (ImGui::Button("Triggerbot", ImVec2(140 - 15, 29))) {
		//			SettingsTab = 1;
		//		}
		//		if (ImGui::Button("Visuals", ImVec2(140 - 15, 29))) {
		//			SettingsTab = 2;
		//		}
		//		if (ImGui::Button("Misc", ImVec2(140 - 15, 29))) {
		//			SettingsTab = 3;
		//		}
		//		if (ImGui::Button("Colours", ImVec2(140 - 15, 29))) {
		//			SettingsTab = 4;
		//		}
		//		static std::wstring selected = {};
		//		int nConfig = 0;
		//
		//		for (const auto& entry : std::filesystem::directory_iterator(g_CFG.m_sConfigPath)) {
		//			if (std::string(std::filesystem::path(entry).filename().string()).find(_(".fed")) == std::string_view::npos)
		//			{
		//				continue;
		//			}
		//			nConfig++;
		//		}
		//
		//		if (nConfig < 100) {
		//			std::string output = {};
		//
		//			ImGui::PushItemWidth(140 - 15);
		//			if (ImGui::InputText("###configname", &output, ImGuiInputTextFlags_EnterReturnsTrue)) {
		//				std::wstring outstring(output.begin(), output.end());
		//				if (!std::filesystem::exists(g_CFG.m_sConfigPath + L"\\" + outstring)) {
		//					g_CFG.Save(outstring.c_str());
		//				}
		//			}
		//			ImGui::PopItemWidth();
		//		}
		//
		//		for (const auto& entry : std::filesystem::directory_iterator(g_CFG.m_sConfigPath)) {
		//			if (std::string(std::filesystem::path(entry).filename().string()).find(_(".fed")) == std::string_view::npos) {
		//				continue;
		//			}
		//			std::wstring s = entry.path().filename().wstring();
		//			s.erase(s.end() - 4, s.end());
		//			std::string configName(s.begin(), s.end());
		//			if (s == selected) {
		//				ImGuiStyle* style2 = &ImGui::GetStyle();
		//				ImVec4* colors2 = style2->Colors;
		//				ImVec4 buttonColor = colors2[ImGuiCol_Button];
		//				buttonColor.w *= 0.5;
		//				ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		//				if (ImGui::Button(configName.c_str(), ImVec2(140 - 15, 18))) {
		//					selected = s;
		//				}
		//				ImGui::PopStyleColor();
		//			}
		//			else {
		//				if (ImGui::Button(configName.c_str(), ImVec2(140 - 15, 18))) {
		//					selected = s;
		//				}
		//			}
		//		}
		//		if (!selected.empty())
		//		{
		//			ImGui::PushItemWidth(45);
		//			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		//			if (ImGui::Button("Save")) {
		//				ImGui::OpenPopup("Save config?");
		//			}
		//			ImGui::SameLine();
		//			if (ImGui::Button("Load")) {
		//				g_CFG.Load(selected.c_str());
		//				selected.clear();
		//			}
		//			ImGui::SameLine();
		//			if (ImGui::Button("Remove")) {
		//				ImGui::OpenPopup("Remove config?");
		//			}
		//			// Save config dialog
		//			if (ImGui::BeginPopupModal("Save config?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		//				ImGui::Text("Do you really want to override this config?\n\n");
		//				ImGui::Separator();
		//				if (ImGui::Button("Yes, override!", ImVec2(120, 0))) {
		//					g_CFG.Save(selected.c_str());
		//					selected.clear();
		//					ImGui::CloseCurrentPopup();
		//				}
		//				ImGui::SameLine();
		//				if (ImGui::Button("No", ImVec2(120, 0))) {
		//					ImGui::CloseCurrentPopup();
		//				}
		//				ImGui::EndPopup();
		//			}
		//			// Delete config dialog
		//			if (ImGui::BeginPopupModal("Remove config?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		//				ImGui::Text("Do you really want to delete this config?\n\n");
		//				ImGui::Separator();
		//				if (ImGui::Button("Yes, remove!", ImVec2(120, 0))) {
		//					g_CFG.Remove(selected.c_str());
		//					selected.clear();
		//					ImGui::CloseCurrentPopup();
		//				}
		//				ImGui::SameLine();
		//				if (ImGui::Button("No", ImVec2(120, 0))) {
		//					ImGui::CloseCurrentPopup();
		//				}
		//				ImGui::EndPopup();
		//			}
		//			ImGui::PopStyleVar();
		//			ImGui::PopItemWidth();
		//		}
		//	}
		//	ImGui::NextColumn();
		//	{
		//		if (SettingsTab == 0) {
		//			ImGui::BeginChild("Global");
		//			{
		//				ImGui::Columns(3);
		//				{
		//					ImGui::TextUnformatted("Global");
		//					ImGui::Checkbox("Aimbot###Aim bot", &Vars::Aimbot::Global::Active.m_Var); HelpMarker("Aimbot master switch");
		//					if (Vars::Aimbot::Global::Active.m_Var) {
		//						ImGui::Checkbox("AutoShoot", &Vars::Aimbot::Global::AutoShoot.m_Var); HelpMarker("Automatically shoot when a target is found");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Aimbot FoV", &Vars::Aimbot::Global::AimFOV.m_Var, 1.0f, 180.f, "%.0f", 1.0f); ImGui::PopItemWidth(); HelpMarker("How many degrees the aimbot's FoV will have");
		//						InputKeybind("Aim key", Vars::Aimbot::Global::AimKey); HelpMarker("The key to enable aimbot");
		//						{
		//							ImGui::PushItemWidth(100);
		//							std::vector<std::string> aimtargetsvec;
		//							static bool aimtargetFlags[]{ Vars::Aimbot::Global::AimPlayers.m_Var,Vars::Aimbot::Global::AimBuildings.m_Var };
		//							const char* pAimtargets[] = { "Players", "Buildings" }; static std::string aimtargetsPreview = "PH";
		//							if (aimtargetsPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
		//								aimtargetsPreview = "";
		//								for (size_t i = 0; i < IM_ARRAYSIZE(pAimtargets); i++) {
		//									if (aimtargetFlags[i])
		//										aimtargetsvec.push_back(pAimtargets[i]);
		//								}
		//								for (size_t i = 0; i < aimtargetsvec.size(); i++)
		//								{
		//									if (aimtargetsvec.size() == 1)
		//										aimtargetsPreview += aimtargetsvec.at(i);
		//									else if (!(i == aimtargetsvec.size() - 1))
		//										aimtargetsPreview += aimtargetsvec.at(i) + ", ";
		//									else
		//										aimtargetsPreview += aimtargetsvec.at(i);
		//								}
		//							}
		//							if (ImGui::BeginCombo("Targets", aimtargetsPreview.c_str()))
		//							{
		//								aimtargetsPreview = "";
		//								for (size_t i = 0; i < IM_ARRAYSIZE(pAimtargets); i++)
		//								{
		//									ImGui::Selectable(pAimtargets[i], &aimtargetFlags[i]);
		//									if (aimtargetFlags[i])
		//										aimtargetsvec.push_back(pAimtargets[i]);
		//								}
		//								for (size_t i = 0; i < aimtargetsvec.size(); i++)
		//								{
		//									if (aimtargetsvec.size() == 1)
		//										aimtargetsPreview += aimtargetsvec.at(i);
		//									else if (!(i == aimtargetsvec.size() - 1))
		//										aimtargetsPreview += aimtargetsvec.at(i) + ", ";
		//									else
		//										aimtargetsPreview += aimtargetsvec.at(i);
		//								}
		//								ImGui::EndCombo();
		//							}
		//							for (size_t i = 0; i < IM_ARRAYSIZE(aimtargetFlags); i++) {
		//								if (aimtargetFlags[i]) {
		//									switch (i + 1) {
		//									case 1: { Vars::Aimbot::Global::AimPlayers.m_Var = true; break; }
		//									case 2: { Vars::Aimbot::Global::AimBuildings.m_Var = true; break; }
		//									}
		//								}
		//								else {
		//									switch (i + 1) {
		//									case 1: { Vars::Aimbot::Global::AimPlayers.m_Var = false; break; }
		//									case 2: { Vars::Aimbot::Global::AimBuildings.m_Var = false; break; }
		//									}
		//								}
		//							}
		//						} // aim targets combobox
		//						{
		//							ImGui::PushItemWidth(100);
		//							std::vector<std::string> ignoretargetsvec;
		//							static bool ignoretargetFlags[]{ Vars::Aimbot::Global::IgnoreInvlunerable.m_Var,Vars::Aimbot::Global::IgnoreCloaked.m_Var,Vars::Aimbot::Global::IgnoreFriends.m_Var,Vars::Aimbot::Global::IgnoreTaunting.m_Var };
		//							const char* pIgnoretargets[] = { "Invulnerable", "Cloaked", "Friends", "Taunting"}; static std::string ignoretargetsPreview = "PH";
		//							if (ignoretargetsPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
		//								ignoretargetsPreview = "";
		//								for (size_t i = 0; i < IM_ARRAYSIZE(pIgnoretargets); i++) {
		//									if (ignoretargetFlags[i])
		//										ignoretargetsvec.push_back(pIgnoretargets[i]);
		//								}
		//								for (size_t i = 0; i < ignoretargetsvec.size(); i++)
		//								{
		//									if (ignoretargetsvec.size() == 1)
		//										ignoretargetsPreview += ignoretargetsvec.at(i);
		//									else if (!(i == ignoretargetsvec.size() - 1))
		//										ignoretargetsPreview += ignoretargetsvec.at(i) + ", ";
		//									else
		//										ignoretargetsPreview += ignoretargetsvec.at(i);
		//								}
		//							}
		//							if (ImGui::BeginCombo("Ignore Flags", ignoretargetsPreview.c_str()))
		//							{
		//								ignoretargetsPreview = "";
		//								for (size_t i = 0; i < IM_ARRAYSIZE(pIgnoretargets); i++)
		//								{
		//									ImGui::Selectable(pIgnoretargets[i], &ignoretargetFlags[i]);
		//									if (ignoretargetFlags[i])
		//										ignoretargetsvec.push_back(pIgnoretargets[i]);
		//								}
		//								for (size_t i = 0; i < ignoretargetsvec.size(); i++)
		//								{
		//									if (ignoretargetsvec.size() == 1)
		//										ignoretargetsPreview += ignoretargetsvec.at(i);
		//									else if (!(i == ignoretargetsvec.size() - 1))
		//										ignoretargetsPreview += ignoretargetsvec.at(i) + ", ";
		//									else
		//										ignoretargetsPreview += ignoretargetsvec.at(i);
		//								}
		//								ImGui::EndCombo();
		//							}
		//							for (size_t i = 0; i < IM_ARRAYSIZE(ignoretargetFlags); i++) {
		//								if (ignoretargetFlags[i]) {
		//									switch (i + 1) {
		//									case 1: { Vars::Aimbot::Global::IgnoreInvlunerable.m_Var = true; break; }
		//									case 2: { Vars::Aimbot::Global::IgnoreCloaked.m_Var = true; break; }
		//									case 3: { Vars::Aimbot::Global::IgnoreFriends.m_Var = true; break; }
		//									case 4: { Vars::Aimbot::Global::IgnoreTaunting.m_Var = true; break; }
		//									}
		//								}
		//								else {
		//									switch (i + 1) {
		//									case 1: { Vars::Aimbot::Global::IgnoreInvlunerable.m_Var = false; break; }
		//									case 2: { Vars::Aimbot::Global::IgnoreCloaked.m_Var = false; break; }
		//									case 3: { Vars::Aimbot::Global::IgnoreFriends.m_Var = false; break; }
		//									case 4: { Vars::Aimbot::Global::IgnoreTaunting.m_Var = false; break; }
		//									}
		//								}
		//							}
		//						} // ignore targets combobox
		//						ImGui::Checkbox("BAim when lethal", &Vars::Aimbot::Global::BAimLethal.m_Var); HelpMarker("The aimbot will aim for body when damage is lethal to it");
		//						ImGui::Checkbox("Doubletap", &Vars::Misc::CL_Move::Doubletap.m_Var); HelpMarker("When enough ticks are choked, the aimbot will shoot them all at once in a burst, leading to a rapid-fire effect");
		//						ImGui::TextUnformatted("");
		//						ImGui::TextUnformatted("Crithack");
		//						ImGui::Checkbox("Active###critsactive", &Vars::Crits::Active.m_Var); HelpMarker("Crit hack - this is the worst fucking crit hack known to man and I'm sorry for adding it");
		//						if (Vars::Crits::Active.m_Var)
		//							InputKeybind("Crit key", Vars::Crits::CritKey); HelpMarker("Crit key, self explanatory");
		//					}
		//				}
		//				if (Vars::Aimbot::Global::Active.m_Var) {
		//					ImGui::NextColumn();
		//					{
		//						ImGui::TextUnformatted("Hitscan");
		//						//ImGui::Checkbox("Active###Hit scan", &Vars::Aimbot::Hitscan::Active.m_Var); HelpMarker("Hitscan aimbot master switch");
		//						static const char* hitscanSortMethod[]{ "FoV", "Distance" }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###hitscanSortMethod", &Vars::Aimbot::Hitscan::SortMethod.m_Var, hitscanSortMethod, IM_ARRAYSIZE(hitscanSortMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to decide which target to aim at");
		//						static const char* hitscanAimMethod[]{ "Plain", "Smooth", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###hitscanAimMethod", &Vars::Aimbot::Hitscan::AimMethod.m_Var, hitscanAimMethod, IM_ARRAYSIZE(hitscanAimMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to aim at the target");
		//						static const char* hitscanAimHitbox[]{ "Head", "Body", "Auto" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim position###hitscanAimPosition", &Vars::Aimbot::Hitscan::AimHitbox.m_Var, hitscanAimHitbox, IM_ARRAYSIZE(hitscanAimHitbox)); ImGui::PopItemWidth(); HelpMarker("Which hitbox the aimbot will target");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Smooth factor", &Vars::Aimbot::Hitscan::SmoothingAmount.m_Var, 1.0f, 10.f, "%.0f", 1.0f); ImGui::PopItemWidth(); HelpMarker("How many degrees the aimbot's FoV will have");
		//						static const char* hitscanTapFire[]{ "Off", "Distance", "Always" }; ImGui::PushItemWidth(100); ImGui::Combo("Tap-fire", &Vars::Aimbot::Hitscan::TapFire.m_Var, hitscanTapFire, IM_ARRAYSIZE(hitscanTapFire)); ImGui::PopItemWidth(); HelpMarker("When to tap-fire with inaccurate rapid-fire weapons");
		//						ImGui::Checkbox("Minigun rev up", &Vars::Aimbot::Hitscan::AutoRev.m_Var); HelpMarker("Will rev heavy's minigun regardless of if aimbot has a target");
		//						ImGui::Checkbox("Body multipoint", &Vars::Aimbot::Hitscan::ScanHitboxes.m_Var); HelpMarker("Will attempt to shoot the body hitbox by scanning the edges");
		//						ImGui::Checkbox("Head multipoint", &Vars::Aimbot::Hitscan::ScanHead.m_Var); HelpMarker("Will attempt to shoot the head hitbox by scanning the edges");
		//						ImGui::Checkbox("Building multipoint", &Vars::Aimbot::Hitscan::ScanBuildings.m_Var); HelpMarker("Will attempt to shoot buildings by scanning the edges");
		//						ImGui::Checkbox("Wait for headshot", &Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var); HelpMarker("The aimbot will wait until it can headshot (if applicable)");
		//						ImGui::Checkbox("Wait for charge", &Vars::Aimbot::Hitscan::WaitForCharge.m_Var); HelpMarker("The aimbot will wait until the rifle has charged long enough to kill in one shot");
		//						ImGui::Checkbox("Smooth if spectated", &Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var); HelpMarker("The aimbot will switch to the smooth method if being spectated");
		//						ImGui::Checkbox("Scoped only", &Vars::Aimbot::Hitscan::ScopedOnly.m_Var); HelpMarker("The aimbot will only shoot if scoped");
		//						ImGui::Checkbox("Scope automatically", &Vars::Aimbot::Hitscan::AutoScope.m_Var); HelpMarker("The aimbot will automatically scope in to shoot");
		//					}
		//					ImGui::NextColumn();
		//					{
		//						ImGui::TextUnformatted("Projectile");
		//						ImGui::Checkbox("Performance mode", &Vars::Aimbot::Projectile::PerformanceMode.m_Var); HelpMarker("Only target enemy closest to the crosshair");
		//						static const char* projectileSortMethod[]{ "FoV", "Distance" }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###projectileSortMethod", &Vars::Aimbot::Projectile::SortMethod.m_Var, projectileSortMethod, IM_ARRAYSIZE(projectileSortMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to decide which target to aim at");
		//						static const char* projectileAimMethod[]{ "Plain", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###projectileAimMethod", &Vars::Aimbot::Projectile::AimMethod.m_Var, projectileAimMethod, IM_ARRAYSIZE(projectileAimMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to aim at the target");
		//						static const char* projectileAimHitbox[]{ "Body", "Feet", "Auto" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim position###projectileAimPosition", &Vars::Aimbot::Projectile::AimPosition.m_Var, projectileAimHitbox, IM_ARRAYSIZE(projectileAimHitbox)); ImGui::PopItemWidth(); HelpMarker("Which hitbox the aimbot will target");
		//						ImGui::Checkbox("Aim at feet if on ground (Demoman)", &Vars::Aimbot::Projectile::FeetAimIfOnGround.m_Var); HelpMarker("If true, aimbot will aim at enemies feet if target is on the ground (Demoman only and will only work if the aim postition is set to auto");
		//						ImGui::TextUnformatted("");
		//						ImGui::Checkbox("Manual Z Adjust", &Vars::Aimbot::Projectile::ManualZAdjust.m_Var); HelpMarker("Manual Z Adjust for projectiles");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Manual Z Adjust amount", &Vars::Aimbot::Projectile::ZAdjustAmount.m_Var, 0.0f, 10.0f, "%.1f"); ImGui::PopItemWidth(); HelpMarker("Note: Higher values may cause inaccuracy");
		//						ImGui::TextUnformatted("");
		//						ImGui::TextUnformatted("Melee");
		//						static const char* meleeSortMethod[]{ "FoV", "Distance" }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###meleeSortMethod", &Vars::Aimbot::Melee::SortMethod.m_Var, meleeSortMethod, IM_ARRAYSIZE(meleeSortMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to decide which target to aim at");
		//						static const char* meleeAimMethod[]{ "Plain", "Smooth", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###meleeAimMethod", &Vars::Aimbot::Melee::AimMethod.m_Var, meleeAimMethod, IM_ARRAYSIZE(meleeAimMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to aim at the target");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Smooth factor###meleeSmooth", &Vars::Aimbot::Melee::SmoothingAmount.m_Var, 1.0f, 10.f, "%.0f", 1.0f); ImGui::PopItemWidth(); HelpMarker("How many degrees the aimbot's FoV will have");
		//						ImGui::Checkbox("Range check", &Vars::Aimbot::Melee::RangeCheck.m_Var); HelpMarker("Only aim at target if within melee range");
		//						ImGui::Checkbox("Melee prediction", &Vars::Aimbot::Melee::PredictSwing.m_Var); HelpMarker("Aimbot will attack preemptively, predicting you will be in range of the target");
		//						ImGui::Checkbox("Whip teammates", &Vars::Aimbot::Melee::WhipTeam.m_Var); HelpMarker("Aimbot will target teammates if holding the Disciplinary Action");
		//					}
		//				}
		//			}
		//			ImGui::EndChild();
		//		}
		//		if (SettingsTab == 1) {
		//			ImGui::BeginChild("Trigger");
		//			{
		//				ImGui::Columns(3);
		//				{
		//					ImGui::TextUnformatted("Global");
		//					ImGui::Checkbox("Triggerbot###gTrigger", &Vars::Triggerbot::Global::Active.m_Var); HelpMarker("Global triggerbot master switch");
		//					if (Vars::Triggerbot::Global::Active.m_Var) {
		//						InputKeybind("Trigger key", Vars::Triggerbot::Global::TriggerKey); HelpMarker("The key which activates the triggerbot");
		//						ImGui::Checkbox("Ignore invulnerable###gTriggerIgnoreInvuln", &Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var); HelpMarker("The triggerbot will ignore targets who can't be damaged");
		//						ImGui::Checkbox("Ignore cloaked###gTriggerIgnoreCloak", &Vars::Triggerbot::Global::IgnoreCloaked.m_Var); HelpMarker("The triggerbot will ignore spies who are cloaked");
		//						ImGui::Checkbox("Ignore friend###gTriggerIgnoreFriend", &Vars::Triggerbot::Global::IgnoreFriends.m_Var); HelpMarker("The triggerbot will ignore steam friends");
		//						ImGui::TextUnformatted("");
		//						ImGui::TextUnformatted("");
		//						ImGui::TextUnformatted("Auto airblast");
		//						ImGui::Checkbox("Active###gAAB", &Vars::Triggerbot::Blast::Active.m_Var); HelpMarker("Auto airblast master switch");
		//						ImGui::Checkbox("Rage mode###gAABr", &Vars::Triggerbot::Blast::Rage.m_Var); HelpMarker("Will airblast whenever possible, regardless of FoV");
		//						ImGui::Checkbox("Silent###gAABs", &Vars::Triggerbot::Blast::Silent.m_Var); HelpMarker("Aim changes made by the rage mode setting aren't visible");
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("Airblast FoV###gAABFoV", &Vars::Triggerbot::Blast::Fov.m_Var, 1, 60, "%d"); ImGui::PopItemWidth(); HelpMarker("How many degrees the auto airblast's FoV will have");
		//					}
		//				}
		//				if (Vars::Triggerbot::Global::Active.m_Var) {
		//					ImGui::NextColumn();
		//					{
		//						ImGui::TextUnformatted("Auto shoot");
		//						ImGui::Checkbox("Active###gAS", &Vars::Triggerbot::Shoot::Active.m_Var); HelpMarker("Shoots if mouse is over a target");
		//						ImGui::Checkbox("Shoot players###gASsp", &Vars::Triggerbot::Shoot::TriggerPlayers.m_Var); HelpMarker("Auto shoot will target players");
		//						ImGui::Checkbox("Shoot buildings###gASsb", &Vars::Triggerbot::Shoot::TriggerBuildings.m_Var); HelpMarker("Auto shoot will target buildings");
		//						ImGui::Checkbox("Head only###gASho", &Vars::Triggerbot::Shoot::HeadOnly.m_Var); HelpMarker("Auto shoot will only shoot if you are aiming at the head");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Head scale###gAShs", &Vars::Triggerbot::Shoot::HeadScale.m_Var, 0.5f, 1.0f, "%.1f"); ImGui::PopItemWidth(); HelpMarker("The scale at which the auto shoot will try to shoot the targets head");
		//						ImGui::Checkbox("Wait for charge###gASwfc", &Vars::Triggerbot::Shoot::WaitForCharge.m_Var); HelpMarker("Auto shoot will only shoot if the sniper is charged enough to kill in one hit / is fully charged");
		//						ImGui::TextUnformatted("");
		//						ImGui::TextUnformatted("");
		//						ImGui::TextUnformatted("Auto detonate");
		//						ImGui::Checkbox("Active###gAD", &Vars::Triggerbot::Detonate::Active.m_Var); HelpMarker("Auto detonate master switch");
		//						ImGui::Checkbox("Stickybombs###gADs", &Vars::Triggerbot::Detonate::Stickies.m_Var); HelpMarker("Detonate sticky bombs");
		//						ImGui::Checkbox("Detonator flares###gADd", &Vars::Triggerbot::Detonate::Flares.m_Var); HelpMarker("Detonate detonator flares");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Detonate radius###gADr", &Vars::Triggerbot::Detonate::RadiusScale.m_Var, 0.5f, 1.0f, "%.1f"); ImGui::PopItemWidth(); HelpMarker("The radius around the projectile that it will detonate if a player is in");
		//					}
		//					ImGui::NextColumn();
		//					{
		//						ImGui::TextUnformatted("Auto backstab");
		//						ImGui::Checkbox("Active###gABS", &Vars::Triggerbot::Stab::Active.m_Var); HelpMarker("Auto backstab will attempt to backstab the target if possible");
		//						ImGui::Checkbox("Rage mode###gABSr", &Vars::Triggerbot::Stab::RageMode.m_Var); HelpMarker("Stabs whenever possible by aiming toward the back");
		//						ImGui::Checkbox("Silent###gABSs", &Vars::Triggerbot::Stab::Silent.m_Var); HelpMarker("Aim changes made by the rage mode setting aren't visible");
		//						ImGui::Checkbox("Disguise after stab###gABSd", &Vars::Triggerbot::Stab::Disguise.m_Var); HelpMarker("Will apply the previous disguise after stabbing");
		//						ImGui::Checkbox("Ignore razorback###gABSig", &Vars::Triggerbot::Stab::IgnRazor.m_Var); HelpMarker("Will not attempt to backstab snipers wearing the razorback");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Stab range", &Vars::Triggerbot::Stab::Range.m_Var, 0.5f, 1.0f, "%.1f"); ImGui::PopItemWidth(); HelpMarker("The range at which auto backstab will attempt to stab");
		//						ImGui::TextUnformatted("");
		//						ImGui::TextUnformatted("");
		//						ImGui::TextUnformatted("Auto uber");
		//						ImGui::Checkbox("Active###gAU", &Vars::Triggerbot::Uber::Active.m_Var); HelpMarker("Auto uber master switch");
		//						ImGui::Checkbox("Only on friends###gAUf", &Vars::Triggerbot::Uber::OnlyFriends.m_Var); HelpMarker("Auto uber will only activate if healing steam friends");
		//						ImGui::Checkbox("Uber self###gAUs", &Vars::Triggerbot::Uber::PopLocal.m_Var); HelpMarker("Auto uber will activate if local player's health falls below the percentage");
		//						ImGui::Checkbox("Auto Vaccinator", &Vars::Triggerbot::Uber::AutoVacc.m_Var); HelpMarker("Choses the best resistance and pops uber if needed");
		//						if (!Vars::Triggerbot::Uber::AutoVacc.m_Var) {
		//							ImGui::PushItemWidth(100); ImGui::SliderFloat("Health left (%)###gAUhp", &Vars::Triggerbot::Uber::HealthLeft.m_Var, 1.f, 99.f, "%.0f%%", 1.0f); ImGui::PopItemWidth(); HelpMarker("The amount of health the heal target must be below to actiavte");
		//						}
		//					}
		//				}
		//			}
		//			ImGui::EndChild();
		//		}
		//		if (SettingsTab == 2) {
		//			ImGui::BeginChild("Visuals");
		//			{
		//				ImGui::Columns(3);
		//				{
		//					ImGui::TextUnformatted("Players");
		//					ImGui::Checkbox("Enable Team/Enemy colours", &Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
		//					if (ImGui::CollapsingHeader("ESP###playerESp", ImGuiTreeNodeFlags_DefaultOpen))
		//					{
		//						//ImGui::TextUnformatted("ESP");
		//						ImGui::Checkbox("Player ESP", &Vars::ESP::Players::Active.m_Var); HelpMarker("Will draw useful information/indicators on players");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Player ESP Opacity", &Vars::ESP::Players::Alpha.m_Var, 0.05f, 1.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque/transparent the ESP will be");
		//						static const char* textOutline[]{ "Off", "Text Only", "All" }; ImGui::PushItemWidth(100); ImGui::Combo("Text outline", &Vars::ESP::Main::Outlinedbar.m_Var, textOutline, IM_ARRAYSIZE(textOutline)); ImGui::PopItemWidth(); HelpMarker("Choose when to use an outline on elements drawn by ESP");
		//						ImGui::Checkbox("Local ESP", &Vars::ESP::Players::ShowLocal.m_Var); HelpMarker("Will draw ESP on local player (thirdperson)");
		//						static const char* ignoreTeammatesEsp[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###ESPteam", &Vars::ESP::Players::IgnoreTeammates.m_Var, ignoreTeammatesEsp, IM_ARRAYSIZE(ignoreTeammatesEsp)); ImGui::PopItemWidth(); HelpMarker("Which teammates the ESP will ignore drawing on");
		//						static const char* ignoreCloakedEsp[]{ "Off", "All", "Enemies only" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore cloaked###ESPcloak", &Vars::ESP::Players::IgnoreCloaked.m_Var, ignoreCloakedEsp, IM_ARRAYSIZE(ignoreCloakedEsp)); ImGui::PopItemWidth(); HelpMarker("Which cloaked spies the ESP will ignore drawing on");
		//						ImGui::Checkbox("Player name", &Vars::ESP::Players::Name.m_Var); HelpMarker("Will draw the players name");
		//						ImGui::Checkbox("Name box", &Vars::ESP::Players::NameBox.m_Var); HelpMarker("Will draw a box around players name to make it stand out");
		//						static const char* classEsp[]{ "Off", "Icon", "Text" }; ImGui::PushItemWidth(100); ImGui::Combo("Player class", &Vars::ESP::Players::Class.m_Var, classEsp, IM_ARRAYSIZE(classEsp)); ImGui::PopItemWidth(); HelpMarker("Will draw the class the player is");
		//						ImGui::Checkbox("Weapon icons", &Vars::ESP::Players::WeaponIcon.m_Var); HelpMarker("Shows the killfeed icon for the wepaon the player is holding (some weapons dont have killfeed icons and others idk the killfeed icon name for"); // CBaseCombatWeapon::GetWeaponIcon
		//						ImGui::Checkbox("Player health", &Vars::ESP::Players::Health.m_Var); HelpMarker("Will draw the players health, as well as their max health");
		//						ImGui::Checkbox("Player health bar", &Vars::ESP::Players::HealthBar.m_Var); HelpMarker("Will draw a bar visualizing how much health the player has");
		//						ImGui::Checkbox("Player conditions", &Vars::ESP::Players::Cond.m_Var); HelpMarker("Will draw what conditions the player is under");
		//						static const char* uberESP[]{ "Off", "Text", "Bar" }; ImGui::PushItemWidth(100); ImGui::Combo("Player ubercharge", &Vars::ESP::Players::Uber.m_Var, uberESP, IM_ARRAYSIZE(uberESP)); ImGui::PopItemWidth(); HelpMarker("Will draw how much ubercharge a medic has");
		//						static const char* boxESP[]{ "Off", "Simple", "Cornered", "3D" }; ImGui::PushItemWidth(100); ImGui::Combo("Player box", &Vars::ESP::Players::Box.m_Var, boxESP, IM_ARRAYSIZE(boxESP)); ImGui::PopItemWidth(); HelpMarker("What sort of box to draw on players");
		//						static const char* bonesESP[]{ "Off", "Custom colour", "Health" }; ImGui::PushItemWidth(100); ImGui::Combo("Player bones", &Vars::ESP::Players::Bones.m_Var, bonesESP, IM_ARRAYSIZE(bonesESP)); ImGui::PopItemWidth(); HelpMarker("Will draw the bone structure of the player");
		//						ImGui::Checkbox("Player GUID", &Vars::ESP::Players::GUID.m_Var); HelpMarker("Show's the players Steam ID");
		//						ImGui::Checkbox("Player lines", &Vars::ESP::Players::Lines.m_Var); HelpMarker("Draws lines from the local players position to enemies position");
		//						ImGui::Checkbox("Player Dlights", &Vars::ESP::Players::Dlights.m_Var); HelpMarker("Will make players emit a dynamic light around them");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Dlight radius", &Vars::ESP::Players::DlightRadius.m_Var, 5.0f, 400.f, "%.0f"); ImGui::PopItemWidth(); HelpMarker("How far the Dlight will illuminate");
		//					}
		//					if (ImGui::CollapsingHeader("Chams###playerChamss")) {
		//						ImGui::Checkbox("Player chams", &Vars::Chams::Players::Active.m_Var); HelpMarker("Player chams master switch");
		//						if (g_Interfaces.CVars->FindVar("mat_phong")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_specular")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_bumpmap")->GetInt() == 1) {
		//							static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel" }; ImGui::PushItemWidth(100); ImGui::Combo("Player material", &Vars::Chams::Players::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth(); HelpMarker("Which material the chams will apply to the player");
		//						}
		//						else {
		//							static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur" }; ImGui::PushItemWidth(100); ImGui::Combo("Player material", &Vars::Chams::Players::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth(); HelpMarker("Which material the chams will apply to the player");
		//						}
		//						ImGui::Checkbox("Local chams", &Vars::Chams::Players::ShowLocal.m_Var); HelpMarker("Will draw chams on local player (thirdperson");
		//						static const char* ignoreTeammatesChams[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###chamsteam", &Vars::Chams::Players::IgnoreTeammates.m_Var, ignoreTeammatesChams, IM_ARRAYSIZE(ignoreTeammatesChams)); ImGui::PopItemWidth(); HelpMarker("Which teammates the chams will ignore drawing on");
		//						ImGui::Checkbox("Chams on cosmetics", &Vars::Chams::Players::Wearables.m_Var); HelpMarker("Will draw chams on player cosmetics");
		//						ImGui::Checkbox("Chams on weapons", &Vars::Chams::Players::Weapons.m_Var); HelpMarker("Will draw chams on player weapons");
		//						ImGui::Checkbox("Chams through walls", &Vars::Chams::Players::IgnoreZ.m_Var); HelpMarker("Will draw chams on the player regardless of if the player is actually visible");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Player chams opacity", &Vars::Chams::Players::Alpha.m_Var, 0.0f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the chams are");
		//						if (ImGui::Button("Fix chams")) {
		//							g_Chams.Init();
		//						}
		//					}
		//					if (ImGui::CollapsingHeader("Glow###playerGloww")) {
		//						ImGui::Checkbox("Wireframe glow", &Vars::Glow::Main::Wireframe.m_Var); HelpMarker("Wireframe glow");
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("Global glow scale", &Vars::Glow::Main::Scale.m_Var, 1, 10, "%d"); ImGui::PopItemWidth(); HelpMarker("The scale at which the glow will render");
		//						ImGui::Checkbox("Player glow", &Vars::Glow::Players::Active.m_Var); HelpMarker("Player glow master switch");
		//						ImGui::Checkbox("Local rainbow glow", &Vars::Glow::Players::LocalRainbow.m_Var); HelpMarker("Local player glow is rainbow coloured");
		//						static const char* ignoreTeammatesGlow[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###glowteam", &Vars::Glow::Players::IgnoreTeammates.m_Var, ignoreTeammatesGlow, IM_ARRAYSIZE(ignoreTeammatesGlow)); ImGui::PopItemWidth(); HelpMarker("Which teammates the glow will ignore drawing on");
		//						ImGui::Checkbox("Glow on cosmetics", &Vars::Glow::Players::Wearables.m_Var); HelpMarker("Will draw glow on player cosmetics");
		//						ImGui::Checkbox("Glow on weapons", &Vars::Glow::Players::Weapons.m_Var); HelpMarker("Will draw glow on player weapons");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Player glow opacity", &Vars::Glow::Players::Alpha.m_Var, 0.1f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the glow is");
		//						static const char* colorGlow[]{ "Team", "Health" }; ImGui::PushItemWidth(100); ImGui::Combo("Player glow colour", &Vars::Glow::Players::Color.m_Var, colorGlow, IM_ARRAYSIZE(colorGlow)); ImGui::PopItemWidth(); HelpMarker("Which colour the glow will draw");
		//					}
		//					if (ImGui::CollapsingHeader("DME chams")) {
		//						ImGui::Checkbox("DME chams###dmeactive", &Vars::Chams::DME::Active.m_Var); HelpMarker("DME chams master switch");
		//						static const char* handsMaterial[]{
		//							"Original",
		//							"Shaded",
		//							"Shiny",
		//							"Flat",
		//							"Wireframe shaded",
		//							"Wireframe shiny",
		//							"Wireframe flat",
		//							"Fresnel",
		//							"Brick"
		//						};
		//						ImGui::PushItemWidth(100);
		//						ImGui::Combo("Arm material", &Vars::Chams::DME::Hands.m_Var, handsMaterial, IM_ARRAYSIZE(handsMaterial));
		//						ImGui::PopItemWidth();
		//						HelpMarker("What material to put on your viewmodels arms/hands");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Arm opacity", &Vars::Chams::DME::HandsAlpha.m_Var, 0.0f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque your viewmodel arms will appear");
		//						ImGui::Checkbox("Arm glow overlay", &Vars::Chams::DME::HandsGlowOverlay.m_Var); HelpMarker("Will place a second glow-like material overlayed on top of the original material");
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("Arm glow boost", &Vars::Chams::DME::HandsRimMultiplier.m_Var, 1, 100, "%d"); ImGui::PopItemWidth(); HelpMarker("How much the glow effect will be boosted by");
		//						static const char* weaponMaterial[]{
		//							"Original",
		//							"Shaded",
		//							"Shiny",
		//							"Flat",
		//							"Wireframe shaded",
		//							"Wireframe shiny",
		//							"Wireframe flat",
		//							"Fresnel",
		//							"Brick"
		//						};
		//						ImGui::PushItemWidth(100);
		//						ImGui::Combo("Weapon material", &Vars::Chams::DME::Weapon.m_Var, weaponMaterial, IM_ARRAYSIZE(weaponMaterial));
		//						ImGui::PopItemWidth();
		//						HelpMarker("What material to put on your viewmodels weapon");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Weapon opacity", &Vars::Chams::DME::WeaponAlpha.m_Var, 0.0f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque your viewmodel arms will appear");
		//						ImGui::Checkbox("Weapon glow overlay", &Vars::Chams::DME::WeaponGlowOverlay.m_Var); HelpMarker("Will place a second glow-like material overlayed on top of the original material");
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("Weapon glow boost", &Vars::Chams::DME::WeaponRimMultiplier.m_Var, 1, 100, "%d"); ImGui::PopItemWidth(); HelpMarker("How much the glow effect will be boosted by");
		//					}
		//				}
		//				ImGui::NextColumn();
		//				{
		//					ImGui::TextUnformatted("Buildings");
		//					if (ImGui::CollapsingHeader("ESP###buildingESP", ImGuiTreeNodeFlags_DefaultOpen)) {
		//						ImGui::Checkbox("Building ESP", &Vars::ESP::Buildings::Active.m_Var); HelpMarker("Will draw useful information/indicators on buildings");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Building ESP Opacity", &Vars::ESP::Buildings::Alpha.m_Var, 0.05f, 1.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque/transparent the ESP will be");
		//						ImGui::Checkbox("Ignore team buildings###ESPbuildingsteam", &Vars::ESP::Buildings::IgnoreTeammates.m_Var); HelpMarker("Whether or not to draw ESP on your teams buildings");
		//						ImGui::Checkbox("Building name", &Vars::ESP::Buildings::Name.m_Var); HelpMarker("Will draw the players name");
		//						ImGui::Checkbox("Name box###buildingnamebox", &Vars::ESP::Buildings::NameBox.m_Var); HelpMarker("Will draw a box around the buildings name to make it stand out");
		//						ImGui::Checkbox("Building health", &Vars::ESP::Buildings::Health.m_Var); HelpMarker("Will draw the building's health, as well as its max health");
		//						ImGui::Checkbox("Building health bar", &Vars::ESP::Buildings::HealthBar.m_Var); HelpMarker("Will draw a bar visualizing how much health the building has");
		//						ImGui::Checkbox("Building conditions", &Vars::ESP::Buildings::Cond.m_Var); HelpMarker("Will draw what conditions the building is under");
		//						ImGui::Checkbox("Building level", &Vars::ESP::Buildings::Level.m_Var); HelpMarker("Will draw what level the building is");
		//						ImGui::Checkbox("Building owner", &Vars::ESP::Buildings::Owner.m_Var); HelpMarker("Shows who built the building");
		//						static const char* boxESPb[]{ "Off", "Simple", "Cornered", "3D" }; ImGui::PushItemWidth(100); ImGui::Combo("Building box", &Vars::ESP::Buildings::Box.m_Var, boxESPb, IM_ARRAYSIZE(boxESPb)); ImGui::PopItemWidth(); HelpMarker("What sort of box to draw on buildings");
		//						ImGui::Checkbox("Building lines", &Vars::ESP::Buildings::Lines.m_Var); HelpMarker("Draws lines from the local players position to the buildings position");
		//						ImGui::Checkbox("Building Dlights", &Vars::ESP::Buildings::Dlights.m_Var); HelpMarker("Will make buildings emit a dynamic light around them, although buildings can't move some I'm not sure that the lights are actually dynamic here...");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Dlight radius###buildingsdlights", &Vars::ESP::Buildings::DlightRadius.m_Var, 5.0f, 400.f, "%.0f"); ImGui::PopItemWidth(); HelpMarker("How far the Dlight will illuminate");
		//					}
		//					if (ImGui::CollapsingHeader("Chams###buildingchamss")) {
		//						ImGui::Checkbox("Building chams", &Vars::Chams::Buildings::Active.m_Var); HelpMarker("Building chams master switch");
		//						if (g_Interfaces.CVars->FindVar("mat_phong")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_specular")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_bumpmap")->GetInt() == 1) {
		//							static const char* pchamsbMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel" }; ImGui::PushItemWidth(100); ImGui::Combo("Building material", &Vars::Chams::Buildings::Material.m_Var, pchamsbMaterials, IM_ARRAYSIZE(pchamsbMaterials)); ImGui::PopItemWidth(); HelpMarker("Which material the chams will apply to the player");
		//						}
		//						else {
		//							static const char* pchamsbMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur" }; ImGui::PushItemWidth(100); ImGui::Combo("Building material", &Vars::Chams::Buildings::Material.m_Var, pchamsbMaterials, IM_ARRAYSIZE(pchamsbMaterials)); ImGui::PopItemWidth(); HelpMarker("Which material the chams will apply to the player");
		//						}
		//						ImGui::Checkbox("Ignore team buildings###Chamsbuildingsteam", &Vars::Chams::Buildings::IgnoreTeammates.m_Var);  HelpMarker("Whether or not to draw chams on your teams buildings");
		//						ImGui::Checkbox("Chams through walls###buildingsignorez", &Vars::Chams::Buildings::IgnoreZ.m_Var); HelpMarker("Will draw chams on the building regardless of if the building is actually visible");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Building chams opacity", &Vars::Chams::Buildings::Alpha.m_Var, 0.0f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the chams are");
		//					}
		//					if (ImGui::CollapsingHeader("Glow###buildinggloww")) {
		//						ImGui::Checkbox("Building glow", &Vars::Glow::Buildings::Active.m_Var); HelpMarker("Player glow master switch");
		//						ImGui::Checkbox("Ignore team buildings###glowbuildingsteam", &Vars::Glow::Buildings::IgnoreTeammates.m_Var);  HelpMarker("Whether or not to draw glow on your teams buildings");
		//						//static const char* ignoreTeammatesGlow[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###glowteam", &Vars::Glow::Buildings::IgnoreTeammates.m_Var, ignoreTeammatesGlow, IM_ARRAYSIZE(ignoreTeammatesGlow)); ImGui::PopItemWidth(); HelpMarker("Which teammates the glow will ignore drawing on");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Building glow opacity", &Vars::Glow::Buildings::Alpha.m_Var, 0.1f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the glow is");
		//						static const char* colorGlow[]{ "Team", "Health" }; ImGui::PushItemWidth(100); ImGui::Combo("Building glow colour", &Vars::Glow::Buildings::Color.m_Var, colorGlow, IM_ARRAYSIZE(colorGlow)); ImGui::PopItemWidth(); HelpMarker("Which colour the glow will draw");
		//					}
		//				}
		//				ImGui::NextColumn();
		//				{
		//					const char* skyNames[] = {
		//						"Custom",
		//						"sky_tf2_04",
		//						"sky_upward",
		//						"sky_dustbowl_01",
		//						"sky_goldrush_01",
		//						"sky_granary_01",
		//						"sky_well_01",
		//						"sky_gravel_01",
		//						"sky_badlands_01",
		//						"sky_hydro_01",
		//						"sky_night_01",
		//						"sky_nightfall_01",
		//						"sky_trainyard_01",
		//						"sky_stormfront_01",
		//						"sky_morningsnow_01",
		//						"sky_alpinestorm_01",
		//						"sky_harvest_01",
		//						"sky_harvest_night_01",
		//						"sky_halloween",
		//						"sky_halloween_night_01",
		//						"sky_halloween_night2014_01",
		//						"sky_island_01",
		//						"sky_rainbow_01"
		//					};
		//					ImGui::TextUnformatted("World");
		//					if (ImGui::CollapsingHeader("ESP###worldesp", ImGuiTreeNodeFlags_DefaultOpen)) {
		//						ImGui::Checkbox("Pickups ESP", &Vars::ESP::World::Active.m_Var); HelpMarker("Pickup ESP master switch");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Pickups ESP Opacity", &Vars::ESP::Players::Alpha.m_Var, 0.05f, 1.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque/transparent the ESP will be");
		//						ImGui::Checkbox("Healthpack ESP", &Vars::ESP::World::HealthText.m_Var); HelpMarker("Will draw ESP on health packs");
		//						ImGui::Checkbox("Ammopack ESP", &Vars::ESP::World::AmmoText.m_Var); HelpMarker("Will draw ESP on ammo packs");
		//					}
		//					if (ImGui::CollapsingHeader("Chams###chamsworld")) {
		//						ImGui::Checkbox("Pickup chams", &Vars::Chams::World::Active.m_Var); HelpMarker("Pickup chams master switch");
		//						ImGui::Checkbox("Healthpack chams", &Vars::Chams::World::Health.m_Var); HelpMarker("Will draw chams on health packs");
		//						ImGui::Checkbox("Ammopack chams", &Vars::Chams::World::Ammo.m_Var); HelpMarker("Will draw chams on ammo packs");
		//						static const char* projectilesTeam[]{ "Off", "All", "Enemy Only" }; ImGui::PushItemWidth(100); ImGui::Combo("Projectile chams", &Vars::Chams::World::Projectiles.m_Var, projectilesTeam, IM_ARRAYSIZE(projectilesTeam)); ImGui::PopItemWidth(); HelpMarker("When to draw chams on projectiles");
		//						//Gui::Checkbox("", &Vars::Chams::World::Projectiles.m_Var);
		//						if (g_Interfaces.CVars->FindVar("mat_phong")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_specular")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_bumpmap")->GetInt() == 1) {
		//							static const char* pchamspMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel" }; ImGui::PushItemWidth(100); ImGui::Combo("Pickup chams material", &Vars::Chams::World::Material.m_Var, pchamspMaterials, IM_ARRAYSIZE(pchamspMaterials)); ImGui::PopItemWidth(); HelpMarker("Which material the chams will apply to the pickups");
		//						}
		//						else {
		//							static const char* pchamspMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur" }; ImGui::PushItemWidth(100); ImGui::Combo("Pickup chams material", &Vars::Chams::World::Material.m_Var, pchamspMaterials, IM_ARRAYSIZE(pchamspMaterials)); ImGui::PopItemWidth(); HelpMarker("Which material the chams will apply to the pickups");
		//						}
		//						//ImGui::Checkbox("", &Vars::Chams::World::Material.m_Var);
		//						ImGui::Checkbox("Chams through walls###pickupsignorez", &Vars::Chams::World::IgnoreZ.m_Var); HelpMarker("Will draw chams on pickups through walls");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Pickup chams opacity", &Vars::Chams::World::Alpha.m_Var, 0.0f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the chams are");
		//					}
		//					if (ImGui::CollapsingHeader("Glow###glowworldd")) {
		//						ImGui::Checkbox("Pickup glow", &Vars::Glow::World::Active.m_Var); HelpMarker("Pickup glow master switch");
		//						ImGui::Checkbox("Healthpack glow", &Vars::Glow::World::Health.m_Var); HelpMarker("Will draw glow on health packs");
		//						ImGui::Checkbox("Ammopack glow", &Vars::Glow::World::Ammo.m_Var); HelpMarker("Will draw glow on ammo packs");
		//						static const char* projectilesgTeam[]{ "Off", "All", "Enemy Only" }; ImGui::PushItemWidth(100); ImGui::Combo("Projectile glow", &Vars::Glow::World::Projectiles.m_Var, projectilesgTeam, IM_ARRAYSIZE(projectilesgTeam)); ImGui::PopItemWidth(); HelpMarker("When to draw glow on projectiles");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Pickup glow opacity", &Vars::Glow::World::Alpha.m_Var, 0.1f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the glow is");
		//					}
		//					if (ImGui::CollapsingHeader("Miscellaneous")) {
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("Field of view", &Vars::Visuals::FieldOfView.m_Var, 70, 150, "%d"); ImGui::PopItemWidth(); HelpMarker("How many degrees of field of vision you would like");
		//						//ImGui::PushItemWidth(100); ImGui::SliderInt("Aimbot FoV circle alpha", &Vars::Visuals::AimFOVAlpha.m_Var, 0, 255, "%d"); ImGui::PopItemWidth(); HelpMarker("How opaque the aimbot's FoV circle is");
		//						//ImGui::PushItemWidth(100); ImGui::SliderFloat("Prop opacity", &Vars::Visuals::PropAlpha.m_Var, 0.01f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the glow is");
		//						// ok having a slider for this is as bad as the aimbot fov alpha, we have it as a colour, set the prop alpha, to Color::PropModulation.a (or whatever its called)
		//						//	i have no idea where i'm supposed to put it so i just said fuck the var and send Colors::StaticPropModulation.a to the hook
		//						const char* visionModifiers[]{ "Off", "Pyrovision", "Halloween", "Romevision" }; ImGui::PushItemWidth(100); ImGui::Combo("Vision modifiers", &Vars::Visuals::Vision.m_Var, visionModifiers, IM_ARRAYSIZE(visionModifiers)); ImGui::PopItemWidth(); HelpMarker("Vision modifiers");
		//						{
		//							ImGui::PushItemWidth(100);
		//							std::vector<std::string> modulationvec;
		//							static bool modulationFlags[]{ Vars::Visuals::WorldModulation.m_Var,Vars::Visuals::SkyModulation.m_Var,Vars::Visuals::PropWireframe.m_Var };
		//							const char* pmodulation[] = { "World", "Sky", "Prop Wireframe" }; static std::string modulationPreview = "PH";
		//							if (modulationPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
		//								modulationPreview = "";
		//								for (size_t i = 0; i < IM_ARRAYSIZE(pmodulation); i++) {
		//									if (modulationFlags[i])
		//										modulationvec.push_back(pmodulation[i]);
		//								}
		//								for (size_t i = 0; i < modulationvec.size(); i++)
		//								{
		//									if (modulationvec.size() == 1)
		//										modulationPreview += modulationvec.at(i);
		//									else if (!(i == modulationvec.size() - 1))
		//										modulationPreview += modulationvec.at(i) + ", ";
		//									else
		//										modulationPreview += modulationvec.at(i);
		//								}
		//							}
		//							if (ImGui::BeginCombo("World", modulationPreview.c_str()))
		//							{
		//								modulationPreview = "";
		//								for (size_t i = 0; i < IM_ARRAYSIZE(pmodulation); i++)
		//								{
		//									ImGui::Selectable(pmodulation[i], &modulationFlags[i]);
		//									if (modulationFlags[i])
		//										modulationvec.push_back(pmodulation[i]);
		//								}
		//								for (size_t i = 0; i < modulationvec.size(); i++)
		//								{
		//									if (modulationvec.size() == 1)
		//										modulationPreview += modulationvec.at(i);
		//									else if (!(i == modulationvec.size() - 1))
		//										modulationPreview += modulationvec.at(i) + ", ";
		//									else
		//										modulationPreview += modulationvec.at(i);
		//								}
		//								ImGui::EndCombo();
		//							}
		//							for (size_t i = 0; i < IM_ARRAYSIZE(modulationFlags); i++) {
		//								if (modulationFlags[i]) {
		//									switch (i + 1) {
		//									case 1: { Vars::Visuals::WorldModulation.m_Var = true; break; }
		//									case 2: { Vars::Visuals::SkyModulation.m_Var = true; break; }
		//									case 3: { Vars::Visuals::PropWireframe.m_Var = true; break; }
		//									}
		//								}
		//								else {
		//									switch (i + 1) {
		//									case 1: { Vars::Visuals::WorldModulation.m_Var = false; break; }
		//									case 2: { Vars::Visuals::SkyModulation.m_Var = false; break; }
		//									case 3: { Vars::Visuals::PropWireframe.m_Var = false; break; }
		//									}
		//								}
		//							}
		//						}
		//						{
		//							ImGui::PushItemWidth(100);
		//							std::vector<std::string> removalsvec;
		//							static bool removalFlags[6]{ Vars::Visuals::RemoveScope.m_Var,Vars::Visuals::RemoveZoom.m_Var,Vars::Visuals::RemoveDisguises.m_Var,Vars::Visuals::RemoveTaunts.m_Var,Vars::Misc::DisableInterpolation.m_Var,Vars::Visuals::RemovePunch.m_Var };
		//							const char* pRemovals[] = { "Scope", "Zoom", "Disguises", "Taunts", "Interpolation", "View Punch" }; static std::string removalsPreview = "PH";
		//							if (removalsPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
		//								removalsPreview = "";
		//								for (size_t i = 0; i < IM_ARRAYSIZE(pRemovals); i++) {
		//									if (removalFlags[i])
		//										removalsvec.push_back(pRemovals[i]);
		//								}
		//								for (size_t i = 0; i < removalsvec.size(); i++)
		//								{
		//									if (removalsvec.size() == 1)
		//										removalsPreview += removalsvec.at(i);
		//									else if (!(i == removalsvec.size() - 1))
		//										removalsPreview += removalsvec.at(i) + ", ";
		//									else
		//										removalsPreview += removalsvec.at(i);
		//								}
		//							}
		//							if (ImGui::BeginCombo("Removals", removalsPreview.c_str()))
		//							{
		//								removalsPreview = "";
		//								for (size_t i = 0; i < IM_ARRAYSIZE(pRemovals); i++)
		//								{
		//									ImGui::Selectable(pRemovals[i], &removalFlags[i]);
		//									if (removalFlags[i])
		//										removalsvec.push_back(pRemovals[i]);
		//								}
		//								for (size_t i = 0; i < removalsvec.size(); i++)
		//								{
		//									if (removalsvec.size() == 1)
		//										removalsPreview += removalsvec.at(i);
		//									else if (!(i == removalsvec.size() - 1))
		//										removalsPreview += removalsvec.at(i) + ", ";
		//									else
		//										removalsPreview += removalsvec.at(i);
		//								}
		//								ImGui::EndCombo();
		//							}
		//							for (size_t i = 0; i < IM_ARRAYSIZE(removalFlags); i++) {
		//								if (removalFlags[i]) {
		//									switch (i + 1) {
		//									case 1: { Vars::Visuals::RemoveScope.m_Var = true; break; }
		//									case 2: { Vars::Visuals::RemoveZoom.m_Var = true; break; }
		//									case 3: { Vars::Visuals::RemoveDisguises.m_Var = true; break; }
		//									case 4: { Vars::Visuals::RemoveTaunts.m_Var = true; break; }
		//									case 5: { Vars::Misc::DisableInterpolation.m_Var = true; break; }
		//									case 6: { Vars::Visuals::RemovePunch.m_Var = true; break; }
		//									}
		//								}
		//								else {
		//									switch (i + 1) {
		//									case 1: { Vars::Visuals::RemoveScope.m_Var = false; break; }
		//									case 2: { Vars::Visuals::RemoveZoom.m_Var = false; break; }
		//									case 3: { Vars::Visuals::RemoveDisguises.m_Var = false; break; }
		//									case 4: { Vars::Visuals::RemoveTaunts.m_Var = false; break; }
		//									case 5: { Vars::Misc::DisableInterpolation.m_Var = false; break; }
		//									case 6: { Vars::Visuals::RemovePunch.m_Var = false; break; }
		//									}
		//								}
		//							}
		//						}
		//						
		//						{
		//							ImGui::PushItemWidth(100);
		//							std::vector<std::string> predictionsvec;
		//							static bool predictionFlags[]{ Vars::Visuals::CrosshairAimPos.m_Var,Vars::Visuals::AimPosSquare.m_Var,Vars::Visuals::BulletTracer.m_Var,Vars::Visuals::AimbotViewmodel.m_Var };
		//							const char* pPredictions[] = { "Aimbot Crosshair", "Render Proj Line", "Bullet Tracers", "Viewmodel Aimbot" }; static std::string predictionsPreview = "PH";
		//							if (predictionsPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
		//								predictionsPreview = "";
		//								for (size_t i = 0; i < IM_ARRAYSIZE(pPredictions); i++) {
		//									if (predictionFlags[i])
		//										predictionsvec.push_back(pPredictions[i]);
		//								}
		//								for (size_t i = 0; i < predictionsvec.size(); i++)
		//								{
		//									if (predictionsvec.size() == 1)
		//										predictionsPreview += predictionsvec.at(i);
		//									else if (!(i == predictionsvec.size() - 1))
		//										predictionsPreview += predictionsvec.at(i) + ", ";
		//									else
		//										predictionsPreview += predictionsvec.at(i);
		//								}
		//							}
		//							if (ImGui::BeginCombo("Prediction", predictionsPreview.c_str()))
		//							{
		//								predictionsPreview = "";
		//								for (size_t i = 0; i < IM_ARRAYSIZE(pPredictions); i++)
		//								{
		//									ImGui::Selectable(pPredictions[i], &predictionFlags[i]);
		//									if (predictionFlags[i])
		//										predictionsvec.push_back(pPredictions[i]);
		//								}
		//								for (size_t i = 0; i < predictionsvec.size(); i++)
		//								{
		//									if (predictionsvec.size() == 1)
		//										predictionsPreview += predictionsvec.at(i);
		//									else if (!(i == predictionsvec.size() - 1))
		//										predictionsPreview += predictionsvec.at(i) + ", ";
		//									else
		//										predictionsPreview += predictionsvec.at(i);
		//								}
		//								ImGui::EndCombo();
		//							} // i got tired of trying better ways so this is new method fr*ck you
		//							for (size_t i = 0; i < IM_ARRAYSIZE(predictionFlags); i++) {
		//								if (predictionFlags[i]) {
		//									switch (i + 1) {
		//									case 1: { Vars::Visuals::CrosshairAimPos.m_Var = true; break; }
		//									case 2: { Vars::Visuals::AimPosSquare.m_Var = true; break; }
		//									case 3: { Vars::Visuals::BulletTracer.m_Var = true; break; }
		//									case 4: { Vars::Visuals::AimbotViewmodel.m_Var = true; break; }
		//									}
		//								}
		//								else {
		//									switch (i + 1) {
		//									case 1: { Vars::Visuals::CrosshairAimPos.m_Var = false; break; }
		//									case 2: { Vars::Visuals::AimPosSquare.m_Var = false; break; }
		//									case 3: { Vars::Visuals::BulletTracer.m_Var = false; break; }
		//									case 4: { Vars::Visuals::AimbotViewmodel.m_Var = false; break; }
		//									}
		//								}
		//							}
		//						}
		//						static const char* bullettracers[]{ "Off", "Machina", "C.A.P.P.E.R", "Short Circuit", "Merasmus ZAP", "Merasmus ZAP Beam 2", "Big Nasty", "Distortion Trail", "Black Ink", "Custom" }; ImGui::PushItemWidth(100); ImGui::Combo("Particle tracer", &Vars::Visuals::ParticleTracer.m_Var, bullettracers, IM_ARRAYSIZE(bullettracers)); ImGui::PopItemWidth();
		//						if (Vars::Visuals::ParticleTracer.m_Var == 9) {
		//							ImGui::PushItemWidth(100); ImGui::InputText("Custom Tracer", &Vars::Visuals::ParticleName); ImGui::PopItemWidth(); HelpMarker("If you want to use a custom particle tracer");
		//						}
		//						if (Vars::Visuals::BulletTracer.m_Var) {
		//							ImGui::Checkbox("Rainbow tracers", &Vars::Visuals::BulletTracerRainbow.m_Var); HelpMarker("Bullet tracer color will be dictated by a changing color");
		//						}
		//						if (Vars::Visuals::RemoveScope.m_Var) {
		//							ImGui::Checkbox("Noscope lines", &Vars::Visuals::ScopeLines.m_Var); HelpMarker("Will draw a custom overlay");
		//						}
		//						ImGui::Checkbox("Draw Hitboxes", &Vars::Aimbot::Global::showHitboxes.m_Var); HelpMarker("Shows client hitboxes for enemies once they are attacked (not bbox)");
		//						if (Vars::Aimbot::Global::showHitboxes.m_Var) {
		//							ImGui::Checkbox("Clear Hitboxes", &Vars::Aimbot::Global::clearPreviousHitbox.m_Var); HelpMarker("Removes previous drawn hitboxes to mitigate clutter");
		//							ImGui::PushItemWidth(100); ImGui::SliderInt("Hitbox Draw Time", &Vars::Aimbot::Global::hitboxTime.m_Var, 1, 5); HelpMarker("Removes previous drawn hitboxes after n seconds");
		//						}
		//						ImGui::TextUnformatted("");
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("VM Off X", &Vars::Visuals::VMOffX.m_Var, -90, 90);
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("VM Off Y", &Vars::Visuals::VMOffY.m_Var, -90, 90);
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("VM Off Z", &Vars::Visuals::VMOffZ.m_Var, -90, 90);
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("VM Roll", &Vars::Visuals::VMRoll.m_Var, -180, 180);
		//						ImGui::TextUnformatted("");
		//						ImGui::Checkbox("Thirdperson", &Vars::Visuals::ThirdPerson.m_Var); HelpMarker("Will move your camera to be in a thirdperson view");
		//						InputKeybind("Thirdperson key", Vars::Visuals::ThirdPersonKey); HelpMarker("What key to toggle thirdperson, press ESC if no bind is desired");
		//						ImGui::Checkbox("Thirdperson show real angles", &Vars::Visuals::ThirdPersonSilentAngles.m_Var); HelpMarker("Will show your real angles on thirdperson (not what others see)");
		//						ImGui::Checkbox("Thirdperson instant yaw", &Vars::Visuals::ThirdPersonInstantYaw.m_Var); HelpMarker("Will set your yaw instantly in thirdperson, showing your actual angle, instead of what others see");
		//						ImGui::Checkbox("Thirdperson show server hitboxes", &Vars::Visuals::ThirdPersonServerHitbox.m_Var); HelpMarker("Will show the server angles in thirdperson");
		//						ImGui::TextUnformatted("");
		//						ImGui::Checkbox("Skybox changer", &Vars::Visuals::SkyboxChanger.m_Var); HelpMarker("Will change the skybox, either to a base TF2 one or a custom one");
		//						ImGui::PushItemWidth(100); ImGui::Combo("Skybox", &Vars::Skybox::SkyboxNum, skyNames, IM_ARRAYSIZE(skyNames), 6);  ImGui::PopItemWidth();
		//						ImGui::PushItemWidth(100); ImGui::InputText("Custom skybox", &Vars::Skybox::SkyboxName); ImGui::PopItemWidth(); HelpMarker("If you want to load a custom skybox, type it here (tf/materials/skybox)");
		//						ImGui::TextUnformatted("");
		//						ImGui::Checkbox("World Textures Override", &Vars::Visuals::OverrideWorldTextures.m_Var); HelpMarker("Turn this off when in-game so you don't drop fps :p");
		//					}
		//				}
		//			}
		//			ImGui::EndChild();
		//		}
		//		if (SettingsTab == 3) {
		//			//Misc
		//			ImGui::BeginChild("Misc");
		//			{
		//				ImGui::Columns(3);
		//				{
		//					if (ImGui::CollapsingHeader("Miscellanious features", ImGuiTreeNodeFlags_DefaultOpen)) {
		//						ImGui::Checkbox("Bunnyhop", &Vars::Misc::AutoJump.m_Var); HelpMarker("Will jump as soon as you touch the ground again, keeping speed between jumps");
		//						const char* autoStrafeModes[]{ "Off", "Normal", "Directional" }; ImGui::PushItemWidth(100); ImGui::Combo("Autostrafer", &Vars::Misc::AutoStrafe.m_Var, autoStrafeModes, IM_ARRAYSIZE(autoStrafeModes)); ImGui::PopItemWidth(); HelpMarker("Will strafe for you in air automatically so that you gain speed");
		//						ImGui::Checkbox("Edge jump", &Vars::Misc::EdgeJump.m_Var); HelpMarker("Will jump at the very end of whatever platform you're on, allowing you to perfectly make longer jumps.");
		//						if (Vars::Misc::EdgeJump.m_Var) {
		//							InputKeybind("Edge jump key", Vars::Misc::EdgeJumpKey); HelpMarker("Edge jump bind, leave as None for always on");
		//						}
		//						const char* specModes[]{ "Off", "Draggable", "Static", "Static + Avatars" }; ImGui::PushItemWidth(100); ImGui::Combo("Spectator list", &Vars::Visuals::SpectatorList.m_Var, specModes, IM_ARRAYSIZE(specModes)); ImGui::PopItemWidth(); HelpMarker("Will show who is currently spectating you");
		//						ImGui::Checkbox("Taunt slide", &Vars::Misc::TauntSlide.m_Var); HelpMarker("Allows you to input in taunts");
		//						ImGui::Checkbox("Taunt control", &Vars::Misc::TauntControl.m_Var); HelpMarker("Gives full control if enabled with taunt slide");
		//						ImGui::Checkbox("Bypass pure", &Vars::Misc::BypassPure.m_Var); HelpMarker("Allows you to load any custom files, even if disallowed by the sv_pure setting");
		//						ImGui::Checkbox("Medal flip", &Vars::Misc::MedalFlip.m_Var); HelpMarker("Medal go spinny spinny weeeeeee");
		//						ImGui::Checkbox("Noisemaker spam", &Vars::Misc::NoisemakerSpam.m_Var); HelpMarker("Will spam your noisemaker without using its charges");
		//						ImGui::Checkbox("Auto rocketjump", &Vars::Misc::AutoRocketJump.m_Var); HelpMarker("Will rocket jump at the angle you're looking at when you press mouse2 with a rocket launcher");
		//						// ImGui::Checkbox("Chat spam", &Vars::Misc::ChatSpam.m_Var); HelpMarker("Spam the chat with Fedoraware adverts");
		//						const char* spamModes[]{ "Off", "Fedoraware", "Lmaobox", "Cathook" }; ImGui::PushItemWidth(100); ImGui::Combo("Chat spam", &Vars::Misc::ChatSpam.m_Var, spamModes, IM_ARRAYSIZE(spamModes)); ImGui::PopItemWidth(); HelpMarker("Spams the chat with the chosen chat spam");
		//						ImGui::Checkbox("No push", &Vars::Misc::NoPush.m_Var); HelpMarker("Will make teammates unable to push you around");
		//						const char* rollModes[]{ "Off", "Backwards", "Fake forward" }; ImGui::PushItemWidth(100); ImGui::Combo("Crouch speed", &Vars::Misc::Roll.m_Var, rollModes, IM_ARRAYSIZE(rollModes)); ImGui::PopItemWidth(); HelpMarker("Allows you to go at normal walking speed when crouching (affects many things, use with caution)");
		//						ImGui::Checkbox("Show class changes", &Vars::Visuals::ChatInfo.m_Var); HelpMarker("Will say when people change class in chat");
		//						ImGui::Checkbox("Vote revealer", &Vars::Misc::VoteRevealer.m_Var); HelpMarker("Will say who voted F1 or F2 in chat");
		//						ImGui::Checkbox("Votes to party", &Vars::Misc::VotesInChat.m_Var); HelpMarker("Will send vote information to party chat (use with caution)");
		//						const char* logModes[]{ "Off", "Chat", "Text" }; ImGui::PushItemWidth(100); ImGui::Combo("Damage logger", &Vars::Visuals::damageLogger.m_Var, logModes, IM_ARRAYSIZE(logModes)); ImGui::PopItemWidth(); HelpMarker("Will log any damage you deal to players");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Damage logger time", &Vars::Visuals::despawnTime.m_Var, 0.5f, 10.f, "%.1f"); ImGui::PopItemWidth(); HelpMarker("How many ticks the damage logger text will stay");
		//						ImGui::Checkbox("Anti-AFK", &Vars::Misc::AntiAFK.m_Var); HelpMarker("Will make you jump every now and again so you don't get kicked for idling");
		//						ImGui::Checkbox("Force sv_cheats", &Vars::Misc::CheatsBypass.m_Var); HelpMarker("Will force sv_cheats 1, allowing commands like tf_viewmodels_offset_override, fog_override");
		//						ImGui::Checkbox("Be marked as a cat", &Vars::Misc::BeCat.m_Var); HelpMarker("Will mark you as a cathook instance to other cathook instances (basically catbots)");
		//						ImGui::Checkbox("Chat censor", &Vars::Misc::ChatCensor.m_Var); HelpMarker("Clears the chat when someone accuses you");
		//						ImGui::Checkbox("Menu tooltips", &tooltips); HelpMarker("Will enable/disable these");
		//						ImGui::Checkbox("Menu snow", &Vars::Visuals::Snow.m_Var); HelpMarker("Enable the snow when menu is open");
		//						ImGui::Checkbox("Rage Retry", &Vars::Misc::RageRetry.m_Var); HelpMarker("Will automatically reconnect when your health is low");
		//						if (Vars::Misc::RageRetry.m_Var) {
		//							ImGui::PushItemWidth(100); ImGui::SliderInt("Rage Retry health", &Vars::Misc::RageRetryHealth.m_Var, 1, 99); HelpMarker("Minimum health in % that will cause a retry");
		//						}
		//						//ImGui::Checkbox("MVM Instant Respawn", &Vars::Misc::MVMRes.m_Var);
		//					}
		//					if (ImGui::CollapsingHeader("Out of FoV arrows")) {
		//						ImGui::Checkbox("Active###fovar", &Vars::Visuals::OutOfFOVArrows.m_Var); HelpMarker("Will draw arrows to players who are outside of the range of your FoV");
		//						ImGui::Checkbox("Outline arrows###ballsinyourjawballsninjafart", &Vars::Visuals::OutOfFOVArrowsOutline.m_Var); HelpMarker("16 missed calls");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Arrow length", &Vars::Visuals::ArrowLength.m_Var, 5.f, 50.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How long the arrows are");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Arrow angle", &Vars::Visuals::ArrowAngle.m_Var, 5.f, 180.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("The angle of the arrow");
		//						//ImGui::PushItemWidth(100); ImGui::SliderFloat("Arrow range", &Vars::Visuals::ScreenRange.m_Var, 1.1f, 4.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How far on the screen the arrows will go");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Max distance", &Vars::Visuals::MaxDist.m_Var, 0.f, 4000.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How far until the arrows will not show");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Min distance", &Vars::Visuals::MinDist.m_Var, 0.f, 1000.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How close until the arrows will be fully opaque");
		//					}
		//					if (ImGui::CollapsingHeader("Spy warning")) {
		//						ImGui::Checkbox("Active###spywarn", &Vars::Visuals::SpyWarning.m_Var); HelpMarker("Will alert you when spies with their knife out may attempt to backstab you");
		//						ImGui::Checkbox("Voice command###spywarn1", &Vars::Visuals::SpyWarningAnnounce.m_Var); HelpMarker("Will make your character say \"Spy!\" when a spy is detected");
		//						ImGui::Checkbox("Visible only###spywarn2", &Vars::Visuals::SpyWarningVisibleOnly.m_Var); HelpMarker("Will only alert you to visible spies");
		//						ImGui::Checkbox("Ignore friends###spywarn3", &Vars::Visuals::SpyWarningIgnoreFriends.m_Var); HelpMarker("Will ignore spies who are on your friends list");
		//						const char* spyWmodes[]{ "Arrow", "Flash" }; ImGui::PushItemWidth(100); ImGui::Combo("Warning style", &Vars::Visuals::SpyWarningStyle.m_Var, spyWmodes, IM_ARRAYSIZE(spyWmodes)); ImGui::PopItemWidth();
		//					}
		//					if (ImGui::CollapsingHeader("Discord")) {
		//						ImGui::Checkbox("Enable Discord RPC", &Vars::Misc::Discord::EnableRPC.m_Var); HelpMarker("Enable Discord Rich Presence");
		//						ImGui::Checkbox("Include map", &Vars::Misc::Discord::IncludeMap.m_Var); HelpMarker("Should Discord Rich Presence contain current map name?");
		//						ImGui::Checkbox("Include class", &Vars::Misc::Discord::IncludeClass.m_Var); HelpMarker("Should Discord Rich Presence contain current class?");
		//						ImGui::Checkbox("Include timestamp", &Vars::Misc::Discord::IncludeTimestamp.m_Var); HelpMarker("Should time since you started playing TF2 be included?");
		//						const char* imagething[]{ "Big fedora, small TF2", "Small TF2, big fedora" }; ImGui::PushItemWidth(100); ImGui::Combo("Image Options", &Vars::Misc::Discord::WhatImagesShouldBeUsed.m_Var, imagething, IM_ARRAYSIZE(imagething)); ImGui::PopItemWidth(); HelpMarker("Which image should be big and which small");
		//					}
		//					if (ImGui::CollapsingHeader("Steam")) {
		//						ImGui::Checkbox("Enable Steam RPC", &Vars::Misc::Steam::EnableRPC.m_Var); HelpMarker("Enable Steam Rich Presence");
		//						const char* matchGroups[]{ "Special Event", "MvM Mann Up", "Competitive", "Casual", "MvM Boot Camp" }; ImGui::PushItemWidth(150); ImGui::Combo("Match group", &Vars::Misc::Steam::MatchGroup.m_Var, matchGroups, IM_ARRAYSIZE(matchGroups)); ImGui::PopItemWidth(); HelpMarker("Which match group should be used?");
		//						ImGui::Checkbox("Override in menu", &Vars::Misc::Steam::OverrideMenu.m_Var); HelpMarker("Override match group to \"Main Menu\" when in main menu");
		//						const char* mapTexts[]{ "Fedoraware", "Figoraware", "Meowhook.club", "Rathook.cc", "Nitro.tf", "custom" }; ImGui::PushItemWidth(150); ImGui::Combo("Map text", &Vars::Misc::Steam::MapText.m_Var, mapTexts, IM_ARRAYSIZE(mapTexts)); ImGui::PopItemWidth(); HelpMarker("Which map text should be used?");
		//						ImGui::PushItemWidth(100); ImGui::InputText("Custom map text", &Vars::Misc::Steam::CustomText); ImGui::PopItemWidth(); HelpMarker("For when \"custom\" is selcted in \"Map text\". Sets custom map text.");
		//						ImGui::InputInt("Group size", &Vars::Misc::Steam::GroupSize.m_Var); HelpMarker("Sets party size");
		//					}
		//				}
		//				ImGui::NextColumn();
		//				{
		//					if (ImGui::CollapsingHeader("Tickbase exploits", ImGuiTreeNodeFlags_DefaultOpen)) {
		//						ImGui::Checkbox("Active", &Vars::Misc::CL_Move::Enabled.m_Var); HelpMarker("Tickbase exploit master switch");
		//						if (Vars::Misc::CL_Move::Enabled.m_Var) {
		//							InputKeybind("Recharge key", Vars::Misc::CL_Move::RechargeKey); HelpMarker("Recharges ticks for shifting");
		//							InputKeybind("Teleport key", Vars::Misc::CL_Move::TeleportKey); HelpMarker("Shifts ticks to move fast");
		//							if (Vars::Misc::CL_Move::DTMode.m_Var == 0 || Vars::Misc::CL_Move::DTMode.m_Var == 2)
		//								InputKeybind("Doubletap key", Vars::Misc::CL_Move::DoubletapKey); HelpMarker("Shifts ticks when shooting for a rapid-fire effect");
		//							{
		//								ImGui::PushItemWidth(100);
		//								std::vector<std::string> dtvec;
		//								static bool dtFlags[]{ Vars::Misc::CL_Move::RechargeWhileDead.m_Var,Vars::Misc::CL_Move::AutoRecharge.m_Var,Vars::Misc::CL_Move::WaitForDT.m_Var,Vars::Misc::CL_Move::AntiWarp.m_Var,Vars::Misc::CL_Move::NotInAir.m_Var };
		//								const char* pDt[] = { "Recharge While Dead", "Auto Recharge", "Wait for DT", "Anti-warp", "Avoid airborne" }; static std::string dtPreview = "PH";
		//								if (dtPreview == "PH") { // super simple, iterate through this once so we don't have clear combo boxesB
		//									dtPreview = "";
		//									for (size_t i = 0; i < IM_ARRAYSIZE(pDt); i++) {
		//										if (dtFlags[i])
		//											dtvec.push_back(pDt[i]);
		//									}
		//									for (size_t i = 0; i < dtvec.size(); i++)
		//									{
		//										if (dtvec.size() == 1)
		//											dtPreview += dtvec.at(i);
		//										else if (!(i == dtvec.size() - 1))
		//											dtPreview += dtvec.at(i) + ", ";
		//										else
		//											dtPreview += dtvec.at(i);
		//									}
		//								}
		//								if (ImGui::BeginCombo("Options", dtPreview.c_str()))
		//								{
		//									dtPreview = "";
		//									for (size_t i = 0; i < IM_ARRAYSIZE(pDt); i++)
		//									{
		//										ImGui::Selectable(pDt[i], &dtFlags[i]);
		//										if (dtFlags[i])
		//											dtvec.push_back(pDt[i]);
		//									}
		//									for (size_t i = 0; i < dtvec.size(); i++)
		//									{
		//										if (dtvec.size() == 1)
		//											dtPreview += dtvec.at(i);
		//										else if (!(i == dtvec.size() - 1))
		//											dtPreview += dtvec.at(i) + ", ";
		//										else
		//											dtPreview += dtvec.at(i);
		//									}
		//									ImGui::EndCombo();
		//								} // i got tired of trying better ways so this is new method fr*ck you
		//								for (size_t i = 0; i < IM_ARRAYSIZE(dtFlags); i++) {
		//									if (dtFlags[i]) {
		//										switch (i + 1) {
		//										case 1: { Vars::Misc::CL_Move::RechargeWhileDead.m_Var = true; break; }
		//										case 2: { Vars::Misc::CL_Move::AutoRecharge.m_Var = true; break; }
		//										case 3: { Vars::Misc::CL_Move::WaitForDT.m_Var = true; break; }
		//										case 4: { Vars::Misc::CL_Move::AntiWarp.m_Var = true; break; }
		//										case 5: { Vars::Misc::CL_Move::NotInAir.m_Var = true; break; }
		//										}
		//									}
		//									else {
		//										switch (i + 1) {
		//										case 1: { Vars::Misc::CL_Move::RechargeWhileDead.m_Var = false; break; }
		//										case 2: { Vars::Misc::CL_Move::AutoRecharge.m_Var = false; break; }
		//										case 3: { Vars::Misc::CL_Move::WaitForDT.m_Var = false; break; }
		//										case 4: { Vars::Misc::CL_Move::AntiWarp.m_Var = false; break; }
		//										case 5: { Vars::Misc::CL_Move::NotInAir.m_Var = false; break; }
		//										}
		//									}
		//								}
		//							}
		//							const char* dtModes[]{ "On key", "Always", "Disable on key", "Disabled" }; ImGui::PushItemWidth(100); ImGui::Combo("DT Mode", &Vars::Misc::CL_Move::DTMode.m_Var, dtModes, IM_ARRAYSIZE(dtModes)); ImGui::PopItemWidth(); HelpMarker("How should DT behave");
		//							const char* dtBarStyles[]{ "Off", "Default", "Nitro", "Rijin" }; ImGui::PushItemWidth(100); ImGui::Combo("DT indicator style", &Vars::Misc::CL_Move::DTBarStyle.m_Var, dtBarStyles, IM_ARRAYSIZE(dtBarStyles)); ImGui::PopItemWidth(); HelpMarker("Which style to do the doubletap bar style");
		//							ImGui::PushItemWidth(100); ImGui::SliderInt("Ticks to shift", &Vars::Misc::CL_Move::DTTicks.m_Var, 1, 24, "%d"); ImGui::PopItemWidth(); HelpMarker("How many ticks to shift");
		//							if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 1) {
		//								ImGui::SliderInt("Dt bar height", &Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var, 1, 30);
		//								ImGui::SliderInt("Dt bar width", &Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var, 1, 30);
		//							}
		//							else if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 3) {
		//								ImGui::SliderInt("DT Bar Height", &Vars::Misc::CL_Move::DTBarScaleY.m_Var, 1, 25);
		//								ImGui::SliderInt("DT Bar Width", &Vars::Misc::CL_Move::DTBarScaleX.m_Var, 100, 1000);
		//							}
		//						}
		//					}
		//					if (ImGui::CollapsingHeader("Attribute changer")) {
		//						const char* unuEffects[]{
		//							"None",
		//							"Hot",
		//							"Isotope",
		//							"Cool",
		//							"Energy orb"
		//						};
		//						const char* unuEffects2[]{
		//							"None",
		//							"Hot",
		//							"Isotope",
		//							"Cool",
		//							"Energy orb"
		//						};
		//						ImGui::PushItemWidth(100);
		//						if (ImGui::Combo("Unusual effect 1", &unu1, unuEffects, IM_ARRAYSIZE(unuEffects))) {
		//							switch (unu1) {
		//							case 0:
		//								Vars::Visuals::Skins::Particle.m_Var = 0;
		//								break;
		//							case 1:
		//								Vars::Visuals::Skins::Particle.m_Var = 701;
		//								break;
		//							case 2:
		//								Vars::Visuals::Skins::Particle.m_Var = 702;
		//								break;
		//							case 3:
		//								Vars::Visuals::Skins::Particle.m_Var = 703;
		//								break;
		//							case 4:
		//								Vars::Visuals::Skins::Particle.m_Var = 704;
		//								break;
		//							default:
		//								break;
		//							}
		//						}
		//						ImGui::PopItemWidth();
		//						HelpMarker("The first unusual effect to be applied to the weapon");
		//						ImGui::PushItemWidth(100);
		//						if (ImGui::Combo("Unusual effect 2", &unu2, unuEffects2, IM_ARRAYSIZE(unuEffects2))) {
		//							switch (unu2) {
		//							case 0:
		//								Vars::Visuals::Skins::Effect.m_Var = 0;
		//								break;
		//							case 1:
		//								Vars::Visuals::Skins::Effect.m_Var = 701;
		//								break;
		//							case 2:
		//								Vars::Visuals::Skins::Effect.m_Var = 702;
		//								break;
		//							case 3:
		//								Vars::Visuals::Skins::Effect.m_Var = 703;
		//								break;
		//							case 4:
		//								Vars::Visuals::Skins::Effect.m_Var = 704;
		//								break;
		//							default:
		//								break;
		//							}
		//						}
		//						ImGui::PopItemWidth();
		//						HelpMarker("The second unusual effect to be applied to the weapon");
		//						const char* sheens[]{
		//							"None",
		//							"Team shine",
		//							"Deadly daffodil",
		//							"Manndarin",
		//							"Mean green",
		//							"Agonizing emerald",
		//							"Villainous violet",
		//							"Hot rod"
		//						};
		//						ImGui::PushItemWidth(100);
		//						ImGui::Combo("Sheen", &Vars::Visuals::Skins::Sheen.m_Var, sheens, IM_ARRAYSIZE(sheens));
		//						ImGui::PopItemWidth();
		//						HelpMarker("Which sheen to apply to the weapon");
		//						ImGui::Checkbox("Style override", &Vars::Visuals::Skins::Override.m_Var);
		//						if (ImGui::Button("Set current")) {
		//							g_AttributeChanger.m_bSet = true;
		//						}
		//						if (ImGui::Button("Save all")) {
		//							g_AttributeChanger.m_bSave = true;
		//						}
		//						if (ImGui::Button("Load all")) {
		//							g_AttributeChanger.m_bLoad = true;
		//						}
		//					}
		//					if (ImGui::CollapsingHeader(_("Body part scales"))) {
		//						ImGui::Checkbox(_("Body part scales switch"), &Vars::ESP::Players::Funnybodypartslol.m_Var);
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat(_("Head scale"), &Vars::ESP::Players::Headscale.m_Var, -180.f, 180.f, _("%.0f"), 1.0f); ImGui::PopItemWidth(); HelpMarker(_("How many degrees the aimbot's FoV will have"));
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat(_("Body scale"), &Vars::ESP::Players::Torsoscale.m_Var, -180.0f, 180.f, _("%.0f"), 1.0f); ImGui::PopItemWidth(); HelpMarker(_("How many degrees the aimbot's FoV will have"));
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat(_("Hand scale"), &Vars::ESP::Players::Handscale.m_Var, -180.0f, 180.f, _("%.0f"), 1.0f); ImGui::PopItemWidth(); HelpMarker(_("How many degrees the aimbot's FoV will have"));
		//					}
		//				}
		//				ImGui::NextColumn();
		//				{
		//					if (ImGui::CollapsingHeader("HvH", ImGuiTreeNodeFlags_DefaultOpen)) {
		//						ImGui::Checkbox("Anti-aim", &Vars::AntiHack::AntiAim::Active.m_Var); HelpMarker("Anti-aim master switch");
		//						const char* pitch[]{ "None", "Up", "Down", "Fake up", "Fake down", "Random" }; ImGui::PushItemWidth(100); ImGui::Combo("Pitch", &Vars::AntiHack::AntiAim::Pitch.m_Var, pitch, IM_ARRAYSIZE(pitch)); ImGui::PopItemWidth(); HelpMarker("Which way to look up/down");
		//						const char* realYaw[]{ "None", "Left", "Right", "Backwards", "Random", "Spin", "Edge"}; ImGui::PushItemWidth(100); ImGui::Combo("Real yaw", &Vars::AntiHack::AntiAim::YawReal.m_Var, realYaw, IM_ARRAYSIZE(realYaw)); ImGui::PopItemWidth(); HelpMarker("Which way to look horizontally");
		//						const char* fakeYaw[]{ "None", "Left", "Right", "Backwards", "Random", "Spin", "Edge"}; ImGui::PushItemWidth(100); ImGui::Combo("Fake yaw", &Vars::AntiHack::AntiAim::YawFake.m_Var, fakeYaw, IM_ARRAYSIZE(fakeYaw)); ImGui::PopItemWidth(); HelpMarker("Which way to appear to look horizontally");
		//						ImGui::PushItemWidth(100); ImGui::SliderFloat("Spin Speed", &Vars::AntiHack::AntiAim::SpinSpeed.m_Var, -30.f, 30.f, "%.1f", 0); ImGui::PopItemWidth(); HelpMarker("You spin me right 'round, baby, right 'round");
		//						ImGui::Checkbox("Resolver", &Vars::AntiHack::Resolver::Resolver.m_Var); HelpMarker("Enables AntiAim resolver in the playerlist");
		//						const char* flgModes[]{ "None", "Plain", "Random", "Velocity Based"}; ImGui::PushItemWidth(100); ImGui::Combo("Fake Lag", &Vars::Misc::CL_Move::FakelagMode.m_Var, flgModes, IM_ARRAYSIZE(flgModes)); ImGui::PopItemWidth(); HelpMarker("Controls how fakelag will be controlled.");
		//						if (Vars::Misc::CL_Move::FakelagMode.m_Var > 0) { Vars::Misc::CL_Move::Fakelag.m_Var = true; }
		//						else { Vars::Misc::CL_Move::Fakelag.m_Var = false; }
		//						if (Vars::Misc::CL_Move::Fakelag.m_Var) {
		//							if (Vars::Misc::CL_Move::FakelagMode.m_Var != 2) {
		//								ImGui::PushItemWidth(100); ImGui::SliderInt("Fakelag value", &Vars::Misc::CL_Move::FakelagValue.m_Var, 1, 22, "%d"); ImGui::PopItemWidth(); HelpMarker("How much lag you should fake(?)");
		//								if (Vars::Misc::CL_Move::FakelagMode.m_Var == 1) { // nested bc checking outside of this is useless
		//									ImGui::Checkbox("Fakelag on key", &Vars::Misc::CL_Move::FakelagOnKey.m_Var); HelpMarker("Fakelag will only activate when an assigned key is held");
		//									InputKeybind("Fakelag key", Vars::Misc::CL_Move::FakelagKey); HelpMarker("Fakelag will only activate when this key is held");
		//								}
		//							}
		//							else {
		//								ImGui::PushItemWidth(100); ImGui::SliderInt("Fakelag Max Val", &Vars::Misc::CL_Move::FakelagMax.m_Var, Vars::Misc::CL_Move::FakelagMin.m_Var + 1, 22, "%d"); ImGui::PopItemWidth();
		//								ImGui::PushItemWidth(100); ImGui::SliderInt("Fakelag Min Val", &Vars::Misc::CL_Move::FakelagMin.m_Var, 1, Vars::Misc::CL_Move::FakelagMax.m_Var - 1, "%d"); ImGui::PopItemWidth();
		//							}
		//							ImGui::Checkbox("Fakelag Indicator", &Vars::Misc::CL_Move::FakelagIndicator.m_Var); HelpMarker("Shows your fakelag position in thirdperson");
		//						}
		//					}
		//					if (ImGui::CollapsingHeader("Radar")) {
		//						ImGui::Checkbox("Active###radar", &Vars::Radar::Main::Active.m_Var); HelpMarker("Will show nearby things relative to your player");
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("Radar size", &Vars::Radar::Main::Size.m_Var, 20, 200); ImGui::PopItemWidth(); HelpMarker("The size of the radar window");
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("Radar range", &Vars::Radar::Main::Range.m_Var, 50, 3000, "%d"); ImGui::PopItemWidth(); HelpMarker("The range of the radar");
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("Radar background alpha", &Vars::Radar::Main::BackAlpha.m_Var, 0, 255, "%d"); ImGui::PopItemWidth(); HelpMarker("The background alpha of the radar");
		//						ImGui::PushItemWidth(100); ImGui::SliderInt("Radar lines alpha", &Vars::Radar::Main::LineAlpha.m_Var, 0, 255, "%d"); ImGui::PopItemWidth(); HelpMarker("The line alpha of the radar");
		//						if (ImGui::CollapsingHeader("Players###radarplayers")) {
		//							ImGui::Checkbox("Active###radarp", &Vars::Radar::Players::Active.m_Var); HelpMarker("Will show players on the radar");
		//							const char* iconPlayersRadar[]{ "Scoreboard", "Portraits", "Avatar" }; ImGui::PushItemWidth(100); ImGui::Combo("Icon###radari", &Vars::Radar::Players::IconType.m_Var, iconPlayersRadar, IM_ARRAYSIZE(iconPlayersRadar)); ImGui::PopItemWidth(); HelpMarker("What sort of icon to represent players with");
		//							const char* backgroundpradar[]{ "Off", "Rectangle", "Texture" }; ImGui::PushItemWidth(100); ImGui::Combo("Background###radarb", &Vars::Radar::Players::BackGroundType.m_Var, backgroundpradar, IM_ARRAYSIZE(backgroundpradar)); ImGui::PopItemWidth(); HelpMarker("What sort of background to put on players on the radar");
		//							ImGui::Checkbox("Outline###radaro", &Vars::Radar::Players::Outline.m_Var); HelpMarker("Will put an outline on players on the radar");
		//							static const char* ignoreTeammatespRadar[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###radarplayersteam", &Vars::Radar::Players::IgnoreTeam.m_Var, ignoreTeammatespRadar, IM_ARRAYSIZE(ignoreTeammatespRadar)); ImGui::PopItemWidth(); HelpMarker("Which teammates the radar will ignore drawing on");
		//							static const char* ignoreCloakedpRadar[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore cloaked###radarplayerscloaked", &Vars::Radar::Players::IgnoreCloaked.m_Var, ignoreCloakedpRadar, IM_ARRAYSIZE(ignoreCloakedpRadar)); ImGui::PopItemWidth(); HelpMarker("Which cloaked players the radar will ignore drawing on");
		//							ImGui::Checkbox("Health bar###radarhealt", &Vars::Radar::Players::Health.m_Var); HelpMarker("Will show players health on the radar");
		//							ImGui::PushItemWidth(100); ImGui::SliderInt("Icon size###playersizeiconradar", &Vars::Radar::Players::IconSize.m_Var, 12, 30, "%d"); ImGui::PopItemWidth(); HelpMarker("The icon size of players on the radar");
		//						}
		//						if (ImGui::CollapsingHeader("Buildings###radarbuildings")) {
		//							ImGui::Checkbox("Active###radarbuildingsa", &Vars::Radar::Buildings::Active.m_Var); HelpMarker("Will show buildings on the radar");
		//							ImGui::Checkbox("Outline###radarbuildingsao", &Vars::Radar::Buildings::Outline.m_Var); HelpMarker("Will put an outline on buildings on the radar");
		//							ImGui::Checkbox("Ignore team###radarbuildingsb", &Vars::Radar::Buildings::IgnoreTeam.m_Var); HelpMarker("Will ignore team buildings when drawing buildings on the radar");
		//							ImGui::Checkbox("Health bar###radarbuildingsc", &Vars::Radar::Buildings::Health.m_Var); HelpMarker("Will show building health on the radar");
		//							ImGui::PushItemWidth(100); ImGui::SliderInt("Icon size###buildingsizeiconradar", &Vars::Radar::Buildings::IconSize.m_Var, 12, 30, "%d"); ImGui::PopItemWidth(); HelpMarker("The icon size of buildings on the radar");
		//						}
		//						if (ImGui::CollapsingHeader("World###radarworld")) {
		//							ImGui::Checkbox("Active###radarworldd", &Vars::Radar::World::Active.m_Var); HelpMarker("Will show pickups on the radar");
		//							ImGui::Checkbox("Health###radarworldda", &Vars::Radar::World::Active.m_Var); HelpMarker("Will show health icons where health packs are");
		//							ImGui::Checkbox("Ammo###radarworlddb", &Vars::Radar::World::Active.m_Var); HelpMarker("Will show ammo icons where ammo packs are");
		//							ImGui::PushItemWidth(100); ImGui::SliderInt("Icon size###worldsizeiconradar", &Vars::Radar::World::IconSize.m_Var, 12, 30, "%d"); ImGui::PopItemWidth(); HelpMarker("The icon size of pickups on the radar");
		//						}
		//					}
		//				}
		//			}
		//			ImGui::EndChild();
		//		}
		//		if (SettingsTab == 4) {
		//			//Colors
		//			ImGui::BeginChild("Colours");
		//			{
		//				ImGui::Columns(2);
		//				{
		//					ImGui::TextDisabled("Cheat colours");
		//					if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 1 || Vars::Misc::CL_Move::DTBarStyle.m_Var == 3) {
		//						ColorPicker("Dt bar chargin left", Colors::DtChargingLeft);
		//						ColorPicker("Dt bar chargin right", Colors::DtChargingRight);
		//						ColorPicker("Dt bar charged left", Colors::DtChargedLeft);
		//						ColorPicker("Dt bar charged right", Colors::DtChargedRight);
		//						ColorPicker("Dt bar outline", Colors::DtOutline);
		//					}
		//					ColorPicker("Outline ESP", Colors::OutlineESP);
		//					ColorPicker("Conditions", Colors::Cond);
		//					ColorPicker("Target", Colors::Target);
		//					ColorPicker("Invulnerable", Colors::Invuln);
		//					ColorPicker("Cloaked", Colors::Cloak);
		//					ColorPicker("Friend", Colors::Friend);
		//					ColorPicker("Local", Colors::Local);
		//					ColorPicker("Ignored", Colors::Ignored);
		//					ColorPicker("Bone matrix color", Colors::bonecolor); HelpMarker("This is for fakelag");
		//					ColorPicker("Overheal", Colors::Overheal);
		//					ColorPicker("Health pack", Colors::Health);
		//					ColorPicker("Ammo pack", Colors::Ammo);
		//					ColorPicker("Ubercharged", Colors::UberColor);
		//					if (!Vars::ESP::Main::EnableTeamEnemyColors.m_Var) {
		//						ColorPicker("RED team", Colors::TeamRed);
		//						ColorPicker("BLU team", Colors::TeamBlu);
		//					}
		//					else {
		//						ColorPicker("Enemy", Colors::Enemy);
		//						ColorPicker("Team", Colors::Team);
		//					}
		//					ColorPicker("Hand chams", Colors::Hands);
		//					ColorPicker("Hand glow chams", Colors::HandsOverlay);
		//					ColorPicker("Weapon chams", Colors::Weapon);
		//					ColorPicker("Weapon glow chams", Colors::WeaponOverlay);
		//					ColorPicker("World modulation", Colors::WorldModulation);
		//					ColorPicker("Sky modulation", Colors::SkyModulation);
		//					ColorPicker("Static prop modulation", Colors::StaticPropModulation);
		//					ColorPicker("FoV circle", Colors::FOVCircle);
		//					ColorPicker("Bones", Colors::Bones);
		//					ColorPicker("Bullet tracer", Colors::BulletTracer);
		//					ColorPicker("Fresnel chams base", Colors::FresnelBase);
		//					ColorPicker("Aimbot prediction", Colors::AimSquareCol);
		//					ColorPicker("Damage logger background", Colors::DmgLoggerBackground);
		//					ColorPicker("Damage logger outline", Colors::DmgLoggerOutline);
		//					ColorPicker("Damage logger text", Colors::DmgLoggerText);
		//					ColorPicker("Weapon icon", Colors::WeaponIcon);
		//					ColorPicker("Noscope lines 1", Colors::NoscopeLines1);
		//					ColorPicker("Noscope lines 2", Colors::NoscopeLines2);
		//					//ColorPicker("Fresnel chams top", Colors::FresnelTop);
		//				}
		//				ImGui::NextColumn();
		//				{
		//					if (ImGui::CollapsingHeader("Surface colours")) {
		//						ImVec4* const colors = ImGui::GetCurrentContext()->Style.Colors;
		//						ImGui::PushItemWidth(150);
		//						ColorPicker("Window background", Vars::Menu::Colors::WindowBackground);
		//						ColorPicker("Title bar", Vars::Menu::Colors::TitleBar);
		//						ColorPicker("Widget", Vars::Menu::Colors::Widget);
		//						ColorPicker("Widget active", Vars::Menu::Colors::WidgetActive);
		//						ColorPicker("Menu outline", Vars::Menu::Colors::OutlineMenu);
		//						ColorPicker("Text", Vars::Menu::Colors::Text);
		//						//drawList->AddRect(ImVec2(0, 0), ImVec2(g_ScreenSize.w, g_ScreenSize.h), ImColor(255, 0, 0, 255));
		//						ImGui::PopItemWidth();
		//					}
		//					if (ImGui::CollapsingHeader("ImGui Style")) {
		//						ImGui::ShowStyleEditor();
		//					}
		//				}
		//			}
		//			ImGui::EndChild();
		//		}
		//		if (ImGui::BeginMainMenuBar())
		//		{
		//			if (ImGui::BeginMenu("Shortcuts"))
		//			{
		//				if (ImGui::MenuItem("Full Update"))
		//					g_Interfaces.Engine->ClientCmd_Unrestricted("cl_fullupdate");
		//				if(ImGui::MenuItem("Reload HUD"))
		//					g_Interfaces.Engine->ClientCmd_Unrestricted("hud_reloadscheme");
		//				if (ImGui::MenuItem("Restart sound system"))
		//					g_Interfaces.Engine->ClientCmd_Unrestricted("snd_restart");
		//				if (ImGui::MenuItem("Stop sound"))
		//					g_Interfaces.Engine->ClientCmd_Unrestricted("stopsound");
		//				if (ImGui::MenuItem("Status"))
		//					g_Interfaces.Engine->ClientCmd_Unrestricted("status");
		//				if (ImGui::MenuItem("Ping"))
		//					g_Interfaces.Engine->ClientCmd_Unrestricted("ping");
		//				if (ImGui::MenuItem("Retry"))
		//					g_Interfaces.Engine->ClientCmd_Unrestricted("retry");
		//				ImGui::Separator();
		//				if (ImGui::MenuItem("Exit"))
		//					g_Interfaces.Engine->ClientCmd_Unrestricted("exit");
		//				ImGui::EndMenu();
		//			}
		//			if (ImGui::MenuItem("Playerlist"))
		//				g_PlayerList.showWindow = !g_PlayerList.showWindow;
		//			if (ImGui::MenuItem("Fonts"))
		//			{
		//				showFonts = !showFonts;
		//			}
		//			if (ImGui::BeginMenu("Menus")) {
		//				if (ImGui::MenuItem("Console"))
		//					g_Interfaces.Engine->ClientCmd_Unrestricted("showconsole");
		//				if (ImGui::MenuItem("Demo Playback"))
		//					g_Interfaces.Engine->ClientCmd_Unrestricted("demoui");
		//				if (ImGui::MenuItem("Demo Trackbar"))
		//					g_Interfaces.Engine->ClientCmd_Unrestricted("demoui2");
		//				if (ImGui::MenuItem("Itemtest"))
		//					g_Interfaces.Engine->ClientCmd_Unrestricted("itemtest");
		//				ImGui::EndMenu();
		//			}
		//			ImGui::EndMainMenuBar();
		//		}
		//		if (showFonts) {
		//			if (ImGui::Begin("Fonts###fontmenu", &showFonts, ImGuiWindowFlags_NoCollapse))
		//			{
		//				std::vector<Font_t> fonts;
		//				static const char* flags[]{ "None", "Italic", "Underline", "Strikeout", "Symbol", "Antialias", "Gaussian", "Rotary", "Dropshadow", "Additive", "Outline", "Custom" };
		//				static int fontflags[]{ 0x000, 0x001, 0x002, 0x004, 0x008,0x010, 0x020, 0x040, 0x080, 0x100, 0x200, 0x400 };
		//				//{ dwFont, szName, nTall, nWeight, nFlags}
		//				if (ImGui::CollapsingHeader("FONT_ESP"))
		//				{
		//					HelpMarker("ESP Font");
		//					ImGui::InputText("Font name###espfontname", &Vars::Fonts::FONT_ESP::szName);
		//					ImGui::InputInt("Font height###espfontheight", &Vars::Fonts::FONT_ESP::nTall.m_Var);
		//					ImGui::InputInt("Font weight###espfontweight", &Vars::Fonts::FONT_ESP::nWeight.m_Var); HelpMarker("How bold the font is (full bold is like 800)");
		//					static bool flagbools[12]{ 0,0,0,0,0,0,0,0,0,0,0,0 };
		//					static std::string previewValue = "";
		//					std::vector<std::string> vec;
		//					if (ImGui::BeginCombo("Font flags###espfonttttttttttt", previewValue.c_str()))
		//					{
		//						previewValue = "";
		//						for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
		//						{
		//							ImGui::Selectable(flags[i], &flagbools[i]);
		//							if (flagbools[i])
		//								vec.push_back(flags[i]);
		//						}
		//						for (size_t i = 0; i < vec.size(); i++)
		//						{
		//							if (vec.size() == 1)
		//								previewValue += vec.at(i);
		//							else if (!(i == vec.size() - 1))
		//								previewValue += vec.at(i) + ",";
		//							else
		//								previewValue += vec.at(i);
		//						}
		//						ImGui::EndCombo();
		//					}
		//					Vars::Fonts::FONT_ESP::nFlags.m_Var = 0;
		//					for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
		//					{
		//						if (flagbools[i]) {
		//							Vars::Fonts::FONT_ESP::nFlags.m_Var |= fontflags[i];
		//						}
		//					}
		//					HelpMarker(std::to_string(Vars::Fonts::FONT_ESP::nFlags.m_Var).c_str());
		//				}
		//				if (ImGui::CollapsingHeader("FONT_ESP_NAME"))
		//				{
		//					HelpMarker("ESP Name Font");
		//					ImGui::InputText("Font name###espfontnamename", &Vars::Fonts::FONT_ESP_NAME::szName);
		//					ImGui::InputInt("Font height###espfontnameheight", &Vars::Fonts::FONT_ESP_NAME::nTall.m_Var);
		//					ImGui::InputInt("Font weight###espfontnameweight", &Vars::Fonts::FONT_ESP_NAME::nWeight.m_Var); HelpMarker("How bold the font is (full bold is like 800)");
		//					static bool flagbools[12]{ 0,0,0,0,0,0,0,0,0,0,0,0 };
		//					static std::string previewValue = "";
		//					std::vector<std::string> vec;
		//					if (ImGui::BeginCombo("Font flags###espfonnamettttttttttt", previewValue.c_str()))
		//					{
		//						previewValue = "";
		//						for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
		//						{
		//							ImGui::Selectable(flags[i], &flagbools[i]);
		//							if (flagbools[i])
		//								vec.push_back(flags[i]);
		//						}
		//						for (size_t i = 0; i < vec.size(); i++)
		//						{
		//							if (vec.size() == 1)
		//								previewValue += vec.at(i);
		//							else if (!(i == vec.size() - 1))
		//								previewValue += vec.at(i) + ",";
		//							else
		//								previewValue += vec.at(i);
		//						}
		//						ImGui::EndCombo();
		//					}
		//					Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var = 0;
		//					for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
		//					{
		//						if (flagbools[i]) {
		//							Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var |= fontflags[i];
		//						}
		//					}
		//					HelpMarker(std::to_string(Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var).c_str());
		//				}
		//				if (ImGui::CollapsingHeader("FONT_ESP_COND"))
		//				{
		//					HelpMarker("ESP Condition Font");
		//					ImGui::InputText("Font name###espfontcondname", &Vars::Fonts::FONT_ESP_COND::szName);
		//					ImGui::InputInt("Font height###espfontcondheight", &Vars::Fonts::FONT_ESP_COND::nTall.m_Var);
		//					ImGui::InputInt("Font weight###espfontcondweight", &Vars::Fonts::FONT_ESP_COND::nWeight.m_Var); HelpMarker("How bold the font is (full bold is like 800)");
		//					static bool flagbools[12]{ 0,0,0,0,0,0,0,0,0,0,0,0 };
		//					static std::string previewValue = "";
		//					std::vector<std::string> vec;
		//					if (ImGui::BeginCombo("Font flags###espfoncondttttttttttt", previewValue.c_str()))
		//					{
		//						previewValue = "";
		//						for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
		//						{
		//							ImGui::Selectable(flags[i], &flagbools[i]);
		//							if (flagbools[i])
		//								vec.push_back(flags[i]);
		//						}
		//						for (size_t i = 0; i < vec.size(); i++)
		//						{
		//							if (vec.size() == 1)
		//								previewValue += vec.at(i);
		//							else if (!(i == vec.size() - 1))
		//								previewValue += vec.at(i) + ",";
		//							else
		//								previewValue += vec.at(i);
		//						}
		//						ImGui::EndCombo();
		//					}
		//					Vars::Fonts::FONT_ESP_COND::nFlags.m_Var = 0;
		//					for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
		//					{
		//						if (flagbools[i]) {
		//							Vars::Fonts::FONT_ESP_COND::nFlags.m_Var |= fontflags[i];
		//						}
		//					}
		//					HelpMarker(std::to_string(Vars::Fonts::FONT_ESP_COND::nFlags.m_Var).c_str());
		//				}
		//				if (ImGui::CollapsingHeader("FONT_ESP_PICKUPS"))
		//				{
		//					HelpMarker("ESP Pickups Font");
		//					ImGui::InputText("Font name###espfontpickupsname", &Vars::Fonts::FONT_ESP_PICKUPS::szName);
		//					ImGui::InputInt("Font height###espfontpickupsheight", &Vars::Fonts::FONT_ESP_PICKUPS::nTall.m_Var);
		//					ImGui::InputInt("Font weight###espfontpickupsweight", &Vars::Fonts::FONT_ESP_PICKUPS::nWeight.m_Var); HelpMarker("How bold the font is (full bold is like 800)");
		//					static bool flagbools[12]{ 0,0,0,0,0,0,0,0,0,0,0,0 };
		//					static std::string previewValue = "";
		//					std::vector<std::string> vec;
		//					if (ImGui::BeginCombo("Font flags###espfonpickupsttttttttttt", previewValue.c_str()))
		//					{
		//						previewValue = "";
		//						for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
		//						{
		//							ImGui::Selectable(flags[i], &flagbools[i]);
		//							if (flagbools[i])
		//								vec.push_back(flags[i]);
		//						}
		//						for (size_t i = 0; i < vec.size(); i++)
		//						{
		//							if (vec.size() == 1)
		//								previewValue += vec.at(i);
		//							else if (!(i == vec.size() - 1))
		//								previewValue += vec.at(i) + ",";
		//							else
		//								previewValue += vec.at(i);
		//						}
		//						ImGui::EndCombo();
		//					}
		//					Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var = 0;
		//					for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
		//					{
		//						if (flagbools[i]) {
		//							Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var |= fontflags[i];
		//						}
		//					}
		//					HelpMarker(std::to_string(Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var).c_str());
		//				}
		//				if (ImGui::CollapsingHeader("FONT_MENU"))
		//				{
		//					HelpMarker("Menu Font");
		//					ImGui::InputText("Font name###espfontnamenameneby", &Vars::Fonts::FONT_MENU::szName);
		//					ImGui::InputInt("Font height###espfontnameheightafsdfads", &Vars::Fonts::FONT_MENU::nTall.m_Var);
		//					ImGui::InputInt("Font weight###espfontnameweightasfdafsd", &Vars::Fonts::FONT_MENU::nWeight.m_Var); HelpMarker("How bold the font is (full bold is like 800)");
		//					static bool flagbools[12]{ 0,0,0,0,0,0,0,0,0,0,0,0 };
		//					static std::string previewValue = "";
		//					std::vector<std::string> vec;
		//					if (ImGui::BeginCombo("Font flags###espfonnametttttttttttafsafds", previewValue.c_str()))
		//					{
		//						previewValue = "";
		//						for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
		//						{
		//							ImGui::Selectable(flags[i], &flagbools[i]);
		//							if (flagbools[i])
		//								vec.push_back(flags[i]);
		//						}
		//						for (size_t i = 0; i < vec.size(); i++)
		//						{
		//							if (vec.size() == 1)
		//								previewValue += vec.at(i);
		//							else if (!(i == vec.size() - 1))
		//								previewValue += vec.at(i) + ",";
		//							else
		//								previewValue += vec.at(i);
		//						}
		//						ImGui::EndCombo();
		//					}
		//					Vars::Fonts::FONT_MENU::nFlags.m_Var = 0;
		//					for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
		//					{
		//						if (flagbools[i]) {
		//							Vars::Fonts::FONT_MENU::nFlags.m_Var |= fontflags[i];
		//						}
		//					}
		//					HelpMarker(std::to_string(Vars::Fonts::FONT_MENU::nFlags.m_Var).c_str());
		//				}
		//				if (ImGui::CollapsingHeader("FONT_INDICATORS"))
		//				{
		//					HelpMarker("ESP Indicator Font");
		//					ImGui::InputText("Font name###espfontcondname", &Vars::Fonts::FONT_INDICATORS::szName);
		//					ImGui::InputInt("Font height###espfontcondheight", &Vars::Fonts::FONT_INDICATORS::nTall.m_Var);
		//					ImGui::InputInt("Font weight###espfontcondweight", &Vars::Fonts::FONT_INDICATORS::nWeight.m_Var); HelpMarker("How bold the font is (full bold is like 800)");
		//					static bool flagbools[12]{ 0,0,0,0,0,0,0,0,0,0,0,0 };
		//					static std::string previewValue = "";
		//					std::vector<std::string> vec;
		//					if (ImGui::BeginCombo("Font flags###espfoncondttttttttttt", previewValue.c_str()))
		//					{
		//						previewValue = "";
		//						for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
		//						{
		//							ImGui::Selectable(flags[i], &flagbools[i]);
		//							if (flagbools[i])
		//								vec.push_back(flags[i]);
		//						}
		//						for (size_t i = 0; i < vec.size(); i++)
		//						{
		//							if (vec.size() == 1)
		//								previewValue += vec.at(i);
		//							else if (!(i == vec.size() - 1))
		//								previewValue += vec.at(i) + ",";
		//							else
		//								previewValue += vec.at(i);
		//						}
		//						ImGui::EndCombo();
		//					}
		//					Vars::Fonts::FONT_INDICATORS::nFlags.m_Var = 0;
		//					for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
		//					{
		//						if (flagbools[i]) {
		//							Vars::Fonts::FONT_INDICATORS::nFlags.m_Var |= fontflags[i];
		//						}
		//					}
		//					HelpMarker(std::to_string(Vars::Fonts::FONT_INDICATORS::nFlags.m_Var).c_str());
		//				}
		//				if (ImGui::Button("Apply settings###fontapply"))
		//				{
		//					Font_t fontEsp = { 0x0, Vars::Fonts::FONT_ESP::szName.c_str(), Vars::Fonts::FONT_ESP::nTall.m_Var, Vars::Fonts::FONT_ESP::nWeight.m_Var, Vars::Fonts::FONT_ESP::nFlags.m_Var };
		//					Font_t fontEspName = { 0x0, Vars::Fonts::FONT_ESP_NAME::szName.c_str(), Vars::Fonts::FONT_ESP_NAME::nTall.m_Var, Vars::Fonts::FONT_ESP_NAME::nWeight.m_Var, Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var };
		//					Font_t fontEspCond = { 0x0, Vars::Fonts::FONT_ESP_COND::szName.c_str(), Vars::Fonts::FONT_ESP_COND::nTall.m_Var, Vars::Fonts::FONT_ESP_COND::nWeight.m_Var, Vars::Fonts::FONT_ESP_COND::nFlags.m_Var };
		//					Font_t fontIndicator = { 0x0, Vars::Fonts::FONT_INDICATORS::szName.c_str(), Vars::Fonts::FONT_INDICATORS::nTall.m_Var, Vars::Fonts::FONT_INDICATORS::nWeight.m_Var, Vars::Fonts::FONT_INDICATORS::nFlags.m_Var };
		//					Font_t fontEspPickups = { 0x0, Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(), Vars::Fonts::FONT_ESP_PICKUPS::nTall.m_Var, Vars::Fonts::FONT_ESP_PICKUPS::nWeight.m_Var, Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var };
		//					Font_t fontMenu = { 0x0, Vars::Fonts::FONT_MENU::szName.c_str(), Vars::Fonts::FONT_MENU::nTall.m_Var, Vars::Fonts::FONT_MENU::nWeight.m_Var, Vars::Fonts::FONT_MENU::nFlags.m_Var };
		//					std::vector<Font_t> fonts = { fontEsp, fontEspName, fontEspCond, fontEspPickups, fontMenu, fontIndicator };
		//					g_Draw.RemakeFonts(fonts);
		//				}
		//				ImGui::End();
		//			}
		//		}
		//		if (g_PlayerList.showWindow)
		//		{
		//			g_PlayerList.Render();
		//		}
		//	}
		//	ImGui::End();
		//}
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
}
