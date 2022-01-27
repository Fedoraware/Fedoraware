#pragma once
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"
#include "../../SDK/SDK.h"

class CMenu
{
private:
	ImFont* Normal;
	ImFont* DT;
	ImDrawList* drawList;

public:
	bool config = false;
	bool menuOpen = false;
	bool menuUnload = false;
	void TextCenter(std::string text);
	void Render(IDirect3DDevice9* pDevice);

public:
	bool m_bReopened = false;
	float m_flFadeAlpha = 0.0f;
	float m_flFadeElapsed = 0.0f;
	float m_flFadeDuration = 0.1f;
	float flTimeOnChange = 0.0f;
	bool m_bOpen = false;
	void Run();
};

inline CMenu g_Menu;