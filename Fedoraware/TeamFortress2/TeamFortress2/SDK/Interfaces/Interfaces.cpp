#include "Interfaces.h"

#define _valid(x) if (!x) WinAPI::MessageBoxW(0, _(L#x), _(L"CInterfaces::Init() -> nullptr"), MB_ICONERROR)
#define _validS(x) if (!x) WinAPI::MessageBoxW(0, _(L#x), _(L"CSteamInterfaces::Init() -> nullptr"), MB_ICONERROR)

#define CLIENT _(L"client.dll")
#define STEAMCLIENT _(L"steamclient.dll")
#define ENGINE _(L"engine.dll")
#define VGUI2 _(L"vgui2.dll")
#define MATSURFACE _(L"vguimatsurface.dll")
#define VSTDLIB _(L"vstdlib.dll")
#define MATSYSTEM _(L"materialsystem.dll")


void CInterfaces::Init()
{
	Client = g_Interface.Get<CBaseClientDLL*>(CLIENT, _(CLIENT_DLL_INTERFACE_VERSION));
	_valid(Client);

	ClientShared = g_Interface.Get<CClientDLLSharedAppSystems*>(CLIENT, _(CLIENT_DLL_SHARED_APPSYSTEMS));
	_valid(ClientShared);

	EntityList = g_Interface.Get<CClientEntityList*>(CLIENT, _(VCLIENTENTITYLIST_INTERFACE_VERSION));
	_valid(EntityList);

	Prediction = g_Interface.Get<CPrediction*>(CLIENT, _(VCLIENT_PREDICTION_INTERFACE_VERSION));
	_valid(Prediction);

	GameMovement = g_Interface.Get<CGameMovement*>(CLIENT, _(CLIENT_GAMEMOVEMENT_INTERFACE_VERSION));
	_valid(GameMovement);

	ModelInfo = g_Interface.Get<CModelInfoClient*>(ENGINE, _(VMODELINFO_CLIENT_INTERFACE_VERSION));
	_valid(ModelInfo);

	Engine = g_Interface.Get<CEngineClient*>(ENGINE, _(VENGINE_CLIENT_INTERFACE_VERSION_13));
	_valid(Engine);

	EngineEffects = g_Interface.Get<IVEngineEffects*>(ENGINE, _(VENGINE_EFFECTS_INTERFACE_VERSION));
	_valid(EngineEffects);

	EngineTrace = g_Interface.Get<CEngineTrace*>(ENGINE, _(VENGINE_TRACE_CLIENT_INTERFACE_VERSION));
	_valid(EngineTrace);

	Panel = g_Interface.Get<CPanel*>(VGUI2, _(VGUI_PANEL_INTERFACE_VERSION));
	_valid(Panel);

	Surface = g_Interface.Get<CSurface*>(MATSURFACE, _(VGUI_SURFACE_INTERFACE_VERSION));
	_valid(Surface);

	CVars = g_Interface.Get<ICvar*>(VSTDLIB, _(VENGINE_CVAR_INTERFACE_VERSION));
	_valid(CVars);

	GlobalVars = *reinterpret_cast<CGlobalVarsBase**>(g_Pattern.Find(ENGINE, _(L"A1 ? ? ? ? 8B 11 68")) + 0x8);
	_valid(GlobalVars);

	ClientState = *reinterpret_cast<CClientState**>(g_Pattern.Find(ENGINE, _(L"68 ? ? ? ? E8 ? ? ? ? 83 C4 08 5F 5E 5B 5D C3")) + 0x1);
	_valid(ClientState);

	void* ClientTable = reinterpret_cast<void*>(g_Pattern.Find(CLIENT, _(L"8B 0D ? ? ? ? 8B 02 D9 05")));
	_valid(ClientTable);

	ClientMode = **reinterpret_cast<CClientModeShared***>(reinterpret_cast<DWORD>(ClientTable) + 2);
	_valid(ClientMode);

	EngineVGui = g_Interface.Get<CEngineVGui*>(ENGINE, _(VENGINE_VGUI_VERSION));
	_valid(EngineVGui);

	RandomSeed = *reinterpret_cast<int32_t**>(g_Pattern.Find(CLIENT, _(L"C7 05 ? ? ? ? ? ? ? ? 5D C3 8B 40 34")) + 0x2);
	_valid(RandomSeed);

	DemoPlayer = **reinterpret_cast<void***>(g_Pattern.Find(ENGINE, _(L"8B 0D ? ? ? ? 85 C9 74 3B 8B 01 8B 40 18 FF D0 84 C0 74 30")) + 0x2);
	_valid(DemoPlayer);

	RenderView = g_Interface.Get<IVRenderView*>(ENGINE, _(VENGINE_RENDERVIEW_INTERFACE_VERSION));
	_valid(RenderView);

	DebugOverlay = g_Interface.Get<CDebugOverlay*>(ENGINE, _(VENGINE_DEBUGOVERLAY_INTERFACE_VERSION));
	_valid(RenderView);

	GameEvent = g_Interface.Get<CGameEventManager*>(ENGINE, _(GAMEEVENTSMANAGER_ENGINE_INTERFACE));
	_valid(GameEvent);

	ModelRender = g_Interface.Get<CModelRender*>(ENGINE, _(VENGINE_MODELRENDER_INTERFACE));
	_valid(ModelRender);

	MatSystem = g_Interface.Get<CMaterialSystem*>(MATSYSTEM, _(VMATERIALSYSTEM_INTERFACE));
	_valid(MatSystem);

	auto pdwClient = reinterpret_cast<PDWORD>(Client);     _valid(pdwClient);
	auto pdwTable = *reinterpret_cast<PDWORD*>(pdwClient); _valid(pdwTable);

	if (pdwTable)
		ViewRender = **reinterpret_cast<IViewRender***>(static_cast<DWORD>(pdwTable[27]) + 5);
	_valid(ViewRender);

	Input =	**reinterpret_cast<IInput***>(
		g_Pattern.Find(CLIENT, _(L"8B 0D ? ? ? ? 56 8B 01 FF 50 24 8B 45 FC")) + 0x2);
	_valid(Input);

	auto GetKeyValuesSystem = [&]() -> IKeyValuesSystem* {
		static auto fn = reinterpret_cast<IKeyValuesSystem*(__cdecl *)()>(reinterpret_cast<DWORD>(WinAPI::GetProcessAddr(reinterpret_cast<DWORD>(WinAPI::GetModuleHandleW(VSTDLIB)), _("KeyValuesSystem"))));
		return fn();
	};

	KeyValuesSystem = GetKeyValuesSystem();
	_valid(KeyValuesSystem);

	UniformRandomStream = *reinterpret_cast<IUniformRandomStream **>(g_Pattern.Find(VSTDLIB, _(L"B9 ? ? ? ? 85 C0 0F 45 C8 89 0D ? ? ? ? 5D C3")) + 0x1);
	_valid(UniformRandomStream);

	StudioRender = g_Interface.Get<void *>(_(L"studiorender.dll"), _("VStudioRender025"));
	_valid(StudioRender);

	InputSystem = reinterpret_cast<IInputSystem*>(g_Interface.Get<IInputSystem*>(_(L"inputsystem.dll"), _("InputSystemVersion001")));
	_valid(InputSystem);

	EffectsClient = g_Interface.Get<CEffectsClient*>(CLIENT, _(IEFFECTS_INTERFACE_VERSION));
	_valid(EffectsClient);

	typedef IAchievementMgr* (*getachievementmgr)();

	AchievementMgr = reinterpret_cast<IAchievementMgr*>(GetVFunc<getachievementmgr>(g_Interfaces.Engine, 115));
	_valid(AchievementMgr);

	ILOVEBEAMS = **reinterpret_cast<IViewRenderBeams***>(g_Pattern.Find(L"client.dll", L"8B 0D ? ? ? ? 56 8B 01 FF 50 18 0F B7 96 ? ? ? ?") + 0x2);
	_valid(ILOVEBEAMS);
}

