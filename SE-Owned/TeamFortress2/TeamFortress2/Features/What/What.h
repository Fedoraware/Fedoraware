#pragma once
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"
#include "../../SDK/SDK.h"

class CWhat
{
private:
	ImFont* Normal;
	ImFont* DT;
	ImDrawList* drawList;
public:
	bool menuOpen = false;
	bool menuUnload = false;
	void TextCenter(std::string text);
	void Render(IDirect3DDevice9* pDevice);
};

inline CWhat g_What;