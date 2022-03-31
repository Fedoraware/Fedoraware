#include "Menu.h"
#include "../Vars.h"
#include "../Camera/CameraWindow.h"

constexpr int MENU_KEY = VK_INSERT;

/* The main menu */
void CMenu::DrawMenu()
{
	ImGui::GetStyle().WindowMinSize = ImVec2(700, 500);
	
	ImGui::Begin("Fedoraware", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	ImGui::PushFont(Segoe);
	{
		// TODO: Draw menu
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

		io.Fonts->Build();
	}

	// Style
	{

	}
}