#pragma once
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"
#include "../../SDK/SDK.h"

class CMenu {
	void DrawMenu();
	void DrawSidebar();
	void MenuAimbot();
	void MenuTrigger();
	void MenuVisuals();
	void MenuHvH();
	void MenuMisc();
	void MenuConfigs();
	void DrawCameraWindow();

	enum class MenuTab {
		Aimbot,
		Trigger,
		Visuals,
		HvH,
		Misc,
		Configs
	};

	MenuTab CurrentTab = MenuTab::Aimbot;

	// Fonts
	ImFont* SegoeLight = nullptr;	// 16px
	ImFont* Segoe = nullptr;		// 16px
	ImFont* SegoeBold = nullptr;	// 16px

	ImFont* TabFont = nullptr;		// 26px

	ImFont* TitleFont = nullptr;		// 38px
	ImFont* TitleLightFont = nullptr;	// 38px

	float SidebarWidth = 225.f;	// Sidewar width (left)
	float TitleHeight = 80.f;	// Titlebox height (top)
	float BorderWidth = 8.f;	// Border size (left, top)

public:
	void Render(IDirect3DDevice9* pDevice);
	void Init(IDirect3DDevice9* pDevice);

	bool IsOpen = false;
	bool ConfigLoaded = false;
	bool Unload = false;

	// Colors
	ImColor Accent = { 225, 177, 44 };
	ImColor AccentDark = { 191, 150, 38 };
	ImColor BackgroundDark = { 13, 13, 13 };
	ImColor Background = { 23, 23, 23, 250 };
};

inline CMenu g_Menu;