#pragma once
#include "../../SDK/SDK.h"

#include <fstream>
#include <filesystem>

class CKeybinds
{
public:
	void Run();
	int m_nKeybindsX = 700, m_nKeybindsY = 700;
private:
	bool ShouldRun();
	void DragNewWindow();
	void DrawNewWindow();
	bool DrawButton(const wchar_t* label, int x, int y, int w, int h);
	void DrawFeature(const wchar_t* label, int x, int y, int w, int h, bool active);
	//, bool &buttonClick);
	bool buttonPressed = false;

	int m_nKeybindsSize;
};

inline CKeybinds g_Keybinds;