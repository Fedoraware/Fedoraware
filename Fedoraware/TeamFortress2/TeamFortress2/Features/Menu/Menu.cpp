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
#include "../Camera/CameraWindow.h"
#include "ImGui/imgui_color_gradient.h"
ImFont* g_pImFontDefaultFont = nullptr;
ImFont* g_pImFontChineseFont = nullptr;
bool tooltips = true;
bool showFonts = false;
void AlignRight(int offset) {
	ImGui::SameLine(ImGui::GetContentRegionMax().x - offset);
	ImGui::SetNextItemWidth(offset);
}

std::string toolTipText = "";
bool hovered = false;

static void HelpMarker(const char* desc)
{
	if (tooltips) {
		//ImGui::SameLine();
		//ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			hovered = true;
			toolTipText = desc;
			//ImGui::BeginTooltip();
			//ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			//ImGui::TextUnformatted(desc);
			//ImGui::PopTextWrapPos();
			//ImGui::EndTooltip();
		}
		else {
			hovered = false;
		}
	}
}

static void SectionTitle(const char* title) {
	ImVec2 font_size = ImGui::CalcTextSize(title);
	ImGui::SameLine(
		ImGui::GetWindowSize().x / 2 -
		font_size.x + (font_size.x / 2)
	);
	ImGui::Text(title);
}

