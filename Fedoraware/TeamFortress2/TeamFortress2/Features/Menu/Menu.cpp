#include "Menu.h"
#include "../Vars.h"
#include "../Camera/CameraWindow.h"
#include "ImGui/imgui_internal.h"
#include "Components.hpp"

constexpr int MENU_KEY = VK_INSERT;

/* The main menu */
void CMenu::DrawMenu()
{
	ImGui::GetStyle().WindowMinSize = ImVec2(800, 500);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_Once);
	if (ImGui::Begin("Fedoraware", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
	{
		// Draw rects
		const auto window = ImGui::GetCurrentWindow();
		const auto drawList = ImGui::GetWindowDrawList();
		const auto windowSize = ImGui::GetWindowSize();
		const auto windowPos = ImGui::GetWindowPos();

		constexpr float sBorder = 8.f;		// Border size (left, top)
		constexpr float wSidebar = 225.f;	// Sidewar width (left)
		constexpr float hTitle = 80.f;		// Titlebox height (top)

		// Border structure
		drawList->AddRectFilled(windowPos, { windowPos.x + wSidebar, windowPos.y + windowSize.y }, BackgroundDark);
		drawList->AddRectFilled(windowPos, { windowPos.x + wSidebar, windowPos.y + hTitle }, Accent);

		drawList->AddRectFilled(windowPos, { windowPos.x + windowSize.x, windowPos.y + sBorder }, Accent);
		drawList->AddRectFilled(windowPos, { windowPos.x + sBorder, windowPos.y + windowSize.y }, Accent);

		// "Fedoraware" label
		drawList->AddText(TitleFont, TitleFont->FontSize, { windowPos.x + 32.f, windowPos.y + 20.f }, BackgroundDark, "Fedora");
		drawList->AddText(TitleLightFont, TitleLightFont->FontSize, { windowPos.x + 125.f, windowPos.y + 20.f }, BackgroundDark, "ware");

		// Sidebar
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 10.f, 20.f });
		ImGui::SetCursorPos({ sBorder, hTitle });
		if (ImGui::BeginChild("Sidebar", { wSidebar - sBorder, windowSize.y - hTitle }, false, ImGuiWindowFlags_AlwaysUseWindowPadding))
		{
			DrawSidebar();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();

		// Main content
		ImGui::SetCursorPos({ wSidebar, sBorder });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 15.f, 15.f });
		if (ImGui::BeginChild("Content", { windowSize.x - wSidebar, windowSize.y - sBorder }, false, ImGuiWindowFlags_AlwaysUseWindowPadding))
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
				default: { MenuAimbot(); break; }
			}
			ImGui::PopFont();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();

		// End
		ImGui::End();
	}
	ImGui::PopStyleVar();
}

/* Draws the menu sidebar */
void CMenu::DrawSidebar()
{
	ImGui::PushFont(TabFont);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f, 10.f });
	ImGui::PushStyleColor(ImGuiCol_Border, ImColor(38, 38, 38).Value);
	{
		if (Cmp::SidebarButton("AIMBOT", CurrentTab == MenuTab::Aimbot))
		{
			CurrentTab = MenuTab::Aimbot;
		}

		if (Cmp::SidebarButton("TRIGGER", CurrentTab == MenuTab::Trigger))
		{
			CurrentTab = MenuTab::Trigger;
		}

		if (Cmp::SidebarButton("VISUALS", CurrentTab == MenuTab::Visuals))
		{
			CurrentTab = MenuTab::Visuals;
		}

		if (Cmp::SidebarButton("HVH", CurrentTab == MenuTab::HvH))
		{
			CurrentTab = MenuTab::HvH;
		}

		if (Cmp::SidebarButton("MISC", CurrentTab == MenuTab::Misc))
		{
			CurrentTab = MenuTab::Misc;
		}

		if (Cmp::SidebarButton("CONFIGS", CurrentTab == MenuTab::Configs))
		{
			CurrentTab = MenuTab::Configs;
		}
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::PopFont();
}

/* Tab: Aimbot */
void CMenu::MenuAimbot()
{
	if (ImGui::BeginTable("c1", 3))
	{
		ImGui::TableNextColumn();

		if (Cmp::BeginContainer("Global"))
		{
			Cmp::Switch("Aimbot", &Vars::Aimbot::Global::Active.m_Var);

			ImGui::SliderFloat("Aimbot FOV", &Vars::Aimbot::Global::AimFOV.m_Var, 1, 100);
		}

		ImGui::TableNextColumn();

		ImGui::TableNextColumn();

		ImGui::EndTable();
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
		ImGui::PushStyleColor(ImGuiCol_Text, BackgroundDark.Value);
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
		//fontConfig.OversampleV = 1;
		//fontConfig.PixelSnapH = true;

		auto wideFontConfig = ImFontConfig();
		wideFontConfig.GlyphExtraSpacing = { 6.f, 0.f };

		constexpr ImWchar fontRange[]{ 0x0020, 0x00FF,0x0400, 0x044F,0 }; // Basic Latin, Latin Supplement and Cyrillic
		SegoeLight = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuisl.ttf", 16.0f, &fontConfig, fontRange);
		Segoe = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeui.ttf", 16.0f, &fontConfig, fontRange);
		SegoeBold = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuib.ttf", 16.0f, &fontConfig, fontRange);

		TabFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuisl.ttf", 26.0f, &wideFontConfig, fontRange);

		TitleFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuib.ttf", 38.0f, &fontConfig, fontRange);
		TitleLightFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuisl.ttf", 38.0f, &fontConfig, fontRange);

		io.Fonts->Build();
	}

	// Style
	{
		// https://raais.github.io/ImStudio/

		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);	// Center window title
		style.WindowMinSize = ImVec2(708, 708);
		style.WindowPadding = ImVec2(0, 0);
		style.WindowBorderSize = 1.f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.4f);		// Center button text
		style.FrameBorderSize = 0.f;
		style.FrameRounding = 0.f;
		style.ChildBorderSize = 0.f;
		style.ChildRounding = 0.f;
		style.ScrollbarSize = 4.f;
		style.GrabMinSize = 14.f;
		style.GrabRounding = 12.f;

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Border] = Accent;
		colors[ImGuiCol_WindowBg] = Background;
		colors[ImGuiCol_TitleBg] = BackgroundDark;
		colors[ImGuiCol_TitleBgActive] = Accent;
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.4f);
		colors[ImGuiCol_Button] = ImColor(0, 0, 0, 255);
		colors[ImGuiCol_ButtonHovered] = Background;
		colors[ImGuiCol_ButtonActive] = ImColor(38, 38, 38);
		colors[ImGuiCol_HeaderActive] = Accent;
		colors[ImGuiCol_HeaderHovered] = Accent;
		colors[ImGuiCol_Header] = AccentDark;
	}
}