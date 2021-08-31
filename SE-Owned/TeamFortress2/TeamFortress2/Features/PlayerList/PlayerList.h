#pragma once
#include "../../SDK/SDK.h"

#include <fstream>
#include <filesystem>

class CPlayerList
{
public:
	void Run();
	int m_nNewWindowX = 700, m_nNewWindowY = 700;
	int margin = 10;
private:
	bool ShouldRun();
	void DragNewWindow();
	void DrawNewWindow();
	void DrawPlayerNames();
	bool DrawKickButton(const wchar_t* label, int x, int y, int w, int h);//, bool &buttonClick);
	bool kickButton = false;

	int m_nNewWindowSize;
	int playerIndex;
};

inline CPlayerList g_PlayerList;