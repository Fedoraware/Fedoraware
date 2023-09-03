#include "HookManager.h"

#include <ranges>

#include "Hooks.h"
#include "../SDK/SDK.h"
#include "MenuHook/MenuHook.h"

inline uintptr_t GetVFuncPtr(void* pBaseClass, unsigned int nIndex)
{
	return static_cast<uintptr_t>((*static_cast<int**>(pBaseClass))[nIndex]);
}

CHook::CHook(const std::string& name, void* pInitFunction)
{
	this->m_Name = name;
	this->m_InitFunction = pInitFunction;
	g_HookManager.GetMapHooks()[name] = this;
}

void CHookManager::Release()
{
	assert(MH_Uninitialize() == MH_OK);
	WndProc::Unload();
}

void CHookManager::Init()
{
	while (!I::DirectXDevice)
	{
		I::DirectXDevice = *reinterpret_cast<IDirect3DDevice9**>(g_Pattern.Find("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 0x1);
	}

	MH_Initialize();
	{
		WndProc::Init();
		for (const auto& hook : GetMapHooks() | std::views::values)
		{
			hook->Init();
		}
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
	{
		MessageBoxW(nullptr, L"MH failed to enable all hooks!", L"ERROR!", MB_ICONERROR);
	}
}