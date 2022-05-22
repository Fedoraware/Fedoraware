#include "DTBar.h"
#include "../Menu.h"

bool CDTBar::ShouldRun()
{
	if (I::EngineVGui->IsGameUIVisible())
	{
		return false;
	}

	return true;
}

void CDTBar::Dragbar()
{
	int mousex, mousey;
	I::Surface->GetCursorPos(mousex, mousey);

	static POINT pCorrect{};
	static bool isDragging = false;
	static bool isMoving = false;
	const bool bHeld = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

	if (mousex > (g_ScreenSize.w / 2 + Vars::Misc::CL_Move::DTBarX.m_Var - Vars::Misc::CL_Move::DTBarScaleX.m_Var / 2)
		&&
		mousex < (g_ScreenSize.w / 2 + Vars::Misc::CL_Move::DTBarX.m_Var + Vars::Misc::CL_Move::DTBarScaleX.m_Var / 2)
		&&
		mousey >(g_ScreenSize.h / 2 + Vars::Misc::CL_Move::DTBarY.m_Var - Vars::Misc::CL_Move::DTBarScaleY.m_Var / 2 +
			20) &&
		mousey < (g_ScreenSize.h / 2 + Vars::Misc::CL_Move::DTBarY.m_Var - Vars::Misc::CL_Move::DTBarScaleY.m_Var / 2 +
			Vars::Misc::CL_Move::DTBarScaleY.m_Var + 20) && bHeld)
	{
		isDragging = true;

		if (!isMoving)
		{
			pCorrect.x = (g_ScreenSize.w / 2);
			pCorrect.y = (g_ScreenSize.h / 2 + 10 + Vars::Misc::CL_Move::DTBarScaleY.m_Var / 2);
			isMoving = true;
		}
	}

	if (isDragging)
	{
		Vars::Misc::CL_Move::DTBarX.m_Var = mousex - pCorrect.x;
		Vars::Misc::CL_Move::DTBarY.m_Var = mousey - pCorrect.y;
	}

	if (!bHeld)
	{
		isDragging = false;
		isMoving = false;
	}
}

void CDTBar::Run()
{
	if (ShouldRun() && g_Menu.IsOpen)
	{
		Dragbar();
	}
}
