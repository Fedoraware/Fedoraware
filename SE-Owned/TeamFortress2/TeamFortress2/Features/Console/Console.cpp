#include "Console.h"
#include "../Vars.h"
#include "../Menu/Menu.h"

// Credits to [an absolute fucking bitch]
constexpr Color_t clrBlack = { 0, 0, 0, 255 };
constexpr Color_t clrWhite = { 255, 255, 255, 255 };


void CConsole::Run()
{
	/*if (!ShouldRun())
		return;*/

		// Set window size, could maybe use a variable for this wink wink

	m_nConsoleSize = 150;

	// Draw background, handle input.
	DrawNewWindow();
}

struct Console_t { std::wstring m_sName; std::wstring m_sMode; bool m_bIsFriend; int m_nTeam; int m_nIndex; };
std::vector<Console_t> m_vecPlayers;

bool CConsole::DrawKickButton(const wchar_t* label, int x, int y, int w, int h) {
	bool callback = false;
	int mousex, mousey;
	g_Interfaces.Surface->GetCursorPos(mousex, mousey);

	//g_Draw.String(FONT_MENU, 800, 800, { 0, 255, 0, 255 }, ALIGN_CENTER, reinterpret_cast<wchar_t *>(mousex));
	//g_Draw.String(FONT_MENU, 800, 840, { 0, 255, 0, 255 }, ALIGN_CENTER, reinterpret_cast<wchar_t *>(mousey));



	bool bHeld = (GetAsyncKeyState(VK_LBUTTON));
	if (mousex > x &&	// if mouse is within left bound
		mousex < x + w &&	// if mouse is within right bound
		mousey > y &&	// if mouse is within top bound
		mousey < y + h) // &&	// if mouse is within bottom bound)						// if mouse key is held
	{
		g_Draw.Rect(x, y, w, h, Vars::Menu::Colors::WidgetActive);
	}

	if (mousex > x &&	// if mouse is within left bound
		mousex < x + w &&	// if mouse is within right bound
		mousey > y &&	// if mouse is within top bound
		mousey < y + h && // &&	// if mouse is within bottom bound
		bHeld)						// if mouse key is held
	{
		g_Draw.Rect(x, y, w, h, Vars::Menu::Colors::TitleBar);
		callback = true;
	}

	g_Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);
	g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, label);

	return callback;
}

bool ButtonPressed = false;

void CConsole::DrawPlayerNames()
{

	if (DrawKickButton(_(L"CL_FullUpdate"), m_nConsoleX - m_nConsoleSize + 25, m_nConsoleY - m_nConsoleSize + 10, 100, 20) && !ButtonPressed) {
		ButtonPressed = true;
		g_Interfaces.Engine->ClientCmd_Unrestricted("cl_fullupdate");
	}
	if (DrawKickButton(_(L"SND_Restart"), m_nConsoleX - m_nConsoleSize + 25, m_nConsoleY - m_nConsoleSize + 30, 100, 20) && !ButtonPressed) {
		ButtonPressed = true;
		g_Interfaces.Engine->ClientCmd_Unrestricted("snd_restart");
	}
	if (DrawKickButton(_(L"StopSound"), m_nConsoleX - m_nConsoleSize + 25, m_nConsoleY - m_nConsoleSize + 50, 100, 20) && !ButtonPressed) {
		ButtonPressed = true;
		g_Interfaces.Engine->ClientCmd_Unrestricted("stopsound");
	}
	if (DrawKickButton(_(L"Status"), m_nConsoleX - m_nConsoleSize + 25, m_nConsoleY - m_nConsoleSize + 70, 100, 20) && !ButtonPressed) {
		ButtonPressed = true;
		g_Interfaces.Engine->ClientCmd_Unrestricted("status");
	}
	if (DrawKickButton(_(L"Ping"), m_nConsoleX - m_nConsoleSize + 25, m_nConsoleY - m_nConsoleSize + 90, 100, 20) && !ButtonPressed) {
		ButtonPressed = true;
		g_Interfaces.Engine->ClientCmd_Unrestricted("ping");
	}
	if (DrawKickButton(_(L"Retry"), m_nConsoleX - m_nConsoleSize + 25, m_nConsoleY - m_nConsoleSize + 110, 100, 20) && !ButtonPressed) {
		ButtonPressed = true;
		g_Interfaces.Engine->ClientCmd_Unrestricted("retry");
	}

	if (!GetAsyncKeyState(VK_LBUTTON)) {
		ButtonPressed = false;
	}
}

