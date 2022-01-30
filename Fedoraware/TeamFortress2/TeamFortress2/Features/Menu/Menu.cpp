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

	ImGui::TextUnformatted(label);
	ImGui::SameLine();
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
	else if (ImGui::Button(VK2STR(output.m_Var), ImVec2(100, 17))) {
		ImGui::SetActiveID(id, ImGui::GetCurrentWindow());
	}

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
	if (ImGui::ColorEdit4(label, &FUCKOFF.x)) {
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

void CMenu::TextCenter(std::string text) {
	ImGui::PushFont(DT);
	float font_size = ImGui::CalcTextSize(text.c_str()).x;
	//float font_size = ImGui::GetFontSize() * text.size() / 2;
	ImGui::SameLine(
		ImGui::GetWindowSize().x / 2 -
		font_size + (font_size / 2)
	);

	ImGui::Text(text.c_str());
	ImGui::PopFont();
	ImGui::PopFont();
}

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

		ImGui::SetColorEditOptions(ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_Uint8);

		ImGuiStyle* style = &ImGui::GetStyle();
		auto& io = ImGui::GetIO();
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		//colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		//colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.10f, 0.20f);

		style->WindowPadding = ImVec2(8.00f, 8.00f);
		style->FramePadding = ImVec2(5.00f, 2.00f);
		style->CellPadding = ImVec2(6.00f, 6.00f);
		style->ItemSpacing = ImVec2(6.00f, 6.00f);
		style->ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style->TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style->IndentSpacing = 25;
		style->ScrollbarSize = 15;
		style->GrabMinSize = 10;
		style->WindowBorderSize = 1;
		style->ChildBorderSize = 1;
		style->PopupBorderSize = 1;
		style->FrameBorderSize = 1;
		style->TabBorderSize = 1;
		style->WindowRounding = 7;
		style->ChildRounding = 4;
		style->FrameRounding = 3;
		style->PopupRounding = 4;
		style->ScrollbarRounding = 9;
		style->GrabRounding = 3;
		style->LogSliderDeadzone = 4;
		style->TabRounding = 4;

		style->WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style->DisplaySafeAreaPadding = ImVec2(4, 4);

		auto m_font_config = ImFontConfig();
		m_font_config.OversampleH = 1;
		m_font_config.OversampleV = 1;
		m_font_config.PixelSnapH = true;

		drawList = ImGui::GetBackgroundDrawList();

		Normal = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\tahomabd.ttf", 14.0f, &m_font_config, io.Fonts->GetGlyphRangesCyrillic());
		DT = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\tahoma.ttf", 14.0f, &m_font_config, io.Fonts->GetGlyphRangesCyrillic());

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
	/*ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	if (g_Menu.m_bOpen) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		if (ImGui::Begin(" ##poop"), NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize) {
			ImGui::SetWindowSize(ImVec2(g_ScreenSize.w, 50));
			ImGui::PopStyleVar();
			ImGui::TextUnformatted("Hello");
		}
		ImGui::End();
	}*/

	if ((!g_Interfaces.EngineVGui->IsGameUIVisible() || g_Menu.m_bOpen) && Vars::Misc::CL_Move::DTBarStyle.m_Var == 2) {
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.03, 0.03, 0.03, 0.3));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		if (ImGui::Begin("Doubletap bar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | (g_Menu.m_bOpen ? 0 : ImGuiWindowFlags_NoDecoration) | (g_Menu.m_bOpen ? 0 : ImGuiWindowFlags_NoTitleBar)))
		{
			ImGui::SetWindowSize(ImVec2(180, 30));
			static std::string dtstring = "";
			if (const auto& pLocal = g_EntityCache.m_pLocal) {
				dtstring = "Doubletap (" + std::to_string(g_GlobalInfo.m_nShifted) + " / 24)";
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.3f, 0.3f, 1.0f));
				if (g_GlobalInfo.m_nShifted >= Vars::Misc::CL_Move::DTTicks.m_Var) {
					if (!g_GlobalInfo.m_nWaitForShift) {
						ImGui::PopStyleColor();
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.f, 0.3f, 1.0f));
					}
					else {
						ImGui::PopStyleColor();
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.5f, 0.f, 1.0f));
					}
				}
				TextCenter(dtstring);
				ImGui::PopStyleColor();
			}

		}
		ImGui::End();
		ImGui::PopStyleColor(2);
	}
	if (g_Menu.m_bOpen)
	{
		//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.00f));
		if (ImGui::Begin("Fedoraware", nullptr, ImGuiWindowFlags_NoCollapse))
		{
			//ImGui::PopStyleColor();
			ImGui::SetWindowSize(ImVec2(1020, 600), ImGuiCond_Once);

			ImGui::Columns(2);
			ImGui::SetColumnOffset(1, 140);
			{
				static ImVec4 active = to_vec4(30, 30, 30, 255);
				static ImVec4 inactive = to_vec4(30, 30, 30, 255);
				if (ImGui::Button("Aimbot", ImVec2(140 - 15, 29))) {
					SettingsTab = 0;
				}
				if (ImGui::Button("Triggerbot", ImVec2(140 - 15, 29))) {
					SettingsTab = 1;
				}
				if (ImGui::Button("Visuals", ImVec2(140 - 15, 29))) {
					SettingsTab = 2;
				}
				if (ImGui::Button("Misc", ImVec2(140 - 15, 29))) {
					SettingsTab = 3;
				}
				if (ImGui::Button("Colours", ImVec2(140 - 15, 29))) {
					SettingsTab = 4;
				}
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

					ImGui::PushItemWidth(140 - 15);
					if (ImGui::InputText("###configname", &output, ImGuiInputTextFlags_EnterReturnsTrue)) {
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
						if (ImGui::Button(configName.c_str(), ImVec2(140 - 15, 18))) {
							selected = s;
						}
						ImGui::PopStyleColor();
					}
					else {
						if (ImGui::Button(configName.c_str(), ImVec2(140 - 15, 18))) {
							selected = s;
						}
					}
				}

				if (!selected.empty())
				{
					ImGui::PushItemWidth(45);
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

					if (ImGui::Button("Save")) {
						ImGui::OpenPopup("Save config?");
					}

					ImGui::SameLine();
					if (ImGui::Button("Load")) {
						g_CFG.Load(selected.c_str());
						selected.clear();
					}

					ImGui::SameLine();
					if (ImGui::Button("Remove")) {
						ImGui::OpenPopup("Remove config?");
					}

					// Save config dialog
					if (ImGui::BeginPopupModal("Save config?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
						ImGui::Text("Do you really want to override this config?\n\n");
						ImGui::Separator();

						if (ImGui::Button("Yes, override!", ImVec2(120, 0))) {
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

						if (ImGui::Button("Yes, remove!", ImVec2(120, 0))) {
							g_CFG.Remove(selected.c_str());
							selected.clear();
							ImGui::CloseCurrentPopup();
						}

						ImGui::SameLine();
						if (ImGui::Button("No", ImVec2(120, 0))) {
							ImGui::CloseCurrentPopup();
						}

						ImGui::EndPopup();
					}

					ImGui::PopStyleVar();
					ImGui::PopItemWidth();
				}
			}

			ImGui::NextColumn();
			{
				if (SettingsTab == 0) {
					ImGui::BeginChild("Global");
					{
						ImGui::Columns(3);
						{
							ImGui::TextUnformatted("Global");
							ImGui::Checkbox("Aimbot###Aim bot", &Vars::Aimbot::Global::Active.m_Var); HelpMarker("Aimbot master switch");
							ImGui::Checkbox("AutoShoot", &Vars::Aimbot::Global::AutoShoot.m_Var); HelpMarker("Automatically shoot when a target is found");
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Aimbot FoV", &Vars::Aimbot::Global::AimFOV.m_Var, 1.0f, 180.f, "%.0f", 1.0f); ImGui::PopItemWidth(); HelpMarker("How many degrees the aimbot's FoV will have");
							InputKeybind("Aim key", Vars::Aimbot::Global::AimKey); HelpMarker("The key to enable aimbot");
							ImGui::Checkbox("Aim at players", &Vars::Aimbot::Global::AimPlayers.m_Var); HelpMarker("The aimbot will aim at players");
							ImGui::Checkbox("Aim at buildings", &Vars::Aimbot::Global::AimBuildings.m_Var); HelpMarker("The aimbot will aim at buildings");
							ImGui::Checkbox("Ignore invulnerable", &Vars::Aimbot::Global::IgnoreInvlunerable.m_Var); HelpMarker("The aimbot will ignore players who can't be damaged");
							ImGui::Checkbox("Ignore cloaked", &Vars::Aimbot::Global::IgnoreCloaked.m_Var); HelpMarker("The aimbot will ignore spies who are cloaked");
							ImGui::Checkbox("Ignore friends", &Vars::Aimbot::Global::IgnoreFriends.m_Var); HelpMarker("The aimbot will ignore steam friends");
							ImGui::Checkbox("Ignore taunting", &Vars::Aimbot::Global::IgnoreTaunting.m_Var); HelpMarker("The aimbot will ignore taunting players");
							ImGui::Checkbox("BAim when lethal", &Vars::Aimbot::Global::BAimLethal.m_Var); HelpMarker("The aimbot will aim for body when damage is lethal to it");
							ImGui::Checkbox("Doubletap", &Vars::Misc::CL_Move::Doubletap.m_Var); HelpMarker("When enough ticks are choked, the aimbot will shoot them all at once in a burst, leading to a rapid-fire effect");
							ImGui::TextUnformatted("");
							ImGui::TextUnformatted("Crithack");
							ImGui::Checkbox("Active###critsactive", &Vars::Crits::Active.m_Var); HelpMarker("Crit hack - this is the worst fucking crit hack known to man and I'm sorry for adding it");
							InputKeybind("Crit key", Vars::Crits::CritKey); HelpMarker("Crit key, self explanatory");
						}
						ImGui::NextColumn();
						{
							ImGui::TextUnformatted("Hitscan");
							//ImGui::Checkbox("Active###Hit scan", &Vars::Aimbot::Hitscan::Active.m_Var); HelpMarker("Hitscan aimbot master switch");
							static const char* hitscanSortMethod[]{ "FoV", "Distance" }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###hitscanSortMethod", &Vars::Aimbot::Hitscan::SortMethod.m_Var, hitscanSortMethod, IM_ARRAYSIZE(hitscanSortMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to decide which target to aim at");
							static const char* hitscanAimMethod[]{ "Plain", "Smooth", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###hitscanAimMethod", &Vars::Aimbot::Hitscan::AimMethod.m_Var, hitscanAimMethod, IM_ARRAYSIZE(hitscanAimMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to aim at the target");
							static const char* hitscanAimHitbox[]{ "Head", "Body", "Auto" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim position###hitscanAimPosition", &Vars::Aimbot::Hitscan::AimHitbox.m_Var, hitscanAimHitbox, IM_ARRAYSIZE(hitscanAimHitbox)); ImGui::PopItemWidth(); HelpMarker("Which hitbox the aimbot will target");
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Smooth factor", &Vars::Aimbot::Hitscan::SmoothingAmount.m_Var, 1.0f, 10.f, "%.0f", 1.0f); ImGui::PopItemWidth(); HelpMarker("How many degrees the aimbot's FoV will have");
							static const char* hitscanTapFire[]{ "Off", "Distance", "Always" }; ImGui::PushItemWidth(100); ImGui::Combo("Tap-fire", &Vars::Aimbot::Hitscan::TapFire.m_Var, hitscanTapFire, IM_ARRAYSIZE(hitscanTapFire)); ImGui::PopItemWidth(); HelpMarker("When to tap-fire with inaccurate rapid-fire weapons");
							ImGui::Checkbox("Minigun rev up", &Vars::Aimbot::Hitscan::AutoRev.m_Var); HelpMarker("Will rev heavy's minigun regardless of if aimbot has a target");
							ImGui::Checkbox("Body multipoint", &Vars::Aimbot::Hitscan::ScanHitboxes.m_Var); HelpMarker("Will attempt to shoot the body hitbox by scanning the edges");
							ImGui::Checkbox("Head multipoint", &Vars::Aimbot::Hitscan::ScanHead.m_Var); HelpMarker("Will attempt to shoot the head hitbox by scanning the edges");
							ImGui::Checkbox("Building multipoint", &Vars::Aimbot::Hitscan::ScanBuildings.m_Var); HelpMarker("Will attempt to shoot buildings by scanning the edges");
							ImGui::Checkbox("Wait for headshot", &Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var); HelpMarker("The aimbot will wait until it can headshot (if applicable)");
							ImGui::Checkbox("Wait for charge", &Vars::Aimbot::Hitscan::WaitForCharge.m_Var); HelpMarker("The aimbot will wait until the rifle has charged long enough to kill in one shot");
							ImGui::Checkbox("Smooth if spectated", &Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var); HelpMarker("The aimbot will switch to the smooth method if being spectated");
							ImGui::Checkbox("Scoped only", &Vars::Aimbot::Hitscan::ScopedOnly.m_Var); HelpMarker("The aimbot will only shoot if scoped");
							ImGui::Checkbox("Scope automatically", &Vars::Aimbot::Hitscan::AutoScope.m_Var); HelpMarker("The aimbot will automatically scope in to shoot");
						}
						ImGui::NextColumn();
						{
							ImGui::TextUnformatted("Projectile");
							ImGui::Checkbox("Performance mode", &Vars::Aimbot::Projectile::PerformanceMode.m_Var); HelpMarker("Only taget enemy closest to the crosshair");
							static const char* projectileSortMethod[]{ "FoV", "Distance" }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###projectileSortMethod", &Vars::Aimbot::Projectile::SortMethod.m_Var, projectileSortMethod, IM_ARRAYSIZE(projectileSortMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to decide which target to aim at");
							static const char* projectileAimMethod[]{ "Plain", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###projectileAimMethod", &Vars::Aimbot::Projectile::AimMethod.m_Var, projectileAimMethod, IM_ARRAYSIZE(projectileAimMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to aim at the target");
							static const char* projectileAimHitbox[]{ "Body", "Feet", "Auto" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim position###projectileAimPosition", &Vars::Aimbot::Projectile::AimPosition.m_Var, projectileAimHitbox, IM_ARRAYSIZE(projectileAimHitbox)); ImGui::PopItemWidth(); HelpMarker("Which hitbox the aimbot will target");
							ImGui::Checkbox("Aim at feet if on ground (Demoman)", &Vars::Aimbot::Projectile::FeetAimIfOnGround.m_Var); HelpMarker("If true, aimbot will aim at enemies feet if target is on the ground (Demoman only and will only work if the aim postition is set to auto");
							ImGui::TextUnformatted("");
							ImGui::Checkbox("Manual Z Adjust", &Vars::Aimbot::Projectile::ManualZAdjust.m_Var); HelpMarker("Manual Z Adjust for projectiles");
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Manual Z Adjust amount", &Vars::Aimbot::Projectile::ZAdjustAmount.m_Var, 0.0f, 10.0f, "%.1f"); ImGui::PopItemWidth(); HelpMarker("Note: Higher values may cause inaccuracy");
							ImGui::TextUnformatted("");
							ImGui::TextUnformatted("Melee");
							static const char* meleeSortMethod[]{ "FoV", "Distance" }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###meleeSortMethod", &Vars::Aimbot::Melee::SortMethod.m_Var, meleeSortMethod, IM_ARRAYSIZE(meleeSortMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to decide which target to aim at");
							static const char* meleeAimMethod[]{ "Plain", "Smooth", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###meleeAimMethod", &Vars::Aimbot::Melee::AimMethod.m_Var, meleeAimMethod, IM_ARRAYSIZE(meleeAimMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to aim at the target");
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Smooth factor###meleeSmooth", &Vars::Aimbot::Melee::SmoothingAmount.m_Var, 1.0f, 10.f, "%.0f", 1.0f); ImGui::PopItemWidth(); HelpMarker("How many degrees the aimbot's FoV will have");
							ImGui::Checkbox("Range check", &Vars::Aimbot::Melee::RangeCheck.m_Var); HelpMarker("Only aim at target if within melee range");
							ImGui::Checkbox("Melee prediction", &Vars::Aimbot::Melee::PredictSwing.m_Var); HelpMarker("Aimbot will attack preemptively, predicting you will be in range of the target");
							ImGui::Checkbox("Whip teammates", &Vars::Aimbot::Melee::WhipTeam.m_Var); HelpMarker("Aimbot will target teammates if holding the Disciplinary Action");
						}
					}
					ImGui::EndChild();

				}
				if (SettingsTab == 1) {
					ImGui::BeginChild("Trigger");
					{
						ImGui::Columns(3);
						{
							ImGui::TextUnformatted("Global");
							ImGui::Checkbox("Triggerbot###gTrigger", &Vars::Triggerbot::Global::Active.m_Var); HelpMarker("Global triggerbot master switch");
							InputKeybind("Trigger key", Vars::Triggerbot::Global::TriggerKey); HelpMarker("The key which activates the triggerbot");
							ImGui::Checkbox("Ignore invulnerable###gTriggerIgnoreInvuln", &Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var); HelpMarker("The triggerbot will ignore targets who can't be damaged");
							ImGui::Checkbox("Ignore cloaked###gTriggerIgnoreCloak", &Vars::Triggerbot::Global::IgnoreCloaked.m_Var); HelpMarker("The triggerbot will ignore spies who are cloaked");
							ImGui::Checkbox("Ignore friend###gTriggerIgnoreFriend", &Vars::Triggerbot::Global::IgnoreFriends.m_Var); HelpMarker("The triggerbot will ignore steam friends");


							ImGui::TextUnformatted("");
							ImGui::TextUnformatted("");
							ImGui::TextUnformatted("Auto airblast");
							ImGui::Checkbox("Active###gAAB", &Vars::Triggerbot::Blast::Active.m_Var); HelpMarker("Auto airblast master switch");
							ImGui::Checkbox("Rage mode###gAABr", &Vars::Triggerbot::Blast::Rage.m_Var); HelpMarker("Will airblast whenever possible, regardless of FoV");
							ImGui::Checkbox("Silent###gAABs", &Vars::Triggerbot::Blast::Silent.m_Var); HelpMarker("Aim changes made by the rage mode setting aren't visible");
							ImGui::PushItemWidth(100); ImGui::SliderInt("Airblast FoV###gAABFoV", &Vars::Triggerbot::Blast::Fov.m_Var, 1, 60, "%d"); ImGui::PopItemWidth(); HelpMarker("How many degrees the auto airblast's FoV will have");

						}
						ImGui::NextColumn();
						{
							ImGui::TextUnformatted("Auto shoot");
							ImGui::Checkbox("Active###gAS", &Vars::Triggerbot::Shoot::Active.m_Var); HelpMarker("Shoots if mouse is over a target");
							ImGui::Checkbox("Shoot players###gASsp", &Vars::Triggerbot::Shoot::TriggerPlayers.m_Var); HelpMarker("Auto shoot will target players");
							ImGui::Checkbox("Shoot buildings###gASsb", &Vars::Triggerbot::Shoot::TriggerBuildings.m_Var); HelpMarker("Auto shoot will target buildings");
							ImGui::Checkbox("Head only###gASho", &Vars::Triggerbot::Shoot::HeadOnly.m_Var); HelpMarker("Auto shoot will only shoot if you are aiming at the head");
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Head scale###gAShs", &Vars::Triggerbot::Shoot::HeadScale.m_Var, 0.5f, 1.0f, "%.1f"); ImGui::PopItemWidth(); HelpMarker("The scale at which the auto shoot will try to shoot the targets head");
							ImGui::Checkbox("Wait for charge###gASwfc", &Vars::Triggerbot::Shoot::WaitForCharge.m_Var); HelpMarker("Auto shoot will only shoot if the sniper is charged enough to kill in one hit / is fully charged");

							ImGui::TextUnformatted("");
							ImGui::TextUnformatted("");
							ImGui::TextUnformatted("Auto detonate");
							ImGui::Checkbox("Active###gAD", &Vars::Triggerbot::Detonate::Active.m_Var); HelpMarker("Auto detonate master switch");
							ImGui::Checkbox("Stickybombs###gADs", &Vars::Triggerbot::Detonate::Stickies.m_Var); HelpMarker("Detonate sticky bombs");
							ImGui::Checkbox("Detonator flares###gADd", &Vars::Triggerbot::Detonate::Flares.m_Var); HelpMarker("Detonate detonator flares");
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Detonate radius###gADr", &Vars::Triggerbot::Detonate::RadiusScale.m_Var, 0.5f, 1.0f, "%.1f"); ImGui::PopItemWidth(); HelpMarker("The radius around the projectile that it will detonate if a player is in");
						}
						ImGui::NextColumn();
						{
							ImGui::TextUnformatted("Auto backstab");
							ImGui::Checkbox("Active###gABS", &Vars::Triggerbot::Stab::Active.m_Var); HelpMarker("Auto backstab will attempt to backstab the target if possible");
							ImGui::Checkbox("Rage mode###gABSr", &Vars::Triggerbot::Stab::RageMode.m_Var); HelpMarker("Stabs whenever possible by aiming toward the back");
							ImGui::Checkbox("Silent###gABSs", &Vars::Triggerbot::Stab::Silent.m_Var); HelpMarker("Aim changes made by the rage mode setting aren't visible");
							ImGui::Checkbox("Disguise after stab###gABSd", &Vars::Triggerbot::Stab::Disguise.m_Var); HelpMarker("Will apply the previous disguise after stabbing");
							ImGui::Checkbox("Ignore razorback###gABSig", &Vars::Triggerbot::Stab::IgnRazor.m_Var); HelpMarker("Will not attempt to backstab snipers wearing the razorback");
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Stab range", &Vars::Triggerbot::Stab::Range.m_Var, 0.5f, 1.0f, "%.1f"); ImGui::PopItemWidth(); HelpMarker("The range at which auto backstab will attempt to stab");

							ImGui::TextUnformatted("");
							ImGui::TextUnformatted("");
							ImGui::TextUnformatted("Auto uber");
							ImGui::Checkbox("Active###gAU", &Vars::Triggerbot::Uber::Active.m_Var); HelpMarker("Auto uber master switch");
							ImGui::Checkbox("Only on friends###gAUf", &Vars::Triggerbot::Uber::OnlyFriends.m_Var); HelpMarker("Auto uber will only activate if healing steam friends");
							ImGui::Checkbox("Uber self###gAUs", &Vars::Triggerbot::Detonate::Flares.m_Var); HelpMarker("Auto uber will activate if local player's health falls below the percentage");
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Health left (%)###gAUhp", &Vars::Triggerbot::Uber::HealthLeft.m_Var, 1.f, 99.f, "%.0f%%", 1.0f); ImGui::PopItemWidth(); HelpMarker("The amount of health the heal target must be below to actiavte");

						}
					}
					ImGui::EndChild();
				}
				if (SettingsTab == 2) {
					ImGui::BeginChild("Visuals");
					{
						ImGui::Columns(3);
						{
							ImGui::TextUnformatted("Players");
							ImGui::Checkbox("Enable Team/Enemy colors", &Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
							if (ImGui::CollapsingHeader("ESP###playerESp", ImGuiTreeNodeFlags_DefaultOpen))
							{
								//ImGui::TextUnformatted("ESP");
								ImGui::Checkbox("Player ESP", &Vars::ESP::Players::Active.m_Var); HelpMarker("Will draw useful information/indicators on players");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Player ESP Opacity", &Vars::ESP::Players::Alpha.m_Var, 0.05f, 1.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque/transparent the ESP will be");
								static const char* textOutline[]{ "Off", "Text Only", "All" }; ImGui::PushItemWidth(100); ImGui::Combo("Text outline", &Vars::ESP::Main::Outlinedbar.m_Var, textOutline, IM_ARRAYSIZE(textOutline)); ImGui::PopItemWidth(); HelpMarker("Choose when to use an outline on elements drawn by ESP");
								ImGui::Checkbox("Local ESP", &Vars::ESP::Players::ShowLocal.m_Var); HelpMarker("Will draw ESP on local player (thirdperson)");
								static const char* ignoreTeammatesEsp[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###ESPteam", &Vars::ESP::Players::IgnoreTeammates.m_Var, ignoreTeammatesEsp, IM_ARRAYSIZE(ignoreTeammatesEsp)); ImGui::PopItemWidth(); HelpMarker("Which teammates the ESP will ignore drawing on");
								static const char* ignoreCloakedEsp[]{ "Off", "All", "Enemies only" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore cloaked###ESPcloak", &Vars::ESP::Players::IgnoreCloaked.m_Var, ignoreCloakedEsp, IM_ARRAYSIZE(ignoreCloakedEsp)); ImGui::PopItemWidth(); HelpMarker("Which cloaked spies the ESP will ignore drawing on");
								ImGui::Checkbox("Player name", &Vars::ESP::Players::Name.m_Var); HelpMarker("Will draw the players name");
								ImGui::Checkbox("Name box", &Vars::ESP::Players::NameBox.m_Var); HelpMarker("Will draw a box around players name to make it stand out");
								static const char* classEsp[]{ "Off", "Icon", "Text" }; ImGui::PushItemWidth(100); ImGui::Combo("Player class", &Vars::ESP::Players::Class.m_Var, classEsp, IM_ARRAYSIZE(classEsp)); ImGui::PopItemWidth(); HelpMarker("Will draw the class the player is");
								ImGui::Checkbox("Weapon icons", &Vars::ESP::Players::WeaponIcon.m_Var); HelpMarker("Shows the killfeed icon for the wepaon the player is holding (some weapons dont have killfeed icons and others idk the killfeed icon name for"); // CBaseCombatWeapon::GetWeaponIcon
								ImGui::Checkbox("Player health", &Vars::ESP::Players::Health.m_Var); HelpMarker("Will draw the players health, as well as their max health");
								ImGui::Checkbox("Player health bar", &Vars::ESP::Players::HealthBar.m_Var); HelpMarker("Will draw a bar visualizing how much health the player has");
								ImGui::Checkbox("Player conditions", &Vars::ESP::Players::Cond.m_Var); HelpMarker("Will draw what conditions the player is under");
								static const char* uberESP[]{ "Off", "Text", "Bar" }; ImGui::PushItemWidth(100); ImGui::Combo("Player ubercharge", &Vars::ESP::Players::Uber.m_Var, uberESP, IM_ARRAYSIZE(uberESP)); ImGui::PopItemWidth(); HelpMarker("Will draw how much ubercharge a medic has");
								static const char* boxESP[]{ "Off", "Simple", "Cornered", "3D" }; ImGui::PushItemWidth(100); ImGui::Combo("Player box", &Vars::ESP::Players::Box.m_Var, boxESP, IM_ARRAYSIZE(boxESP)); ImGui::PopItemWidth(); HelpMarker("What sort of box to draw on players");
								static const char* bonesESP[]{ "Off", "Custom color", "Health" }; ImGui::PushItemWidth(100); ImGui::Combo("Player bones", &Vars::ESP::Players::Bones.m_Var, bonesESP, IM_ARRAYSIZE(bonesESP)); ImGui::PopItemWidth(); HelpMarker("Will draw the bone structure of the player");
								ImGui::Checkbox("Player GUID", &Vars::ESP::Players::GUID.m_Var); HelpMarker("Show's the players Steam ID");
								ImGui::Checkbox("Player lines", &Vars::ESP::Players::Lines.m_Var); HelpMarker("Draws lines from the local players position to enemies position");
								ImGui::Checkbox("Player Dlights", &Vars::ESP::Players::Dlights.m_Var); HelpMarker("Will make players emit a dynamic light around them");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Dlight radius", &Vars::ESP::Players::DlightRadius.m_Var, 5.0f, 400.f, "%.0f"); ImGui::PopItemWidth(); HelpMarker("How far the Dlight will illuminate");
							}
							if (ImGui::CollapsingHeader("Chams###playerChamss")) {
								ImGui::Checkbox("Player chams", &Vars::Chams::Players::Active.m_Var); HelpMarker("Player chams master switch");
								if (g_Interfaces.CVars->FindVar("mat_phong")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_specular")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_bumpmap")->GetInt() == 1) {
									static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel" }; ImGui::PushItemWidth(100); ImGui::Combo("Player material", &Vars::Chams::Players::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth(); HelpMarker("Which material the chams will apply to the player");
								}
								else {
									static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur" }; ImGui::PushItemWidth(100); ImGui::Combo("Player material", &Vars::Chams::Players::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials)); ImGui::PopItemWidth(); HelpMarker("Which material the chams will apply to the player");

								}
								ImGui::Checkbox("Local chams", &Vars::Chams::Players::ShowLocal.m_Var); HelpMarker("Will draw chams on local player (thirdperson");
								static const char* ignoreTeammatesChams[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###chamsteam", &Vars::Chams::Players::IgnoreTeammates.m_Var, ignoreTeammatesChams, IM_ARRAYSIZE(ignoreTeammatesChams)); ImGui::PopItemWidth(); HelpMarker("Which teammates the chams will ignore drawing on");
								ImGui::Checkbox("Chams on cosmetics", &Vars::Chams::Players::Wearables.m_Var); HelpMarker("Will draw chams on player cosmetics");
								ImGui::Checkbox("Chams on weapons", &Vars::Chams::Players::Weapons.m_Var); HelpMarker("Will draw chams on player weapons");
								ImGui::Checkbox("Chams through walls", &Vars::Chams::Players::IgnoreZ.m_Var); HelpMarker("Will draw chams on the player regardless of if the player is actually visible");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Player chams opacity", &Vars::Chams::Players::Alpha.m_Var, 0.0f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the chams are");
								if (ImGui::Button("Fix chams")) {
									g_Chams.Init();
								}
							}
							if (ImGui::CollapsingHeader("Glow###playerGloww")) {
								ImGui::Checkbox("Wireframe glow", &Vars::Glow::Main::Wireframe.m_Var); HelpMarker("Wireframe glow");
								ImGui::PushItemWidth(100); ImGui::SliderInt("Global glow scale", &Vars::Glow::Main::Scale.m_Var, 1, 10, "%d"); ImGui::PopItemWidth(); HelpMarker("The scale at which the glow will render");
								ImGui::Checkbox("Player glow", &Vars::Glow::Players::Active.m_Var); HelpMarker("Player glow master switch");
								ImGui::Checkbox("Local rainbow glow", &Vars::Glow::Players::LocalRainbow.m_Var); HelpMarker("Local player glow is rainbow coloured");
								static const char* ignoreTeammatesGlow[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###glowteam", &Vars::Glow::Players::IgnoreTeammates.m_Var, ignoreTeammatesGlow, IM_ARRAYSIZE(ignoreTeammatesGlow)); ImGui::PopItemWidth(); HelpMarker("Which teammates the glow will ignore drawing on");
								ImGui::Checkbox("Glow on cosmetics", &Vars::Glow::Players::Wearables.m_Var); HelpMarker("Will draw glow on player cosmetics");
								ImGui::Checkbox("Glow on weapons", &Vars::Glow::Players::Weapons.m_Var); HelpMarker("Will draw glow on player weapons");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Player glow opacity", &Vars::Glow::Players::Alpha.m_Var, 0.1f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the glow is");
								static const char* colorGlow[]{ "Team", "Health" }; ImGui::PushItemWidth(100); ImGui::Combo("Player glow color", &Vars::Glow::Players::Color.m_Var, colorGlow, IM_ARRAYSIZE(colorGlow)); ImGui::PopItemWidth(); HelpMarker("Which color the glow will draw");
							}
							if (ImGui::CollapsingHeader("DME chams")) {
								ImGui::Checkbox("DME chams###dmeactive", &Vars::Chams::DME::Active.m_Var); HelpMarker("DME chams master switch");
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
								ImGui::Combo("Arm material", &Vars::Chams::DME::Hands.m_Var, handsMaterial, IM_ARRAYSIZE(handsMaterial));
								ImGui::PopItemWidth();
								HelpMarker("What material to put on your viewmodels arms/hands");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Arm opacity", &Vars::Chams::DME::HandsAlpha.m_Var, 0.0f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque your viewmodel arms will appear");
								ImGui::Checkbox("Arm glow overlay", &Vars::Chams::DME::HandsGlowOverlay.m_Var); HelpMarker("Will place a second glow-like material overlayed on top of the original material");
								ImGui::PushItemWidth(100); ImGui::SliderInt("Arm glow boost", &Vars::Chams::DME::HandsRimMultiplier.m_Var, 1, 100, "%d"); ImGui::PopItemWidth(); HelpMarker("How much the glow effect will be boosted by");

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
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Weapon opacity", &Vars::Chams::DME::WeaponAlpha.m_Var, 0.0f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque your viewmodel arms will appear");
								ImGui::Checkbox("Weapon glow overlay", &Vars::Chams::DME::WeaponGlowOverlay.m_Var); HelpMarker("Will place a second glow-like material overlayed on top of the original material");
								ImGui::PushItemWidth(100); ImGui::SliderInt("Weapon glow boost", &Vars::Chams::DME::WeaponRimMultiplier.m_Var, 1, 100, "%d"); ImGui::PopItemWidth(); HelpMarker("How much the glow effect will be boosted by");

							}

						}
						ImGui::NextColumn();
						{
							ImGui::TextUnformatted("Buildings");
							if (ImGui::CollapsingHeader("ESP###buildingESP", ImGuiTreeNodeFlags_DefaultOpen)) {
								ImGui::Checkbox("Building ESP", &Vars::ESP::Buildings::Active.m_Var); HelpMarker("Will draw useful information/indicators on buildings");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Building ESP Opacity", &Vars::ESP::Buildings::Alpha.m_Var, 0.05f, 1.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque/transparent the ESP will be");
								ImGui::Checkbox("Ignore team buildings###ESPbuildingsteam", &Vars::ESP::Buildings::IgnoreTeammates.m_Var); HelpMarker("Whether or not to draw ESP on your teams buildings");
								ImGui::Checkbox("Building name", &Vars::ESP::Buildings::Name.m_Var); HelpMarker("Will draw the players name");
								ImGui::Checkbox("Name box###buildingnamebox", &Vars::ESP::Buildings::NameBox.m_Var); HelpMarker("Will draw a box around the buildings name to make it stand out");
								ImGui::Checkbox("Building health", &Vars::ESP::Buildings::Health.m_Var); HelpMarker("Will draw the building's health, as well as its max health");
								ImGui::Checkbox("Building health bar", &Vars::ESP::Buildings::HealthBar.m_Var); HelpMarker("Will draw a bar visualizing how much health the building has");
								ImGui::Checkbox("Building conditions", &Vars::ESP::Buildings::Cond.m_Var); HelpMarker("Will draw what conditions the building is under");
								ImGui::Checkbox("Building level", &Vars::ESP::Buildings::Level.m_Var); HelpMarker("Will draw what level the building is");
								ImGui::Checkbox("Building owner", &Vars::ESP::Buildings::Owner.m_Var); HelpMarker("Shows who built the building");
								static const char* boxESPb[]{ "Off", "Simple", "Cornered", "3D" }; ImGui::PushItemWidth(100); ImGui::Combo("Building box", &Vars::ESP::Buildings::Box.m_Var, boxESPb, IM_ARRAYSIZE(boxESPb)); ImGui::PopItemWidth(); HelpMarker("What sort of box to draw on buildings");
								ImGui::Checkbox("Building lines", &Vars::ESP::Buildings::Lines.m_Var); HelpMarker("Draws lines from the local players position to the buildings position");
								ImGui::Checkbox("Building Dlights", &Vars::ESP::Buildings::Dlights.m_Var); HelpMarker("Will make buildings emit a dynamic light around them, although buildings can't move some I'm not sure that the lights are actually dynamic here...");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Dlight radius###buildingsdlights", &Vars::ESP::Buildings::DlightRadius.m_Var, 5.0f, 400.f, "%.0f"); ImGui::PopItemWidth(); HelpMarker("How far the Dlight will illuminate");
							}
							if (ImGui::CollapsingHeader("Chams###buildingchamss")) {
								ImGui::Checkbox("Building chams", &Vars::Chams::Buildings::Active.m_Var); HelpMarker("Building chams master switch");
								if (g_Interfaces.CVars->FindVar("mat_phong")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_specular")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_bumpmap")->GetInt() == 1) {
									static const char* pchamsbMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel" }; ImGui::PushItemWidth(100); ImGui::Combo("Building material", &Vars::Chams::Buildings::Material.m_Var, pchamsbMaterials, IM_ARRAYSIZE(pchamsbMaterials)); ImGui::PopItemWidth(); HelpMarker("Which material the chams will apply to the player");
								}
								else {
									static const char* pchamsbMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur" }; ImGui::PushItemWidth(100); ImGui::Combo("Building material", &Vars::Chams::Buildings::Material.m_Var, pchamsbMaterials, IM_ARRAYSIZE(pchamsbMaterials)); ImGui::PopItemWidth(); HelpMarker("Which material the chams will apply to the player");
								}
								ImGui::Checkbox("Ignore team buildings###Chamsbuildingsteam", &Vars::Chams::Buildings::IgnoreTeammates.m_Var);  HelpMarker("Whether or not to draw chams on your teams buildings");
								ImGui::Checkbox("Chams through walls###buildingsignorez", &Vars::Chams::Buildings::IgnoreZ.m_Var); HelpMarker("Will draw chams on the building regardless of if the building is actually visible");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Building chams opacity", &Vars::Chams::Buildings::Alpha.m_Var, 0.0f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the chams are");
							}
							if (ImGui::CollapsingHeader("Glow###buildinggloww")) {
								ImGui::Checkbox("Building glow", &Vars::Glow::Buildings::Active.m_Var); HelpMarker("Player glow master switch");
								ImGui::Checkbox("Ignore team buildings###glowbuildingsteam", &Vars::Glow::Buildings::IgnoreTeammates.m_Var);  HelpMarker("Whether or not to draw glow on your teams buildings");
								//static const char* ignoreTeammatesGlow[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###glowteam", &Vars::Glow::Buildings::IgnoreTeammates.m_Var, ignoreTeammatesGlow, IM_ARRAYSIZE(ignoreTeammatesGlow)); ImGui::PopItemWidth(); HelpMarker("Which teammates the glow will ignore drawing on");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Building glow opacity", &Vars::Glow::Buildings::Alpha.m_Var, 0.1f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the glow is");
								static const char* colorGlow[]{ "Team", "Health" }; ImGui::PushItemWidth(100); ImGui::Combo("Building glow color", &Vars::Glow::Buildings::Color.m_Var, colorGlow, IM_ARRAYSIZE(colorGlow)); ImGui::PopItemWidth(); HelpMarker("Which color the glow will draw");
							}
						}
						ImGui::NextColumn();
						{
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
							ImGui::TextUnformatted("World");
							if (ImGui::CollapsingHeader("ESP###worldesp", ImGuiTreeNodeFlags_DefaultOpen)) {
								ImGui::Checkbox("Pickups ESP", &Vars::ESP::World::Active.m_Var); HelpMarker("Pickup ESP master switch");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Pickups ESP Opacity", &Vars::ESP::Players::Alpha.m_Var, 0.05f, 1.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque/transparent the ESP will be");
								ImGui::Checkbox("Healthpack ESP", &Vars::ESP::World::HealthText.m_Var); HelpMarker("Will draw ESP on health packs");
								ImGui::Checkbox("Ammopack ESP", &Vars::ESP::World::AmmoText.m_Var); HelpMarker("Will draw ESP on ammo packs");
							}
							if (ImGui::CollapsingHeader("Chams###chamsworld")) {
								ImGui::Checkbox("Pickup chams", &Vars::Chams::World::Active.m_Var); HelpMarker("Pickup chams master switch");
								ImGui::Checkbox("Healthpack chams", &Vars::Chams::World::Health.m_Var); HelpMarker("Will draw chams on health packs");
								ImGui::Checkbox("Ammopack chams", &Vars::Chams::World::Ammo.m_Var); HelpMarker("Will draw chams on ammo packs");
								static const char* projectilesTeam[]{ "Off", "All", "Enemy Only" }; ImGui::PushItemWidth(100); ImGui::Combo("Projectile chams", &Vars::Chams::World::Projectiles.m_Var, projectilesTeam, IM_ARRAYSIZE(projectilesTeam)); ImGui::PopItemWidth(); HelpMarker("When to draw chams on projectiles");
								//Gui::Checkbox("", &Vars::Chams::World::Projectiles.m_Var);
								if (g_Interfaces.CVars->FindVar("mat_phong")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_specular")->GetInt() == 1 && g_Interfaces.CVars->FindVar("mat_bumpmap")->GetInt() == 1) {
									static const char* pchamspMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel" }; ImGui::PushItemWidth(100); ImGui::Combo("Pickup chams material", &Vars::Chams::World::Material.m_Var, pchamspMaterials, IM_ARRAYSIZE(pchamspMaterials)); ImGui::PopItemWidth(); HelpMarker("Which material the chams will apply to the pickups");
								}
								else {
									static const char* pchamspMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur" }; ImGui::PushItemWidth(100); ImGui::Combo("Pickup chams material", &Vars::Chams::World::Material.m_Var, pchamspMaterials, IM_ARRAYSIZE(pchamspMaterials)); ImGui::PopItemWidth(); HelpMarker("Which material the chams will apply to the pickups");
								}
								//ImGui::Checkbox("", &Vars::Chams::World::Material.m_Var);
								ImGui::Checkbox("Chams through walls###pickupsignorez", &Vars::Chams::World::IgnoreZ.m_Var); HelpMarker("Will draw chams on pickups through walls");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Pickup chams opacity", &Vars::Chams::World::Alpha.m_Var, 0.0f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the chams are");

							}
							if (ImGui::CollapsingHeader("Glow###glowworldd")) {
								ImGui::Checkbox("Pickup glow", &Vars::Glow::World::Active.m_Var); HelpMarker("Pickup glow master switch");
								ImGui::Checkbox("Healthpack glow", &Vars::Glow::World::Health.m_Var); HelpMarker("Will draw glow on health packs");
								ImGui::Checkbox("Ammopack glow", &Vars::Glow::World::Ammo.m_Var); HelpMarker("Will draw glow on ammo packs");
								static const char* projectilesgTeam[]{ "Off", "All", "Enemy Only" }; ImGui::PushItemWidth(100); ImGui::Combo("Projectile glow", &Vars::Glow::World::Projectiles.m_Var, projectilesgTeam, IM_ARRAYSIZE(projectilesgTeam)); ImGui::PopItemWidth(); HelpMarker("When to draw glow on projectiles");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Pickup glow opacity", &Vars::Glow::World::Alpha.m_Var, 0.1f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the glow is");
							}

							if (ImGui::CollapsingHeader("Miscellaneous")) {
								ImGui::PushItemWidth(100); ImGui::SliderInt("Field of view", &Vars::Visuals::FieldOfView.m_Var, 70, 150, "%d"); ImGui::PopItemWidth(); HelpMarker("How many degrees of field of vision you would like");
								//ImGui::PushItemWidth(100); ImGui::SliderInt("Aimbot FoV circle alpha", &Vars::Visuals::AimFOVAlpha.m_Var, 0, 255, "%d"); ImGui::PopItemWidth(); HelpMarker("How opaque the aimbot's FoV circle is");
								ImGui::Checkbox("World modulation", &Vars::Visuals::WorldModulation.m_Var); HelpMarker("Will colour modulate the world");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Prop opacity", &Vars::Visuals::PropAlpha.m_Var, 0.01f, 1.0f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How opaque the glow is");
								ImGui::Checkbox("Sky modulation", &Vars::Visuals::SkyModulation.m_Var); HelpMarker("Will colour modulate the sky");
								
								// literally complete paste from this same cpp file LMAO
								std::vector<std::string> vec;
								static bool removalFlags[6]{ Vars::Visuals::RemoveScope.m_Var,Vars::Visuals::RemoveZoom.m_Var,Vars::Visuals::RemoveDisguises.m_Var,Vars::Visuals::RemoveTaunts.m_Var,Vars::Misc::DisableInterpolation.m_Var,Vars::Visuals::RemovePunch.m_Var };
								const char* pRemovals[] = {"Scope", "Zoom", "Disguises", "Taunts", "Interpolation", "View Punch"}; static std::string previewValue = "";
								if (ImGui::BeginCombo("Removals", previewValue.c_str()))
								{
									previewValue = "";
									for (size_t i = 0; i < IM_ARRAYSIZE(pRemovals); i++)
									{
										ImGui::Selectable(pRemovals[i], &removalFlags[i]);
										if (removalFlags[i])
											vec.push_back(pRemovals[i]);
											
									}
									for (size_t i = 0; i < vec.size(); i++)
									{
										if (vec.size() == 1)
											previewValue += vec.at(i);
										else if (!(i == vec.size() - 1))
											previewValue += vec.at(i) + ", ";
										else
											previewValue += vec.at(i);
									}
									ImGui::EndCombo();
								} // i got tired of trying better ways so this is new method fr*ck you
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

								if (Vars::Visuals::RemoveScope.m_Var) {
									ImGui::Checkbox("Noscope lines", &Vars::Visuals::ScopeLines.m_Var); HelpMarker("Will draw a custom overlay");
								}
								
								ImGui::Checkbox("Aimbot crosshair", &Vars::Visuals::CrosshairAimPos.m_Var); HelpMarker("Will make your crosshair move to where the aimbot is going to shoot");
								ImGui::Checkbox("Aimbot prediction", &Vars::Visuals::AimPosSquare.m_Var); HelpMarker("Will show a rough estimate of where the aimbot is going to aim at");
								ImGui::Checkbox("Draw Hitboxes", &Vars::Aimbot::Global::showHitboxes.m_Var); HelpMarker("Shows client hitboxes for enemies once they are attacked (not bbox)");
								if (Vars::Aimbot::Global::showHitboxes.m_Var) {
									ImGui::Checkbox("Clear Hitboxes", &Vars::Aimbot::Global::clearPreviousHitbox.m_Var); HelpMarker("Removes previous drawn hitboxes to mitigate clutter");
									ImGui::SliderInt("Hitbox Draw Time", &Vars::Aimbot::Global::hitboxTime.m_Var, 1, 5); HelpMarker("Removes previous drawn hitboxes after n seconds");
								}
								ImGui::Checkbox("Bullet tracers", &Vars::Visuals::BulletTracer.m_Var); HelpMarker("Will draw a line from your position to where the aimbot will shoot if hitscan or projectile");
								ImGui::Checkbox("Rainbow tracers", &Vars::Visuals::BulletTracerRainbow.m_Var); HelpMarker("Bullet tracer color will be dictated by a changing color");
								static const char* projectilesgTeam[]{ "Off", "Machina", "C.A.P.P.E.R", "Short Circuit", "Merasmus ZAP", "Merasmus ZAP Beam 2", "Big Nasty", "Distortion Trail", "Black Ink", "Custom"}; ImGui::PushItemWidth(100); ImGui::Combo("Particle tracer", &Vars::Visuals::ParticleTracer.m_Var, projectilesgTeam, IM_ARRAYSIZE(projectilesgTeam)); ImGui::PopItemWidth();
								if (Vars::Visuals::ParticleTracer.m_Var == 9) {
									ImGui::PushItemWidth(100); ImGui::InputText("Custom Tracer", &Vars::Visuals::ParticleName); ImGui::PopItemWidth(); HelpMarker("If you want to use a custom particle tracer");
								}
								ImGui::TextUnformatted("");
								ImGui::Checkbox("Thirdperson", &Vars::Visuals::ThirdPerson.m_Var); HelpMarker("Will move your camera to be in a thirdperson view");
								InputKeybind("Thirdperson key", Vars::Visuals::ThirdPersonKey); HelpMarker("What key to toggle thirdperson, press ESC if no bind is desired");
								ImGui::Checkbox("Thirdperson show silent angles", &Vars::Visuals::ThirdPersonSilentAngles.m_Var); HelpMarker("Will show your silent angles on thirdperson (not what others see)");
								ImGui::Checkbox("Thirdperson instant yaw", &Vars::Visuals::ThirdPersonInstantYaw.m_Var); HelpMarker("Will set your yaw instantly in thirdperson, showing your actual angle, instead of what others see");
								ImGui::TextUnformatted("");
								ImGui::Checkbox("Skybox changer", &Vars::Visuals::SkyboxChanger.m_Var); HelpMarker("Will change the skybox, either to a base TF2 one or a custom one");
								ImGui::PushItemWidth(100); ImGui::Combo("Skybox", &Vars::Skybox::skyboxnum, skyNames, IM_ARRAYSIZE(skyNames), 6);  ImGui::PopItemWidth();
								ImGui::PushItemWidth(100); ImGui::InputText("Custom skybox", &Vars::Skybox::SkyboxName); ImGui::PopItemWidth(); HelpMarker("If you want to load a custom skybox, type it here (tf/materials/skybox)");
								ImGui::TextUnformatted("");
								ImGui::Checkbox("World Textures Override", &Vars::Visuals::OverrideWorldTextures.m_Var); HelpMarker("Turn this off when in-game so you don't drop fps :p");
								const char* visionModifiers[]{ "Off", "Pyrovision", "Halloween", "Romevision" }; ImGui::PushItemWidth(100); ImGui::Combo("Vision modifiers", &Vars::Visuals::Vision.m_Var, visionModifiers, IM_ARRAYSIZE(visionModifiers)); ImGui::PopItemWidth(); HelpMarker("Vision modifiers");
							}

						}
					}
					ImGui::EndChild();
				}
				if (SettingsTab == 3) {
					//Misc
					ImGui::BeginChild("Misc");
					{
						ImGui::Columns(3);
						{
							if (ImGui::CollapsingHeader("Miscellanious features", ImGuiTreeNodeFlags_DefaultOpen)) {
								ImGui::Checkbox("Bunnyhop", &Vars::Misc::AutoJump.m_Var); HelpMarker("Will jump as soon as you touch the ground again, keeping speed between jumps");
								const char* autoStrafeModes[]{ "Off", "Normal", "Directional" }; ImGui::PushItemWidth(100); ImGui::Combo("Autostrafer", &Vars::Misc::AutoStrafe.m_Var, autoStrafeModes, IM_ARRAYSIZE(autoStrafeModes)); ImGui::PopItemWidth(); HelpMarker("Will strafe for you in air automatically so that you gain speed");
								ImGui::Checkbox("Edge jump", &Vars::Misc::EdgeJump.m_Var); HelpMarker("Will jump at the very end of whatever platform you're on, allowing you to perfectly make longer jumps.");
								if (Vars::Misc::EdgeJump.m_Var) {
									InputKeybind("Edge jump key", Vars::Misc::EdgeJumpKey); HelpMarker("Edge jump bind, leave as None for always on");
								}
								const char* specModes[]{ "Off", "Draggable", "Static", "Static + Avatars" }; ImGui::PushItemWidth(100); ImGui::Combo("Spectator list", &Vars::Visuals::SpectatorList.m_Var, specModes, IM_ARRAYSIZE(specModes)); ImGui::PopItemWidth(); HelpMarker("Will show who is currently spectating you");
								ImGui::Checkbox("Taunt slide", &Vars::Misc::TauntSlide.m_Var); HelpMarker("Allows you to input in taunts");
								ImGui::Checkbox("Taunt control", &Vars::Misc::TauntControl.m_Var); HelpMarker("Gives full control if enabled with taunt slide");
								ImGui::Checkbox("Bypass pure", &Vars::Misc::BypassPure.m_Var); HelpMarker("Allows you to load any custom files, even if disallowed by the sv_pure setting");
								ImGui::Checkbox("Medal flip", &Vars::Misc::MedalFlip.m_Var); HelpMarker("Medal go spinny spinny weeeeeee");
								ImGui::Checkbox("Noisemaker spam", &Vars::Misc::NoisemakerSpam.m_Var); HelpMarker("Will spam your noisemaker without using its charges");
								ImGui::Checkbox("Auto rocketjump", &Vars::Misc::AutoRocketJump.m_Var); HelpMarker("Will rocket jump at the angle you're looking at when you press mouse2 with a rocket launcher");
								ImGui::Checkbox("Chat spam", &Vars::Misc::ChatSpam.m_Var); HelpMarker("Spam the chat with Fedoraware adverts");
								ImGui::Checkbox("No push", &Vars::Misc::NoPush.m_Var); HelpMarker("Will make teammates unable to push you around");
								const char* rollModes[]{ "Off", "Backwards", "Fake forward" }; ImGui::PushItemWidth(100); ImGui::Combo("Crouch speed", &Vars::Misc::Roll.m_Var, rollModes, IM_ARRAYSIZE(rollModes)); ImGui::PopItemWidth(); HelpMarker("Allows you to go at normal walking speed when crouching (affects many things, use with caution)");
								ImGui::Checkbox("Show class changes", &Vars::Visuals::ChatInfo.m_Var); HelpMarker("Will say when people change class in chat");
								ImGui::Checkbox("Vote revealer", &Vars::Misc::VoteRevealer.m_Var); HelpMarker("Will say who voted F1 or F2 in chat");
								ImGui::Checkbox("Votes to party", &Vars::Misc::VotesInChat.m_Var); HelpMarker("Will send vote information to party chat (use with caution)");
								const char* logModes[]{ "Off", "Chat", "Text" }; ImGui::PushItemWidth(100); ImGui::Combo("Damage logger", &Vars::Visuals::damageLogger.m_Var, logModes, IM_ARRAYSIZE(logModes)); ImGui::PopItemWidth(); HelpMarker("Will log any damage you deal to players");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Damage logger time", &Vars::Visuals::despawnTime.m_Var, 0.5f, 10.f, "%.1f"); ImGui::PopItemWidth(); HelpMarker("How many ticks the damage logger text will stay");
								ImGui::Checkbox("Anti-AFK", &Vars::Misc::AntiAFK.m_Var); HelpMarker("Will make you jump every now and again so you don't get kicked for idling");
								ImGui::Checkbox("Force sv_cheats", &Vars::Misc::CheatsBypass.m_Var); HelpMarker("Will force sv_cheats 1, allowing commands like tf_viewmodels_offset_override, fog_override");
								ImGui::Checkbox("Be marked as a cat", &Vars::Misc::BeCat.m_Var); HelpMarker("Will mark you as a cathook instance to other cathook instances (basically catbots)");
								ImGui::Checkbox("Menu tooltips", &tooltips); HelpMarker("Will enable/disable these");
								ImGui::Checkbox("Menu snow", &Vars::Visuals::Snow.m_Var); HelpMarker("Enable the snow when menu is open");
								ImGui::Checkbox("Rage Retry", &Vars::Misc::RageRetry.m_Var); HelpMarker("Will automatically reconnect when your health is low");
								ImGui::Checkbox("CatReply", &Vars::Misc::BeCat.m_Var); HelpMarker("Be marked by catbots.");
							}
							if (ImGui::CollapsingHeader("Out of FoV arrows")) {
								ImGui::Checkbox("Active###fovar", &Vars::Visuals::OutOfFOVArrows.m_Var); HelpMarker("Will draw arrows to players who are outside of the range of your FoV");
								ImGui::Checkbox("Outline arrows###ballsinyourjawballsniggerfart", &Vars::Visuals::OutOfFOVArrowsOutline.m_Var); HelpMarker("16 missed calls");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Arrow length", &Vars::Visuals::ArrowLength.m_Var, 5.f, 50.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How long the arrows are");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Arrow angle", &Vars::Visuals::ArrowAngle.m_Var, 5.f, 180.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("The angle of the arrow");
								//ImGui::PushItemWidth(100); ImGui::SliderFloat("Arrow range", &Vars::Visuals::ScreenRange.m_Var, 1.1f, 4.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How far on the screen the arrows will go");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Max distance", &Vars::Visuals::MaxDist.m_Var, 0.f, 4000.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How far until the arrows will not show");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Min distance", &Vars::Visuals::MinDist.m_Var, 0.f, 1000.f, "%.2f"); ImGui::PopItemWidth(); HelpMarker("How close until the arrows will be fully opaque");
							}

							if (ImGui::CollapsingHeader("Spy warning")) {
								ImGui::Checkbox("Active###spywarn", &Vars::Visuals::SpyWarning.m_Var); HelpMarker("Will alert you when spies with their knife out may attempt to backstab you");
								ImGui::Checkbox("Voice command###spywarn1", &Vars::Visuals::SpyWarningAnnounce.m_Var); HelpMarker("Will make your character say \"Spy!\" when a spy is detected");
								ImGui::Checkbox("Visible only###spywarn2", &Vars::Visuals::SpyWarningVisibleOnly.m_Var); HelpMarker("Will only alert you to visible spies");
								ImGui::Checkbox("Ignore friends###spywarn3", &Vars::Visuals::SpyWarningIgnoreFriends.m_Var); HelpMarker("Will ignore spies who are on your friends list");
								const char* spyWmodes[]{ "Arrow", "Flash" }; ImGui::PushItemWidth(100); ImGui::Combo("Warning style", &Vars::Visuals::SpyWarningStyle.m_Var, spyWmodes, IM_ARRAYSIZE(spyWmodes)); ImGui::PopItemWidth();
							}

							if (ImGui::CollapsingHeader("Discord")) {
								ImGui::Checkbox("Enable Discord RPC", &Vars::Misc::Discord::EnableRPC.m_Var); HelpMarker("Enable Discord Rich Presence");
								ImGui::Checkbox("Include map", &Vars::Misc::Discord::IncludeMap.m_Var); HelpMarker("Should Discord Rich Presence contain current map name?");
								ImGui::Checkbox("Include class", &Vars::Misc::Discord::IncludeClass.m_Var); HelpMarker("Should Discord Rich Presence contain current class?");
								ImGui::Checkbox("Include timestamp", &Vars::Misc::Discord::IncludeTimestamp.m_Var); HelpMarker("Should time since you started playing TF2 be included?");
								const char* imagething[]{ "Big fedora, small TF2", "Small TF2, big fedora" }; ImGui::PushItemWidth(100); ImGui::Combo("Image Options", &Vars::Misc::Discord::WhatImagesShouldBeUsed.m_Var, imagething, IM_ARRAYSIZE(imagething)); ImGui::PopItemWidth(); HelpMarker("Which image should be big and which small");
							}

							if (ImGui::CollapsingHeader("Steam")) {
								ImGui::Checkbox("Enable Steam RPC", &Vars::Misc::Steam::EnableRPC.m_Var); HelpMarker("Enable Steam Rich Presence");
								const char* matchGroups[]{ "Special Event", "MvM Mann Up", "Competitive", "Casual", "MvM Boot Camp" }; ImGui::PushItemWidth(150); ImGui::Combo("Match group", &Vars::Misc::Steam::MatchGroup.m_Var, matchGroups, IM_ARRAYSIZE(matchGroups)); ImGui::PopItemWidth(); HelpMarker("Which match group should be used?");
								ImGui::Checkbox("Override in menu", &Vars::Misc::Steam::OverrideMenu.m_Var); HelpMarker("Override match group to \"Main Menu\" when in main menu");
								const char* mapTexts[]{ "Fedoraware", "Figoraware", "Meowhook.club", "Rathook.cc", "Nitro.tf", "custom" }; ImGui::PushItemWidth(150); ImGui::Combo("Map text", &Vars::Misc::Steam::MapText.m_Var, mapTexts, IM_ARRAYSIZE(mapTexts)); ImGui::PopItemWidth(); HelpMarker("Which map text should be used?");
								ImGui::PushItemWidth(100); ImGui::InputText("Custom map text", &Vars::Misc::Steam::CustomText); ImGui::PopItemWidth(); HelpMarker("For when \"custom\" is selcted in \"Map text\". Sets custom map text.");
								ImGui::InputInt("Group size", &Vars::Misc::Steam::GroupSize.m_Var); HelpMarker("Sets party size");
							}
						}
						ImGui::NextColumn();
						{
							if (ImGui::CollapsingHeader("Tickbase exploits", ImGuiTreeNodeFlags_DefaultOpen)) {
								ImGui::Checkbox("Active", &Vars::Misc::CL_Move::Enabled.m_Var); HelpMarker("Tickbase exploit master switch");
								InputKeybind("Recharge key", Vars::Misc::CL_Move::RechargeKey); HelpMarker("Recharges ticks for shifting");
								InputKeybind("Teleport key", Vars::Misc::CL_Move::TeleportKey); HelpMarker("Shifts ticks to move fast");
								InputKeybind("Doubletap key", Vars::Misc::CL_Move::DoubletapKey); HelpMarker("Shifts ticks when shooting for a rapid-fire effect");
								ImGui::Checkbox("Recharge While Dead", &Vars::Misc::CL_Move::RechargeWhileDead.m_Var); HelpMarker("Recharge your DoubleTap bar while you're dead");
								ImGui::Checkbox("AutoRecharge", &Vars::Misc::CL_Move::AutoRecharge.m_Var); HelpMarker("Recharge your DoubleTap bar if you are doing nothing");
								ImGui::Checkbox("Wait for DT", &Vars::Misc::CL_Move::WaitForDT.m_Var); HelpMarker("While the doubletap key is held and ticks are fully charged, it will wait until doubletap is ready to shoot");
								ImGui::Checkbox("Anti Warp", &Vars::Misc::CL_Move::AntiWarp.m_Var); HelpMarker("Will prevent teleporting when using doubletap");
								ImGui::Checkbox("Don't DT in air", &Vars::Misc::CL_Move::NotInAir.m_Var); HelpMarker("When enabled, doubletap will not work if you are mid-air as to avoid movement being uncontrollable");
								const char* dtModes[]{ "On key", "Always", "Disable on key", "Disabled" }; ImGui::PushItemWidth(100); ImGui::Combo("DT Mode", &Vars::Misc::CL_Move::DTMode.m_Var, dtModes, IM_ARRAYSIZE(dtModes)); ImGui::PopItemWidth(); HelpMarker("How should DT behave");
								const char* dtBarStyles[]{ "Off", "Default", "Nitro", "Rijin" }; ImGui::PushItemWidth(100); ImGui::Combo("DT indicator style", &Vars::Misc::CL_Move::DTBarStyle.m_Var, dtBarStyles, IM_ARRAYSIZE(dtBarStyles)); ImGui::PopItemWidth(); HelpMarker("Which style to do the doubletap bar style");
								ImGui::PushItemWidth(100); ImGui::SliderInt("Ticks to shift", &Vars::Misc::CL_Move::DTTicks.m_Var, 1, 24, "%d"); ImGui::PopItemWidth(); HelpMarker("How many ticks to shift");
								if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 1) {
									ImGui::SliderInt("Dt bar height", &Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var, 1, 30);
									ImGui::SliderInt("Dt bar width", &Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var, 1, 30);
								}

							}
							if (ImGui::CollapsingHeader("Attribute changer")) {
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
								ImGui::PushItemWidth(100);
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
								ImGui::PushItemWidth(100);
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
								ImGui::PushItemWidth(100);
								ImGui::Combo("Sheen", &Vars::Visuals::Skins::Sheen.m_Var, sheens, IM_ARRAYSIZE(sheens));
								ImGui::PopItemWidth();
								HelpMarker("Which sheen to apply to the weapon");
								ImGui::Checkbox("Style override", &Vars::Visuals::Skins::Override.m_Var);

								if (ImGui::Button("Set current")) {
									g_AttributeChanger.m_bSet = true;
								}
								if (ImGui::Button("Save all")) {
									g_AttributeChanger.m_bSave = true;
								}
								if (ImGui::Button("Load all")) {
									g_AttributeChanger.m_bLoad = true;
								}

							}

							if (ImGui::CollapsingHeader(_("Body part scales"))) {
								ImGui::Checkbox(_("Body part scales switch"), &Vars::ESP::Players::Funnybodypartslol.m_Var);
								ImGui::PushItemWidth(100); ImGui::SliderFloat(_("Head scale"), &Vars::ESP::Players::Headscale.m_Var, -180.f, 180.f, _("%.0f"), 1.0f); ImGui::PopItemWidth(); HelpMarker(_("How many degrees the aimbot's FoV will have"));
								ImGui::PushItemWidth(100); ImGui::SliderFloat(_("Body scale"), &Vars::ESP::Players::Torsoscale.m_Var, -180.0f, 180.f, _("%.0f"), 1.0f); ImGui::PopItemWidth(); HelpMarker(_("How many degrees the aimbot's FoV will have"));
								ImGui::PushItemWidth(100); ImGui::SliderFloat(_("Hand scale"), &Vars::ESP::Players::Handscale.m_Var, -180.0f, 180.f, _("%.0f"), 1.0f); ImGui::PopItemWidth(); HelpMarker(_("How many degrees the aimbot's FoV will have"));
							}
						}
						ImGui::NextColumn();
						{
							if (ImGui::CollapsingHeader("HvH", ImGuiTreeNodeFlags_DefaultOpen)) {
								ImGui::Checkbox("Anti-aim", &Vars::AntiHack::AntiAim::Active.m_Var); HelpMarker("Anti-aim master switch");
								const char* pitch[]{ "None", "Up", "Down", "Fake up", "Fake down", "Random" }; ImGui::PushItemWidth(100); ImGui::Combo("Pitch", &Vars::AntiHack::AntiAim::Pitch.m_Var, pitch, IM_ARRAYSIZE(pitch)); ImGui::PopItemWidth(); HelpMarker("Which way to look up/down");
								const char* realYaw[]{ "None", "Left", "Right", "Backwards", "Random", "Spin", "Edge"}; ImGui::PushItemWidth(100); ImGui::Combo("Real yaw", &Vars::AntiHack::AntiAim::YawReal.m_Var, realYaw, IM_ARRAYSIZE(realYaw)); ImGui::PopItemWidth(); HelpMarker("Which way to look horizontally");
								const char* fakeYaw[]{ "None", "Left", "Right", "Backwards", "Random", "Spin", "Edge"}; ImGui::PushItemWidth(100); ImGui::Combo("Fake yaw", &Vars::AntiHack::AntiAim::YawFake.m_Var, fakeYaw, IM_ARRAYSIZE(fakeYaw)); ImGui::PopItemWidth(); HelpMarker("Which way to appear to look horizontally");
								ImGui::PushItemWidth(100); ImGui::SliderFloat("Spin Speed", &Vars::AntiHack::AntiAim::SpinSpeed.m_Var, -30.f, 30.f, "%.1f", 0); ImGui::PopItemWidth(); HelpMarker("You spin me right 'round, baby, right 'round");
								ImGui::Checkbox("Resolver", &Vars::AntiHack::Resolver::Resolver.m_Var); HelpMarker("Enables AntiAim resolver in the playerlist");
								ImGui::Checkbox("Fakelag", &Vars::Misc::CL_Move::Fakelag.m_Var); HelpMarker("Fakelag master switch");
								ImGui::PushItemWidth(100); ImGui::SliderInt("Fakelag value", &Vars::Misc::CL_Move::FakelagValue.m_Var, 1, 22, "%d"); ImGui::PopItemWidth(); HelpMarker("How much lag you should fake(?)");
								ImGui::Checkbox("Fakelag Indicator", &Vars::Misc::CL_Move::FakelagIndicator.m_Var); HelpMarker("Shows your fakelag position in thirdperson");
								ImGui::Checkbox("Fakelag on key", &Vars::Misc::CL_Move::FakelagOnKey.m_Var); HelpMarker("Fakelag will only activate when an assigned key is held");
								InputKeybind("Fakelag key", Vars::Misc::CL_Move::FakelagKey); HelpMarker("Fakelag will only activate when this key is held");
							}

							if (ImGui::CollapsingHeader("Radar")) {
								ImGui::Checkbox("Active###radar", &Vars::Radar::Main::Active.m_Var); HelpMarker("Will show nearby things relative to your player");
								ImGui::PushItemWidth(100); ImGui::SliderInt("Radar size", &Vars::Radar::Main::Size.m_Var, 20, 200); ImGui::PopItemWidth(); HelpMarker("The size of the radar window");
								ImGui::PushItemWidth(100); ImGui::SliderInt("Radar range", &Vars::Radar::Main::Range.m_Var, 50, 3000, "%d"); ImGui::PopItemWidth(); HelpMarker("The range of the radar");
								ImGui::PushItemWidth(100); ImGui::SliderInt("Radar background alpha", &Vars::Radar::Main::BackAlpha.m_Var, 0, 255, "%d"); ImGui::PopItemWidth(); HelpMarker("The background alpha of the radar");
								ImGui::PushItemWidth(100); ImGui::SliderInt("Radar lines alpha", &Vars::Radar::Main::LineAlpha.m_Var, 0, 255, "%d"); ImGui::PopItemWidth(); HelpMarker("The line alpha of the radar");
								if (ImGui::CollapsingHeader("Players###radarplayers")) {
									ImGui::Checkbox("Active###radarp", &Vars::Radar::Players::Active.m_Var); HelpMarker("Will show players on the radar");
									const char* iconPlayersRadar[]{ "Scoreboard", "Portraits", "Avatar" }; ImGui::PushItemWidth(100); ImGui::Combo("Icon###radari", &Vars::Radar::Players::IconType.m_Var, iconPlayersRadar, IM_ARRAYSIZE(iconPlayersRadar)); ImGui::PopItemWidth(); HelpMarker("What sort of icon to represent players with");
									const char* backgroundpradar[]{ "Off", "Rectangle", "Texture" }; ImGui::PushItemWidth(100); ImGui::Combo("Background###radarb", &Vars::Radar::Players::BackGroundType.m_Var, backgroundpradar, IM_ARRAYSIZE(backgroundpradar)); ImGui::PopItemWidth(); HelpMarker("What sort of background to put on players on the radar");
									ImGui::Checkbox("Outline###radaro", &Vars::Radar::Players::Outline.m_Var); HelpMarker("Will put an outline on players on the radar");
									static const char* ignoreTeammatespRadar[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###radarplayersteam", &Vars::Radar::Players::IgnoreTeam.m_Var, ignoreTeammatespRadar, IM_ARRAYSIZE(ignoreTeammatespRadar)); ImGui::PopItemWidth(); HelpMarker("Which teammates the radar will ignore drawing on");
									static const char* ignoreCloakedpRadar[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore cloaked###radarplayerscloaked", &Vars::Radar::Players::IgnoreCloaked.m_Var, ignoreCloakedpRadar, IM_ARRAYSIZE(ignoreCloakedpRadar)); ImGui::PopItemWidth(); HelpMarker("Which cloaked players the radar will ignore drawing on");
									ImGui::Checkbox("Health bar###radarhealt", &Vars::Radar::Players::Health.m_Var); HelpMarker("Will show players health on the radar");
									ImGui::PushItemWidth(100); ImGui::SliderInt("Icon size###playersizeiconradar", &Vars::Radar::Players::IconSize.m_Var, 12, 30, "%d"); ImGui::PopItemWidth(); HelpMarker("The icon size of players on the radar");
								}

								if (ImGui::CollapsingHeader("Buildings###radarbuildings")) {
									ImGui::Checkbox("Active###radarbuildingsa", &Vars::Radar::Buildings::Active.m_Var); HelpMarker("Will show buildings on the radar");
									ImGui::Checkbox("Outline###radarbuildingsao", &Vars::Radar::Buildings::Outline.m_Var); HelpMarker("Will put an outline on buildings on the radar");
									ImGui::Checkbox("Ignore team###radarbuildingsb", &Vars::Radar::Buildings::IgnoreTeam.m_Var); HelpMarker("Will ignore team buildings when drawing buildings on the radar");
									ImGui::Checkbox("Health bar###radarbuildingsc", &Vars::Radar::Buildings::Health.m_Var); HelpMarker("Will show building health on the radar");
									ImGui::PushItemWidth(100); ImGui::SliderInt("Icon size###buildingsizeiconradar", &Vars::Radar::Buildings::IconSize.m_Var, 12, 30, "%d"); ImGui::PopItemWidth(); HelpMarker("The icon size of buildings on the radar");
								}

								if (ImGui::CollapsingHeader("World###radarworld")) {
									ImGui::Checkbox("Active###radarworldd", &Vars::Radar::World::Active.m_Var); HelpMarker("Will show pickups on the radar");
									ImGui::Checkbox("Health###radarworldda", &Vars::Radar::World::Active.m_Var); HelpMarker("Will show health icons where health packs are");
									ImGui::Checkbox("Ammo###radarworlddb", &Vars::Radar::World::Active.m_Var); HelpMarker("Will show ammo icons where ammo packs are");
									ImGui::PushItemWidth(100); ImGui::SliderInt("Icon size###worldsizeiconradar", &Vars::Radar::World::IconSize.m_Var, 12, 30, "%d"); ImGui::PopItemWidth(); HelpMarker("The icon size of pickups on the radar");

								}

							}
						}



					}
					ImGui::EndChild();
				}
				if (SettingsTab == 4) {
					//Colors
					ImGui::BeginChild("Colours");
					{
						ImGui::Columns(2);
						{
							ImGui::TextDisabled("Cheat colours");
							if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 1) {
								ColorPicker("Dt bar chargin left", Colors::DtChargingLeft);
								ColorPicker("Dt bar chargin right", Colors::DtChargingRight);
								ColorPicker("Dt bar charged left", Colors::DtChargedLeft);
								ColorPicker("Dt bar charged right", Colors::DtChargedRight);
								ColorPicker("Dt bar outline", Colors::DtOutline);
							}
							ColorPicker("Outline ESP", Colors::OutlineESP);
							ColorPicker("Conditions", Colors::Cond);
							ColorPicker("Target", Colors::Target);
							ColorPicker("Invulnerable", Colors::Invuln);
							ColorPicker("Cloaked", Colors::Cloak);
							ColorPicker("Friend", Colors::Friend);
							ColorPicker("Bone matrix color", Colors::bonecolor); HelpMarker("This is for fakelag");
							ColorPicker("Overheal", Colors::Overheal);
							ColorPicker("Health pack", Colors::Health);
							ColorPicker("Ammo pack", Colors::Ammo);
							ColorPicker("Ubercharged", Colors::UberColor);
							if (!Vars::ESP::Main::EnableTeamEnemyColors.m_Var) {
								ColorPicker("RED team", Colors::TeamRed);
								ColorPicker("BLU team", Colors::TeamBlu);
							}
							else {
								ColorPicker("Enemy", Colors::Enemy);
								ColorPicker("Team", Colors::Team);
							}
							ColorPicker("Hand chams", Colors::Hands);
							ColorPicker("Hand glow chams", Colors::HandsOverlay);
							ColorPicker("Weapon chams", Colors::Weapon);
							ColorPicker("Weapon glow chams", Colors::WeaponOverlay);
							ColorPicker("World modulation", Colors::WorldModulation);
							ColorPicker("Sky modulation", Colors::SkyModulation);
							ColorPicker("Static prop modulation", Colors::StaticPropModulation);
							ColorPicker("FoV circle", Colors::FOVCircle);
							ColorPicker("Bones", Colors::Bones);
							ColorPicker("Bullet tracer", Colors::BulletTracer);
							ColorPicker("Fresnel chams base", Colors::FresnelBase);
							ColorPicker("Aimbot prediction", Colors::AimSquareCol);
							ColorPicker("Damage logger background", Colors::DmgLoggerBackground);
							ColorPicker("Damage logger outline", Colors::DmgLoggerOutline);
							ColorPicker("Damage logger text", Colors::DmgLoggerText);
							ColorPicker("Weapon icon", Colors::WeaponIcon);
							ColorPicker("Noscope lines 1", Colors::NoscopeLines1);
							ColorPicker("Noscope lines 2", Colors::NoscopeLines2);

							//ColorPicker("Fresnel chams top", Colors::FresnelTop);
						}
						ImGui::NextColumn();
						{
							if (ImGui::CollapsingHeader("Surface colours")) {
								ImVec4* const colors = ImGui::GetCurrentContext()->Style.Colors;
								ImGui::PushItemWidth(150);
								ColorPicker("Window background", Vars::Menu::Colors::WindowBackground);
								ColorPicker("Title bar", Vars::Menu::Colors::TitleBar);
								ColorPicker("Widget", Vars::Menu::Colors::Widget);
								ColorPicker("Widget active", Vars::Menu::Colors::WidgetActive);
								ColorPicker("Menu outline", Vars::Menu::Colors::OutlineMenu);
								ColorPicker("Text", Vars::Menu::Colors::Text);
								//drawList->AddRect(ImVec2(0, 0), ImVec2(g_ScreenSize.w, g_ScreenSize.h), ImColor(255, 0, 0, 255));
								ImGui::PopItemWidth();
							}

							if (ImGui::CollapsingHeader("ImGui Style")) {
								ImGui::ShowStyleEditor();
							}
						}
					}
					ImGui::EndChild();
				}


				if (ImGui::BeginMainMenuBar())
				{
					if (ImGui::BeginMenu("Shortcuts"))
					{
						if (ImGui::MenuItem("Full Update"))
							g_Interfaces.Engine->ClientCmd_Unrestricted("cl_fullupdate");
						if(ImGui::MenuItem("Reload HUD"))
							g_Interfaces.Engine->ClientCmd_Unrestricted("hud_reloadscheme");
						if (ImGui::MenuItem("Restart sound system"))
							g_Interfaces.Engine->ClientCmd_Unrestricted("snd_restart");
						if (ImGui::MenuItem("Stop sound"))
							g_Interfaces.Engine->ClientCmd_Unrestricted("stopsound");
						if (ImGui::MenuItem("Status"))
							g_Interfaces.Engine->ClientCmd_Unrestricted("status");
						if (ImGui::MenuItem("Ping"))
							g_Interfaces.Engine->ClientCmd_Unrestricted("ping");
						if (ImGui::MenuItem("Retry"))
							g_Interfaces.Engine->ClientCmd_Unrestricted("retry");
						ImGui::Separator();
						if (ImGui::MenuItem("Exit"))
							g_Interfaces.Engine->ClientCmd_Unrestricted("exit");

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem("Playerlist"))
						g_PlayerList.showWindow = !g_PlayerList.showWindow;

					if (ImGui::MenuItem("Fonts"))
					{
						showFonts = !showFonts;
					}

					if (ImGui::BeginMenu("Menus")) {
						if (ImGui::MenuItem("Console"))
							g_Interfaces.Engine->ClientCmd_Unrestricted("showconsole");
						if (ImGui::MenuItem("Demo UI"))
							g_Interfaces.Engine->ClientCmd_Unrestricted("demoui");
						if (ImGui::MenuItem("Demo Trackbar"))
							g_Interfaces.Engine->ClientCmd_Unrestricted("demoui2");
						if (ImGui::MenuItem("Itemtest"))
							g_Interfaces.Engine->ClientCmd_Unrestricted("itemtest");

						ImGui::EndMenu();
					}

					ImGui::EndMainMenuBar();
				}

				if (showFonts) {
					if (ImGui::Begin("Fonts###fontmenu", &showFonts, ImGuiWindowFlags_NoCollapse))
					{
						std::vector<Font_t> fonts;

						static const char* flags[]{ "None", "Italic", "Underline", "Strikeout", "Symbol", "Antialias", "Gaussian", "Rotary", "Dropshadow", "Additive", "Outline", "Custom" };
						static int fontflags[]{ 0x000, 0x001, 0x002, 0x004, 0x008,0x010, 0x020, 0x040, 0x080, 0x100, 0x200, 0x400 };
						//{ dwFont, szName, nTall, nWeight, nFlags}
						if (ImGui::CollapsingHeader("FONT_ESP"))
						{
							HelpMarker("ESP Font");
							ImGui::InputText("Font name###espfontname", &Vars::Fonts::FONT_ESP::szName);
							ImGui::InputInt("Font height###espfontheight", &Vars::Fonts::FONT_ESP::nTall.m_Var);
							ImGui::InputInt("Font weight###espfontweight", &Vars::Fonts::FONT_ESP::nWeight.m_Var); HelpMarker("How bold the font is (full bold is like 800)");
							static bool flagbools[12]{ 0,0,0,0,0,0,0,0,0,0,0,0 };
							static std::string previewValue = "";
							std::vector<std::string> vec;
							if (ImGui::BeginCombo("Font flags###espfonttttttttttt", previewValue.c_str()))
							{
								previewValue = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
								{
									ImGui::Selectable(flags[i], &flagbools[i]);
									if (flagbools[i])
										vec.push_back(flags[i]);
								}
								for (size_t i = 0; i < vec.size(); i++)
								{
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
							for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
							{
								if (flagbools[i]) {
									Vars::Fonts::FONT_ESP::nFlags.m_Var |= fontflags[i];
								}
							}
							HelpMarker(std::to_string(Vars::Fonts::FONT_ESP::nFlags.m_Var).c_str());
						}
						if (ImGui::CollapsingHeader("FONT_ESP_NAME"))
						{
							HelpMarker("ESP Name Font");
							ImGui::InputText("Font name###espfontnamename", &Vars::Fonts::FONT_ESP_NAME::szName);
							ImGui::InputInt("Font height###espfontnameheight", &Vars::Fonts::FONT_ESP_NAME::nTall.m_Var);
							ImGui::InputInt("Font weight###espfontnameweight", &Vars::Fonts::FONT_ESP_NAME::nWeight.m_Var); HelpMarker("How bold the font is (full bold is like 800)");
							static bool flagbools[12]{ 0,0,0,0,0,0,0,0,0,0,0,0 };
							static std::string previewValue = "";
							std::vector<std::string> vec;
							if (ImGui::BeginCombo("Font flags###espfonnamettttttttttt", previewValue.c_str()))
							{
								previewValue = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
								{
									ImGui::Selectable(flags[i], &flagbools[i]);
									if (flagbools[i])
										vec.push_back(flags[i]);
								}
								for (size_t i = 0; i < vec.size(); i++)
								{
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
							for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
							{
								if (flagbools[i]) {
									Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var |= fontflags[i];
								}
							}
							HelpMarker(std::to_string(Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var).c_str());
						}
						if (ImGui::CollapsingHeader("FONT_ESP_COND"))
						{
							HelpMarker("ESP Condition Font");
							ImGui::InputText("Font name###espfontcondname", &Vars::Fonts::FONT_ESP_COND::szName);
							ImGui::InputInt("Font height###espfontcondheight", &Vars::Fonts::FONT_ESP_COND::nTall.m_Var);
							ImGui::InputInt("Font weight###espfontcondweight", &Vars::Fonts::FONT_ESP_COND::nWeight.m_Var); HelpMarker("How bold the font is (full bold is like 800)");
							static bool flagbools[12]{ 0,0,0,0,0,0,0,0,0,0,0,0 };
							static std::string previewValue = "";
							std::vector<std::string> vec;
							if (ImGui::BeginCombo("Font flags###espfoncondttttttttttt", previewValue.c_str()))
							{
								previewValue = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
								{
									ImGui::Selectable(flags[i], &flagbools[i]);
									if (flagbools[i])
										vec.push_back(flags[i]);
								}
								for (size_t i = 0; i < vec.size(); i++)
								{
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
							for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
							{
								if (flagbools[i]) {
									Vars::Fonts::FONT_ESP_COND::nFlags.m_Var |= fontflags[i];
								}
							}
							HelpMarker(std::to_string(Vars::Fonts::FONT_ESP_COND::nFlags.m_Var).c_str());
						}
						if (ImGui::CollapsingHeader("FONT_ESP_PICKUPS"))
						{
							HelpMarker("ESP Pickups Font");
							ImGui::InputText("Font name###espfontpickupsname", &Vars::Fonts::FONT_ESP_PICKUPS::szName);
							ImGui::InputInt("Font height###espfontpickupsheight", &Vars::Fonts::FONT_ESP_PICKUPS::nTall.m_Var);
							ImGui::InputInt("Font weight###espfontpickupsweight", &Vars::Fonts::FONT_ESP_PICKUPS::nWeight.m_Var); HelpMarker("How bold the font is (full bold is like 800)");
							static bool flagbools[12]{ 0,0,0,0,0,0,0,0,0,0,0,0 };
							static std::string previewValue = "";
							std::vector<std::string> vec;
							if (ImGui::BeginCombo("Font flags###espfonpickupsttttttttttt", previewValue.c_str()))
							{
								previewValue = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
								{
									ImGui::Selectable(flags[i], &flagbools[i]);
									if (flagbools[i])
										vec.push_back(flags[i]);
								}
								for (size_t i = 0; i < vec.size(); i++)
								{
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
							for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
							{
								if (flagbools[i]) {
									Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var |= fontflags[i];
								}
							}
							HelpMarker(std::to_string(Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var).c_str());
						}
						if (ImGui::CollapsingHeader("FONT_MENU"))
						{
							HelpMarker("Menu Font");
							ImGui::InputText("Font name###espfontnamenameneby", &Vars::Fonts::FONT_MENU::szName);
							ImGui::InputInt("Font height###espfontnameheightafsdfads", &Vars::Fonts::FONT_MENU::nTall.m_Var);
							ImGui::InputInt("Font weight###espfontnameweightasfdafsd", &Vars::Fonts::FONT_MENU::nWeight.m_Var); HelpMarker("How bold the font is (full bold is like 800)");
							static bool flagbools[12]{ 0,0,0,0,0,0,0,0,0,0,0,0 };
							static std::string previewValue = "";
							std::vector<std::string> vec;
							if (ImGui::BeginCombo("Font flags###espfonnametttttttttttafsafds", previewValue.c_str()))
							{
								previewValue = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
								{
									ImGui::Selectable(flags[i], &flagbools[i]);
									if (flagbools[i])
										vec.push_back(flags[i]);
								}
								for (size_t i = 0; i < vec.size(); i++)
								{
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
							for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
							{
								if (flagbools[i]) {
									Vars::Fonts::FONT_MENU::nFlags.m_Var |= fontflags[i];
								}
							}
							HelpMarker(std::to_string(Vars::Fonts::FONT_MENU::nFlags.m_Var).c_str());
						}

						if (ImGui::Button("Apply settings###fontapply"))
						{
							Font_t fontEsp = { 0x0, Vars::Fonts::FONT_ESP::szName.c_str(), Vars::Fonts::FONT_ESP::nTall.m_Var, Vars::Fonts::FONT_ESP::nWeight.m_Var, Vars::Fonts::FONT_ESP::nFlags.m_Var };
							Font_t fontEspName = { 0x0, Vars::Fonts::FONT_ESP_NAME::szName.c_str(), Vars::Fonts::FONT_ESP_NAME::nTall.m_Var, Vars::Fonts::FONT_ESP_NAME::nWeight.m_Var, Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var };
							Font_t fontEspCond = { 0x0, Vars::Fonts::FONT_ESP_COND::szName.c_str(), Vars::Fonts::FONT_ESP_COND::nTall.m_Var, Vars::Fonts::FONT_ESP_COND::nWeight.m_Var, Vars::Fonts::FONT_ESP_COND::nFlags.m_Var };
							Font_t fontEspPickups = { 0x0, Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(), Vars::Fonts::FONT_ESP_PICKUPS::nTall.m_Var, Vars::Fonts::FONT_ESP_PICKUPS::nWeight.m_Var, Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var };
							Font_t fontMenu = { 0x0, Vars::Fonts::FONT_MENU::szName.c_str(), Vars::Fonts::FONT_MENU::nTall.m_Var, Vars::Fonts::FONT_MENU::nWeight.m_Var, Vars::Fonts::FONT_MENU::nFlags.m_Var };
							std::vector<Font_t> fonts = { fontEsp, fontEspName, fontEspCond, fontEspPickups, fontMenu };
							g_Draw.RemakeFonts(fonts);
						}

						ImGui::End();
					}
				}

				if (g_PlayerList.showWindow)
				{
					g_PlayerList.Render();
				}
			}
			ImGui::End();
		}
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
}
