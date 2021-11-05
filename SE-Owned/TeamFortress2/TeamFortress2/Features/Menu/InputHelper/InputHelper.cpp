#include "InputHelper.h"

void CInputHelper::Update()
{
	for (int n = 0; n < 256; n++)
	{
		if (GetKeyState(n) & 0x8000)
		{
			if (keys[n] == PRESSED)
				keys[n] = HELD;

			else if (keys[n] != HELD)
				keys[n] = PRESSED;
		}

		else keys[n] = NONE;
	}

	g_Interfaces.Surface->GetCursorPos(m_nMouseX, m_nMouseY);
}

bool CInputHelper::IsPressed(int16_t key)
{
	return (keys[key] == PRESSED);
}

bool CInputHelper::IsHeld(int16_t key)
{
	return (keys[key] == HELD);
}

bool CInputHelper::IsPressedAndHeld(int16_t key)
{
	EKeyState key_state = keys[key];

	static std::chrono::time_point<std::chrono::steady_clock> start[256] = { std::chrono::steady_clock::now() };

	if (key_state == PRESSED)
	{
		start[key] = std::chrono::steady_clock::now();
		return true;
	}

	if (key_state == HELD && std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now() - start[key]).count() > 400)
		return true;

	return false;
}

void CInputHelper::Drag(int mousex, int mousey, int& x, int& y, int w, int h, int offsety)
{
	static POINT delta;
	static bool drag = false;
	static bool move = false;
	bool held = IsHeld(VK_LBUTTON);

	if ((mousex > x && mousex < x + w && mousey > y - offsety && mousey < y - offsety + h) && held)
	{
		drag = true;

		if (!move) {
			delta.x = mousex - x;
			delta.y = mousey - y;
			move = true;
		}
	}

	if (drag) {
		x = mousex - delta.x;
		y = mousey - delta.y;
	}

	if (!held) {
		drag = false;
		move = false;
	}
}

void CInputHelper::NullKey(int16_t key)
{
	keys[key] = EKeyState::NONE;
}