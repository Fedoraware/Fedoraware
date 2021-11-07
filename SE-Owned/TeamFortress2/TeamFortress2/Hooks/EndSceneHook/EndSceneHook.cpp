#include "EndSceneHook.h"
#include "../../Features/What/What.h"

HRESULT __stdcall EndSceneHook::Func(IDirect3DDevice9* pDevice) {
	static void* firstAddress = _ReturnAddress();
	if (firstAddress != _ReturnAddress()) // EndScene is called twice, this prevents double rendering
		return Hook.CallOriginal<fn>()(pDevice);

	g_What.Render(pDevice);
	return Hook.CallOriginal<fn>()(pDevice);
}

void EndSceneHook::Init()
{
	g_dwDirectXDevice = **reinterpret_cast<DWORD**>(g_Pattern.Find(L"shaderapidx9.dll", L"A1 ? ? ? ? 50 8B 08 FF 51 0C") + 0x1);
	fn FN = reinterpret_cast<fn>(Utils::GetVFuncPtr(reinterpret_cast<void**>(g_dwDirectXDevice), 42));
	Hook.Create(reinterpret_cast<void*>(FN), reinterpret_cast<void*>(Func));
}