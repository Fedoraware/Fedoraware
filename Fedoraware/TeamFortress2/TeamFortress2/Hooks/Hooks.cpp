#include "Hooks.h"
#include "../SDK/SDK.h"
#include "../SDK/Includes/proxyfnhook.h"

inline uintptr_t GetVFuncPtr(void *pBaseClass, unsigned int nIndex) {
	return static_cast<uintptr_t>((*static_cast<int **>(pBaseClass))[nIndex]);
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

void CHooks::Init()
{
	MH_Initialize();
	{
		MenuHook::Init();
		Scoreboard::IsPlayerDominated::Init();
		//ResetHook::Init();
		FireBullets::Init();
		Viewmodel::Init();
		IsLocalPlayerUsingVisionFilterFlags::Init();
		GameMovement::Duck::Init();
		Ragdoll::Init();
		//GetIconHook::Init();
		ClientModeHook::SetAbsVelocity::Init();
		MaterialSystemHook::CMaterial::Uncache::Init();
	}

	if (g_Interfaces.Client)
	{
		using namespace ClientHook;

		Table.Init(g_Interfaces.Client);
		Table.Hook(PreEntity::index, &PreEntity::Hook);
		Table.Hook(PostEntity::index, &PostEntity::Hook);
		Table.Hook(ShutDown::index, &ShutDown::Hook);
		Table.Hook(FrameStageNotify::index, &FrameStageNotify::Hook);
		Table.Hook(DispatchUserMessage::index, &DispatchUserMessage::Hook);
	}

	if (g_Interfaces.ClientMode)
	{
		using namespace ClientModeHook;

		Table.Init(g_Interfaces.ClientMode);
		Table.Hook(OverrideView::index, &OverrideView::Hook);
		Table.Hook(ShouldDrawViewModel::index, &ShouldDrawViewModel::Hook);
		Table.Hook(CreateMove::index, &CreateMove::Hook);
		Table.Hook(DoPostScreenSpaceEffects::index, &DoPostScreenSpaceEffects::Hook);
		{
			using namespace StartMessageMode;

			static auto FN = reinterpret_cast<fn>(g_Pattern.Find(L"client.dll", L"55 8B EC A1 ? ? ? ? 83 EC 64"));
			Func.Hook(FN, Hook);
		}
	}

	if (g_Interfaces.Input) {
		using namespace InputHook;
		Table.Init(g_Interfaces.Input);
		Table.Hook(GetUserCmd::index, &GetUserCmd::Hook);
	}

	/*if (g_Interfaces.GameMovement)
	{
		using namespace GameMovement;
		Table.Init(g_Interfaces.GameMovement);
		Table.Hook(ProcessMovement::index, &ProcessMovement::Hook);
	}*/

	if (g_Interfaces.Prediction)
	{
		using namespace PredictionHook;

		Table.Init(g_Interfaces.Prediction);
		Table.Hook(RunCommand::index, &RunCommand::Hook);
	}

	if (g_Interfaces.Surface)
	{
		using namespace SurfaceHook;

		Table.Init(g_Interfaces.Surface);
		Table.Hook(OnScreenSizeChanged::index, &OnScreenSizeChanged::Hook);
		//Table.Hook(LockCursor::index, &LockCursor::Hook);
	}

	if (g_Interfaces.Panel)
	{
		using namespace PanelHook;
		
		Table.Init(g_Interfaces.Panel);
		Table.Hook(PaintTraverse::index, &PaintTraverse::Hook);
	}

	if (g_Interfaces.EngineVGui)
	{
		using namespace EngineVGuiHook;

		Table.Init(g_Interfaces.EngineVGui);
		Table.Hook(Paint::index, &Paint::Hook);
	}

	if (g_Interfaces.Engine)
	{
		using namespace EngineClientHook;

		Table.Init(g_Interfaces.Engine);
		Table.Hook(IsPlayingTimeDemo::index, &IsPlayingTimeDemo::Hook);
	}

	if (g_Interfaces.ModelRender)
	{
		using namespace ModelRenderHook;

		Table.Init(g_Interfaces.ModelRender);
		Table.Hook(DrawModelExecute::index, &DrawModelExecute::Hook);
		Table.Hook(ForcedMaterialOverride::index, &ForcedMaterialOverride::Hook);
	}

	if (g_Interfaces.UniformRandomStream)
	{
		using namespace UniformRandomStreamHook;

		Table.Init(g_Interfaces.UniformRandomStream);
		Table.Hook(RandInt::index, &RandInt::Hook);
	}

	if (g_Interfaces.GameEvent)
	{
		using namespace EventMangerHook;

		Table.Init(g_Interfaces.GameEvent);
		Table.Hook(FireEventClientSide::index, &FireEventClientSide::Hook);
	}

	if (g_Interfaces.ViewRender)
	{
		using namespace ViewRenderHook;

		{
			using namespace LevelInit;
			fn FN = reinterpret_cast<fn>(GetVFuncPtr(g_Interfaces.ViewRender, index));
			Func.Hook(FN, Hook);
		}
		{
			using namespace LevelShutdown;
			fn FN = reinterpret_cast<fn>(GetVFuncPtr(g_Interfaces.ViewRender, index));
			Func.Hook(FN, Hook);
		}
		{
			using namespace RenderView;
			fn FN = reinterpret_cast<fn>(GetVFuncPtr(g_Interfaces.ViewRender, index));
			Func.Hook(FN, Hook);
		}

	}

	while (!m_hwWindow) {
		m_hwWindow = WinAPI::FindWindowW(0, _(L"Team Fortress 2"));
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	//Inventory expander
	{
		CTFInventoryManager* InventoryManager = nullptr;
		InventoryManager = CTFInventoryManager::Get();

		CTFPlayerInventory* playerinventory = nullptr;
		playerinventory = InventoryManager->GetPlayerInventory();

		uintptr_t maxItems = GetVFuncPtr(playerinventory, 9);

		using namespace InventoryExpander;

		Func.Hook((reinterpret_cast<void*>(maxItems)), reinterpret_cast<void*>(Expand));
	}

	//WndProcHook::WndProc = (WNDPROC)SetWindowLongPtr(m_hwWindow, GWL_WNDPROC, (LONG_PTR)WndProcHook::Hook);

	//EngineHook
	{
		using namespace EngineHook;

		//CL_Move
		{
			using namespace CL_Move;

			fn CLMove = reinterpret_cast<fn>(g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC ? 83 3D ? ? ? ? 02 0F 8C ? ? 00 00 E8 ? ? ? 00 84 C0"));
			Func.Hook(reinterpret_cast<void*>(CLMove), reinterpret_cast<void*>(Hook));
		}

		//CL_SendMove
		{
			using namespace CL_SendMove;

			fn CLSendMove = reinterpret_cast<fn>(g_Pattern.Find(L"engine.dll", L"55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 8D"));
			Func.Hook(CLSendMove, Hook);
		}

		//CL_FireEvents
		{
			using namespace CL_FireEvents;

			DWORD dwCLFireEvents = g_Pattern.Find(L"engine.dll", L"E8 ? ? ? ? 83 EC 10") + 0x1;
			DWORD dwEstimate = *(PDWORD)dwCLFireEvents + dwCLFireEvents + 4;
			fn CLFireEvents = reinterpret_cast<fn>(dwEstimate);
			Func.Hook(CLFireEvents, Hook);
		}

		//Q_stricmp
		{
			using namespace Q_stricmp;

			DWORD dwQ_stricmp = g_Pattern.Find(L"engine.dll", L"E8 ? ? ? ? 83 C4 08 33 D2") + 0x1;
			DWORD dwFunc = *(PDWORD)dwQ_stricmp + dwQ_stricmp + 4;
			fn Qstricmp = reinterpret_cast<fn>(dwFunc);
			//Func.Hook(Qstricmp, Hook);
		}

		//UpdateNameFromSteamID
		{
			using namespace UpdateNameFromSteamID;

			DWORD dwUpdateNameFromSteamID = g_Pattern.Find(L"engine.dll", L"E8 ? ? ? ? 83 C4 08 8B 45 FC") + 0x1;
			DWORD dwFunc = *(PDWORD)dwUpdateNameFromSteamID + dwUpdateNameFromSteamID + 4;
			Func.Hook((fn)dwFunc, Hook);
		}

		//CL_NameCvarChanged
		{
			using namespace CL_NameCvarChanged;

			fn dwA = reinterpret_cast<fn>(g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC 30 8D 4D F8"));
			Func.Hook(dwA, Hook);
		}
	}

	//EndSceneHook::Init();

	//GetDrawPosition
	{
		using namespace GetDrawPositionHook;

		fn GetDrawPosition = reinterpret_cast<fn>(
			g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 81 EC ? ? ? ? 53 57 E8 ? ? ? ? 8B 08 89 4D C8 8B 48 04 8B 40 08")));

		Func.Hook(reinterpret_cast<void *>(GetDrawPosition), reinterpret_cast<void *>(Hook));
	}

	//PerformScreenSpaceEffects
	{
		using namespace PerformScreenSpaceEffects;

		DWORD dwAddy = g_Pattern.Find(
			_(L"client.dll"),
			_(L"55 8B EC 83 EC ? 8B 0D ? ? ? ? 53 56 57 33 F6 33 FF 89 75 ? 89 7D ? 8B 01 85 C0 74 ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 57 57 57 57 8D 4D ? 51 50 8B 40 ? FF D0 8B 7D ? 83 C4 ? 8B 75 ? 8B 0D ? ? ? ? 8B 19 8B 0D"));

		Func.Hook(reinterpret_cast<void *>( dwAddy ), reinterpret_cast<void *>(Hook));
	}

	//InCond
	{
		using namespace InCondHook;

		fn InCond = reinterpret_cast<fn>(
			g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 08 56 57 8B 7D 08 8B F1 83 FF 20")));

		Func.Hook(reinterpret_cast<void*>(InCond), reinterpret_cast<void*>(Hook));
	}

	//DrawStaticProps
	{
		using namespace DrawStaticPropsHook;

		fn DrawStaticProps = reinterpret_cast<fn>(
			g_Pattern.Find(_(L"engine.dll"), _(L"55 8B EC 83 EC 10 8B 15 ? ? ? ? 53 56 57 33 F6 89 4D FC 33 FF 89 75 F0 89 7D F4 8B 42 08")));

		Func.Hook(reinterpret_cast<void *>(DrawStaticProps), reinterpret_cast<void *>(Hook));
	}

	//SetColorModulation
	{
		using namespace SetColorModulationHook;

		fn SetColorModulation = reinterpret_cast<fn>(GetVFuncPtr(g_Interfaces.StudioRender, 27));

		Func.Hook(reinterpret_cast<void *>(SetColorModulation), reinterpret_cast<void *>(Hook));
	}

	// I FORGOR
	{
		using namespace SetAlphaModulationHook;

		fn SetAlphaModulation = reinterpret_cast<fn>(GetVFuncPtr(g_Interfaces.StudioRender, 28));

		Func.Hook(reinterpret_cast<void*>(SetAlphaModulation), reinterpret_cast<void*>(Hook));
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_STATUS::MH_OK)
	{
		WinAPI::MessageBoxW(0, _(L"MH failed to enable all hooks!"), _(L"ERROR!"), MB_ICONERROR);
	}
}

void CHooks::Release()
{
	MH_Uninitialize();
	SetWindowLongPtr(WndProc::hwWindow, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc::Original));
}