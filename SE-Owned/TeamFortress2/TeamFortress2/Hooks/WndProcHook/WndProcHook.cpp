#include "WndProcHook.h"

#include "../../Features/Menu/Menu.h"

LONG __stdcall WndProcHook::Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (g_Menu.m_bOpen && (g_Interfaces.EngineVGui->IsGameUIVisible() || g_Menu.m_bTyping)) ? 1 : CallWindowProc(WndProc, hWnd, uMsg, wParam, lParam);
}