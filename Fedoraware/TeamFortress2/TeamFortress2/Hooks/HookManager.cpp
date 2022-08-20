#include "HookManager.h"
#include "Hooks.h"
#include "../SDK/SDK.h"
#include "../SDK/Includes/proxyfnhook.h"
#include "MenuHook/MenuHook.h"

inline uintptr_t GetVFuncPtr(void *pBaseClass, unsigned int nIndex) {
	return static_cast<uintptr_t>((*static_cast<int **>(pBaseClass))[nIndex]);
}

CHook::CHook(const std::string& name, void* pInitFunction)
{
	this->Name = name;
	this->InitFunction = pInitFunction;
	g_HookManager.GetMapHooks()[name] = this;
}

void CHookManager::Release()
{
	MH_Uninitialize();
	SetWindowLongPtr(WndProc::hwWindow, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc::Original));
}

void CHookManager::Init()
{
	while (!I::DirectXDevice)
	{
		I::DirectXDevice = *reinterpret_cast<IDirect3DDevice9**>(g_Pattern.Find(L"shaderapidx9.dll", L"A1 ? ? ? ? 50 8B 08 FF 51 0C") + 0x1);
	}

	MH_Initialize();
	{
		WndProc::Init();
		for (const auto& hook : GetMapHooks())
		{
			hook.second->Init();
		}
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_STATUS::MH_OK)
	{
		MessageBoxW(0, L"MH failed to enable all hooks!", L"ERROR!", MB_ICONERROR);
	}
}