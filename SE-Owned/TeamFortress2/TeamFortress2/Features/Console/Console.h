#pragma once
#include "../../SDK/SDK.h"

class CConsole
{
public:
	void Run();
	int m_nConsoleX = 700, m_nConsoleY = 700;
	int margin = 10;
private:
	bool GetDrawPosition(int& x, int& y, CBaseEntity* pEntity);
	void DragNewWindow();
	void DrawPoints(CBaseEntity* pLocal);
	void DrawNewWindow();
	void DrawPlayerNames();
	bool DrawKickButton(const wchar_t* label, int x, int y, int w, int h);

	int m_nConsoleSize;
	int playerIndex;
};

inline CConsole g_Console;