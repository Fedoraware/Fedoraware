#include "MenuHook.h"
#include "../../Features/Menu/Menu.h"

// REDOING THIS SHIT

//HRESULT __stdcall EndSceneHook::Func(IDirect3DDevice9* pDevice) {
//	static void* firstAddress = _ReturnAddress();
//	if (firstAddress != _ReturnAddress()) // EndScene is called twice, this prevents double rendering
//		return Hook.CallOriginal<fn>()(pDevice);
//
//	g_What.Render(pDevice);
//	return Hook.CallOriginal<fn>()(pDevice);
//}
//
//void EndSceneHook::Init()
//{
//	g_dwDirectXDevice = **reinterpret_cast<DWORD**>(g_Pattern.Find(L"shaderapidx9.dll", L"A1 ? ? ? ? 50 8B 08 FF 51 0C") + 0x1);
//	fn FN = reinterpret_cast<fn>(Utils::GetVFuncPtr(reinterpret_cast<void**>(g_dwDirectXDevice), 42));
//	Hook.Create(reinterpret_cast<void*>(FN), reinterpret_cast<void*>(Func));
//}

void MenuHook::Init()
{
	EndScene::Init();
	Reset::Init();
	WndProc::Init();
	LockCursor::Init();
}

void MenuHook::Unload()
{
	g_Menu.menuOpen = true;
	g_Menu.menuUnload = true;
}

void __stdcall LockCursor::Func()
{
	g_Menu.menuOpen ? g_Interfaces.Surface->UnlockCursor() : Hook.CallOriginal<fn>()(g_Interfaces.Surface);
}

void LockCursor::Init()
{
	fn FN = reinterpret_cast<fn>(Utils::GetVFuncPtr(g_Interfaces.Surface, 62));
	Hook.Create(reinterpret_cast<void*>(FN), reinterpret_cast<void*>(Func));
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HRESULT __stdcall Reset::Func(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT Original = Hook.CallOriginal<fn>()(pDevice, pPresentParams);
	ImGui_ImplDX9_CreateDeviceObjects();
	return Original;
}

void Reset::Init()
{
	fn FN = reinterpret_cast<fn>(Utils::GetVFuncPtr(reinterpret_cast<void**>(g_dwDirectXDevice), 16));
	Hook.Create(reinterpret_cast<void*>(FN), reinterpret_cast<void*>(Func));
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LONG __stdcall WndProc::Func(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (g_Menu.menuOpen) {
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		g_Interfaces.InputSystem->ResetInputStateVFunc();
		return 1;
	}

	return CallWindowProc(Original, hWnd, uMsg, wParam, lParam);
}

void WndProc::Init()
{
	while (!hwWindow)
		hwWindow = FindWindowW(0, L"Team Fortress 2");

	Original = (WNDPROC)SetWindowLongPtr(hwWindow, GWL_WNDPROC, (LONG_PTR)Func);
}

HRESULT __stdcall EndScene::Func(IDirect3DDevice9* pDevice)
{
	static void* fAddr = _ReturnAddress();
	if (fAddr != _ReturnAddress()) {
		return Hook.CallOriginal<fn>()(pDevice);
	}

	g_Menu.Render(pDevice);
	return Hook.CallOriginal<fn>()(pDevice);
}

void EndScene::Init()
{
	while (!g_dwDirectXDevice) {
		g_dwDirectXDevice = **reinterpret_cast<DWORD**>(g_Pattern.Find(_(L"shaderapidx9.dll"), _(L"A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 0x1);
	}
	fn FN = reinterpret_cast<fn>(Utils::GetVFuncPtr(reinterpret_cast<void**>(g_dwDirectXDevice), 42));
	Hook.Create(reinterpret_cast<void*>(FN), reinterpret_cast<void*>(Func));
}

