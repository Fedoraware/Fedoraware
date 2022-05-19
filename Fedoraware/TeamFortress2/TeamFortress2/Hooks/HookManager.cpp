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
	InitFunction = pInitFunction;
	g_HookManager.GetMapHooks()[name] = this;
}

bool HookNetvar(std::vector<std::string> path, ProxyFnHook& hook, RecvVarProxyFn function)
{
	auto pClass = g_Interfaces.Client->GetAllClasses();
	if (path.size() < 2)
		return false;
	while (pClass)
	{
		// Main class found
		if (!strcmp(pClass->m_pRecvTable->m_pNetTableName, path[0].c_str()))
		{
			RecvTable* curr_table = pClass->m_pRecvTable;
			for (size_t i = 1; i < path.size(); i++)
			{
				bool found = false;
				for (int j = 0; j < curr_table->m_nProps; j++)
				{
					RecvPropRedef* pProp = (RecvPropRedef*)&(curr_table->m_pProps[j]);
					if (!pProp)
						continue;
					if (!strcmp(path[i].c_str(), pProp->m_pVarName))
					{
						// Detect last iteration
						if (i == path.size() - 1)
						{
							hook.init(pProp);
							hook.setHook(function);
							return true;
						}
						curr_table = pProp->m_pDataTable;
						found = true;
					}
				}
				// We tried searching the netvar but found nothing
				if (!found)
				{
					std::string full_path;
					for (auto& s : path)
						full_path += s + "";
					std::string poop("Required member not found: " + full_path);
					char* poop2 = (char*)poop.c_str();
					WinAPI::MessageBoxA(0, poop2, _("Hooking netvar failed"), MB_ICONERROR);
					//logging::Info("Hooking netvar with path \"%s\" failed. Required member not found.");
					return false;
				}
			}
		}
		pClass = pClass->m_pNext;
	}
	return false;
}

void CHookManager::Release()
{
	MH_Uninitialize();
	SetWindowLongPtr(WndProc::hwWindow, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc::Original));
}

void CHookManager::Init()
{
	while (!g_dwDirectXDevice)
	{
		g_dwDirectXDevice = **reinterpret_cast<DWORD**>(g_Pattern.Find(_(L"shaderapidx9.dll"), _(L"A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 0x1);
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
		WinAPI::MessageBoxW(0, _(L"MH failed to enable all hooks!"), _(L"ERROR!"), MB_ICONERROR);
	}
}