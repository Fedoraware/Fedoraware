#pragma once
#include "ImGui/imgui_impl_dx9.h"
#include "../../SDK/SDK.h"

class CMenu {
	void DrawMenu();
	void DrawSidebar();
	void DrawTabbar();
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

	ImFont* TabFont = nullptr;		// 22px
	ImFont* TitleFont = nullptr;		// 26px

public:
	void Render(IDirect3DDevice9* pDevice);
	void Init(IDirect3DDevice9* pDevice);

	bool IsOpen = false;
	bool ConfigLoaded = false;
	bool Unload = false;

	float TitleHeight = 22.f;
	float TabHeight = 35.f;

	// Colors
	ImColor Accent = { 255, 101, 101 };
	ImColor AccentDark = { 217, 87, 87 };
	ImColor Background = { 23, 23, 23, 250 };	// Title bar
	ImColor BackgroundLight = { 51, 51, 56 };	// Tab bar
	ImColor BackgroundDark = { 31, 31, 31 };	// Background
	ImColor TextDark = { 13, 13, 13 };
	ImColor TextLight = { 240, 240, 240 };
};

inline CMenu g_Menu;