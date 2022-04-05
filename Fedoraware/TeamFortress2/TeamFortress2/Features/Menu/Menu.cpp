#include "Menu.h"
#include "../Vars.h"
#include "../Camera/CameraWindow.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_color_gradient.h"
#include "Components.hpp"

#define SLIDER(label, var, min, max, format, flagspower) \
ImGui::PushItemWidth(100); \
ImGui::SliderFloat(label, var, min, max, format, flagspower); \
ImGui::PopItemWidth()

#define TOGGLE(label, v) ImGui::Checkbox(label, v);

constexpr int MENU_KEY = VK_INSERT;
static ImGradient titleGradient;
static ImGradient mainGradient;

// Fonts
ImFont* SegoeLight = nullptr;	// 16px
ImFont* Segoe = nullptr;		// 16px
ImFont* SegoeBold = nullptr;	// 16px

ImFont* SectionFont = nullptr;	// 18px
ImFont* TabFont = nullptr;		// 22px
ImFont* TitleFont = nullptr;	// 26px

#pragma region Components
void SectionTitle(const char* title, int yOffset = 6)
{
	ImGui::Dummy({ 0, yOffset });
	ImGui::PushFont(SectionFont);
	const ImVec2 titleSize = ImGui::CalcTextSize(title);
	ImGui::SameLine((ImGui::GetColumnWidth()) / 2 - (titleSize.x / 2));
	ImGui::Text(title);
	ImGui::PopFont();

	const auto widgetPos = ImGui::GetCursorScreenPos();
	ImGui::GradientRect(&mainGradient, { widgetPos.x, widgetPos.y - 2 }, ImGui::GetColumnWidth(), 3);
}
#pragma endregion

/* The main menu */
void CMenu::DrawMenu()
{
	ImGui::GetStyle().WindowMinSize = ImVec2(800, 500);

	ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_Once);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	if (ImGui::Begin("Fedoraware", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
	{
		const auto drawList = ImGui::GetWindowDrawList();
		const auto windowSize = ImGui::GetWindowSize();
		const auto windowPos = ImGui::GetWindowPos();

		// Gradient line
		ImGui::GradientRect(&titleGradient, { windowPos.x, windowPos.y }, windowSize.x, 3.f);
		ImGui::Dummy({ 0, 2 });

		// Title Text
		{
			ImGui::PushFont(TitleFont);
			const auto titleWidth = ImGui::CalcTextSize("Fedoraware").x;
			drawList->AddText(TitleFont, TitleFont->FontSize, { windowPos.x + (windowSize.x / 2) - (titleWidth / 2), windowPos.y }, Accent, "Fedoraware");
			ImGui::PopFont();
		}

		// Tabbar
		ImGui::SetCursorPos({ 0, TitleHeight });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, BackgroundLight.Value);
		if (TabHeight > 5.f)
		{
			if (ImGui::BeginChild("Tabbar", { windowSize.x + 5, TabHeight }))
			{
				DrawTabbar();
			}
			ImGui::EndChild();
		}
		ImGui::PopStyleColor();

		// Main content
		ImGui::SetCursorPos({ 0, TitleHeight + TabHeight });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 15.f, 10.f });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, BackgroundDark.Value);
		if (ImGui::BeginChild("Content", { windowSize.x, windowSize.y - (TitleHeight + TabHeight) }, false, ImGuiWindowFlags_AlwaysUseWindowPadding))
		{
			ImGui::PushFont(Segoe);
			switch (CurrentTab)
			{
			case MenuTab::Aimbot: { MenuAimbot(); break; }
			case MenuTab::Trigger: { MenuTrigger(); break; }
			case MenuTab::Visuals: { MenuVisuals(); break; }
			case MenuTab::HvH: { MenuHvH(); break; }
			case MenuTab::Misc: { MenuMisc(); break; }
			case MenuTab::Configs: { MenuConfigs(); break; }
			}

			ImGui::PopFont();
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		// End
		ImGui::End();
	}
	ImGui::PopStyleVar();
}

void CMenu::DrawTabbar()
{
	ImGui::PushFont(TabFont);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0, 0 });
	if (ImGui::BeginTable("TabbarTable", 4))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, BackgroundLight.Value);
		ImGui::PushStyleColor(ImGuiCol_Text, TextLight.Value);
		if (ImGui::TabButton("Aimbot", CurrentTab == MenuTab::Aimbot))
		{
			CurrentTab = MenuTab::Aimbot;
		}

		if (ImGui::TabButton("Trigger", CurrentTab == MenuTab::Trigger))
		{
			CurrentTab = MenuTab::Trigger;
		}

		if (ImGui::TabButton("Visuals", CurrentTab == MenuTab::Visuals))
		{
			CurrentTab = MenuTab::Visuals;
		}

		if (ImGui::TabButton("HvH", CurrentTab == MenuTab::HvH))
		{
			CurrentTab = MenuTab::HvH;
		}

		if (ImGui::TabButton("Misc", CurrentTab == MenuTab::Misc))
		{
			CurrentTab = MenuTab::Misc;
		}

		ImGui::PopStyleColor(2);
		ImGui::EndTable();
	}
	ImGui::PopStyleVar(1);
	ImGui::PopFont();
}

