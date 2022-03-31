#pragma once
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"
#include "../../SDK/SDK.h"

class CMenu {
private:
	void DrawMenu();
	void DrawCameraWindow();

	// Fonts
	ImFont* SegoeLight;
	ImFont* Segoe;
	ImFont* SegoeBold;

public:
	void Render(IDirect3DDevice9* pDevice);
	void Init(IDirect3DDevice9* pDevice);

	bool IsOpen = false;
	bool ConfigLoaded = false;
	bool Unload = false;
};

inline CMenu g_Menu;