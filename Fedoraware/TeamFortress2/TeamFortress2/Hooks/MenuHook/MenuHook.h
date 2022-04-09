#pragma once
#include "../../SDK/SDK.h"
#include "../Hooks.h"
#include "../../Utils/MinHook/hook.h"

namespace MenuHook
{
	void Init();
	void Unload();
}

namespace EndScene
{
	inline CHook Hook;
	using fn = LRESULT(__stdcall*)(IDirect3DDevice9*);
	LRESULT __stdcall Func(IDirect3DDevice9* pDevice);
	void Init();
}

namespace Reset
{
	inline CHook Hook;
	using fn = HRESULT(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	HRESULT __stdcall Func(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams);
	void Init();
}

namespace WndProc
{
	inline HWND hwWindow;
	inline WNDPROC Original;
	LONG __stdcall Func(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Init();
}

namespace LockCursor
{
	inline CHook Hook;
	using fn = void(__thiscall*)(CSurface*);
	void __stdcall Func();
	void Init();
}