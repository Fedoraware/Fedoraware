#pragma once

#include "../../SDK/SDK.h"

typedef LONG(WINAPI *WNDPROC)(HWND, UINT, WPARAM, LPARAM);

namespace WndProcHook
{
	inline WNDPROC WndProc;
	LONG WINAPI Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}