static void MultiCombo(std::vector<const char*> titles, std::vector<bool*> options,std::string description, std::string comboName) {
	if (titles.size() != options.size()) { return; }

	std::string preview = "<None>##";
	for (size_t i = 0; i < options.size(); i++) {
		if (*options[i]) {
			if (preview == "<None>##") { preview = ""; }
			preview += titles[i];
			preview.append(", ");
		}
	}
	preview.pop_back(); preview.pop_back(); // This is a stupid but easy way to remove the last comma

	if (ImGui::BeginCombo(comboName.c_str(), preview.c_str())) {
		for (size_t i = 0; i < titles.size(); i++) {
			ImGui::Selectable(titles[i], options[i], ImGuiSelectableFlags_DontClosePopups);
		}

		ImGui::EndCombo();
	}

	HelpMarker(description.c_str());
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
		case VK_PRIOR: return "Page Up";
		case VK_NEXT: return "Page Down";
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
		ImGui::Button("...", ImVec2(100, 20));
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
				if ((n > 0x0 && n < 0x7) ||
					(n > L'A' - 1 && n < L'Z' + 1) ||
					(n > L'0' - 1 && n < L'9' + 1) ||
					n == VK_LSHIFT ||
					n == VK_RSHIFT ||
					n == VK_SHIFT ||
					n == VK_ESCAPE ||
					n == VK_HOME ||
					n == VK_CONTROL ||
					n == VK_MENU ||
					n == VK_PRIOR ||
					n == VK_NEXT) {
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
	else if (ImGui::Button(VK2STR(output.m_Var), ImVec2(100, 20))) {
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

bool ColorPicker(const char* label, Color_t& color) {
	bool ret = false;
	ImVec4 tempColor = mColor(color);
	ImGui::PushItemWidth(150);
	if (ImGui::ColorEdit4(label, &tempColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
		color = vColor(tempColor);
		ret = true;
	}
	ImGui::PopItemWidth();
	HelpMarker(label);
	return ret;
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

#define WidthSlider(label, var, min, max, format, flagspower) \
ImGui::PushItemWidth(100); \
ImGui::SliderFloat(label, var, min, max, format, flagspower); \
ImGui::PopItemWidth()

#include "Fonts/font_awesome.h"

void IconText(const char* icon) {
	ImGui::PushFont(g_Menu.Icons);
	ImGui::Text(icon);
	ImGui::PopFont();
}



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

		ImWchar TextFontRange[]{ 0x0020, 0x00FF,0x0400, 0x044F,0};
		ImWchar IconFontRange[]{ ICON_MIN_FA, ICON_MAX_FA, 0 };
		VerdanaNormal = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdana.ttf", 14.0f, &fontConfig, TextFontRange);
		VerdanaSmall = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdana.ttf", 12.0f, &fontConfig, TextFontRange);
		VerdanaBold = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdanab.ttf", 18.0f, &fontConfig, TextFontRange);
		Icons = io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 14.0f, &fontConfig, IconFontRange);
		

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
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
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.4f);

		bInitImGui = true;
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
			titlegradient.addMark(0.9999998f, titlebg);
		}
		static ImGradient hover;
		{
			ImColor buttonhovered = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
			hover.m_marks.clear();
			hover.addMark(0.0f, buttonhovered);
			hover.addMark(0.32f, buttonhovered);
			hover.addMark(0.5f, ImColor(IM_COL32(255, 255, 255, 255)));
			hover.addMark(0.68f, buttonhovered);
			hover.addMark(0.9999998f, buttonhovered); 
		}
		static ImGradient active;
		{
			ImColor buttonactive = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
			active.m_marks.clear();
			active.addMark(0.0f, buttonactive);
			active.addMark(0.32f, buttonactive);
			active.addMark(0.5f, ImColor(IM_COL32(255, 255, 255, 255)));
			active.addMark(0.68f, buttonactive);
			active.addMark(0.9999998f, buttonactive); 
		}
		static ImGradient normal;
		{
			ImColor framenormal = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
			normal.m_marks.clear();
			normal.addMark(0.0f, framenormal);
			normal.addMark(0.32f, framenormal);
			normal.addMark(0.5f, accent);
			normal.addMark(0.68f, framenormal);
			normal.addMark(0.9999998f, framenormal); 
		}
		static MainTabs mainTab = MainTabs::Aimbot;
		static VisualsTabs visualsTab = VisualsTabs::Players;
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
			
			//if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			//	ImGui::BeginPopup("Settings");
			//	{
			//		ColorPicker("Menu accent", Vars::Menu::Colors::MenuAccent);
			//		ImGui::SameLine(); ImGui::Text("Menu accent");
			//		ImGui::EndPopup();
			//	}
			//}


			std::vector < Font_t > fonts;
			static const char* flags[]{ "None", "Italic", "Underline", "Strikeout", "Symbol", "Antialias", "Gaussian", "Rotary", "Dropshadow", "Additive", "Outline", "Custom" };
			static int fontflags[]{ 0x000,0x001,0x002,0x004,0x008,0x010,0x020,0x040,0x080,0x100,0x200,0x400 };

			ImGui::GradientRect(foregroundDrawList, &titlegradient, { winPos.x, winPos.y }, winSize.x, 3);
			ImGui::Dummy(ImVec2(0, 7));

			const float half = winSize.x / 2;
			const float third = winSize.x / 3;
			const float quarter = winSize.x / 4;
			const float fifth = winSize.x / 5;
			const float sixth = winSize.x / 6;

			// Main Tabs
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.1f));
			{
				bool pushedAim = false;

				// Tab 1 (Aimbot)
				{
					if (mainTab == MainTabs::Aimbot) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedAim = true;
					}
					else {
						pushedAim = false;
					}
					if (ImGui::Button("Aimbot", ImVec2(quarter, 27))) {
						mainTab = MainTabs::Aimbot;
					}
					if (mainTab == MainTabs::Aimbot) {
						if (pushedAim) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;

					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x, b1.y - 3 }, quarter, 3);
					}
					else if (mainTab == MainTabs::Aimbot) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, quarter, 3);
					}
					ImGui::SameLine();
				}

				bool pushedTrigger = false;

				// Tab 2 (Triggerbot)
				{
					if (mainTab == MainTabs::Triggerbot) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedTrigger = true;
					}
					else {
						pushedTrigger = false;
					}
					if (ImGui::Button("Triggerbot", ImVec2(quarter, 27))) {
						mainTab = MainTabs::Triggerbot;
					}
					if (mainTab == MainTabs::Triggerbot) {
						if (pushedTrigger) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b2 = window->DC.CursorPos;
					b2.x += quarter;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b2.x, b2.y - 3 }, quarter, 3);
					}
					else if (mainTab == MainTabs::Triggerbot) {
						ImGui::GradientRect(fgDrawList, &active, { b2.x, b2.y - 3 }, quarter, 3);
					}
					ImGui::SameLine();
				}

				bool pushedVisuals = false;

				// Tab 3 (Visuals)
				{
					if (mainTab == MainTabs::Visuals) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedVisuals = true;
					}
					else {
						pushedVisuals = false;
					}
					if (ImGui::Button("Visuals", ImVec2(quarter, 27))) {
						mainTab = MainTabs::Visuals;
					}
					if (mainTab == MainTabs::Visuals) {
						if (pushedVisuals) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b3 = window->DC.CursorPos;
					b3.x += quarter * 2;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b3.x, b3.y - 3 }, quarter, 3);
					}
					else if (mainTab == MainTabs::Visuals) {
						ImGui::GradientRect(fgDrawList, &active, { b3.x, b3.y - 3 }, quarter, 3);
					}
					ImGui::SameLine();
				}

				bool pushedMisc = false;

				// Tab 4 (Misc)
				{
					if (mainTab == MainTabs::Misc) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedMisc = true;
					}
					else {
						pushedMisc = false;
					}
					if (ImGui::Button("Misc", ImVec2(quarter, 27))) {
						mainTab = MainTabs::Misc;
					}
					if (mainTab == MainTabs::Misc) {
						if (pushedMisc) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					b1.x += quarter * 3;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, quarter, 3);
					}
					else if (mainTab == MainTabs::Misc) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, quarter, 3);
					}
					ImGui::SameLine();
				}
			}
			ImGui::Dummy(ImVec2(0, 0));

			// Visuals subsections
			if (mainTab == MainTabs::Visuals) {
				{
					bool pushedtab20 = false;
					if (visualsTab == VisualsTabs::Players) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedtab20 = true;
					}
					else {
						pushedtab20 = false;
					}
					if (ImGui::Button("Players", ImVec2(sixth, 27))) {
						visualsTab = VisualsTabs::Players;
					}
					if (visualsTab == VisualsTabs::Players) {
						if (pushedtab20) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					//b1.x += fifth * 3;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, sixth, 3);
					}
					else if (visualsTab == VisualsTabs::Players) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, sixth, 3);
					}
					ImGui::SameLine();
				}

				{
					bool pushedtab20 = false;
					if (visualsTab == VisualsTabs::Buildings) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedtab20 = true;
					}
					else {
						pushedtab20 = false;
					}
					if (ImGui::Button("Buildings", ImVec2(sixth, 27))) {
						visualsTab = VisualsTabs::Buildings;
					}
					if (visualsTab == VisualsTabs::Buildings) {
						if (pushedtab20) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					b1.x += sixth;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, sixth, 3);
					}
					else if (visualsTab == VisualsTabs::Buildings) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, sixth, 3);
					}
					ImGui::SameLine();
				}
				{
					bool pushedtab20 = false;
					if (visualsTab == VisualsTabs::World) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedtab20 = true;
					}
					else {
						pushedtab20 = false;
					}
					if (ImGui::Button("World", ImVec2(sixth, 27))) {
						visualsTab = VisualsTabs::World;
					}
					if (visualsTab == VisualsTabs::World) {
						if (pushedtab20) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					b1.x += sixth * 2;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, sixth, 3);
					}
					else if (visualsTab == VisualsTabs::World) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, sixth, 3);
					}
					ImGui::SameLine();
				}
				{
					bool pushedVisualsTab = false;
					if (visualsTab == VisualsTabs::Fonts) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedVisualsTab = true;
					}
					else {
						pushedVisualsTab = false;
					}
					if (ImGui::Button("Fonts", ImVec2(sixth, 27))) {
						visualsTab = VisualsTabs::Fonts;
					}
					if (visualsTab == VisualsTabs::Fonts) {
						if (pushedVisualsTab) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					b1.x += sixth * 3;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, sixth, 3);
					}
					else if (visualsTab == VisualsTabs::Fonts) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, sixth, 3);
					}
					ImGui::SameLine();
				}
				{
					bool pushedtab20 = false;
					if (visualsTab == VisualsTabs::MiscVisuals) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedtab20 = true;
					}
					else {
						pushedtab20 = false;
					}
					if (ImGui::Button("Misc###MiscButtonTab", ImVec2(sixth, 27))) {
						visualsTab = VisualsTabs::MiscVisuals;
					}
					if (visualsTab == VisualsTabs::MiscVisuals) {
						if (pushedtab20) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					b1.x += sixth * 4;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, sixth, 3);
					}
					else if (visualsTab == VisualsTabs::MiscVisuals) {
						ImGui::GradientRect(fgDrawList, &active, { b1.x, b1.y - 3 }, sixth, 3);
					}
					ImGui::SameLine();
				}
				{
					bool pushedtab20 = false;
					if (visualsTab == VisualsTabs::Radar) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
						pushedtab20 = true;
					}
					else {
						pushedtab20 = false;
					}
					if (ImGui::Button("Radar###RadarButtonTab", ImVec2(sixth, 27))) {
						visualsTab = VisualsTabs::Radar;
					}
					if (visualsTab == VisualsTabs::Radar) {
						if (pushedtab20) {
							ImGui::PopStyleColor();
						}
					}
					ImVec2 b1 = window->DC.CursorPos;
					b1.x += sixth * 5;
					if (ImGui::IsItemHovered()) {
						ImGui::GradientRect(fgDrawList, &hover, { b1.x - 1, b1.y - 3 }, sixth, 3);
					}
					else if (visualsTab == VisualsTabs::Radar) {
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

				// Columns 1
				ImGui::BeginChild("Feature 1", mainTab == MainTabs::Visuals ? (visualsTab == VisualsTabs::MiscVisuals ? ImVec2((winSize.x / 2) - 16, winSize.y - 113) : ImVec2((winSize.x / 3) - 13, winSize.y - 113)) : ImVec2((winSize.x / 3) - 13, winSize.y - 86), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_HorizontalScrollbar);
				{
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();

					// Menu: Aimbot (Column 1)
					if (mainTab == MainTabs::Aimbot) {
						SectionTitle("Global");

						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Aimbot", &Vars::Aimbot::Global::Active.m_Var); HelpMarker("Aimbot master switch");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Target", Colors::Target);
						InputKeybind("Aimbot key", Vars::Aimbot::Global::AimKey); HelpMarker("The key to enable aimbot");
						WidthSlider("Aimbot FoV####AimbotFoV", &Vars::Aimbot::Global::AimFOV.m_Var, 0.f, 180.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Aimbot FOV circle", Colors::FOVCircle);
						ImGui::Checkbox("Autoshoot###AimbotAutoshoot", &Vars::Aimbot::Global::AutoShoot.m_Var); HelpMarker("Automatically shoot when a target is found");
						ImGui::PushItemWidth(100);
						MultiCombo({ "Players", "Buildings" }, { &Vars::Aimbot::Global::AimPlayers.m_Var, &Vars::Aimbot::Global::AimBuildings.m_Var }, "Choose which targets the Aimbot should aim at", "Aim targets");
						MultiCombo({ "Invulnerable", "Cloaked", "Friends", "Taunting" }, { &Vars::Aimbot::Global::IgnoreInvlunerable.m_Var, &Vars::Aimbot::Global::IgnoreCloaked.m_Var, &Vars::Aimbot::Global::IgnoreFriends.m_Var, &Vars::Aimbot::Global::IgnoreTaunting.m_Var }, "Choose which targets should be ignored", "Ignored targets###HitscanIgnoredTargets");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Invulnerable colour", Colors::Invuln);
						ImGui::Dummy(ImVec2(0, 20));

						SectionTitle("Crits");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 6;
						if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Crit hack", &Vars::Crits::Active.m_Var);  HelpMarker("Enables the crit hack (This doesn't work)");
						InputKeybind("Crit key", Vars::Crits::CritKey); HelpMarker("Will try to fire crits when the key is held");
						

						ImGui::Dummy(ImVec2(0, 20));
						SectionTitle("Backtrack");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 6;
						if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Active", &Vars::Backtrack::Enabled.m_Var); HelpMarker("If you shoot at the backtrack manually it will attempt to hit it");
						ImGui::Checkbox("Aimbot aims last tick", &Vars::Backtrack::Aim.m_Var); HelpMarker("Aimbot aims at the last tick if visible");
						ImGui::PopStyleVar();
					}

					// Menu: Triggerbot (Column 1)
					else if (mainTab == MainTabs::Triggerbot) {
						SectionTitle("Global");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Triggerbot", &Vars::Triggerbot::Global::Active.m_Var); HelpMarker("Global triggerbot master switch");
						InputKeybind("Trigger key", Vars::Triggerbot::Global::TriggerKey); HelpMarker("The key which activates the triggerbot");
						ImGui::PushItemWidth(100);
						MultiCombo({ "Invulnerable", "Cloaked", "Friends" }, { &Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var, &Vars::Aimbot::Global::IgnoreCloaked.m_Var, &Vars::Aimbot::Global::IgnoreFriends.m_Var }, "Choose which targets should be ignored", "Ignored targets###TriggerIgnoredTargets");
						ImGui::PopItemWidth();
						ImGui::Dummy(ImVec2(0, 20));

						SectionTitle("Autoshoot");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Autoshoot###AutoshootTrigger", &Vars::Triggerbot::Shoot::Active.m_Var); HelpMarker("Shoots if mouse is over a target");
						ImGui::PushItemWidth(100);
						MultiCombo({ "Players", "Buildings" }, { &Vars::Triggerbot::Shoot::TriggerPlayers.m_Var, &Vars::Triggerbot::Shoot::TriggerBuildings.m_Var }, "Choose which target the triggerbot should shoot at", "Trigger targets");
						ImGui::PopItemWidth();
						ImGui::Checkbox("Head only###TriggerHeadOnly", &Vars::Triggerbot::Shoot::HeadOnly.m_Var); HelpMarker("Auto shoot will only shoot if you are aiming at the head");
						ImGui::Checkbox("Wait for charge###TriggerbotWaitForCharge", &Vars::Triggerbot::Shoot::WaitForCharge.m_Var); HelpMarker("Auto shoot will only shoot if the sniper is charged enough to kill in one hit / is fully charged");
						WidthSlider("Head scale###TriggerHeadScale", &Vars::Triggerbot::Shoot::HeadScale.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The scale at which the auto shoot will try to shoot the targets head");
						ImGui::PopStyleVar();
					}

					// Menu: Visuals/Players (Column 1)
					else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::Players) {
						SectionTitle("ESP Main");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("ESP###EnableESP", &Vars::ESP::Main::Active.m_Var); HelpMarker("Global ESP master switch");
						ImGui::Checkbox("Outlined health bars", &Vars::ESP::Main::Outlinedbar.m_Var); HelpMarker("Will outline the health bars");
						ImGui::Checkbox("Relative colours", &Vars::ESP::Main::EnableTeamEnemyColors.m_Var); HelpMarker("Chooses colors relative to your team (team/enemy)");
						if (Vars::ESP::Main::EnableTeamEnemyColors.m_Var) {
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
							ImGui::SetNextItemWidth(20);
							ColorPicker("Enemy color", Colors::Enemy);
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
							ImGui::SetNextItemWidth(44);
							ColorPicker("Team color", Colors::rTeam);
						}
						else {
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
							ImGui::SetNextItemWidth(20);
							ColorPicker("RED Team color", Colors::TeamRed);
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
							ImGui::SetNextItemWidth(44);
							ColorPicker("BLU Team color", Colors::TeamBlu);
						}
						ImGui::Dummy(ImVec2(0, 20));

						SectionTitle("Player ESP");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Player ESP###EnablePlayerESP", &Vars::ESP::Players::Active.m_Var); HelpMarker("Will draw useful information/indicators on players");
						ImGui::Checkbox("Name ESP###PlayerNameESP", &Vars::ESP::Players::Name.m_Var); HelpMarker("Will draw the players name");
						ImGui::Checkbox("Name ESP box###PlayerNameESPBox", &Vars::ESP::Players::NameBox.m_Var); HelpMarker("Will draw a box around players name to make it stand out");
						ImGui::Checkbox("Self ESP###SelfESP", &Vars::ESP::Players::ShowLocal.m_Var); HelpMarker("Will draw ESP on local player (thirdperson)");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Local colour", Colors::Local);
						static const char* ignoreTeamArr[]{ "Off", "All", "Only friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore team###IgnoreTeamESPp", &Vars::ESP::Players::IgnoreTeammates.m_Var, ignoreTeamArr, IM_ARRAYSIZE(ignoreTeamArr)); ImGui::PopItemWidth(); HelpMarker("Which teammates the ESP will ignore drawing on");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Friend colour", Colors::Friend);
						static const char* ignoreCloakArr[]{ "Off", "All", "Only enemies" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore cloaked###IgnoreCloakESPp", &Vars::ESP::Players::IgnoreCloaked.m_Var, ignoreCloakArr, IM_ARRAYSIZE(ignoreCloakArr)); ImGui::PopItemWidth(); HelpMarker("Which cloaked spies the ESP will ignore drawing on");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Cloaked colour", Colors::Cloak);
						static const char* espUberArr[]{ "Off", "Text", "Bar" }; ImGui::PushItemWidth(100); ImGui::Combo("Ubercharge###PlayerUber", &Vars::ESP::Players::Uber.m_Var, espUberArr, IM_ARRAYSIZE(espUberArr)); ImGui::PopItemWidth(); HelpMarker("Will draw how much ubercharge a medic has");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Ubercharge colour", Colors::UberColor);
						static const char* classArr[]{ "Off", "Icon", "Text", "Both" }; ImGui::PushItemWidth(100); ImGui::Combo("Class###PlayerIconClass", &Vars::ESP::Players::Class.m_Var, classArr, IM_ARRAYSIZE(classArr)); ImGui::PopItemWidth(); HelpMarker("Will draw the class the player is");
						ImGui::Checkbox("Weapon icons", &Vars::ESP::Players::WeaponIcon.m_Var); HelpMarker("Shows an icon for the weapon that the player has currently equipped");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Invulnerable colour", Colors::WeaponIcon);
						ImGui::Checkbox("Health bar###ESPPlayerHealthBar", &Vars::ESP::Players::HealthBar.m_Var); HelpMarker("Will draw a bar visualizing how much health the player has");
						ImGui::Checkbox("Health text###ESPPlayerHealthText", &Vars::ESP::Players::Health.m_Var); HelpMarker("Will draw the players health, as well as their max health");
						ImGui::Checkbox("Condition", &Vars::ESP::Players::Cond.m_Var); HelpMarker("Will draw what conditions the player is under");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Condition colour", Colors::Cond);
						ImGui::Checkbox("GUID", &Vars::ESP::Players::GUID.m_Var); HelpMarker("Show's the players Steam ID");
						static const char* boxArr[]{ "Off", "Bounding", "Cornered", "3D" }; ImGui::PushItemWidth(100); ImGui::Combo("Box###PlayerBoxESP", &Vars::ESP::Players::Box.m_Var, boxArr, IM_ARRAYSIZE(boxArr)); ImGui::PopItemWidth(); HelpMarker("What sort of box to draw on players");
						static const char* bonesESP[]{ "Off", "Custom colour", "Health" }; ImGui::PushItemWidth(100); ImGui::Combo("Skeleton###PlayerSkellington", &Vars::ESP::Players::Bones.m_Var, bonesESP, IM_ARRAYSIZE(bonesESP)); ImGui::PopItemWidth(); HelpMarker("Will draw the bone structure of the player");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Skellington colour", Colors::Bones);
						ImGui::Checkbox("Lines###Playerlines", &Vars::ESP::Players::Lines.m_Var); HelpMarker("Draws lines from the local players position to enemies position");
						ImGui::Checkbox("Dlights###PlayerDlights", &Vars::ESP::Players::Dlights.m_Var); HelpMarker("Will make players emit a dynamic light around them");
						WidthSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Players::DlightRadius.m_Var, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
						WidthSlider("ESP alpha###PlayerESPAlpha", &Vars::ESP::Players::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

						ImGui::PopStyleVar();
					}

					// Menu: Visuals/Buildings (Column 1)
					else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::Buildings) {
						SectionTitle("Building ESP");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Building ESP###BuildinGESPSwioifas", &Vars::ESP::Buildings::Active.m_Var); HelpMarker("Will draw useful information/indicators on buildings");
						ImGui::Checkbox("Ignore team buildings###BuildingESPIgnoreTeammates", &Vars::ESP::Buildings::IgnoreTeammates.m_Var); HelpMarker("Whether or not to draw ESP on your teams buildings");
						ImGui::Checkbox("Name ESP###BuildingNameESP", &Vars::ESP::Buildings::Name.m_Var); HelpMarker("Will draw the players name");
						ImGui::Checkbox("Name ESP box###BuildingNameESPBox", &Vars::ESP::Buildings::NameBox.m_Var); HelpMarker("Will draw a box around the buildings name to make it stand out");
						ImGui::Checkbox("Health bar###Buildinghelathbar", &Vars::ESP::Buildings::HealthBar.m_Var); HelpMarker("Will draw a bar visualizing how much health the building has");
						ImGui::Checkbox("Health text###buildinghealth", &Vars::ESP::Buildings::Health.m_Var); HelpMarker("Will draw the building's health, as well as its max health");
						ImGui::Checkbox("Building owner###Buildingowner", &Vars::ESP::Buildings::Owner.m_Var); HelpMarker("Shows who built the building");
						ImGui::Checkbox("Building level###Buildinglevel", &Vars::ESP::Buildings::Level.m_Var); HelpMarker("Will draw what level the building is");
						ImGui::Checkbox("Building condition###Buildingconditions", &Vars::ESP::Buildings::Cond.m_Var); HelpMarker("Will draw what conditions the building is under");
						ImGui::Checkbox("Lines###buildinglines", &Vars::ESP::Buildings::Lines.m_Var); HelpMarker("Draws lines from the local players position to the buildings position");
						static const char* boxArr[]{ "Off", "Bounding", "Cornered", "3D" }; ImGui::PushItemWidth(100); ImGui::Combo("Box###PBuildingBoxESP", &Vars::ESP::Buildings::Box.m_Var, boxArr, IM_ARRAYSIZE(boxArr)); ImGui::PopItemWidth(); HelpMarker("What sort of box to draw on buildings");
						ImGui::Checkbox("Dlights###PlayerDlights", &Vars::ESP::Buildings::Dlights.m_Var); HelpMarker("Will make buildings emit a dynamic light around them, although buildings can't move some I'm not sure that the lights are actually dynamic here...");
						WidthSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Buildings::DlightRadius.m_Var, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
						WidthSlider("ESP alpha###BuildingESPAlpha", &Vars::ESP::Buildings::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the ESP should be");
						ImGui::PopStyleVar();
					}

					// Menu: Visuals/World (Column 1)
					else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::World) {
						SectionTitle("World ESP");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("World ESP###WorldESPActive", &Vars::ESP::World::Active.m_Var); HelpMarker("World ESP master switch");
						ImGui::Checkbox("Health packs###WorldESPHealthPacks", &Vars::ESP::World::HealthText.m_Var); HelpMarker("Will draw ESP on health packs");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Health pack colour", Colors::Health); HelpMarker("Color for health pack ESP");
						ImGui::Checkbox("Ammo packs###WorldESPAmmoPacks", &Vars::ESP::World::AmmoText.m_Var); HelpMarker("Will draw chams on ammo packs");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Ammo pack colour", Colors::Ammo); HelpMarker("Color for ammo pack ESP");
						WidthSlider("ESP alpha###WordlESPAlpha", &Vars::ESP::World::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the world ESP should be");
						ImGui::PopStyleVar();
					}

					// Menu: Visuals/Fonts (Column 1)
					else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::Fonts) {
						SectionTitle("ESP Font");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
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

						SectionTitle("Name Font");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
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

					// Menu: Visual/Misc (Column 1)
					else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::MiscVisuals) {
						SectionTitle("World & UI");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::PushItemWidth(150); ImGui::SliderInt("Field of view", &Vars::Visuals::FieldOfView.m_Var, 70, 150, "%d"); ImGui::PopItemWidth(); HelpMarker("How many degrees of field of vision you would like");
						const char* visionModifiers[]{ "Off", "Pyrovision", "Halloween", "Romevision" }; ImGui::PushItemWidth(150); ImGui::Combo("Vision modifiers", &Vars::Visuals::Vision.m_Var, visionModifiers, IM_ARRAYSIZE(visionModifiers)); ImGui::PopItemWidth(); HelpMarker("Vision modifiers");
						ImGui::PushItemWidth(150);
						MultiCombo({ "World", "Sky", "Prop Wireframe" }, { &Vars::Visuals::WorldModulation.m_Var, &Vars::Visuals::SkyModulation.m_Var, &Vars::Visuals::PropWireframe.m_Var }, "Select which types of modulation you want to enable", "Modulations");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("World modulation colour", Colors::WorldModulation);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
						ImGui::SetNextItemWidth(44);
						ColorPicker("Sky modulation colour", Colors::SkyModulation);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 68);
						ImGui::SetNextItemWidth(68);
						ColorPicker("Prop modulation colour", Colors::StaticPropModulation);
						ImGui::PushItemWidth(150);
						MultiCombo({ "Scope", "Zoom", "Disguises", "Taunts", "Interpolation", "View Punch" }, { &Vars::Visuals::RemoveScope.m_Var, &Vars::Visuals::RemoveZoom.m_Var, &Vars::Visuals::RemoveDisguises.m_Var, &Vars::Visuals::RemoveTaunts.m_Var, &Vars::Misc::DisableInterpolation.m_Var, &Vars::Visuals::RemovePunch.m_Var }, "Select what you want to remove", "Removals");
						MultiCombo({ "Aimbot Crosshair", "Render Proj Line", "Bullet Tracers", "Viewmodel Aimbot" }, { &Vars::Visuals::CrosshairAimPos.m_Var, &Vars::Visuals::AimPosSquare.m_Var, &Vars::Visuals::BulletTracer.m_Var, &Vars::Visuals::AimbotViewmodel.m_Var }, "Which types of predictions should be drawn", "Predictions");
						ImGui::PopItemWidth(); // ?
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Bullet tracer colour", Colors::BulletTracer);
						MultiCombo({ "Votes (Console)", "Votes (Text)", "Votes (Chat)", "Votes (Party)", "Damage Logs (Console)", "Damage Logs (Text)", "Damage Logs (Chat)", "Class Changes (Text)", "Class Changes (Chat)"}, {&Vars::Misc::VoteRevealerConsole.m_Var, &Vars::Misc::VoteRevealerText.m_Var, &Vars::Misc::VoteRevealerChat.m_Var, &Vars::Misc::VoteRevealerParty.m_Var, &Vars::Visuals::damageLoggerConsole.m_Var, &Vars::Visuals::damageLoggerText.m_Var, &Vars::Visuals::damageLoggerChat.m_Var, &Vars::Visuals::ChatInfoText.m_Var, &Vars::Visuals::ChatInfoChat.m_Var}, "What & How should events be logged", "Event Logging");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("GUI Notif Background", Colors::NotifBG);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
						ImGui::SetNextItemWidth(44);
						ColorPicker("GUI Notif Outline", Colors::NotifOutline);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 68);
						ImGui::SetNextItemWidth(68);
						ColorPicker("GUI Notif Colour", Colors::NotifText);
						ImGui::PushItemWidth(150); ImGui::SliderFloat("GUI Notif Time", &Vars::Visuals::despawnTime.m_Var, 0.5f, 3.f, "%.1f"); ImGui::PopItemWidth();
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
							ColorPicker("Inner line color", Colors::NoscopeLines1);
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
							ImGui::SetNextItemWidth(44);
							ColorPicker("Outer line color", Colors::NoscopeLines2);
						}
						ImGui::Checkbox("Draw Hitboxes", &Vars::Aimbot::Global::showHitboxes.m_Var); HelpMarker("Shows client hitboxes for enemies once they are attacked (not bbox)");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Hitbox matrix colour", Colors::bonecolor);
						ImGui::Checkbox("Clear Hitboxes", &Vars::Aimbot::Global::clearPreviousHitbox.m_Var); HelpMarker("Removes previous drawn hitboxes to mitigate clutter");
						ImGui::PushItemWidth(150); ImGui::SliderInt("Hitbox Draw Time", &Vars::Aimbot::Global::hitboxTime.m_Var, 1, 5); HelpMarker("Removes previous drawn hitboxes after n seconds");

						const char* specModes[]{ "Off", "Draggable", "Static", "Static + Avatars" }; ImGui::PushItemWidth(100); ImGui::Combo("Spectator list", &Vars::Visuals::SpectatorList.m_Var, specModes, IM_ARRAYSIZE(specModes)); ImGui::PopItemWidth();
						ImGui::Dummy(ImVec2(0, 20));

						SectionTitle("Viewmodel Offset");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushItemWidth(150); ImGui::SliderInt("VM Off X", &Vars::Visuals::VMOffX.m_Var, -90, 90);
						ImGui::PushItemWidth(150); ImGui::SliderInt("VM Off Y", &Vars::Visuals::VMOffY.m_Var, -90, 90);
						ImGui::PushItemWidth(150); ImGui::SliderInt("VM Off Z", &Vars::Visuals::VMOffZ.m_Var, -90, 90);
						ImGui::PushItemWidth(150); ImGui::SliderInt("VM Roll", &Vars::Visuals::VMRoll.m_Var, -180, 180);
						ImGui::Dummy(ImVec2(0, 20));

						SectionTitle("DT Indicator");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						const char* dtBarStyles[]{ "Off", "Default", "Nitro", "Rijin" }; ImGui::PushItemWidth(150); ImGui::Combo("DT indicator style", &Vars::Misc::CL_Move::DTBarStyle.m_Var, dtBarStyles, IM_ARRAYSIZE(dtBarStyles)); ImGui::PopItemWidth(); HelpMarker("Which style to do the bar style");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("DT charging left", Colors::DtChargingLeft);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
						ImGui::SetNextItemWidth(44);
						ColorPicker("DT charging right", Colors::DtChargingRight);
						if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 3) {
							ImGui::PushItemWidth(150); ImGui::SliderInt("DT Bar height###dtBHeightNitro", &Vars::Misc::CL_Move::DTBarScaleY.m_Var, 1, 25);
							ImGui::PopItemWidth(); ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
							ImGui::SetNextItemWidth(20);
							ColorPicker("DT charged left (Nitro)", Colors::DtChargedLeft);
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
							ImGui::SetNextItemWidth(44);
							ColorPicker("DT charged right (Nitro)", Colors::DtChargedRight);
							ImGui::PushItemWidth(150); ImGui::SliderInt("DT Bar width###dtBWidthNitro", &Vars::Misc::CL_Move::DTBarScaleX.m_Var, 100, 1000);
							ImGui::PopItemWidth();
						}
						else {
							ImGui::PushItemWidth(150); ImGui::SliderInt("DT Bar height###dtBHeight", &Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var, 1, 30);
							ImGui::PopItemWidth(); ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
							ImGui::SetNextItemWidth(20);
							ColorPicker("DT charged left", Colors::DtChargedLeft);
							ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
							ImGui::SetNextItemWidth(44);
							ColorPicker("DT charged right", Colors::DtChargedRight);
							ImGui::PushItemWidth(150); ImGui::SliderInt("DT Bar width###dtBWidth", &Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var, 1, 30);
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

						SectionTitle("Attribute Changer");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
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
						
						ImGui::Dummy(ImVec2(0, 20));

						SectionTitle("Ragdoll effects");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Enemy only###RagdollEnemyOnly", &Vars::Visuals::RagdollEffects::EnemyOnly.m_Var); HelpMarker("Only runs it on enemies");
						MultiCombo({ "Burning", "Electrocuted", "Become ash", "Dissolve"}, {&Vars::Visuals::RagdollEffects::Burning.m_Var, &Vars::Visuals::RagdollEffects::Electrocuted.m_Var, &Vars::Visuals::RagdollEffects::BecomeAsh.m_Var, &Vars::Visuals::RagdollEffects::Dissolve.m_Var }, "Ragdoll particle effects", "Effects###RagdollEffects");
						if (ImGui::Checkbox("Gold ragdoll", &Vars::Visuals::RagdollEffects::Gold.m_Var)) {
							Vars::Visuals::RagdollEffects::Ice.m_Var = false;
						}
						HelpMarker("Will make their ragdoll gold");
						if (ImGui::Checkbox("Ice ragdoll", &Vars::Visuals::RagdollEffects::Ice.m_Var)) {
							Vars::Visuals::RagdollEffects::Gold.m_Var = false;
						}
						HelpMarker("Will make their ragdoll ice");

						ImGui::Dummy(ImVec2(0, 20));
						SectionTitle("Freecam");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						InputKeybind("Freecam Key", Vars::Visuals::FreecamKey);  HelpMarker("Allows you to freely move your camera when holding the key");
						WidthSlider("Freecam Speed", &Vars::Visuals::FreecamSpeed.m_Var, 1.f, 20.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Movement speed of freecam");

						ImGui::Dummy(ImVec2(0, 20));
						SectionTitle("Camera");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						static const char* camModes[]{ "Off", "Mirror", "Spy" }; ImGui::PushItemWidth(100); ImGui::Combo("Camera mode", &Vars::Visuals::CameraMode.m_Var, camModes, IM_ARRAYSIZE(camModes)); ImGui::PopItemWidth(); HelpMarker("What the camera should display");
						WidthSlider("Camera FOV", &Vars::Visuals::CameraFOV.m_Var, 40.f, 130.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("FOV of the camera window");

						ImGui::PopStyleVar();
					}

					// Menu: Visuals/Radar (Column 1)
					else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::Radar) {
						SectionTitle("Main");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Enable Radar###RadarActive", &Vars::Radar::Main::Active.m_Var); HelpMarker("Will show nearby things relative to your player");
						ImGui::PushItemWidth(100); ImGui::SliderInt("Size###RadarSize", &Vars::Radar::Main::Size.m_Var, 20, 200); ImGui::PopItemWidth(); HelpMarker("The size of the radar window");
						ImGui::PushItemWidth(100); ImGui::SliderInt("Range###RadarRange", &Vars::Radar::Main::Range.m_Var, 50, 3000, "%d"); ImGui::PopItemWidth(); HelpMarker("The range of the radar");
						ImGui::PushItemWidth(100); ImGui::SliderInt("Background alpha###RadarBGA", &Vars::Radar::Main::BackAlpha.m_Var, 0, 255, "%d"); ImGui::PopItemWidth(); HelpMarker("The background alpha of the radar");
						ImGui::PushItemWidth(100); ImGui::SliderInt("Line alpha###RadarLineA", &Vars::Radar::Main::LineAlpha.m_Var, 0, 255, "%d"); ImGui::PopItemWidth(); HelpMarker("The line alpha of the radar");

						ImGui::Dummy(ImVec2(0, 20));
						SectionTitle("Players");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						const char* iconPlayersRadar[]{ "Scoreboard", "Portraits", "Avatar" }; ImGui::PushItemWidth(100); ImGui::Combo("Icon###radari", &Vars::Radar::Players::IconType.m_Var, iconPlayersRadar, IM_ARRAYSIZE(iconPlayersRadar)); ImGui::PopItemWidth(); HelpMarker("What sort of icon to represent players with");
						const char* backgroundpradar[]{ "Off", "Rectangle", "Texture" }; ImGui::PushItemWidth(100); ImGui::Combo("Background###radarb", &Vars::Radar::Players::BackGroundType.m_Var, backgroundpradar, IM_ARRAYSIZE(backgroundpradar)); ImGui::PopItemWidth(); HelpMarker("What sort of background to put on players on the radar");
						ImGui::Checkbox("Outline###radaro", &Vars::Radar::Players::Outline.m_Var); HelpMarker("Will put an outline on players on the radar");
						static const char* ignoreTeammatespRadar[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###radarplayersteam", &Vars::Radar::Players::IgnoreTeam.m_Var, ignoreTeammatespRadar, IM_ARRAYSIZE(ignoreTeammatespRadar)); ImGui::PopItemWidth(); HelpMarker("Which teammates the radar will ignore drawing on");
						static const char* ignoreCloakedpRadar[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore cloaked###radarplayerscloaked", &Vars::Radar::Players::IgnoreCloaked.m_Var, ignoreCloakedpRadar, IM_ARRAYSIZE(ignoreCloakedpRadar)); ImGui::PopItemWidth(); HelpMarker("Which cloaked players the radar will ignore drawing on");
						ImGui::Checkbox("Health bar###radarhealt", &Vars::Radar::Players::Health.m_Var); HelpMarker("Will show players health on the radar");
						ImGui::PushItemWidth(100); ImGui::SliderInt("Icon size###playersizeiconradar", &Vars::Radar::Players::IconSize.m_Var, 12, 30, "%d"); ImGui::PopItemWidth(); HelpMarker("The icon size of players on the radar");
						ImGui::PopStyleVar();
					}

					// Menu: Misc (Column 1)
					else if (mainTab == MainTabs::Misc) {
						SectionTitle("Movement");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("No push", &Vars::Misc::NoPush.m_Var); HelpMarker("Will make teammates unable to push you around");
						ImGui::Checkbox("Bunnyhop", &Vars::Misc::AutoJump.m_Var); HelpMarker("Will jump as soon as you touch the ground again, keeping speed between jumps");
						const char* autoStrafeModes[]{ "Off", "Normal", "Directional" }; ImGui::PushItemWidth(100); ImGui::Combo("Autostrafe", &Vars::Misc::AutoStrafe.m_Var, autoStrafeModes, IM_ARRAYSIZE(autoStrafeModes)); ImGui::PopItemWidth(); HelpMarker("Will strafe for you in air automatically so that you gain speed");
						ImGui::Checkbox("Edge jump", &Vars::Misc::EdgeJump.m_Var); HelpMarker("Will jump at the very end of whatever platform you're on, allowing you to perfectly make longer jumps.");
						InputKeybind("Edge jump key", Vars::Misc::EdgeJumpKey, false);  HelpMarker("Edge jump bind, leave as None for always on");
						ImGui::Checkbox("Auto rocket jump", &Vars::Misc::AutoRocketJump.m_Var); HelpMarker("Will rocket jump at the angle you're looking at when you press mouse2 with a rocket launcher");
						ImGui::Checkbox("Anti-AFK", &Vars::Misc::AntiAFK.m_Var); HelpMarker("Will make you jump every now and then so you don't get kicked for idling");
						ImGui::Checkbox("Taunt slide", &Vars::Misc::TauntSlide.m_Var); HelpMarker("Allows you to input in taunts");
						ImGui::Checkbox("Taunt control", &Vars::Misc::TauntControl.m_Var); HelpMarker("Gives full control if enabled with taunt slide");
						const char* rollModes[]{ "Off", "Backwards", "Fake forward" }; ImGui::PushItemWidth(100); ImGui::Combo("Crouch speed", &Vars::Misc::Roll.m_Var, rollModes, IM_ARRAYSIZE(rollModes)); ImGui::PopItemWidth(); HelpMarker("Allows you to go at normal walking speed when crouching (affects many things, use with caution)");

						ImGui::Dummy(ImVec2(0, 20));
						SectionTitle("Misc");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Noisemaker spam", &Vars::Misc::NoisemakerSpam.m_Var); HelpMarker("Will spam your noisemaker without using its charges");
						const char* spamModes[]{ "Off", "Fedoraware", "Lmaobox", "Cathook" }; ImGui::PushItemWidth(100); ImGui::Combo("Chat spam", &Vars::Misc::ChatSpam.m_Var, spamModes, IM_ARRAYSIZE(spamModes)); ImGui::PopItemWidth();
						ImGui::Checkbox("Chat censor", &Vars::Misc::ChatCensor.m_Var); HelpMarker("Clears the chat when someone accuses you of cheating");
						ImGui::Checkbox("Rage retry", &Vars::Misc::RageRetry.m_Var); HelpMarker("Will automatically reconnect when your health is low");
						ImGui::PushItemWidth(100); ImGui::SliderInt("Rage Retry health", &Vars::Misc::RageRetryHealth.m_Var, 1, 99, "%d%%"); HelpMarker("Minimum health percentage that will cause a retry");
						ImGui::Checkbox("Anti autobalance", &Vars::Misc::AntiAutobal.m_Var); HelpMarker("Will rejoin the server to prevent you being autobalanced");
						ImGui::Checkbox("Cat identify", &Vars::Misc::BeCat.m_Var); HelpMarker("Will mark you as a cathook instance to other cathook instances (basically catbots)");
						ImGui::Checkbox("Force sv_cheats", &Vars::Misc::CheatsBypass.m_Var); HelpMarker("Will force sv_cheats 1, allowing commands like tf_viewmodels_offset_override, fog_override etc.");
						ImGui::Checkbox("MvM instant respawn", &Vars::Misc::MVMRes.m_Var); HelpMarker("Will respawn you instantly in MvM");
						ImGui::Checkbox("Vote revealer", &Vars::Misc::VoteRevealerText.m_Var); HelpMarker("Will say who voted F1 or F2 in chat");
						ImGui::Checkbox("Log votes to party", &Vars::Misc::VoteRevealerParty.m_Var); HelpMarker("Will send vote information to party chat (use with caution)");
						ImGui::Checkbox("Ping reducer", &Vars::Misc::PingReducer.m_Var); HelpMarker("Reduces your ping on the scoreboard");
						if (Vars::Misc::PingReducer.m_Var) {
							ImGui::PushItemWidth(100); ImGui::SliderInt("Target ping", &Vars::Misc::PingTarget.m_Var, 0, 200); HelpMarker("Target ping that should be reached");
						}
						ImGui::Checkbox("Infinite Respawn", &Vars::Misc::ExtendFreeze.m_Var); HelpMarker("Grants infinite respawn time");
						const char* autoClass[]{ "Off", "Scout", "Soldier", "Pyro", "Demoman", "Heavy", "Engineer", "Medic", "Sniper", "Spy" }; ImGui::PushItemWidth(100); ImGui::Combo("Auto join", &Vars::Misc::AutoJoin.m_Var, autoClass, IM_ARRAYSIZE(autoClass)); ImGui::PopItemWidth(); HelpMarker("Automatically joins the given class");
						ImGui::PopStyleVar();
					}
					ImGui::EndChild();
				}
				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));

				// Columnd 2
				ImGui::BeginChild("Feature 2", mainTab == MainTabs::Visuals ? (visualsTab == VisualsTabs::MiscVisuals ? ImVec2((winSize.x / 2) - 16, winSize.y - 113) : ImVec2((winSize.x / 3) - 13, winSize.y - 113)) : ImVec2((winSize.x / 3) - 13, winSize.y - 86), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_HorizontalScrollbar);
				{
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();

					// Menu: Aimbot (Column 2)
					if (mainTab == MainTabs::Aimbot) {
						SectionTitle("Hitscan");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						static const char* sortMethodArr[]{ "FOV", "Distance", }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###HitscanSortMethod", &Vars::Aimbot::Hitscan::SortMethod.m_Var, sortMethodArr, IM_ARRAYSIZE(sortMethodArr)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to decide which target to aim at");
						static const char* aimMethodArr[]{ "Plain", "Smooth", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###HitscanAimMethod", &Vars::Aimbot::Hitscan::AimMethod.m_Var, aimMethodArr, IM_ARRAYSIZE(aimMethodArr)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to aim at the target");
						static const char* aimHitboxArr[]{ "Head", "Body", "Auto" }; ImGui::PushItemWidth(100); ImGui::Combo("Hitbox###HitscanHitbox", &Vars::Aimbot::Hitscan::AimHitbox.m_Var, aimHitboxArr, IM_ARRAYSIZE(aimHitboxArr)); ImGui::PopItemWidth(); HelpMarker("Which hitbox the aimbot will target");
						static const char* tapfireMethodArr[]{ "Off", "Distance", "Always" }; ImGui::PushItemWidth(100); ImGui::Combo("Tapfire###HitscanTapfire", &Vars::Aimbot::Hitscan::TapFire.m_Var, tapfireMethodArr, IM_ARRAYSIZE(tapfireMethodArr)); ImGui::PopItemWidth(); HelpMarker("How/If the aimbot chooses to tapfire enemies.");
						WidthSlider("Smooth factor###HitscanSmoothing", &Vars::Aimbot::Hitscan::SmoothingAmount.m_Var, 0.f, 20.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Changes how smooth the aimbot will aim at the target");
						ImGui::PushItemWidth(100);
						MultiCombo({ "Body", "Head", "Buildings" }, { &Vars::Aimbot::Hitscan::ScanHitboxes.m_Var, &Vars::Aimbot::Hitscan::ScanHead.m_Var, &Vars::Aimbot::Hitscan::ScanBuildings.m_Var }, "Choose what the aimbot should multipoint", "Multipoint");
						ImGui::PopItemWidth();
						ImGui::Checkbox("Wait for headshot", &Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var); HelpMarker("The aimbot will wait until it can headshot (if applicable)");
						ImGui::Checkbox("Wait for charge", &Vars::Aimbot::Hitscan::WaitForCharge.m_Var); HelpMarker("The aimbot will wait until the rifle has charged long enough to kill in one shot");
						ImGui::Checkbox("Smooth if spectated", &Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var); HelpMarker("The aimbot will switch to the smooth method if being spectated");
						ImGui::Checkbox("Scoped only", &Vars::Aimbot::Hitscan::ScopedOnly.m_Var); HelpMarker("The aimbot will only shoot if scoped");
						ImGui::Checkbox("Auto scope", &Vars::Aimbot::Hitscan::AutoScope.m_Var); HelpMarker("The aimbot will automatically scope in to shoot");
						ImGui::Checkbox("Auto rev minigun", &Vars::Aimbot::Hitscan::AutoRev.m_Var); HelpMarker("Will rev heavy's minigun regardless of if aimbot has a target");
						ImGui::Checkbox("Bodyaim if lethal", &Vars::Aimbot::Global::BAimLethal.m_Var); HelpMarker("The aimbot will aim for body when damage is lethal to it");
						ImGui::PopStyleVar();
					}

					// Menu: Triggerbot (Column 2)
					else if (mainTab == MainTabs::Triggerbot) {
						SectionTitle("Autostab");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Auto backstab###TriggerAutostab", &Vars::Triggerbot::Stab::Active.m_Var); HelpMarker("Auto backstab will attempt to backstab the target if possible");
						ImGui::Checkbox("Rage mode", &Vars::Triggerbot::Stab::RageMode.m_Var); HelpMarker("Stabs whenever possible by aiming toward the back");
						ImGui::Checkbox("Silent", &Vars::Triggerbot::Stab::Silent.m_Var); HelpMarker("Aim changes made by the rage mode setting aren't visible");
						ImGui::Checkbox("Disguise on kill", &Vars::Triggerbot::Stab::Disguise.m_Var); HelpMarker("Will apply the previous disguise after stabbing");
						ImGui::Checkbox("Ignore razorback", &Vars::Triggerbot::Stab::IgnRazor.m_Var); HelpMarker("Will not attempt to backstab snipers wearing the razorback");
						WidthSlider("Stab range###StabRange", &Vars::Triggerbot::Stab::Range.m_Var, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The range at which auto backstab will attempt to stab");

						ImGui::Dummy(ImVec2(0, 20));

						SectionTitle("Auto Detonate");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Autodetonate###TriggerDet", &Vars::Triggerbot::Detonate::Active.m_Var);
						ImGui::Checkbox("Explode stickies###TriggerSticky", &Vars::Triggerbot::Detonate::Stickies.m_Var); HelpMarker("Detonate sticky bombs when a player is in range");
						ImGui::Checkbox("Detonate flares###TriggerFlares", &Vars::Triggerbot::Detonate::Flares.m_Var); HelpMarker("Detonate detonator flares when a player is in range");
						WidthSlider("Detonation radius###TriggerDetRadius", &Vars::Triggerbot::Detonate::RadiusScale.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The radius around the projectile that it will detonate if a player is in");

						ImGui::PopStyleVar();
					}

					// Menu: Visuals/Buildings (Column 2)
					else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::Buildings) {
						SectionTitle("Building Chams");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Building chams###BuildingChamsBox", &Vars::Chams::Buildings::Active.m_Var); HelpMarker("Building chams master switch");
						ImGui::Checkbox("Ignore team buildings", &Vars::Chams::Buildings::IgnoreTeammates.m_Var); HelpMarker("Whether or not to draw chams on your teams buildings");
						//static const char* ignoreTeamArr[]{ "Off", "All", "Only friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore team###IgnoreTeamChamsb", &Vars::Chams::Buildings::IgnoreTeammates.m_Var, ignoreTeamArr, IM_ARRAYSIZE(ignoreTeamArr)); ImGui::PopItemWidth();
						static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel", "Plastic" }; ImGui::PushItemWidth(100); ImGui::Combo("Building material", &Vars::Chams::Buildings::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth();
						HelpMarker("Which material the chams will apply to the player");

						ImGui::Checkbox("Ignore Z###BuildingChamsIgnoreZ", &Vars::Chams::Players::IgnoreZ.m_Var); HelpMarker("Draw building chams through walls");
						ImGui::PopStyleVar();
					}

					// Menu: Visuals/Players (Column 2)
					else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::Players) {
						SectionTitle("Chams Main");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Chams###ChamsMasterSwitch", &Vars::Chams::Main::Active.m_Var);  HelpMarker("Chams master switch");
						ImGui::Dummy(ImVec2(0, 20));

						SectionTitle("Player Chams");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Player chams###PlayerChamsBox", &Vars::Chams::Players::Active.m_Var); HelpMarker("Player chams master switch");
						ImGui::Checkbox("Self chams###PlayerSelfChamsBox", &Vars::Chams::Players::ShowLocal.m_Var); HelpMarker("Draw chams on the local player");
						static const char* ignoreTeamArr[]{ "Off", "All", "Only friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore team###IgnoreTeamChamsp", &Vars::Chams::Players::IgnoreTeammates.m_Var, ignoreTeamArr, IM_ARRAYSIZE(ignoreTeamArr)); ImGui::PopItemWidth();
						ImGui::Checkbox("Wearable chams###PlayerWearableChams", &Vars::Chams::Players::Wearables.m_Var);  HelpMarker("Will draw chams on player cosmetics");
						ImGui::Checkbox("Weapon chams###PlayerWeaponChams", &Vars::Chams::Players::Weapons.m_Var);  HelpMarker("Will draw chams on player weapons");
						static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel", "Plastic"}; ImGui::PushItemWidth(100); ImGui::Combo("Player material", &Vars::Chams::Players::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth();
						HelpMarker("Which material the chams will apply to the player");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Fresnel base colour", Colors::FresnelBase);
						ImGui::Checkbox("Player glow overlay", &Vars::Chams::Players::GlowOverlay.m_Var);
						ImGui::Checkbox("Ignore Z###PlayerChamsIgnoreZ", &Vars::Chams::Players::IgnoreZ.m_Var); HelpMarker("Draw chams through walls");
						ImGui::Dummy(ImVec2(0, 20));

						SectionTitle("DME Chams");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("DME chams###dmeactive", &Vars::Chams::DME::Active.m_Var); HelpMarker("DME chams master switch");
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
							"Brick",
							"What",
							"Wacky"
						};
						ImGui::PushItemWidth(100);
						ImGui::Combo("Hand material", &Vars::Chams::DME::Hands.m_Var, handsMaterial, IM_ARRAYSIZE(handsMaterial));
						ImGui::PopItemWidth();
						HelpMarker("What material to put on your viewmodels arms/hands");

						static const char* handsProxyMaterial[]{
							"None",
							"Spectrum splattered",
							"Electro skulls",
							"Jazzy",
							"Frozen aurora",
							"Hana",
							"IDK",
							"Ghost thing",
							"Flames",
							"Spook wood",
							"Edgy",
							"Starlight serenity",
							"Fade"
						};
						ImGui::PushItemWidth(100);
						ImGui::Combo("Hand proxy material", &Vars::Chams::DME::HandsProxySkin.m_Var, handsProxyMaterial, IM_ARRAYSIZE(handsProxyMaterial));
						ImGui::PopItemWidth();
						HelpMarker("Puts a cool looking animated skin on your hands");
						ImGui::Checkbox("Hand glow overlay", &Vars::Chams::DME::HandsGlowOverlay.m_Var); HelpMarker("Will place a second glow-like material overlayed on top of the original material");
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
							"Brick",
							"What",
							"Wacky"
						};
						ImGui::PushItemWidth(100);
						ImGui::Combo("Weapon material", &Vars::Chams::DME::Weapon.m_Var, weaponMaterial, IM_ARRAYSIZE(weaponMaterial));
						ImGui::PopItemWidth();
						HelpMarker("What material to put on your viewmodels weapon");

						static const char* weaponProxyMaterial[]{
							"None",
							"Spectrum splattered",
							"Electro skulls",
							"Jazzy",
							"Frozen aurora",
							"Hana",
							"IDK",
							"Ghost thing",
							"Flames",
							"Spook wood",
							"Edgy",
							"Starlight serenity",
							"Fade"
						};
						ImGui::PushItemWidth(100);
						ImGui::Combo("Weapon proxy material", &Vars::Chams::DME::WeaponsProxySkin.m_Var, weaponProxyMaterial, IM_ARRAYSIZE(weaponProxyMaterial));
						ImGui::PopItemWidth();
						HelpMarker("Puts a cool looking animated skin on your weapons");
						ImGui::Checkbox("Weapon glow overlay", &Vars::Chams::DME::WeaponGlowOverlay.m_Var); HelpMarker("Will place a second glow-like material overlayed on top of the original material");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Weapon glow colour", Colors::WeaponOverlay);
						ImGui::PushItemWidth(100); ImGui::SliderInt("Weapon glow boost", &Vars::Chams::DME::WeaponRimMultiplier.m_Var, 1, 100, "%d"); ImGui::PopItemWidth(); HelpMarker("How much the glow effect will be boosted by");
						ImGui::PushItemWidth(100);
						MultiCombo({ "Hands", "Hands overlay", "Weapon", "Weapon overlay" }, { &Vars::Chams::DME::HandsRainbow.m_Var, &Vars::Chams::DME::HandsOverlayRainbow.m_Var, &Vars::Chams::DME::WeaponRainbow.m_Var, &Vars::Chams::DME::WeaponOverlayRainbow.m_Var }, "Rainbow DME chams", "Rainbow DME###RainbowDMEChams");
						ImGui::PopItemWidth();
						
						ImGui::Dummy(ImVec2(0, 20));
						SectionTitle("Backtrack chams");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Backtrack chams", &Vars::Backtrack::BtChams::Enabled.m_Var); HelpMarker("Draws chams to show where a player is");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Backtrack colour", Vars::Backtrack::BtChams::BacktrackColor);
						ImGui::Checkbox("Only draw last tick", &Vars::Backtrack::BtChams::LastOnly.m_Var); HelpMarker("Only draws the last tick (can save FPS)");
						ImGui::Checkbox("Enemy only", &Vars::Backtrack::BtChams::EnemyOnly.m_Var); HelpMarker("You CAN backtrack your teammates. (Whip, medigun)");
						static const char* backtrackMaterial[]{
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
						ImGui::Combo("Backtrack material", &Vars::Backtrack::BtChams::Material.m_Var, backtrackMaterial, IM_ARRAYSIZE(backtrackMaterial));
						ImGui::PopItemWidth();
						ImGui::Checkbox("Ignore Z###BtIgnoreZ", &Vars::Backtrack::BtChams::IgnoreZ.m_Var); HelpMarker("Draws them through walls");

						ImGui::Dummy(ImVec2(0, 20));
						SectionTitle("Fakelag chams");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Fakelag chams", &Vars::Misc::CL_Move::FakelagIndicator.m_Var); HelpMarker("Draws chams to show your fakelag position");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("Fakelag colour", Vars::Misc::CL_Move::FLGChams::FakelagColor);
						ImGui::PushItemWidth(100);
						ImGui::Combo("Fakelag material", &Vars::Misc::CL_Move::FLGChams::Material.m_Var, backtrackMaterial, IM_ARRAYSIZE(backtrackMaterial));
						ImGui::PopStyleVar();
					}

					// Menu: Visuals/World (Column 2)
					else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::World) {
						SectionTitle("World Chams");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("World chams###woldchamsbut", &Vars::Chams::World::Active.m_Var);
						ImGui::Checkbox("Health packs###WorldchamsHealthPacks", &Vars::Chams::World::Health.m_Var);
						ImGui::Checkbox("Ammo packs###Worldchamsammopacks", &Vars::Chams::World::Ammo.m_Var);
						static const char* wChamsProjectiles[]{ "Off", "All", "Enemy only" }; ImGui::PushItemWidth(100); ImGui::Combo("Projectiles###WorldChamsProjectiles", &Vars::Chams::World::Projectiles.m_Var, wChamsProjectiles, IM_ARRAYSIZE(wChamsProjectiles)); ImGui::PopItemWidth();
						static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel", "Plastic" }; ImGui::PushItemWidth(100); ImGui::Combo("Player material", &Vars::Chams::World::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth();
						ImGui::Checkbox("Ignore Z###worldignorez", &Vars::Chams::World::IgnoreZ.m_Var);
						ImGui::PopStyleVar();
					}

					// Menu: Visuals/Fonts (Column 2)
					else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::Fonts) {
						SectionTitle("Condition Font");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
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

						SectionTitle("Pickup Font");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
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

					// Menu: Visuals/Misc (Column 2)
					else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::MiscVisuals) {
						SectionTitle("Skybox & Textures");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 97 && (widget_pos.y - winPos.y) + winSize.y - 12) {
							ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							HelpMarker(tfm::format("x: %d, y: %d", widget_pos.x, widget_pos.y).c_str());
						}
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
						ImGui::Checkbox("Skybox changer", &Vars::Visuals::SkyboxChanger.m_Var); HelpMarker("Will change the skybox, either to a base TF2 one or a custom one");
						ImGui::PushItemWidth(150); ImGui::Combo("Skybox", &Vars::Skybox::SkyboxNum, skyNames, IM_ARRAYSIZE(skyNames), 6);  ImGui::PopItemWidth();
						if (Vars::Skybox::SkyboxNum == 0) {
							ImGui::PushItemWidth(150); ImGui::InputText("Custom skybox name", &Vars::Skybox::SkyboxName); ImGui::PopItemWidth(); HelpMarker("Name of the skybox you want to you (tf/materials/skybox)");
						}
						ImGui::Checkbox("World Textures Override", &Vars::Visuals::OverrideWorldTextures.m_Var); HelpMarker("Turn this off when in-game so you don't drop fps :p");
						ImGui::Checkbox("Bypass sv_pure", &Vars::Misc::BypassPure.m_Var); HelpMarker("Allows you to load any custom files, even if disallowed by the sv_pure setting");
						ImGui::Checkbox("Medal flip", &Vars::Misc::MedalFlip.m_Var); HelpMarker("Medal go spinny spinny weeeeeee");
						const char* weather[]{ "Off", "Rain", "Snow" };
						ImGui::PushItemWidth(150);
						ImGui::Combo("Precipitation", &Vars::Visuals::Rain.m_Var, weather, IM_ARRAYSIZE(weather), 3);
						ImGui::PopItemWidth();
						ImGui::Dummy(ImVec2(0, 20));


						SectionTitle("Custom fog");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24) ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						if (ImGui::Checkbox("Custom fog", &Vars::Visuals::Fog::CustomFog.m_Var)) {
							if (static auto fog_enable = g_Interfaces.CVars->FindVar("fog_enable"); fog_enable) {
								fog_enable->SetValue(Vars::Visuals::Fog::CustomFog.m_Var);
							}
							if (static auto fog_enableskybox = g_Interfaces.CVars->FindVar("fog_enableskybox"); fog_enableskybox) {
								fog_enableskybox->SetValue(Vars::Visuals::Fog::CustomFog.m_Var);
							}
							if (static auto fog_override = g_Interfaces.CVars->FindVar("fog_override"); fog_override) {
								fog_override->SetValue(Vars::Visuals::Fog::CustomFog.m_Var);
							}
						}
						ImGui::PushItemWidth(150);
						if (ImGui::SliderFloat("Fog density", &Vars::Visuals::Fog::FogDensity.m_Var, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
							if (static auto fog_density = g_Interfaces.CVars->FindVar("fog_maxdensity"); fog_density) {
								fog_density->SetValue(Vars::Visuals::Fog::FogDensity.m_Var);
							}

						}
						ImGui::PopItemWidth();
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						if (ColorPicker("Fog colour", Vars::Visuals::Fog::FogColor)) {
							if (static auto fog_color = g_Interfaces.CVars->FindVar("fog_color"); fog_color) {
								fog_color->SetValue(std::string("").
									append(std::to_string(Vars::Visuals::Fog::FogColor.r)).
									append(" ").
									append(std::to_string(Vars::Visuals::Fog::FogColor.g)).
									append(" ").
									append(std::to_string(Vars::Visuals::Fog::FogColor.b)).
									append(" ").c_str());
							}
						}

						ImGui::PushItemWidth(150);
						if (ImGui::SliderFloat("Fog start", &Vars::Visuals::Fog::FogStart.m_Var, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None)) {
							if (static auto fog_start = g_Interfaces.CVars->FindVar("fog_start"); fog_start) {
								fog_start->SetValue(Vars::Visuals::Fog::FogStart.m_Var);
							}
						}
						if (ImGui::SliderFloat("Fog end", &Vars::Visuals::Fog::FogEnd.m_Var, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None)) {
							if (static auto fog_end = g_Interfaces.CVars->FindVar("fog_end"); fog_end) {
								fog_end->SetValue(Vars::Visuals::Fog::FogEnd.m_Var);
							}
						}
						if (ImGui::SliderFloat("Skybox fog density", &Vars::Visuals::Fog::FogDensitySkybox.m_Var, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
							if (static auto fog_density = g_Interfaces.CVars->FindVar("fog_maxdensityskybox"); fog_density) {
								fog_density->SetValue(Vars::Visuals::Fog::FogDensitySkybox.m_Var);
							}
						}
						ImGui::PushItemWidth(150);
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						if (ColorPicker("Skybox fog colour", Vars::Visuals::Fog::FogColorSkybox)) {
							if (static auto fog_colorskybox = g_Interfaces.CVars->FindVar("fog_colorskybox"); fog_colorskybox) {
								fog_colorskybox->SetValue(std::string("").
									append(std::to_string(Vars::Visuals::Fog::FogColorSkybox.r)).
									append(" ").
									append(std::to_string(Vars::Visuals::Fog::FogColorSkybox.g)).
									append(" ").
									append(std::to_string(Vars::Visuals::Fog::FogColorSkybox.b)).
									append(" ").c_str());
							}
						}
						ImGui::PushItemWidth(150);
						if (ImGui::SliderFloat("Skybox fog start", &Vars::Visuals::Fog::FogStart.m_Var, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None)) {
							if (static auto fog_start = g_Interfaces.CVars->FindVar("fog_startskybox"); fog_start) {
								fog_start->SetValue(Vars::Visuals::Fog::FogStartSkybox.m_Var);
							}
						}
						if (ImGui::SliderFloat("Skybox fog end", &Vars::Visuals::Fog::FogEndSkybox.m_Var, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None)) {
							if (static auto fog_end = g_Interfaces.CVars->FindVar("fog_endskybox"); fog_end) {
								fog_end->SetValue(Vars::Visuals::Fog::FogEndSkybox.m_Var);
							}
						}
						ImGui::PopItemWidth();
						ImGui::Dummy(ImVec2(0, 20));


						SectionTitle("Thirdperson");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24) ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Thirdperson", &Vars::Visuals::ThirdPerson.m_Var); HelpMarker("Will move your camera to be in a thirdperson view");
						InputKeybind("Thirdperson key", Vars::Visuals::ThirdPersonKey); HelpMarker("What key to toggle thirdperson, press ESC if no bind is desired");
						ImGui::Checkbox("Show real angles###tpRealAngles", &Vars::Visuals::ThirdPersonSilentAngles.m_Var); HelpMarker("Will show your real angles on thirdperson (not what others see)");
						ImGui::Checkbox("Instant yaw###tpInstantYaw", &Vars::Visuals::ThirdPersonInstantYaw.m_Var); HelpMarker("Will set your yaw instantly in thirdperson, showing your actual angle, instead of what others see");
						ImGui::Checkbox("Show server hitboxes (localhost only)###tpShowServer", &Vars::Visuals::ThirdPersonServerHitbox.m_Var); HelpMarker("Will show the server angles in thirdperson");

						ImGui::Checkbox("Thirdperson offsets", &Vars::Visuals::ThirdpersonOffset.m_Var); HelpMarker("These will mess you up if you use a small FoV");
						WidthSlider("Thirdperson distance", &Vars::Visuals::ThirdpersonDist.m_Var, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
						WidthSlider("Thirdperson right", &Vars::Visuals::ThirdpersonRight.m_Var, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
						WidthSlider("Thirdperson up", &Vars::Visuals::ThirdpersonUp.m_Var, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
						ImGui::Checkbox("Thirdperson crosshair", &Vars::Visuals::ThirdpersonCrosshair.m_Var);
						ImGui::Checkbox("Offset with arrow keys", &Vars::Visuals::ThirdpersonOffsetWithArrows.m_Var);
						InputKeybind("Move offset key", Vars::Visuals::ThirdpersonArrowOffsetKey, false);
						ImGui::Dummy(ImVec2(0, 20));


						SectionTitle("Out of FOV arrows");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Active###fovar", &Vars::Visuals::OutOfFOVArrows.m_Var); HelpMarker("Will draw arrows to players who are outside of the range of your FoV");
						ImGui::Checkbox("Outline arrows###OutlinedArrows", &Vars::Visuals::OutOfFOVArrowsOutline.m_Var); HelpMarker("16 missed calls");
						ImGui::PushItemWidth(150); ImGui::SliderFloat("Arrow length", &Vars::Visuals::ArrowLength.m_Var, 5.f, 50.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How long the arrows are");
						ImGui::PushItemWidth(150); ImGui::SliderFloat("Arrow angle", &Vars::Visuals::ArrowAngle.m_Var, 5.f, 180.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("The angle of the arrow");
						ImGui::PushItemWidth(100); ImGui::SliderFloat("Distance from center", &Vars::Visuals::FovArrowsDist.m_Var, 0.01f, 0.2f, "%.3f"); ImGui::PopItemWidth(); HelpMarker("How far from the center of the screen the arrows will draw");
						ImGui::PushItemWidth(150); ImGui::SliderFloat("Max distance", &Vars::Visuals::MaxDist.m_Var, 0.f, 4000.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How far until the arrows will not show");
						ImGui::PushItemWidth(150); ImGui::SliderFloat("Min distance", &Vars::Visuals::MinDist.m_Var, 0.f, 1000.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How close until the arrows will be fully opaque");
						ImGui::Dummy(ImVec2(0, 20));

						SectionTitle("Spy Warning");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24) ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						
						ImGui::Checkbox("Active###spywarn", &Vars::Visuals::SpyWarning.m_Var); HelpMarker("Will alert you when spies with their knife out may attempt to backstab you");
						ImGui::Checkbox("Voice command###spywarn1", &Vars::Visuals::SpyWarningAnnounce.m_Var); HelpMarker("Will make your character say \"Spy!\" when a spy is detected");
						ImGui::Checkbox("Visible only###spywarn2", &Vars::Visuals::SpyWarningVisibleOnly.m_Var); HelpMarker("Will only alert you to visible spies");
						ImGui::Checkbox("Ignore friends###spywarn3", &Vars::Visuals::SpyWarningIgnoreFriends.m_Var); HelpMarker("Will ignore spies who are on your friends list");
						const char* spyWmodes[]{ "Arrow", "Flash" }; ImGui::PushItemWidth(150); ImGui::Combo("Warning style", &Vars::Visuals::SpyWarningStyle.m_Var, spyWmodes, IM_ARRAYSIZE(spyWmodes)); ImGui::PopItemWidth(); HelpMarker("Choose the style of the spy indicator");


						ImGui::PopStyleVar();
					}

					// Menu: Visuals/Radar (Column 2)
					else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::Radar) {
						SectionTitle("Building");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

						ImGui::Checkbox("Show buildings###radarbuildingsa", &Vars::Radar::Buildings::Active.m_Var);
						ImGui::Checkbox("Outline###radarbuildingsao", &Vars::Radar::Buildings::Outline.m_Var);
						ImGui::Checkbox("Ignore team###radarbuildingsb", &Vars::Radar::Buildings::IgnoreTeam.m_Var);
						ImGui::Checkbox("Health bar###radarbuildingsc", &Vars::Radar::Buildings::Health.m_Var);
						ImGui::PushItemWidth(100); ImGui::SliderInt("Icon size###buildingsizeiconradar", &Vars::Radar::Buildings::IconSize.m_Var, 12, 30, "%d"); ImGui::PopItemWidth();
						ImGui::PopStyleVar();
					}

					// Menu: Misc (Column 2)
					else if (mainTab == MainTabs::Misc) {
						SectionTitle("Tickbase Exploits");
						ImVec2 widget_pos = ImGui::GetCursorScreenPos();
						if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
						ImGui::Checkbox("Doubletap", &Vars::Misc::CL_Move::Enabled.m_Var); HelpMarker("Shifts ticks when shooting for a rapid-fire effect");
						ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
						ImGui::SetNextItemWidth(20);
						ColorPicker("DT bar outline colour", Colors::DtOutline);
						InputKeybind("Recharge key", Vars::Misc::CL_Move::RechargeKey); HelpMarker("Recharges ticks for shifting");
						InputKeybind("Teleport key", Vars::Misc::CL_Move::TeleportKey); HelpMarker("Shifts ticks to warp");
						if (Vars::Misc::CL_Move::DTMode.m_Var == 0 || Vars::Misc::CL_Move::DTMode.m_Var == 2) {
							InputKeybind("Doubletap key", Vars::Misc::CL_Move::DoubletapKey); HelpMarker("Only doubletap when the key is pressed. Leave as (None) for always active.");
						}
							
						ImGui::PushItemWidth(100);
						MultiCombo({ "Recharge While Dead", "Auto Recharge", "Wait for DT", "Anti-warp", "Avoid airborne" }, { &Vars::Misc::CL_Move::RechargeWhileDead.m_Var, &Vars::Misc::CL_Move::AutoRecharge.m_Var, &Vars::Misc::CL_Move::WaitForDT.m_Var, &Vars::Misc::CL_Move::AntiWarp.m_Var, &Vars::Misc::CL_Move::NotInAir.m_Var }, "Enable various features regarding tickbase exploits", "Options");
						const char* dtModes[]{ "On key", "Always", "Disable on key", "Disabled" }; ImGui::PushItemWidth(100); ImGui::Combo("DT Mode", &Vars::Misc::CL_Move::DTMode.m_Var, dtModes, IM_ARRAYSIZE(dtModes)); ImGui::PopItemWidth(); HelpMarker("How should DT behave");
						ImGui::PushItemWidth(100); ImGui::SliderInt("Ticks to shift", &Vars::Misc::CL_Move::DTTicks.m_Var, 1, 24, "%d"); HelpMarker("How many ticks to shift");
						ImGui::Checkbox("SpeedHack", &Vars::Misc::CL_Move::SEnabled.m_Var); HelpMarker("Speedhack Master Switch");
						if (Vars::Misc::CL_Move::SEnabled.m_Var)
							ImGui::SliderInt("SpeedHack Factor", &Vars::Misc::CL_Move::SFactor.m_Var, 1, 66, "%d"); HelpMarker("High values are not recommended");

						ImGui::Dummy(ImVec2(0, 20));

						SectionTitle("HvH");
						widget_pos = ImGui::GetCursorScreenPos();
						widget_pos.y -= 4;
						if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
						ImGui::Checkbox("Anti-aim", &Vars::AntiHack::AntiAim::Active.m_Var);
						const char* pitch[]{ "None", "Up", "Down", "Fake up", "Fake down", "Random" }; ImGui::PushItemWidth(100); ImGui::Combo("Pitch", &Vars::AntiHack::AntiAim::Pitch.m_Var, pitch, IM_ARRAYSIZE(pitch)); ImGui::PopItemWidth(); HelpMarker("Which way to look up/down");
						const char* realYaw[]{ "None", "Left", "Right", "Backwards", "Random", "Spin", "Edge" }; ImGui::PushItemWidth(100); ImGui::Combo("Real yaw", &Vars::AntiHack::AntiAim::YawReal.m_Var, realYaw, IM_ARRAYSIZE(realYaw)); ImGui::PopItemWidth(); HelpMarker("Which way to look horizontally");
						const char* fakeYaw[]{ "None", "Left", "Right", "Backwards", "Random", "Spin", "Edge" }; ImGui::PushItemWidth(100); ImGui::Combo("Fake yaw", &Vars::AntiHack::AntiAim::YawFake.m_Var, fakeYaw, IM_ARRAYSIZE(fakeYaw)); ImGui::PopItemWidth(); HelpMarker("Which way to appear to look horizontally");
						if (Vars::AntiHack::AntiAim::YawFake.m_Var == 5 || Vars::AntiHack::AntiAim::YawReal.m_Var == 5) {
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Spin Speed", &Vars::AntiHack::AntiAim::SpinSpeed.m_Var, -30.f, 30.f, "%.1f", 0); ImGui::PopItemWidth(); HelpMarker("You spin me right 'round, baby, right 'round");
						}
						ImGui::Checkbox("Resolver", &Vars::AntiHack::Resolver::Resolver.m_Var); HelpMarker("Enables Anti-aim resolver in the playerlist");
						const char* flgModes[]{ "None", "Plain", "Random", "Velocity Based" }; ImGui::PushItemWidth(100); ImGui::Combo("Fakelag Mode###FLmode", &Vars::Misc::CL_Move::FakelagMode.m_Var, flgModes, IM_ARRAYSIZE(flgModes)); ImGui::PopItemWidth(); HelpMarker("Controls how fakelag will be controlled.");
						if (Vars::Misc::CL_Move::FakelagMode.m_Var > 0) {
							Vars::Misc::CL_Move::Fakelag.m_Var = true;
						}
						else {
							Vars::Misc::CL_Move::Fakelag.m_Var = false;
						}

						if (Vars::Misc::CL_Move::Fakelag.m_Var) {
							ImGui::Dummy(ImVec2(0, 20));

							SectionTitle("Fakelag");
							widget_pos = ImGui::GetCursorScreenPos();
							widget_pos.y -= 4;
							if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							if (Vars::Misc::CL_Move::FakelagMode.m_Var == 1 || Vars::Misc::CL_Move::FakelagMode.m_Var == 3) {
								ImGui::PushItemWidth(100); ImGui::SliderInt("Fakelag value", &Vars::Misc::CL_Move::FakelagValue.m_Var, 1, 22, "%d"); ImGui::PopItemWidth(); HelpMarker("How much lag you should fake(?)");
								if (Vars::Misc::CL_Move::FakelagMode.m_Var == 1) {
									ImGui::Checkbox("Fakelag on key", &Vars::Misc::CL_Move::FakelagOnKey.m_Var); HelpMarker("Fakelag will only activate when an assigned key is held");
									if (Vars::Misc::CL_Move::FakelagOnKey.m_Var)
										InputKeybind("Fakelag key", Vars::Misc::CL_Move::FakelagKey); HelpMarker("The key to activate fakelag as long as it's held");
								}
							}
							if (Vars::Misc::CL_Move::FakelagMode.m_Var == 2) {
								ImGui::PushItemWidth(100); ImGui::SliderInt("Random max###flRandMax", &Vars::Misc::CL_Move::FakelagMax.m_Var, Vars::Misc::CL_Move::FakelagMin.m_Var + 1, 22, "%d"); ImGui::PopItemWidth(); HelpMarker("Maximum random fakelag value");
								ImGui::PushItemWidth(100); ImGui::SliderInt("Random min###flRandMin", &Vars::Misc::CL_Move::FakelagMin.m_Var, 1, Vars::Misc::CL_Move::FakelagMax.m_Var - 1, "%d"); ImGui::PopItemWidth(); HelpMarker("Minimum random fakelag value");
							}
						}
						//ImGui::Checkbox("Fakelag Indicator", &Vars::Misc::CL_Move::FakelagIndicator.m_Var); HelpMarker("Shows your fakelag position in thirdperson");
						// this already exists in visuals misc near the thirdperson
						ImGui::PopStyleVar();
					}
					ImGui::EndChild();
				}

				// Column 3
				if (!(mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::MiscVisuals)) {
					ImGui::SameLine(); //
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
					ImGui::BeginChild("Feature 3", mainTab == MainTabs::Visuals ? ImVec2((winSize.x / 3 - 16), winSize.y - 113) : ImVec2((winSize.x / 3) - 13, winSize.y - 86), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_HorizontalScrollbar);
					{
						ImGui::PopStyleVar();
						ImGui::PopStyleVar();
						ImGui::PopStyleVar();

						// Menu: Aimbot (Column 3)
						if (mainTab == MainTabs::Aimbot) {
							SectionTitle("Projectile");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("Performance mode", &Vars::Aimbot::Projectile::PerformanceMode.m_Var); HelpMarker("Only target enemy closest to the crosshair");
							ImGui::Checkbox("Movement simulation", &Vars::Aimbot::Projectile::MovementSimulation.m_Var); HelpMarker("Uses game functions to predict where the player will be");
							if (Vars::Aimbot::Projectile::MovementSimulation.m_Var) {
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Prediction Time", &Vars::Aimbot::Projectile::predTime.m_Var, 0.1f, 10.f, "%.1f");
							}
							{
								static const char* sortMethodArr[]{ "FOV", "Distance", }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###ProjectileSortMethod", &Vars::Aimbot::Projectile::SortMethod.m_Var, sortMethodArr, IM_ARRAYSIZE(sortMethodArr)); ImGui::PopItemWidth();
								static const char* aimMethodArr[]{ "Plain", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###ProjectileAimMethod", &Vars::Aimbot::Projectile::AimMethod.m_Var, aimMethodArr, IM_ARRAYSIZE(aimMethodArr)); ImGui::PopItemWidth();
								static const char* aimHitboxArr[]{ "Body", "Feet", "Auto" }; ImGui::PushItemWidth(100); ImGui::Combo("Hitbox###ProjectileHitbox", &Vars::Aimbot::Projectile::AimPosition.m_Var, aimHitboxArr, IM_ARRAYSIZE(aimHitboxArr)); ImGui::PopItemWidth();
							}
							ImGui::Checkbox("Feet aim on ground (Demoman)", &Vars::Aimbot::Projectile::FeetAimIfOnGround.m_Var); HelpMarker("Will aim at feet if target is on the ground");
							ImGui::Checkbox("Custom huntsman Z-Adjust", &Vars::Aimbot::Projectile::ManualZAdjust.m_Var); HelpMarker("Enables the ability to adjust the Z-Position for huntsman");
							if (Vars::Aimbot::Projectile::ManualZAdjust.m_Var) {
								WidthSlider("Z-Value###ZAdjustValue", &Vars::Aimbot::Projectile::ZAdjustAmount.m_Var, 0.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Manual Z-Adjust for projectiles");
							}
							ImGui::Dummy(ImVec2(0, 20));

							SectionTitle("Melee");
							widget_pos = ImGui::GetCursorScreenPos();

							widget_pos.y -= 6;
							if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							{
								static const char* sortMethodArr[]{ "FOV", "Distance", }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###MeleeSortMethod", &Vars::Aimbot::Melee::SortMethod.m_Var, sortMethodArr, IM_ARRAYSIZE(sortMethodArr)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to decide which target to aim at");
								static const char* aimMethodArr[]{ "Plain", "Smooth", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###MeleeAimMethod", &Vars::Aimbot::Melee::AimMethod.m_Var, aimMethodArr, IM_ARRAYSIZE(aimMethodArr)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to aim at the target");
							}
							WidthSlider("Smooth factor###MeleeSmoothing", &Vars::Aimbot::Melee::SmoothingAmount.m_Var, 0.f, 20.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How smooth the aimbot should be");
							ImGui::Checkbox("Range check", &Vars::Aimbot::Melee::RangeCheck.m_Var); HelpMarker("Only aim at target if within melee range");
							ImGui::Checkbox("Swing prediction", &Vars::Aimbot::Melee::PredictSwing.m_Var); HelpMarker("Aimbot will attack preemptively, predicting you will be in range of the target");
							ImGui::Checkbox("Whip teammates", &Vars::Aimbot::Melee::WhipTeam.m_Var); HelpMarker("Aimbot will target teammates if holding the Disciplinary Action");
							ImGui::PopStyleVar();
						}

						// Menu: Triggerbot (Column 3)
						else if (mainTab == MainTabs::Triggerbot) {
							SectionTitle("Autoblast");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("Autoblast###Triggreairblast", &Vars::Triggerbot::Blast::Active.m_Var); HelpMarker("Auto airblast master switch");
							ImGui::Checkbox("Rage airblast###TriggerAirRage", &Vars::Triggerbot::Blast::Rage.m_Var); HelpMarker("Will airblast whenever possible, regardless of FoV");
							ImGui::Checkbox("Silent###triggerblastsilent", &Vars::Triggerbot::Blast::Silent.m_Var); HelpMarker("Aim changes made by the rage mode setting aren't visible");

							ImGui::Dummy(ImVec2(0, 20));

							SectionTitle("Autouber");
							widget_pos = ImGui::GetCursorScreenPos();
							widget_pos.y -= 4;
							if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::Checkbox("Autouber###Triggeruber", &Vars::Triggerbot::Uber::Active.m_Var); HelpMarker("Auto uber master switch");
							ImGui::Checkbox("Only uber friends", &Vars::Triggerbot::Uber::OnlyFriends.m_Var); HelpMarker("Auto uber will only activate if healing steam friends");
							ImGui::Checkbox("Preserve self", &Vars::Triggerbot::Uber::PopLocal.m_Var); HelpMarker("Auto uber will activate if local player's health falls below the percentage");
							ImGui::Checkbox("Vaccinator resistances", &Vars::Triggerbot::Uber::AutoVacc.m_Var); HelpMarker("Auto uber will automatically find the best resistance and pop when needed (This doesn't work properly)");
							WidthSlider("Health left (%)###TriggerUberHealthLeft", &Vars::Triggerbot::Uber::HealthLeft.m_Var, 1.f, 99.f, "%.0f%%", 1.0f); HelpMarker("The amount of health the heal target must be below to actiavte");

							ImGui::PopStyleVar();
						}

						// Menu: Visuals/Players (Column 3)
						else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::Players) {
							SectionTitle("Glow Main");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("Glow", &Vars::Glow::Main::Active.m_Var);
							ImGui::Checkbox("Stencil glow", &Vars::Glow::Main::Stencil.m_Var);
							if (!Vars::Glow::Main::Stencil.m_Var) { ImGui::PushItemWidth(100); ImGui::SliderInt("Glow scale", &Vars::Glow::Main::Scale.m_Var, 1, 10, "%d", ImGuiSliderFlags_AlwaysClamp); ImGui::PopItemWidth(); }

							ImGui::Dummy(ImVec2(0, 20));
							SectionTitle("Player Glow");
							widget_pos = ImGui::GetCursorScreenPos();
							widget_pos.y -= 4;
							if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::Checkbox("Player glow###PlayerGlowButton", &Vars::Glow::Players::Active.m_Var); HelpMarker("Player glow master switch");
							ImGui::Checkbox("Self glow###SelfGlow", &Vars::Glow::Players::ShowLocal.m_Var); HelpMarker("Draw glow on the local player");
							ImGui::Checkbox("Self rainbow glow###SelfGlowRainbow", &Vars::Glow::Players::LocalRainbow.m_Var); HelpMarker("Homosapien");
							static const char* ignoreTeamArr[]{ "Off", "All", "Only friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore team###IgnoreTeamGlowp", &Vars::Glow::Players::IgnoreTeammates.m_Var, ignoreTeamArr, IM_ARRAYSIZE(ignoreTeamArr)); ImGui::PopItemWidth(); HelpMarker("Which teammates the glow will ignore drawing on");
							ImGui::Checkbox("Wearable glow###PlayerWearableGlow", &Vars::Glow::Players::Wearables.m_Var); HelpMarker("Will draw glow on player cosmetics");
							ImGui::Checkbox("Weapon glow###PlayerWeaponGlow", &Vars::Glow::Players::Weapons.m_Var); HelpMarker("Will draw glow on player weapons");
							WidthSlider("Glow alpha###PlayerGlowAlpha", &Vars::Glow::Players::Alpha.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
							static const char* colourArr[]{ "Team", "Health" }; ImGui::PushItemWidth(100); ImGui::Combo("Glow colour###GlowColour", &Vars::Glow::Players::Color.m_Var, colourArr, IM_ARRAYSIZE(colourArr)); ImGui::PopItemWidth(); HelpMarker("Which colour the glow will draw");

							ImGui::PopStyleVar();
						}

						// Menu: Visuals/Buildings (Column 3)
						else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::Buildings) {
							SectionTitle("Building Glow");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("Building glow###BuildiongGlowButton", &Vars::Glow::Buildings::Active.m_Var);
							ImGui::Checkbox("Ignore team buildings###buildingglowignoreteams", &Vars::Glow::Buildings::IgnoreTeammates.m_Var);
							WidthSlider("Glow alpha###BuildingGlowAlpha", &Vars::Glow::Buildings::Alpha.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
							static const char* colourArr[]{ "Team", "Health" }; ImGui::PushItemWidth(100); ImGui::Combo("Glow colour###GlowColourBuildings", &Vars::Glow::Buildings::Color.m_Var, colourArr, IM_ARRAYSIZE(colourArr)); ImGui::PopItemWidth();

							ImGui::PopStyleVar();
						}

						// Menu: Visuals/World (Column 3)
						else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::World) {
							SectionTitle("World Glow");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("World glow###Worldglowbutton", &Vars::Glow::World::Active.m_Var);
							ImGui::Checkbox("Health packs###worldhealthpackglow", &Vars::Glow::World::Health.m_Var);
							ImGui::Checkbox("Health packs###worldammopackglow", &Vars::Glow::World::Ammo.m_Var);
							static const char* ignoreTeamArr[]{ "Off", "All", "Only enemies" }; ImGui::PushItemWidth(100); ImGui::Combo("Projectile glow###teamprojectileglow", &Vars::Glow::World::Projectiles.m_Var, ignoreTeamArr, IM_ARRAYSIZE(ignoreTeamArr)); ImGui::PopItemWidth();
							WidthSlider("Glow alpha###WorldGlowAlpha", &Vars::Glow::World::Alpha.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

							ImGui::PopStyleVar();
						}

						// Menu: Visuals/Fonts (Column 3)
						else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::Fonts) {
							SectionTitle("Menu Font");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
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

							SectionTitle("Indicator Font");
							widget_pos = ImGui::GetCursorScreenPos();
							widget_pos.y -= 4;
							if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
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

						// Menu: Visualt/Radar (Column 3)
						else if (mainTab == MainTabs::Visuals && visualsTab == VisualsTabs::Radar) {
							SectionTitle("World");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							if (widget_pos.y - winPos.y > 97 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("Active###radarworldd", &Vars::Radar::World::Active.m_Var);
							ImGui::Checkbox("Health###radarworldda", &Vars::Radar::World::Health.m_Var);
							ImGui::Checkbox("Ammo###radarworlddb", &Vars::Radar::World::Ammo.m_Var);
							ImGui::PushItemWidth(100); ImGui::SliderInt("Icon size###worldsizeiconradar", &Vars::Radar::World::IconSize.m_Var, 12, 30, "%d"); ImGui::PopItemWidth();

							ImGui::PopStyleVar();
						}

						// Menu: Visuals/Misc (Column 3)
						else if (mainTab == MainTabs::Misc) {
							SectionTitle("Discord RPC");
							ImVec2 widget_pos = ImGui::GetCursorScreenPos();
							if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
							ImGui::Checkbox("Discord RPC", &Vars::Misc::Discord::EnableRPC.m_Var); HelpMarker("Enable Discord Rich Presence");
							ImGui::Checkbox("Include map", &Vars::Misc::Discord::IncludeMap.m_Var); HelpMarker("Should Discord Rich Presence contain current map name?");
							ImGui::Checkbox("Include class", &Vars::Misc::Discord::IncludeClass.m_Var); HelpMarker("Should Discord Rich Presence contain current class?");
							ImGui::Checkbox("Include timestamp", &Vars::Misc::Discord::IncludeTimestamp.m_Var); HelpMarker("Should time since you started playing TF2 be included?");
							const char* imagething[]{ "Big fedora + Small TF2", "Big TF2 + Small fedora" }; ImGui::PushItemWidth(100); ImGui::Combo("Image Options", &Vars::Misc::Discord::WhatImagesShouldBeUsed.m_Var, imagething, IM_ARRAYSIZE(imagething)); ImGui::PopItemWidth(); HelpMarker("Which image should be big and which small");
							ImGui::Dummy(ImVec2(0, 20));

							SectionTitle("Steam RPC");
							widget_pos = ImGui::GetCursorScreenPos();
							widget_pos.y -= 4;
							if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);
							ImGui::Checkbox("Steam RPC", &Vars::Misc::Steam::EnableRPC.m_Var); HelpMarker("Enable Steam Rich Presence"); HelpMarker("Enable Steam Rich Presence");
							const char* matchGroups[]{ "Special Event", "MvM Mann Up", "Competitive", "Casual", "MvM Boot Camp" }; ImGui::PushItemWidth(100); ImGui::Combo("Match group", &Vars::Misc::Steam::MatchGroup.m_Var, matchGroups, IM_ARRAYSIZE(matchGroups)); ImGui::PopItemWidth(); HelpMarker("Which match group should be used?");
							ImGui::Checkbox("Override in menu", &Vars::Misc::Steam::OverrideMenu.m_Var); HelpMarker("Override match group to \"Main Menu\" when in main menu");
							const char* mapTexts[]{ "Custom", "Fedoraware", "Figoraware", "Meowhook.club", "Rathook.cc", "Nitro.tf" }; ImGui::PushItemWidth(100); ImGui::Combo("Map text", &Vars::Misc::Steam::MapText.m_Var, mapTexts, IM_ARRAYSIZE(mapTexts)); ImGui::PopItemWidth(); HelpMarker("Which map text should be used?");
							if (Vars::Misc::Steam::MapText.m_Var == 0) {
								ImGui::PushItemWidth(100); ImGui::InputText("Custom map text", &Vars::Misc::Steam::CustomText.m_Var); ImGui::PopItemWidth(); HelpMarker("For when \"Custom\" is selcted in \"Map text\". Sets custom map text.");
							}
							ImGui::PushItemWidth(100); ImGui::InputInt("Group size", &Vars::Misc::Steam::GroupSize.m_Var); HelpMarker("Sets party size"); ImGui::PopItemWidth();
							ImGui::Dummy(ImVec2(0, 20));

							SectionTitle("Utilities");
							widget_pos = ImGui::GetCursorScreenPos();
							widget_pos.y -= 4;
							if (widget_pos.y - winPos.y > 70 && widget_pos.y < winPos.y + winSize.y - 24)  ImGui::GradientRect(fgDrawList, &normal, widget_pos, ImGui::GetContentRegionMax().x - 12, 3);

							auto a = ImGui::GetContentRegionMax().x - 12;

							if (ImGui::Button("Toggle playerlist", ImVec2(a, 20)))
								g_PlayerList.showWindow = !g_PlayerList.showWindow;
							if (ImGui::Button("Full update", ImVec2(a, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("cl_fullupdate");
							if (ImGui::Button("Reload HUD", ImVec2(a, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("hud_reloadscheme");
							if (ImGui::Button("Restart sound", ImVec2(a, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("snd_restart");
							if (ImGui::Button("Stop sound", ImVec2(a, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("stopsound");
							if (ImGui::Button("Status", ImVec2(a, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("status");
							if (ImGui::Button("Ping", ImVec2(a, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("ping");
							if (ImGui::Button("Retry", ImVec2(a, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("retry");
							if (ImGui::Button("Exit", ImVec2(a, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("exit");
							if (ImGui::Button("Console", ImVec2(a, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("showconsole");
							if (ImGui::Button("Demo playback", ImVec2(a, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("demoui");
							if (ImGui::Button("Demo trackbar", ImVec2(a, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("demoui2");
							if (ImGui::Button("Itemtest", ImVec2(a, 20)))
								g_Interfaces.Engine->ClientCmd_Unrestricted("itemtest");
							if (ImGui::Button("Unlock all achievements", ImVec2(a, 20))) {
								using fn = IAchievementMgr * (*)(void);
								auto achievementmgr = GetVFunc<fn>(g_Interfaces.Engine, 114)();
								if (achievementmgr) {
									g_SteamInterfaces.UserStats->RequestCurrentStats();
									for (int i = 0; i < achievementmgr->GetAchievementCount(); i++) {
										achievementmgr->AwardAchievement(achievementmgr->GetAchievementByIndex(i)->GetAchievementID());
									}
									g_SteamInterfaces.UserStats->StoreStats();
									g_SteamInterfaces.UserStats->RequestCurrentStats();
								}
							}
							if (ImGui::Button("Lock all achievements", ImVec2(a, 20))) {
								using fn = IAchievementMgr * (*)(void);
								auto achievementmgr = GetVFunc<fn>(g_Interfaces.Engine, 114)();
								if (achievementmgr) {
									g_SteamInterfaces.UserStats->RequestCurrentStats();
									for (int i = 0; i < achievementmgr->GetAchievementCount(); i++) {
										g_SteamInterfaces.UserStats->ClearAchievement(achievementmgr->GetAchievementByIndex(i)->GetName());
									}
									g_SteamInterfaces.UserStats->StoreStats();
									g_SteamInterfaces.UserStats->RequestCurrentStats();
								}
							}

							ImGui::PopStyleVar();
						}
						ImGui::EndChild();
					}

				}
				ImGui::EndChild();
			}
			ImGui::SetCursorPos(ImVec2(12, winSize.y - 20));
			ImGui::TextUnformatted(toolTipText.c_str());
			ImGui::SameLine(ImGui::GetContentRegionMax().x - 25);
			ImGui::SetNextItemWidth(25);
			ImGui::PushFont(Icons);
			ImGui::TextUnformatted(ICON_FA_COG);
			ImGui::PopFont();
			static bool showSettings = false;
			if (ImGui::IsItemClicked()) {
				showSettings = !showSettings;
			}
			if (showSettings) {
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(1, 1));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.43f, 0.43f, 0.50f, 1.00f));
				
				if (ImGui::Begin("Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings)) {
					ImGui::PopStyleVar();
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

							// Save, Load and Remove buttons
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
						else {
							if (ImGui::Button(configName.c_str(), ImVec2(200, 20))) {
								selected = s;
							}
						}
					}

					ImGui::End();
				}
				else {
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
			}
		}
		ImGui::PopFont();

		// Draw playerlist
		if (g_PlayerList.showWindow)
		{
			g_PlayerList.Render();
		}

		// Draw camera window
		if (g_Interfaces.Engine->IsInGame() && Vars::Visuals::CameraMode.m_Var != 0) {
			ImGui::SetNextWindowSize({ static_cast<float>(g_CameraWindow.ViewRect.w), static_cast<float>(g_CameraWindow.ViewRect.h) }, ImGuiCond_Once);
			ImGui::SetNextWindowPos({ static_cast<float>(g_CameraWindow.ViewRect.x), static_cast<float>(g_CameraWindow.ViewRect.y) }, ImGuiCond_Once);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 60.f, 60.f });

			int windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;
			if (Vars::Visuals::CameraMode.m_Var <= 1 || g_CameraWindow.CanDraw) {
				windowFlags |= ImGuiWindowFlags_NoBackground;
			}
			if (ImGui::Begin("Camera", &g_PlayerList.showWindow, windowFlags))
			{
				ImVec2 winPos = ImGui::GetWindowPos();
				ImVec2 winSize = ImGui::GetWindowSize();

				g_CameraWindow.ViewRect.x = static_cast<int>(winPos.x);
				g_CameraWindow.ViewRect.y = static_cast<int>(winPos.y);
				g_CameraWindow.ViewRect.w = static_cast<int>(winSize.x);
				g_CameraWindow.ViewRect.h = static_cast<int>(winSize.y);

				ImGui::End();
			}
			ImGui::PopStyleVar();
		}

		ImGui::End();
		
		// Old menu: https://github.com/tf2cheater2013/Fedoraware/blob/259389c24d40dfacc3388d9404c11c6079255260/Fedoraware/TeamFortress2/TeamFortress2/Features/Menu/Menu.cpp
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
}
