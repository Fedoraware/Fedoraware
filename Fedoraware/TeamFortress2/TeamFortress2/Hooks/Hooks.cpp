#include "Hooks.h"
#include "../SDK/SDK.h"

inline uintptr_t GetVFuncPtr(void *pBaseClass, unsigned int nIndex) {
	return static_cast<uintptr_t>((*static_cast<int **>(pBaseClass))[nIndex]);
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
		//GetIconHook::Init();
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
		WinAPI::MessageBoxW(0, _(L"MH failed to enable all hooks!"), _(L"ERROR!"), MB_ICONERROR);
}

void CHooks::Release()
{
	MH_Uninitialize();
	SetWindowLongPtr(WndProc::hwWindow, GWL_WNDPROC, (LONG_PTR)WndProc::Original);
}