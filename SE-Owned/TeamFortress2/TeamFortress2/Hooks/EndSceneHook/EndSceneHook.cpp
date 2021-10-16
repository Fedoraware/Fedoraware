#include "EndSceneHook.h"
#include "../../Features/What/What.h"

HRESULT __stdcall EndSceneHook::Func(IDirect3DDevice9* pDevice) {
	g_What.Render(pDevice);
	return Hook.CallOriginal<fn>()(pDevice);
}

void EndSceneHook::Init()
{
	fn FN = reinterpret_cast<fn>(Utils::GetVFuncPtr(reinterpret_cast<void**>(g_dwDirectXDevice), 42));
	Hook.Create(reinterpret_cast<void*>(FN), reinterpret_cast<void*>(Func));
}