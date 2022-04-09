#include "MenuHook.h"
#include "../../Features/Menu/Menu.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Initialize hooks
void MenuHook::Init()
{
	EndScene::Init();
	Reset::Init();
	WndProc::Init();
	// LockCursor::Init();
}

// Prepares the menu for unloading
void MenuHook::Unload()
{
	g_Menu.IsOpen = true;
	g_Menu.Unload = true;
}

// Render the menu
LRESULT __stdcall EndScene::Func(IDirect3DDevice9* pDevice)
{
	static void* fAddr = _ReturnAddress();
	if (fAddr != _ReturnAddress())
	{
		return Hook.CallOriginal<fn>()(pDevice);
	}

	g_Menu.Render(pDevice);
	return Hook.CallOriginal<fn>()(pDevice);
}

// Initialize the EndScene hook
void EndScene::Init()
{
	while (!g_dwDirectXDevice)
	{
		g_dwDirectXDevice = **reinterpret_cast<DWORD**>(g_Pattern.Find(_(L"shaderapidx9.dll"), _(L"A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 0x1);
	}
	const auto FN = reinterpret_cast<fn>(Utils::GetVFuncPtr(reinterpret_cast<void**>(g_dwDirectXDevice), 42));
	Hook.Create(reinterpret_cast<void*>(FN), reinterpret_cast<void*>(Func));
}

HRESULT __stdcall Reset::Func(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const HRESULT Original = Hook.CallOriginal<fn>()(pDevice, pPresentParams);
	ImGui_ImplDX9_CreateDeviceObjects();
	return Original;
}

void Reset::Init()
{
	const auto FN = reinterpret_cast<fn>(Utils::GetVFuncPtr(reinterpret_cast<void**>(g_dwDirectXDevice), 16));
	Hook.Create(reinterpret_cast<void*>(FN), reinterpret_cast<void*>(Func));
}

LONG __stdcall WndProc::Func(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (g_Menu.IsOpen)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		g_Interfaces.InputSystem->ResetInputStateVFunc();
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

void __stdcall LockCursor::Func()
{
	const auto FN = reinterpret_cast<fn>(Utils::GetVFuncPtr(g_Interfaces.Surface, 62));
	Hook.Create(reinterpret_cast<void*>(FN), reinterpret_cast<void*>(Func));
}

void LockCursor::Init()
{
	g_Menu.IsOpen ? g_Interfaces.Surface->UnlockCursor() : Hook.CallOriginal<fn>()(g_Interfaces.Surface);
}
