#include "NewWindow.h"
#include "../Vars.h"
#include "../Menu/Menu.h"
#include "../Menu/ConfigManager/ConfigManager.h"
#include <string>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>
#include "../Vars.h"

constexpr Color_t clrBlack = { 0, 0, 0, 255 };
constexpr Color_t clrWhite = { 255, 255, 255, 255 };


void CNewWindow::Run()
{
	if (!ShouldRun())
		return;

	// Set window size, could maybe use a variable for this wink wink

	m_nNewWindowSize = 300;

	// Draw background, handle input.
	DrawNewWindow();
}

struct Playerlist_t { std::wstring m_sName; std::wstring m_sMode; bool m_bIsFriend; int m_nTeam; int m_nIndex; };
std::vector<Playerlist_t> m_vecPlayers;

bool CNewWindow::DrawKickButton(const wchar_t* label, int x, int y, int w, int h) {//, bool &buttonClick) {
	bool callback = false;
	int mousex, mousey;
	g_Interfaces.Surface->GetCursorPos(mousex, mousey);

	bool bHeld = GetAsyncKeyState(VK_LBUTTON);
	// Hover
	if (mousex > x &&	// if mouse is within left bound
		mousex < x + w &&	// if mouse is within right bound
		mousey > y &&	// if mouse is within top bound
		mousey < y + h) // &&	// if mouse is within bottom bound)						// if mouse key is held
	{
		g_Draw.Rect(x, y, w, h, Vars::Menu::Colors::WidgetActive);
	}


	// Click
	if (mousex > x &&	// if mouse is within left bound
		mousex < x + w &&	// if mouse is within right bound
		mousey > y &&	// if mouse is within top bound
		mousey < y + h && // &&	// if mouse is within bottom bound
		bHeld) // && !buttonClick)						// if mouse key is held
	{
		g_Draw.Rect(x, y, w, h, Vars::Menu::Colors::TitleBar);
		//buttonClick = true;
		callback = true;
	}

	g_Draw.OutlinedRect(x, y, w, h, Vars::Menu::Colors::OutlineMenu);
	g_Draw.String(FONT_MENU, x + (w / 2), y + (h / 2), Vars::Menu::Colors::Text, ALIGN_CENTER, label);

	return callback;
}



void CNewWindow::DrawPlayerNames()
{
	playerIndex = 0;
	for (int i = 0; i < 32; i++) {
		PlayerInfo_t pi;
		if (g_Interfaces.Engine->GetPlayerInfo(i, &pi)) {
			Color_t teamColor;
			teamColor = { 255, 255, 255, 255 };
			if (CBaseEntity* pEntity = g_Interfaces.EntityList->GetClientEntity(i)) {

				int team = pEntity->GetTeamNum(); // spec = 1, red = 2, blu = 3
				int classNum = pEntity->GetClassNum();
				//Color_t teamColor = team ? Colors::TeamBlu : Colors::TeamRed;
				switch (team) {
				case 1:
					//Spectator
					teamColor = { 255, 255, 255, 255 };
					break;
				case 2:
					//Red
					teamColor = Colors::TeamRed;
					break;
				case 3:
					//Blue
					teamColor = Colors::TeamBlu;
					break;
				default:
					//Unknown team
					teamColor = { 0, 0, 0, 255 };
					break;
				}

				int nTexture = pEntity->GetClassNum();

				// Class picture
				/*int nEntTeam = pEntity->GetTeamNum();
				if (Vars::Radar::Players::IconType.m_Var == 1)
				{
					nTexture += 10;
					if (nEntTeam == 3)
						nTexture += 10;
				}
				g_Draw.Texture(m_nNewWindowX - m_nNewWindowSize + 120, m_nNewWindowY + -m_nNewWindowSize + 2 + playerIndex, 18, 18, Colors::White, nTexture);*/


				g_Draw.Texture(m_nNewWindowX - m_nNewWindowSize, m_nNewWindowY + -m_nNewWindowSize + 2 + playerIndex, 16, 16, Colors::White, nTexture); //Class icon
				// Steam id
				//g_Draw.String(FONT_MENU, m_nNewWindowX - m_nNewWindowSize + 120, m_nNewWindowY + -m_nNewWindowSize + 2 + playerIndex, teamColor, ALIGN_DEFAULT, pi.guid);
				// Player index
				//g_Draw.String(FONT_MENU, m_nNewWindowX - m_nNewWindowSize + 240, m_nNewWindowY + -m_nNewWindowSize + 2 + playerIndex, teamColor, ALIGN_DEFAULT, std::to_string(i).data());
				// ID
				//g_Draw.String(FONT_MENU, m_nNewWindowX - m_nNewWindowSize + 240, m_nNewWindowY + -m_nNewWindowSize + 2 + playerIndex, teamColor, ALIGN_DEFAULT, std::to_string(pi.userID).data());
				// Team number
				//g_Draw.String(FONT_MENU, m_nNewWindowX - m_nNewWindowSize + 260, m_nNewWindowY + -m_nNewWindowSize + 2 + playerIndex, teamColor, ALIGN_DEFAULT, std::to_string(team).data());

			}
			g_Draw.String(FONT_MENU, m_nNewWindowX - m_nNewWindowSize + 18, m_nNewWindowY + -m_nNewWindowSize + 2 + playerIndex, teamColor, ALIGN_DEFAULT, pi.name);
			g_Draw.String(FONT_MENU, g_ScreenSize.c, g_ScreenSize.h, {255,255,255,255}, ALIGN_DEFAULT, std::to_string(kickButton).data());

			int iUserID = pi.userID;
			std::string kickID = std::to_string(pi.userID);
			const char* kick = kickID.data();
			wchar_t m_buffer[256];

			swprintf_s(m_buffer, L"%d", iUserID);
			if (DrawKickButton(_(L"Kick"), m_nNewWindowX - m_nNewWindowSize + 270, m_nNewWindowY - m_nNewWindowSize + playerIndex, 30, 16) && !kickButton) {
				kickButton = true;
				const char* kickCommand = "callvote kick ";
				char buffer[256];
				strncpy(buffer, kickCommand, sizeof(buffer));
#pragma warning (push)
#pragma warning (disable : 6053)
#pragma warning (disable : 6059)
				strncat(buffer, kick, sizeof(buffer));
#pragma warning ( pop )
				//g_Draw.String(FONT_MENU, m_nNewWindowX - m_nNewWindowSize + 240, m_nNewWindowY + -m_nNewWindowSize + 2 + playerIndex, teamColor, ALIGN_DEFAULT, buffer);
				g_Interfaces.Engine->ClientCmd_Unrestricted(buffer);
			}

			if (!GetAsyncKeyState(VK_LBUTTON)) {
				kickButton = false;
			}
			playerIndex += 20;
		}
	}
}

