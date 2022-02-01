#include "DTBar.h"
#include "../Vars.h"
#include "../Menu/Menu.h"

constexpr Color_t clrBlack = { 0, 0, 0, 255 };
constexpr Color_t clrWhite = { 255, 255, 255, 255 };

void CDTBar::Run()
{
	if (!ShouldRun())
		return;
	if (g_Menu.m_bOpen || g_Menu.m_flFadeElapsed < g_Menu.m_flFadeDuration)
		Dragbar();
}

// if we have the esc-menu open do not draw, supa simple
bool CDTBar::ShouldRun()
{
	if (g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	return true;
}

// paste, og @ Radar.cpp
void CDTBar::Dragbar()
{
	int mousex, mousey;
	g_Interfaces.Surface->GetCursorPos(mousex, mousey);

	static POINT pCorrect;
	static bool m_bDrag = false;
	static bool m_bMove = false;
	bool bHeld = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

	if (mousex > (g_ScreenSize.w / 2 + Vars::Misc::CL_Move::DTBarX.m_Var - Vars::Misc::CL_Move::DTBarScaleX.m_Var / 2) &&
		mousex < (g_ScreenSize.w / 2 + Vars::Misc::CL_Move::DTBarX.m_Var + Vars::Misc::CL_Move::DTBarScaleX.m_Var / 2) &&
		mousey > (g_ScreenSize.h / 2 + Vars::Misc::CL_Move::DTBarY.m_Var - Vars::Misc::CL_Move::DTBarScaleY.m_Var / 2 + 20) &&
		mousey < (g_ScreenSize.h / 2 + Vars::Misc::CL_Move::DTBarY.m_Var - Vars::Misc::CL_Move::DTBarScaleY.m_Var / 2 + Vars::Misc::CL_Move::DTBarScaleY.m_Var + 20) && bHeld)
	{
		m_bDrag = true;

		if (!m_bMove)
		{
			pCorrect.x = (g_ScreenSize.w / 2);
			pCorrect.y = (g_ScreenSize.h / 2 + 10 + Vars::Misc::CL_Move::DTBarScaleY.m_Var / 2);
			m_bMove = true;
		}
	}

	if (m_bDrag)
	{
		Vars::Misc::CL_Move::DTBarX.m_Var = mousex - pCorrect.x;
		Vars::Misc::CL_Move::DTBarY.m_Var = mousey - pCorrect.y;
	}

	if (!bHeld)
	{
		m_bDrag = false;
		m_bMove = false;
	}
}