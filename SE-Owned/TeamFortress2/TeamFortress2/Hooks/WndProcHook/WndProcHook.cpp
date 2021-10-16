#include "WndProcHook.h"

#include "../../Features/Menu/Menu.h"
#include "../../Features/What/What.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LONG __stdcall WndProcHook::Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{/*
	if (g_What.menuOpen) {
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return 1;
	}
	return (g_Menu.m_bOpen && (g_Interfaces.EngineVGui->IsGameUIVisible() || g_Menu.m_bTyping) ? 1 : CallWindowProc(WndProc, hWnd, uMsg, wParam, lParam));*/
	if (g_What.menuOpen) {
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return 1;
	}

	return CallWindowProcW(WndProc, hWnd, uMsg, wParam, lParam);
}