/* Tab: Aimbot */
void CMenu::MenuAimbot()
{
	using namespace ImGui;
	if (BeginTable("AimbotTable", 3))
	{
		TableNextColumn();

		// Global Aimbot
		SectionTitle("Global");
		TOGGLE("Aimbot", &Vars::Aimbot::Global::Active.m_Var); HelpMarker("Aimbot master switch");
		ColorPickerL("Aim Target", Colors::Target);
		InputKeybind("Aimbot key", Vars::Aimbot::Global::AimKey); HelpMarker("The key to enable aimbot");
		SLIDER("Aimbot FoV####AimbotFoV", &Vars::Aimbot::Global::AimFOV.m_Var, 0.f, 180.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
		ColorPickerL("Aimbot FOV circle", Colors::FOVCircle);
		TOGGLE("Autoshoot###AimbotAutoshoot", &Vars::Aimbot::Global::AutoShoot.m_Var); HelpMarker("Automatically shoot when a target is found");
		MultiCombo({ "Players", "Buildings" }, { &Vars::Aimbot::Global::AimPlayers.m_Var, &Vars::Aimbot::Global::AimBuildings.m_Var }, "Choose which targets the Aimbot should aim at", "Aim targets");
		MultiCombo({ "Invulnerable", "Cloaked", "Friends", "Taunting" }, { &Vars::Aimbot::Global::IgnoreInvlunerable.m_Var, &Vars::Aimbot::Global::IgnoreCloaked.m_Var, &Vars::Aimbot::Global::IgnoreFriends.m_Var, &Vars::Aimbot::Global::IgnoreTaunting.m_Var }, "Choose which targets should be ignored", "Ignored targets###HitscanIgnoredTargets");
		ColorPickerL("Invulnerable colour", Colors::Invuln);

		// Crithack
		SectionTitle("Crithack", 12);
		TOGGLE("Crit hack", &Vars::CritHack::Active.m_Var); HelpMarker("Enables the crit hack (BETA)");
		//MultiCombo({ "Indicators", "Avoid Random" }, { &Vars::CritHack::indicators.m_Var, &Vars::CritHack::avoidrandom.m_Var }, "Misc options for crithack", "Misc###CrithackMiscOptions");
		InputKeybind("Crit key", Vars::CritHack::CritKey); HelpMarker("Will try to force crits when the key is held");

		// Backtrack
		SectionTitle("Backtrack", 12);
		TOGGLE("Active", &Vars::Backtrack::Enabled.m_Var); HelpMarker("If you shoot at the backtrack manually it will attempt to hit it");
		TOGGLE("Aimbot aims last tick", &Vars::Backtrack::Aim.m_Var); HelpMarker("Aimbot aims at the last tick if visible");

		TableNextColumn();

		TableNextColumn();

		EndTable();
	}
}

/* Tab: Trigger */
void CMenu::MenuTrigger()
{
	
}

/* Tab: Visuals */
void CMenu::MenuVisuals()
{

}

/* Tab: HvH */
void CMenu::MenuHvH()
{

}

/* Tab: Misc */
void CMenu::MenuMisc()
{

}

/* Tab: Configs */
void CMenu::MenuConfigs()
{

}

/* Window for the camera feature */
void CMenu::DrawCameraWindow()
{
	if (g_Interfaces.Engine->IsInGame() && Vars::Visuals::CameraMode.m_Var != 0)
	{
		int windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;
		if (Vars::Visuals::CameraMode.m_Var <= 1 || g_CameraWindow.CanDraw) {
			windowFlags |= ImGuiWindowFlags_NoBackground;
		}

		// Draw the camera window
		ImGui::SetNextWindowSize({ static_cast<float>(g_CameraWindow.ViewRect.w), static_cast<float>(g_CameraWindow.ViewRect.h) }, ImGuiCond_Once);
		ImGui::SetNextWindowPos({ static_cast<float>(g_CameraWindow.ViewRect.x), static_cast<float>(g_CameraWindow.ViewRect.y) }, ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 60.f, 60.f });
		ImGui::PushStyleColor(ImGuiCol_Text, TextDark.Value);
		if (ImGui::Begin("Camera", nullptr, windowFlags))
		{
			const ImVec2 winPos = ImGui::GetWindowPos();
			const ImVec2 winSize = ImGui::GetWindowSize();

			g_CameraWindow.ViewRect.x = static_cast<int>(winPos.x);
			g_CameraWindow.ViewRect.y = static_cast<int>(winPos.y);
			g_CameraWindow.ViewRect.w = static_cast<int>(winSize.x);
			g_CameraWindow.ViewRect.h = static_cast<int>(winSize.y);

			ImGui::End();
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
}

void CMenu::Render(IDirect3DDevice9* pDevice)
{
	static bool initialized = false;
	if (!ConfigLoaded) { return; }

	if (!initialized)
	{
		Init(pDevice);
		initialized = true;
	}

	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

	// Toggle menu
	if (GetAsyncKeyState(MENU_KEY) & 1)
	{
		// TOOD: & 8000?
		g_Menu.IsOpen = !g_Menu.IsOpen;
		g_Interfaces.Surface->SetCursorAlwaysVisible(g_Menu.IsOpen);
	}

	// Begin current frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (g_Menu.IsOpen)
	{
		ImGui::PushFont(Segoe);
		DrawMenu();
		DrawCameraWindow();

		// TODO: Draw DT-Bar, Playerlist, Spectator list etc.
		ImGui::PopFont();
	}

	// End frame and render
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
}

void CMenu::Init(IDirect3DDevice9* pDevice)
{
	// Initialize ImGui and device
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(FindWindowA(nullptr, "Team Fortress 2"));
	ImGui_ImplDX9_Init(pDevice);

	const auto& io = ImGui::GetIO();
	auto& style = ImGui::GetStyle();

	// Fonts
	{
		auto fontConfig = ImFontConfig();
		fontConfig.OversampleH = 2;

		auto wideFontConfig = ImFontConfig();
		wideFontConfig.GlyphExtraSpacing = { 1.f, 0.f };

		constexpr ImWchar fontRange[]{ 0x0020, 0x00FF,0x0400, 0x044F,0 }; // Basic Latin, Latin Supplement and Cyrillic
		SegoeLight = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuisl.ttf", 16.0f, &fontConfig, fontRange);
		Segoe = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeui.ttf", 16.0f, &fontConfig, fontRange);
		SegoeBold = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuib.ttf", 16.0f, &fontConfig, fontRange);

		SectionFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeui.ttf", 18.0f, &wideFontConfig, fontRange);
		TabFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuisl.ttf", 20.0f, &fontConfig, fontRange);
		TitleFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuib.ttf", 22.0f, &fontConfig, fontRange);

		io.Fonts->Build();
	}

	// Style
	{
		// https://raais.github.io/ImStudio/

		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);	// Center window title
		style.WindowMinSize = ImVec2(700, 700);
		style.WindowPadding = ImVec2(0, 0);
		style.WindowBorderSize = 1.f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.4f);		// Center button text
		// style.CellPadding = ImVec2(12.f, 0.f);
		style.FrameBorderSize = 0.f;
		style.FrameRounding = 0.f;
		style.ChildBorderSize = 0.f;
		style.ChildRounding = 0.f;
		style.ScrollbarSize = 4.f;
		style.GrabMinSize = 15.f;
		style.ItemSpacing = ImVec2(8.f, 5.f);

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Border] = ImColor(110, 110, 128);
		colors[ImGuiCol_WindowBg] = Background;
		colors[ImGuiCol_TitleBg] = BackgroundDark;
		colors[ImGuiCol_TitleBgActive] = Accent;
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.4f);
		colors[ImGuiCol_Button] = BackgroundLight;
		colors[ImGuiCol_ButtonHovered] = ImColor(69, 69, 77);
		colors[ImGuiCol_ButtonActive] = ImColor(82, 79, 87);
		colors[ImGuiCol_PopupBg] = BackgroundDark;
		colors[ImGuiCol_FrameBg] = ImColor(50, 50, 50);
		colors[ImGuiCol_FrameBgHovered] = ImColor(60, 60, 60);
		colors[ImGuiCol_FrameBgActive] = ImColor(60, 60, 60);
		colors[ImGuiCol_CheckMark] = Accent;
		colors[ImGuiCol_Text] = TextLight;

		colors[ImGuiCol_SliderGrab] = Accent;
		colors[ImGuiCol_SliderGrabActive] = AccentDark;
		colors[ImGuiCol_ResizeGrip] = AccentDark;
		colors[ImGuiCol_ResizeGripActive] = Accent;
		colors[ImGuiCol_ResizeGripHovered] = Accent;
		colors[ImGuiCol_HeaderActive] = Accent;
		colors[ImGuiCol_HeaderHovered] = Accent;
		colors[ImGuiCol_Header] = Accent;
	}

	// Misc
	{
		titleGradient.ClearMarks();
		titleGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		titleGradient.AddMark(0.3f, ImColor(0, 0, 0, 0));
		titleGradient.AddMark(0.5f, Accent);
		titleGradient.AddMark(0.7f, ImColor(0, 0, 0, 0));
		titleGradient.AddMark(1.f, ImColor(0, 0, 0, 0));
	}

	{
		mainGradient.ClearMarks();
		mainGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		mainGradient.AddMark(0.15f, ImColor(0, 0, 0, 0));
		mainGradient.AddMark(0.45f, Accent);
		mainGradient.AddMark(0.75f, ImColor(0, 0, 0, 0));
		mainGradient.AddMark(1.f, ImColor(0, 0, 0, 0));
	}
}

#undef SLIDER
#undef TOGGLE