void CSteamInterfaces::Init()
{
	Client = g_Interface.Get<ISteamClient017*>(STEAMCLIENT, _(STEAMCLIENT_INTERFACE_VERSION_017));
	_validS(Client);

	HSteamPipe hsNewPipe = Client->CreateSteamPipe();              _validS(hsNewPipe);
	HSteamPipe hsNewUser = Client->ConnectToGlobalUser(hsNewPipe); _validS(hsNewUser);

	Friends002 = reinterpret_cast<ISteamFriends002*>(Client->GetISteamFriends(hsNewUser, hsNewPipe, _(STEAMFRIENDS_INTERFACE_VERSION_002)));
	_validS(Friends002);
	
	Friends015 = reinterpret_cast<ISteamFriends015*>(Client->GetISteamFriends(hsNewUser, hsNewPipe, _(STEAMFRIENDS_INTERFACE_VERSION_015)));
	_validS(Friends015);
	
	Utils007   = reinterpret_cast<ISteamUtils007*>(Client->GetISteamUtils(hsNewUser, _(STEAMUTILS_INTERFACE_VERSION_007)));
	_validS(Utils007);
	
	SteamApps  = reinterpret_cast<ISteamApps006*>(Client->GetISteamApps(hsNewUser, hsNewPipe, _(STEAMAPPS_INTERFACE_VERSION_006)));
	_validS(SteamApps);
	
	UserStats  = reinterpret_cast<ISteamUserStats011*>(Client->GetISteamUserStats(hsNewUser, hsNewPipe, _(STEAMUSERSTATS_INTERFACE_VERSION_011)));
	_validS(UserStats);
	
	User       = reinterpret_cast<ISteamUser017*>(Client->GetISteamUser(hsNewUser, hsNewPipe, _(STEAMUSER_INTERFACE_VERSION_017)));
	_validS(User);

	/*Networking = reinterpret_cast<ISteamNetworking004*>(Client->GetISteamNetworking(hsNewUser, hsNewPipe, _(STEAMNETWORKINGUTILS_INTERFACE_VERSION)));
	_validS(Networking);*/
}