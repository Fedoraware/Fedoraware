#include "MenuHook.h"
#include "../../Features/Menu/Menu.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Prepares the menu for unloading
void MenuHook::Unload()
{
	g_Menu.IsOpen = true;
	g_Menu.Unload = true;
}

MAKE_HOOK(WINAPI_EndScene, Utils::GetVFuncPtr(reinterpret_cast<void**>(g_dwDirectXDevice), 42), HRESULT, __stdcall,
		  LPDIRECT3DDEVICE9 pDevice)
{
	static void* fAddr = _ReturnAddress();
	if (fAddr != _ReturnAddress())
	{
		return Hook.Original<FN>()(pDevice);
	}

	g_Menu.Render(pDevice);
	return Hook.Original<FN>()(pDevice);
}

MAKE_HOOK(WINAPI_Reset, Utils::GetVFuncPtr(reinterpret_cast<void**>(g_dwDirectXDevice), 16), HRESULT, __stdcall,
		  LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const HRESULT Original = Hook.Original<FN>()(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();
	return Original;
}

LONG __stdcall WndProc::Func(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (g_Menu.IsOpen)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		I::InputSystem->ResetInputStateVFunc();
		return 1;
	}

	return CallWindowProc(Original, hWnd, uMsg, wParam, lParam);
}

void WndProc::Init()
{
	while (!hwWindow)
	{
		hwWindow = FindWindowW(nullptr, L"Team Fortress 2");
	}

	Original = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hwWindow, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(Func)));
}