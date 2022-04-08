#pragma once
#include "ImGui/imgui_impl_dx9.h"
#include "../../SDK/SDK.h"
#include "ImGui/imgui_color_gradient.h"

class CMenu {
	float GetContentHeight();
	void DrawMenu();
	void DrawTabbar();
	void MenuAimbot();
	void MenuTrigger();
	void MenuVisuals();
	void MenuHvH();
	void MenuMisc();
	void SettingsWindow();
	void DrawCameraWindow();
	void LoadStyle();

	enum class MenuTab {
		Aimbot,
		Trigger,
		Visuals,
		HvH,
		Misc
	};

	enum class VisualsTab {
		Players,
		Buildings,
		World,
		Font,
		Misc,
		Radar
	};

	MenuTab CurrentTab = MenuTab::Aimbot;
	VisualsTab CurrentVisualsTab = VisualsTab::Players;

	bool ShowSettings = false;

public:
	void Render(IDirect3DDevice9* pDevice);
	void Init(IDirect3DDevice9* pDevice);

	bool IsOpen = false;
	bool ConfigLoaded = false;
	bool Unload = false;

	float TitleHeight = 22.f;
	float TabHeight = 30.f;
	float SubTabHeight = 0.f;

	// Colors
	ImColor Accent = { 255, 101, 101 };
	ImColor AccentDark = { 217, 87, 87 };
	ImColor Background = { 23, 23, 23, 250 };	// Title bar
	ImColor BackgroundLight = { 51, 51, 56 };		// Tab bar
	ImColor BackgroundDark = { 31, 31, 31 };		// Background
	ImColor TextLight = { 240, 240, 240 };

	// Fonts
	ImFont* SegoeLight = nullptr;	// 16px
	ImFont* Segoe = nullptr;		// 16px
	ImFont* SegoeBold = nullptr;	// 16px

	ImFont* SectionFont = nullptr;	// 18px
	ImFont* TabFont = nullptr;		// 22px
	ImFont* TitleFont = nullptr;	// 26px
	ImFont* IconFont = nullptr;		// 16px

	// Gradients
	ImGradient TitleGradient;
	ImGradient MainGradient;
};

inline CMenu g_Menu;