#pragma once
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"
#include "../../SDK/SDK.h"

class CMenu {
private:
	void DrawMenu();
	void DrawCameraWindow();

	// Fonts
	ImFont* SegoeLight = nullptr;
	ImFont* Segoe = nullptr;
	ImFont* SegoeBold = nullptr;

	ImFont* Title = nullptr;
	ImFont* TitleLight = nullptr;

public:
	void Render(IDirect3DDevice9* pDevice);
	void Init(IDirect3DDevice9* pDevice);

	bool IsOpen = false;
	bool ConfigLoaded = false;
	bool Unload = false;

	// Colors
	ImColor AccentColor = { 225, 177, 44 };
	ImColor BackgroundDark = { 13, 13, 13 };
	ImColor Background = { 23, 23, 23, 240 };
};

inline CMenu g_Menu;