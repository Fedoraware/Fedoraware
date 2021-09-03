#include "What.h"
#include "../Vars.h"
#include "ImGui/imgui_internal.h"
#include "../Menu/InputHelper/InputHelper.h"

ImFont* g_pImFontDefaultFont = nullptr;
ImFont* g_pImFontChineseFont = nullptr;

static void HelpMarker(const char* desc)
{
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

bool InputKeybind(const char * label, CVar<int>& output, bool bAllowNone = true)
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
				if ((n > 0x0 && n < 0x7) || (n > L'A' - 1 && n < L'Z' + 1) || (n > L'0' - 1 && n < L'9' + 1) || n == VK_LSHIFT || n == VK_RSHIFT || n == VK_SHIFT || n == VK_ESCAPE || n == VK_HOME || n == VK_CONTROL || n == VK_MENU ) {
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
	else if (ImGui::Button(VK2STR(output.m_Var), ImVec2(100,17))) {
		ImGui::SetActiveID(id, ImGui::GetCurrentWindow());
	}

	ImGui::PopID();

	return true;
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
		(byte)(color.x * 255.0f),
		(byte)(color.y * 255.0f),
		(byte)(color.z * 255.0f),
		(byte)(color.w * 255.0f)
	};
}

void CWhat::Render(IDirect3DDevice9* pDevice) {
	static bool bInitImGui = false;

	if (!bInitImGui) {
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(FindWindowA(0, "Team Fortress 2"));
		ImGui_ImplDX9_Init(pDevice);

		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.90f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.93f, 0.65f, 0.14f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.21f, 0.21f, 0.21f, 0.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.91f, 0.64f, 0.13f, 0.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.78f, 0.55f, 0.21f, 0.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		style->FramePadding = ImVec2(4, 2);
		style->ItemSpacing = ImVec2(10, 2);
		style->IndentSpacing = 12;
		style->ScrollbarSize = 10;

		style->WindowRounding = 4;
		style->FrameRounding = 4;
		style->PopupRounding = 4;
		style->ScrollbarRounding = 6;
		style->GrabRounding = 4;
		style->TabRounding = 4;

		style->WindowTitleAlign = ImVec2(1.0f, 0.5f);
		style->WindowMenuButtonPosition = ImGuiDir_Right;

		style->DisplaySafeAreaPadding = ImVec2(4, 4);

		/*
		colors[ImGuiCol_Text] = mColor(Vars::Menu::Colors::Text); //ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
		colors[ImGuiCol_TextDisabled] = mColor(Vars::Menu::Colors::Text);//ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
		colors[ImGuiCol_WindowBg] = mColor(Vars::Menu::Colors::WindowBackground);//ImVec4(0.11f, 0.15f, 0.17f, 0.80f);
		colors[ImGuiCol_ChildBg] = mColor(Vars::Menu::Colors::WindowBackground); // ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
		colors[ImGuiCol_PopupBg] = mColor(Vars::Menu::Colors::WindowBackground); // ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = mColor(Vars::Menu::Colors::OutlineMenu);//ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);*/

		

		bInitImGui = true;
	}

	if (GetAsyncKeyState(VK_INSERT) & 1)
		g_Interfaces.Surface->SetCursorAlwaysVisible(menuOpen = !menuOpen);

	if (menuOpen)
	{


		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.00f));
		if (ImGui::Begin("Fedoraware", nullptr, ImGuiWindowFlags_NoCollapse))
		{
			ImGui::PopStyleColor();
			ImGui::SetWindowSize(ImVec2(870, 540), ImGuiCond_Once);

			ImGui::Columns(2);
			ImGui::SetColumnOffset(1, 140);
			{
				static ImVec4 active = to_vec4(30, 30, 30, 255);
				static ImVec4 inactive = to_vec4(30, 30, 30, 255);
				ImGui::PushStyleColor(ImGuiCol_Button, SettingsTab == 0 ? active : inactive);
				if (ImGui::Button("Aimbot", ImVec2(140 - 15, 29))) {
					SettingsTab = 0;
				}
				ImGui::PopStyleColor();
				ImGui::Spacing();
				ImGui::PushStyleColor(ImGuiCol_Button, SettingsTab == 0 ? active : inactive);
				if (ImGui::Button("Triggerbot", ImVec2(140 - 15, 29))) {
					SettingsTab = 1;
				}
				ImGui::PopStyleColor();
				ImGui::Spacing();
				ImGui::PushStyleColor(ImGuiCol_Button, SettingsTab == 0 ? active : inactive);
				if (ImGui::Button("Visuals", ImVec2(140 - 15, 29))) {
					SettingsTab = 2;
				}
				ImGui::PopStyleColor();
				ImGui::Spacing();
				ImGui::PushStyleColor(ImGuiCol_Button, SettingsTab == 0 ? active : inactive);
				if (ImGui::Button("Misc", ImVec2(140 - 15, 29))) {
					SettingsTab = 3;
				}
				ImGui::PopStyleColor();
				ImGui::Spacing();
				ImGui::PushStyleColor(ImGuiCol_Button, SettingsTab == 0 ? active : inactive);
				if (ImGui::Button("Config", ImVec2(140 - 15, 29))) {
					SettingsTab = 4;
				}
				ImGui::PopStyleColor();
				ImGui::Spacing();
			}

			ImGui::NextColumn();
			{
				if (SettingsTab == 0) {
					ImGui::BeginChild("Global");
					{
						
						ImGui::Columns(3);
						{
							ImGui::SetColumnWidth(0, 210);
							ImGui::TextUnformatted("Global");
							ImGui::Checkbox("Aimbot###Aim bot", &Vars::Aimbot::Global::Active.m_Var); HelpMarker("Aimbot master switch");
							ImGui::Checkbox("Autoshoot", &Vars::Aimbot::Global::AutoShoot.m_Var); HelpMarker("Automatically shoot when a target is found");
							InputKeybind("Aimkey", Vars::Aimbot::Global::AimKey); HelpMarker("The key to enable aimbot");
							ImGui::Checkbox("Aim at players", &Vars::Aimbot::Global::AimPlayers.m_Var); HelpMarker("The aimbot will aim at players");
							ImGui::Checkbox("Aim at buildings", &Vars::Aimbot::Global::AimBuildings.m_Var); HelpMarker("The aimbot will aim at buildings");
							ImGui::Checkbox("Ignore invulnerable", &Vars::Aimbot::Global::IgnoreInvlunerable.m_Var); HelpMarker("The aimbot will ignore players who can't be damaged");
							ImGui::Checkbox("Ignore cloaked", &Vars::Aimbot::Global::IgnoreCloaked.m_Var); HelpMarker("The aimbot will ignore spies who are cloaked");
							ImGui::Checkbox("Ignore friends", &Vars::Aimbot::Global::IgnoreFriends.m_Var); HelpMarker("The aimbot will ignore steam friends");
							ImGui::Checkbox("Ignore taunting", &Vars::Aimbot::Global::IgnoreTaunting.m_Var); HelpMarker("The aimbot will ignore taunting players");
						}
						ImGui::NextColumn();
						{
							ImGui::SetColumnWidth(1, 250);
							ImGui::TextUnformatted("Hitscan");
							ImGui::Checkbox("Active###Hit scan", &Vars::Aimbot::Hitscan::Active.m_Var); HelpMarker("Hitscan aimbot master switch");
							static const char* hitscanSortMethod[]{ "FoV", "Distance" }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###hitscanSortMethod", &Vars::Aimbot::Hitscan::SortMethod.m_Var, hitscanSortMethod, IM_ARRAYSIZE(hitscanSortMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to decide which target to aim at");
							static const char* hitscanAimMethod[]{ "Plain", "Smooth", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###hitscanAimMethod", &Vars::Aimbot::Hitscan::AimMethod.m_Var, hitscanAimMethod, IM_ARRAYSIZE(hitscanAimMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to aim at the target");
							static const char* hitscanAimHitbox[]{ "Head", "Body", "Auto" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim position###hitscanAimPosition", &Vars::Aimbot::Hitscan::AimHitbox.m_Var, hitscanAimHitbox, IM_ARRAYSIZE(hitscanAimHitbox)); ImGui::PopItemWidth(); HelpMarker("Which hitbox the aimbot will target");
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Aimbot FoV", &Vars::Aimbot::Hitscan::AimFOV.m_Var, 1.0f, 180.f, "%.0f", 1.0f); ImGui::PopItemWidth(); HelpMarker("How many degrees the aimbot's FoV will have");
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
							ImGui::Checkbox("Doubletap", &Vars::Misc::CL_Move::Doubletap.m_Var); HelpMarker("When enough ticks are choked, the aimbot will shoot them all at once in a burst, leading to a rapid-fire effect");
						}
						ImGui::NextColumn();
						{
							ImGui::TextUnformatted("Projectile");
							ImGui::Checkbox("Active###Project ile", &Vars::Aimbot::Projectile::Active.m_Var); HelpMarker("Projectile aimbot master switch");
							ImGui::Checkbox("Performance mode", &Vars::Aimbot::Projectile::PerformanceMode.m_Var); HelpMarker("Only taget enemy closest to the crosshair");
							static const char* projectileSortMethod[]{ "FoV", "Distance" }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###projectileSortMethod", &Vars::Aimbot::Projectile::SortMethod.m_Var, projectileSortMethod, IM_ARRAYSIZE(projectileSortMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to decide which target to aim at");
							static const char* projectileAimMethod[]{ "Plain", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###projectileAimMethod", &Vars::Aimbot::Projectile::AimMethod.m_Var, projectileAimMethod, IM_ARRAYSIZE(projectileAimMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to aim at the target");
							static const char* projectileAimHitbox[]{ "Body", "Feet", "Auto" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim position###projectileAimPosition", &Vars::Aimbot::Projectile::AimPosition.m_Var, projectileAimHitbox, IM_ARRAYSIZE(projectileAimHitbox)); ImGui::PopItemWidth(); HelpMarker("Which hitbox the aimbot will target");
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Aim FoV###projectileFoV", &Vars::Aimbot::Projectile::AimFOV.m_Var, 1.0f, 180.f, "%.0f", 1.0f); ImGui::PopItemWidth(); HelpMarker("How many degrees the aimbot's FoV will have");
							ImGui::Separator();
							ImGui::TextUnformatted("");
							ImGui::TextUnformatted("");
							ImGui::TextUnformatted("Melee");
							ImGui::Checkbox("Active###Mele e", &Vars::Aimbot::Melee::Active.m_Var); HelpMarker("Melee aimbot master switch");
							static const char* meleeSortMethod[]{ "FoV", "Distance" }; ImGui::PushItemWidth(100); ImGui::Combo("Sort method###meleeSortMethod", &Vars::Aimbot::Melee::SortMethod.m_Var, meleeSortMethod, IM_ARRAYSIZE(meleeSortMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to decide which target to aim at");
							static const char* meleeAimMethod[]{ "Plain", "Smooth", "Silent" }; ImGui::PushItemWidth(100); ImGui::Combo("Aim method###meleeAimMethod", &Vars::Aimbot::Melee::AimMethod.m_Var, meleeAimMethod, IM_ARRAYSIZE(meleeAimMethod)); ImGui::PopItemWidth(); HelpMarker("Which method the aimbot uses to aim at the target");
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Aim FoV###meleeFoV", &Vars::Aimbot::Melee::AimFOV.m_Var, 1.0f, 180.f, "%.0f", 1.0f); ImGui::PopItemWidth(); HelpMarker("How many degrees the aimbot's FoV will have");
							ImGui::PushItemWidth(100); ImGui::SliderFloat("Smooth factor###meleeSmooth", &Vars::Aimbot::Melee::SmoothingAmount.m_Var, 1.0f, 10.f, "%.0f", 1.0f); ImGui::PopItemWidth(); HelpMarker("How many degrees the aimbot's FoV will have");
							ImGui::Checkbox("Range check", &Vars::Aimbot::Melee::RangeCheck.m_Var); HelpMarker("Only aim at target if within melee range");
							ImGui::Checkbox("Melee prediction", &Vars::Aimbot::Melee::PredictSwing.m_Var); HelpMarker("Aimbot will attack preemptively, predicting you will be in range of the target");
							ImGui::Checkbox("Whip teammates", &Vars::Aimbot::Melee::WhipTeam.m_Var); HelpMarker("Aimbot will target teammates if holding the Disciplinary Action");
						}
					}
					ImGui::EndChild();

				}
				if (SettingsTab == 1) {

				}
				if (SettingsTab == 2) {

				}
				if (SettingsTab == 3) {

				}
				if (SettingsTab == 4) {

				}
			}
			ImGui::End();
		}
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
}