void CConsole::DrawNewWindow()
{
	//If the menu is open, check for input and draw the titlebar.
	//The titlebar also indicates where we can drag / move the radar.
	if (g_Menu.m_bOpen || g_Menu.m_flFadeElapsed < g_Menu.m_flFadeDuration)
	{
		DragNewWindow();

		//g_Interfaces.Surface->DrawSetAlphaMultiplier(g_Menu.m_flFadeAlpha);
		g_Draw.Rect(m_nConsoleX - m_nConsoleSize, m_nConsoleY - m_nConsoleSize - 20, m_nConsoleSize, 20, Vars::Menu::Colors::TitleBar);
		g_Draw.String(FONT_MENU, m_nConsoleX - (m_nConsoleSize / 2), m_nConsoleY - m_nConsoleSize - 10, { 255, 255, 255, 255 }, ALIGN_CENTER, "Settings");
		//g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);

		//Build the bg color
		Color_t clrBack = Vars::Menu::Colors::WindowBackground;

		//Background
		//g_Interfaces.Surface->DrawSetAlphaMultiplier(g_Menu.m_flFadeAlpha);
		if (playerIndex > 280) {
			g_Draw.Rect(m_nConsoleX - m_nConsoleSize, m_nConsoleY - m_nConsoleSize, m_nConsoleSize, playerIndex + margin, clrBack);
		}
		else {
			g_Draw.Rect(m_nConsoleX - m_nConsoleSize, m_nConsoleY - m_nConsoleSize, m_nConsoleSize, m_nConsoleSize, clrBack);

		}

		//Outline
		if (playerIndex > 280) {
			g_Draw.OutlinedRect(m_nConsoleX - m_nConsoleSize, m_nConsoleY - m_nConsoleSize, m_nConsoleSize, playerIndex + margin, clrBack);
		}
		else {
			g_Draw.OutlinedRect(m_nConsoleX - m_nConsoleSize, m_nConsoleY - m_nConsoleSize, m_nConsoleSize, m_nConsoleSize, clrBack);

		}
		//g_Draw.OutlinedRect(m_nConsoleX - m_nConsoleSize, m_nConsoleY - m_nConsoleSize, m_nConsoleSize, m_nConsoleSize, Vars::Menu::Colors::OutlineMenu);
		//g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);



		DrawPlayerNames();

	}
}

/*bool CConsole::ShouldRun()
{
	//If in game
	if (g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	return true;
}*/

void CConsole::DragNewWindow()
{
	int mousex, mousey;
	g_Interfaces.Surface->GetCursorPos(mousex, mousey);

	static POINT pCorrect;
	static bool m_bDrag = false;
	static bool m_bMove = false;
	bool bHeld = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

	if ((mousex > (m_nConsoleX - m_nConsoleSize) &&
		mousex < (m_nConsoleX - m_nConsoleSize) + (m_nConsoleSize) &&
		mousey >(m_nConsoleY - m_nConsoleSize) - 30 &&
		mousey < (m_nConsoleY - m_nConsoleSize)) && bHeld)
	{
		m_bDrag = true;

		if (!m_bMove)
		{
			pCorrect.x = mousex - m_nConsoleX;
			pCorrect.y = mousey - (m_nConsoleY - m_nConsoleSize);
			m_bMove = true;
		}
	}

	if (m_bDrag)
	{
		m_nConsoleX = (mousex + m_nConsoleSize) - (m_nConsoleSize)-pCorrect.x;
		m_nConsoleY = (mousey + m_nConsoleSize) - pCorrect.y;
	}

	if (!bHeld)
	{
		m_bDrag = false;
		m_bMove = false;
	}
}
