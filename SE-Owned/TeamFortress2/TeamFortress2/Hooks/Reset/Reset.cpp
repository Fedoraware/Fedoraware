#include "Reset.h"
#include "../../Features/What/What.h"
#include "../Hooks.h"

HRESULT __stdcall ResetHook::Func(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT Original = g_Hooks.CallOriginal<fn>()(pDevice, pPresentParams);
	ImGui_ImplDX9_CreateDeviceObjects();
	return Original;
}

void ResetHook::Init()
{
	fn FN = reinterpret_cast<fn>(Utils::GetVFuncPtr(reinterpret_cast<void**>(g_dwDirectXDevice), 16));
	g_Hooks.Create(reinterpret_cast<void*>(FN), reinterpret_cast<void*>(Func));
}