void CNewWindow::DrawNewWindow()
{
	//If the menu is open, check for input and draw the titlebar.
	//The titlebar also indicates where we can drag / move the radar.
	if (g_Menu.m_bOpen || g_Menu.m_flFadeElapsed < g_Menu.m_flFadeDuration)
	{
		DragNewWindow();

		g_Interfaces.Surface->DrawSetAlphaMultiplier(g_Menu.m_flFadeAlpha);
		g_Draw.Rect(m_nNewWindowX - m_nNewWindowSize, m_nNewWindowY - m_nNewWindowSize - 20, m_nNewWindowSize, 20, Vars::Menu::Colors::TitleBar);
		g_Draw.String(FONT_MENU, m_nNewWindowX - (m_nNewWindowSize / 2), m_nNewWindowY - m_nNewWindowSize - 10, { 255, 255, 255, 255 }, ALIGN_CENTER, "Playerlist");
		g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);

		//Build the bg color
		Color_t clrBack = Vars::Menu::Colors::WindowBackground;

		//Background
		g_Interfaces.Surface->DrawSetAlphaMultiplier(g_Menu.m_flFadeAlpha);
		if (playerIndex > 280) {
			g_Draw.Rect(m_nNewWindowX - m_nNewWindowSize, m_nNewWindowY - m_nNewWindowSize, m_nNewWindowSize, playerIndex + margin, clrBack);
		}
		else {
			g_Draw.Rect(m_nNewWindowX - m_nNewWindowSize, m_nNewWindowY - m_nNewWindowSize, m_nNewWindowSize, m_nNewWindowSize, clrBack);
	
		}

		//Outline
		if (playerIndex > 280) {
			g_Draw.OutlinedRect(m_nNewWindowX - m_nNewWindowSize, m_nNewWindowY - m_nNewWindowSize, m_nNewWindowSize, playerIndex + margin, clrBack);
		}
		else {
			g_Draw.OutlinedRect(m_nNewWindowX - m_nNewWindowSize, m_nNewWindowY - m_nNewWindowSize, m_nNewWindowSize, m_nNewWindowSize, clrBack);

		}
		//g_Draw.OutlinedRect(m_nNewWindowX - m_nNewWindowSize, m_nNewWindowY - m_nNewWindowSize, m_nNewWindowSize, m_nNewWindowSize, Vars::Menu::Colors::OutlineMenu);
		g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);

		

		DrawPlayerNames();

	}
}

bool CNewWindow::ShouldRun()
{
	//If in game
	if (g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	return true;
}

void CNewWindow::DragNewWindow()
{
	int mousex, mousey;
	g_Interfaces.Surface->GetCursorPos(mousex, mousey);

	static POINT pCorrect;
	static bool m_bDrag = false;
	static bool m_bMove = false;
	bool bHeld = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

	if ((mousex > (m_nNewWindowX - m_nNewWindowSize) &&
		mousex < (m_nNewWindowX - m_nNewWindowSize) + (m_nNewWindowSize) &&
		mousey >(m_nNewWindowY - m_nNewWindowSize) - 30 &&
		mousey < (m_nNewWindowY - m_nNewWindowSize)) && bHeld)
	{
		m_bDrag = true;

		if (!m_bMove)
		{
			pCorrect.x = mousex - m_nNewWindowX;
			pCorrect.y = mousey - (m_nNewWindowY - m_nNewWindowSize);
			m_bMove = true;
		}
	}

	if (m_bDrag)
	{
		m_nNewWindowX = (mousex + m_nNewWindowSize) - (m_nNewWindowSize)-pCorrect.x;
		m_nNewWindowY = (mousey + m_nNewWindowSize) - pCorrect.y;
	}

	if (!bHeld)
	{
		m_bDrag = false;
		m_bMove = false;
	}
}