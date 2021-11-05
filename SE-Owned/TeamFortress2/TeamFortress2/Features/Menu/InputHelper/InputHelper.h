#pragma once
#include "../../../SDK/SDK.h"

class CInputHelper
{
private:
	enum EKeyState { NONE, PRESSED, HELD };
	EKeyState keys[256];

public:
	int m_nMouseX, m_nMouseY;

	void Update();
	bool IsPressed(int16_t key);
	bool IsHeld(int16_t key);
	bool IsPressedAndHeld(int16_t key);
	void Drag(int mousex, int mousey, int& x, int& y, int w, int h, int offsety);
	void NullKey(int16_t key);
};

inline CInputHelper g_InputHelper;