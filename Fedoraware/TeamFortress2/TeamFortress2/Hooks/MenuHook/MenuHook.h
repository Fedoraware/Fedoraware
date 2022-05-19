#pragma once
#include "../../SDK/SDK.h"
#include "../Hooks.h"

namespace MenuHook
{
	void Init();
	void Unload();
}

namespace WndProc
{
	inline HWND hwWindow;
	inline WNDPROC Original;
	LONG __stdcall Func(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Init();
}