#include "Menu.h"
#include "../Vars.h"
#include "../Camera/CameraWindow.h"
#include "ImGui/imgui_internal.h"

constexpr int MENU_KEY = VK_INSERT;

/* The main menu */
void CMenu::DrawMenu()
{
	ImGui::GetStyle().WindowMinSize = ImVec2(1000, 650);
	
	ImGui::Begin("Fedoraware", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);
	ImGui::PushFont(Segoe);
	{
		// Draw rects
		const auto window = ImGui::GetCurrentWindow();
		const auto drawList = ImGui::GetWindowDrawList();
		const auto windowSize = ImGui::GetWindowSize();
		const auto windowPos = ImGui::GetWindowPos();

		// Border structure
		drawList->AddRectFilled(window->DC.CursorPos, { window->DC.CursorPos.x + 225.f, window->DC.CursorPos.y + windowSize.y }, BackgroundDark);

		drawList->AddRectFilled(window->DC.CursorPos, { window->DC.CursorPos.x + 225.f, window->DC.CursorPos.y + 80.f }, AccentColor);
		drawList->AddRectFilled(window->DC.CursorPos, { window->DC.CursorPos.x + windowSize.x, window->DC.CursorPos.y + 8.f }, AccentColor);
		drawList->AddRectFilled(window->DC.CursorPos, { window->DC.CursorPos.x + 8.f, window->DC.CursorPos.y + windowSize.y }, AccentColor);

		window->DC.CursorPos.x += 32.f;
		window->DC.CursorPos.y += 17.f;

		// Fedoraware label
		ImGui::PushStyleColor(ImGuiCol_Text, BackgroundDark.Value);
		ImGui::PushFont(Title);
		ImGui::Text("Fedora");
		ImGui::PopFont();
		ImGui::SameLine(125.f);
		ImGui::PushFont(TitleLight);
		ImGui::Text("ware");
		ImGui::PopFont();
		ImGui::PopStyleColor();

		// Main content
		ImGui::SetCursorPos({ windowPos.x + 235.f, windowPos.y + 20.f });
	}
	ImGui::PopFont();
	ImGui::End();
}

/* Window for the camera feature */
void CMenu::DrawCameraWindow()
{
	if (g_Interfaces.Engine->IsInGame() && Vars::Visuals::CameraMode.m_Var != 0)
	{
		ImGui::SetNextWindowSize({ static_cast<float>(g_CameraWindow.ViewRect.w), static_cast<float>(g_CameraWindow.ViewRect.h) }, ImGuiCond_Once);
		ImGui::SetNextWindowPos({ static_cast<float>(g_CameraWindow.ViewRect.x), static_cast<float>(g_CameraWindow.ViewRect.y) }, ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 60.f, 60.f });
		{
			int windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;
			if (Vars::Visuals::CameraMode.m_Var <= 1 || g_CameraWindow.CanDraw) {
				windowFlags |= ImGuiWindowFlags_NoBackground;
			}

			// Draw the camera window
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
		}
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
		DrawMenu();
		DrawCameraWindow();

		// TODO: Draw DT-Bar, Playerlist, Spectator list etc.
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

	auto& io = ImGui::GetIO();
	auto& style = ImGui::GetStyle();

	// Fonts
	{
		auto fontConfig = ImFontConfig();
		fontConfig.OversampleH = 1;
		fontConfig.OversampleV = 1;
		fontConfig.PixelSnapH = true;

		const ImWchar TextFontRange[]{ 0x0020, 0x00FF,0x0400, 0x044F,0 }; // Basic Latin, Latin Supplement and Cyrillic
		SegoeLight = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuisl.ttf", 14.0f, &fontConfig, TextFontRange);
		Segoe = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeui.ttf", 14.0f, &fontConfig, TextFontRange);
		SegoeBold = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuib.ttf", 14.0f, &fontConfig, TextFontRange);

		Title = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuib.ttf", 38.0f, &fontConfig, TextFontRange);
		TitleLight = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuisl.ttf", 38.0f, &fontConfig, TextFontRange);

		io.Fonts->Build();
	}

	// Style
	{
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);	// Center window title
		style.WindowMinSize = ImVec2(708, 708);
		style.WindowPadding = ImVec2(0, 0);
		style.WindowBorderSize = 0.f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);		// Center button text
		style.FrameBorderSize = 0.f;
		style.FrameRounding = 0.f;
		style.ScrollbarSize = 3.f;

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_WindowBg] = Background;
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.4f);
	}
}