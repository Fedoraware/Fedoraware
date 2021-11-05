#include "Keybinds.h"
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


void CKeybinds::Run()
{
	if (!ShouldRun())
		return;

	// Set window size, could maybe use a variable for this wink wink

	m_nKeybindsSize = 120;

	// Draw background, handle input.
	DrawNewWindow();
}

bool CKeybinds::DrawButton(const wchar_t* label, int x, int y, int w, int h) {
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

void CKeybinds::DrawFeature(const wchar_t* label, int x, int y, int w, int h, bool active) {
	g_Draw.OutlinedRect(x - 2, y - 2, w + 4, h + 4, Vars::Menu::Colors::FeatureOutline);
	g_Draw.Rect(x, y, w, h, Vars::Menu::Colors::FeatureBackground);
	g_Draw.String(FONT_INDICATORS, x + (w / 2), y + (h / 2), active ? Vars::Menu::Colors::FeatureOn : Vars::Menu::Colors::FeatureOff, ALIGN_CENTER, label);
}



void CKeybinds::DrawNewWindow()
{
	//If the menu is open, check for input and draw the titlebar.
	//The titlebar also indicates where we can drag / move the radar.
	if (g_Menu.m_bOpen || g_Menu.m_flFadeElapsed < g_Menu.m_flFadeDuration)
	{
		DragNewWindow();

		g_Interfaces.Surface->DrawSetAlphaMultiplier(g_Menu.m_flFadeAlpha);
		g_Draw.Rect(m_nKeybindsX - m_nKeybindsSize, m_nKeybindsY - m_nKeybindsSize - 20, m_nKeybindsSize, 20, Vars::Menu::Colors::TitleBar);
		g_Draw.String(FONT_MENU, m_nKeybindsX - (m_nKeybindsSize / 2), m_nKeybindsY - m_nKeybindsSize - 10, { 255, 255, 255, 255 }, ALIGN_CENTER, "Features");
		g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);

		//Build the bg color
		Color_t clrBack = Vars::Menu::Colors::WindowBackground;

		//g_Draw.Rect(m_nKeybindsX - m_nKeybindsSize, m_nKeybindsY - m_nKeybindsSize, m_nKeybindsSize, m_nKeybindsSize, clrBack);
		//g_Draw.OutlinedRect(m_nKeybindsX - m_nKeybindsSize, m_nKeybindsY - m_nKeybindsSize, m_nKeybindsSize, m_nKeybindsSize, clrBack);
		//g_Draw.OutlinedRect(m_nNewWindowX - m_nNewWindowSize, m_nNewWindowY - m_nNewWindowSize, m_nNewWindowSize, m_nNewWindowSize, Vars::Menu::Colors::OutlineMenu);
		g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);

	}
	int height = g_Draw.m_vecFonts[FONT_INDICATORS].nTall;
	DrawFeature(_(L"Aimbot"), m_nKeybindsX - (m_nKeybindsSize / 2) - 50, m_nKeybindsY - m_nKeybindsSize + 10, 100, height, GetAsyncKeyState(Vars::Aimbot::Global::AimKey.m_Var) ? true : false);
	DrawFeature(_(L"Trigger"), m_nKeybindsX - (m_nKeybindsSize / 2) - 50, m_nKeybindsY - m_nKeybindsSize + 10 + 21, 100, height, GetAsyncKeyState(Vars::Triggerbot::Global::TriggerKey.m_Var) ? true : false);
	DrawFeature(_(L"Fakelag"), m_nKeybindsX - (m_nKeybindsSize / 2) - 50, m_nKeybindsY - m_nKeybindsSize + 10 + (21 * 2), 100, height, Vars::Misc::CL_Move::FakelagOnKey.m_Var ? GetAsyncKeyState(Vars::Misc::CL_Move::FakelagKey.m_Var) : Vars::Misc::CL_Move::Fakelag.m_Var);
	DrawFeature(_(L"Doubletap"), m_nKeybindsX - (m_nKeybindsSize / 2) - 50, m_nKeybindsY - m_nKeybindsSize + 10 + (21 * 3), 100, height, GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var) ? true : false);
	DrawFeature(_(L"Recharge"), m_nKeybindsX - (m_nKeybindsSize / 2) - 50, m_nKeybindsY - m_nKeybindsSize + 10 + (21 * 4), 100, height, GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var) ? true : false);
	DrawFeature(_(L"Thirdperson"), m_nKeybindsX - (m_nKeybindsSize / 2) - 50, m_nKeybindsY - m_nKeybindsSize + 10 + (21 * 5), 100, height, Vars::Visuals::ThirdPerson.m_Var ? true : false);
}

bool CKeybinds::ShouldRun()
{
	//If in game
	if (g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	return true;
}

void CKeybinds::DragNewWindow()
{
	int mousex, mousey;
	g_Interfaces.Surface->GetCursorPos(mousex, mousey);

	static POINT pCorrect;
	static bool m_bDrag = false;
	static bool m_bMove = false;
	bool bHeld = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

	if ((mousex > (m_nKeybindsX - m_nKeybindsSize) &&
		mousex < (m_nKeybindsX - m_nKeybindsSize) + (m_nKeybindsSize) &&
		mousey >(m_nKeybindsY - m_nKeybindsSize) - 30 &&
		mousey < (m_nKeybindsY - m_nKeybindsSize)) && bHeld)
	{
		m_bDrag = true;

		if (!m_bMove)
		{
			pCorrect.x = mousex - m_nKeybindsX;
			pCorrect.y = mousey - (m_nKeybindsY - m_nKeybindsSize);
			m_bMove = true;
		}
	}

	if (m_bDrag)
	{
		m_nKeybindsX = (mousex + m_nKeybindsSize) - (m_nKeybindsSize)-pCorrect.x;
		m_nKeybindsY = (mousey + m_nKeybindsSize) - pCorrect.y;
	}

	if (!bHeld)
	{
		m_bDrag = false;
		m_bMove = false;
	}
}