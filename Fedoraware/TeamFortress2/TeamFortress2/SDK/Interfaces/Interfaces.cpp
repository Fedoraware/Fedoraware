#include "Interfaces.h"

#define _valid(x) if (!x) WinAPI::MessageBoxW(0, L#x, L"CInterfaces::Init() -> nullptr", MB_ICONERROR)
#define _validS(x) if (!x) WinAPI::MessageBoxW(0, L#x, L"CSteamInterfaces::Init() -> nullptr", MB_ICONERROR)

#define CLIENT L"client.dll"
#define STEAMCLIENT L"steamclient.dll"
#define ENGINE L"engine.dll"
#define VGUI2 L"vgui2.dll"
#define MATSURFACE L"vguimatsurface.dll"
#define VSTDLIB L"vstdlib.dll"
#define MATSYSTEM L"materialsystem.dll"


void CInterfaces::Init()
{
	using namespace I;

	Client = g_Interface.Get<CBaseClientDLL*>(CLIENT, CLIENT_DLL_INTERFACE_VERSION);
	_valid(Client);

	ClientShared = g_Interface.Get<CClientDLLSharedAppSystems*>(CLIENT, CLIENT_DLL_SHARED_APPSYSTEMS);
	_valid(ClientShared);

	EntityList = g_Interface.Get<CClientEntityList*>(CLIENT, VCLIENTENTITYLIST_INTERFACE_VERSION);
	_valid(EntityList);

	Prediction = g_Interface.Get<CPrediction*>(CLIENT, VCLIENT_PREDICTION_INTERFACE_VERSION);
	_valid(Prediction);

	GameMovement = g_Interface.Get<CGameMovement*>(CLIENT, CLIENT_GAMEMOVEMENT_INTERFACE_VERSION);
	_valid(GameMovement);

	ModelInfo = g_Interface.Get<CModelInfoClient*>(ENGINE, VMODELINFO_CLIENT_INTERFACE_VERSION);
	_valid(ModelInfo);

	Engine = g_Interface.Get<CEngineClient*>(ENGINE, VENGINE_CLIENT_INTERFACE_VERSION_13);
	_valid(Engine);

	EngineEffects = g_Interface.Get<IVEngineEffects*>(ENGINE, VENGINE_EFFECTS_INTERFACE_VERSION);
	_valid(EngineEffects);

	EngineTrace = g_Interface.Get<CEngineTrace*>(ENGINE, VENGINE_TRACE_CLIENT_INTERFACE_VERSION);
	_valid(EngineTrace);

	Panel = g_Interface.Get<CPanel*>(VGUI2, VGUI_PANEL_INTERFACE_VERSION);
	_valid(Panel);

	Surface = g_Interface.Get<CSurface*>(MATSURFACE, VGUI_SURFACE_INTERFACE_VERSION);
	_valid(Surface);

	CVars = g_Interface.Get<ICvar*>(VSTDLIB, VENGINE_CVAR_INTERFACE_VERSION);
	_valid(CVars);

	GlobalVars = *reinterpret_cast<CGlobalVarsBase**>(g_Pattern.Find(ENGINE, L"A1 ? ? ? ? 8B 11 68") + 0x8);
	_valid(GlobalVars);

	ClientState = *reinterpret_cast<CClientState**>(g_Pattern.Find(ENGINE, L"68 ? ? ? ? E8 ? ? ? ? 83 C4 08 5F 5E 5B 5D C3") + 0x1);
	_valid(ClientState);

	void* ClientTable = reinterpret_cast<void*>(g_Pattern.Find(CLIENT, L"8B 0D ? ? ? ? 8B 02 D9 05"));
	_valid(ClientTable);

	ClientMode = **reinterpret_cast<CClientModeShared***>(reinterpret_cast<DWORD>(ClientTable) + 2);
	_valid(ClientMode);

	EngineVGui = g_Interface.Get<CEngineVGui*>(ENGINE, VENGINE_VGUI_VERSION);
	_valid(EngineVGui);

	DemoPlayer = **reinterpret_cast<void***>(g_Pattern.Find(ENGINE, L"8B 0D ? ? ? ? 85 C9 74 3B 8B 01 8B 40 18 FF D0 84 C0 74 30") + 0x2);
	_valid(DemoPlayer);

	RenderView = g_Interface.Get<IVRenderView*>(ENGINE, VENGINE_RENDERVIEW_INTERFACE_VERSION);
	_valid(RenderView);

	DebugOverlay = g_Interface.Get<CDebugOverlay*>(ENGINE, VENGINE_DEBUGOVERLAY_INTERFACE_VERSION);
	_valid(RenderView);

	GameEvent = g_Interface.Get<CGameEventManager*>(ENGINE, GAMEEVENTSMANAGER_ENGINE_INTERFACE);
	_valid(GameEvent);

	ModelRender = g_Interface.Get<CModelRender*>(ENGINE, VENGINE_MODELRENDER_INTERFACE);
	_valid(ModelRender);

	MatSystem = g_Interface.Get<CMaterialSystem*>(MATSYSTEM, VMATERIALSYSTEM_INTERFACE);
	_valid(MatSystem);

	TFGCClientSystem = *reinterpret_cast<CTFGCClientSystem**>(g_Pattern.Find(CLIENT, L"B9 ? ? ? ? 50 E8 ? ? ? ? 8B 5D F8") + 0x1);
	_valid(TFGCClientSystem);

	TFPartyClient = reinterpret_cast<CTFPartyClient*(__cdecl*)()>(g_Pattern.E8(CLIENT, L"E8 ? ? ? ? FF 70 24"))();
	_valid(TFPartyClient);

	TFInventoryManager = *reinterpret_cast<CTFInventoryManager**>(g_Pattern.Find(CLIENT, L"B9 ? ? ? ? E8 ? ? ? ? B9 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ?") + 0x1);
	_valid(TFInventoryManager);

	RandomSeed = *reinterpret_cast<int32_t**>(g_Pattern.Find(CLIENT, L"C7 05 ? ? ? ? ? ? ? ? 5D C3 8B 40 34") + 0x2);
	_valid(RandomSeed);

	AllowSecureServers = *reinterpret_cast<bool**>(g_Pattern.Find(ENGINE, L"C6 05 ? ? ? ? ? 8A C3") + 0x2);
	_valid(AllowSecureServers);

	auto pdwClient = reinterpret_cast<PDWORD>(Client);     _valid(pdwClient);
	auto pdwTable = *reinterpret_cast<PDWORD*>(pdwClient); _valid(pdwTable);

	if (pdwTable)
		ViewRender = **reinterpret_cast<IViewRender***>(static_cast<DWORD>(pdwTable[27]) + 5);
	_valid(ViewRender);

	Input =	**reinterpret_cast<IInput***>(g_Pattern.Find(CLIENT, L"8B 0D ? ? ? ? 56 8B 01 FF 50 24 8B 45 FC") + 0x2);
	_valid(Input);

	auto GetKeyValuesSystem = [&]() -> IKeyValuesSystem* {
		static auto fn = reinterpret_cast<IKeyValuesSystem*(__cdecl *)()>(reinterpret_cast<DWORD>(WinAPI::GetProcessAddr(reinterpret_cast<DWORD>(WinAPI::GetModuleHandleW(VSTDLIB)), "KeyValuesSystem")));
		return fn();
	};

	KeyValuesSystem = GetKeyValuesSystem();
	_valid(KeyValuesSystem);

	UniformRandomStream = *reinterpret_cast<IUniformRandomStream **>(g_Pattern.Find(VSTDLIB, L"B9 ? ? ? ? 85 C0 0F 45 C8 89 0D ? ? ? ? 5D C3") + 0x1);
	_valid(UniformRandomStream);

	StudioRender = g_Interface.Get<void *>(L"studiorender.dll", "VStudioRender025");
	_valid(StudioRender);

	InputSystem = reinterpret_cast<IInputSystem*>(g_Interface.Get<IInputSystem*>(L"inputsystem.dll", "InputSystemVersion001"));
	_valid(InputSystem);

	EffectsClient = g_Interface.Get<CEffectsClient*>(CLIENT, IEFFECTS_INTERFACE_VERSION);
	_valid(EffectsClient);

	typedef IAchievementMgr* (*getachievementmgr)();

	AchievementMgr = reinterpret_cast<IAchievementMgr*>(GetVFunc<getachievementmgr>(I::Engine, 115));
	_valid(AchievementMgr);

	ViewRenderBeams = **reinterpret_cast<IViewRenderBeams***>(g_Pattern.Find(L"client.dll", L"8B 0D ? ? ? ? 56 8B 01 FF 50 18 0F B7 96 ? ? ? ?") + 0x2);
	_valid(ViewRenderBeams);

	EngineSound = g_Interface.Get<IEngineSound*>(L"engine.dll", "IEngineSoundClient003");
	_valid(EngineSound);

	TFGameRules = *reinterpret_cast<CTFGameRules**>(g_Pattern.Find(L"client.dll", L"8B 0D ? ? ? ? 56 8B 01 8B 80 ? ? ? ? FF D0 84 C0 0F 84 ? ? ? ? 80 BB ? ? ? ? ?") + 0x1);
	_valid(TFGameRules);

	ThirdPersonManager = *reinterpret_cast<CThirdPersonManager**>(g_Pattern.Find(L"client.dll", L"B9 ? ? ? ? E8 ? ? ? ? 84 C0 74 42 8B 86") + 0x1);
	_valid(ThirdPersonManager);

	// Forgive the double cast but this was annoying meeeeee
	DirectXDevice = reinterpret_cast<IDirect3DDevice9*>(**reinterpret_cast<DWORD**>(g_Pattern.Find(L"shaderapidx9.dll", L"A1 ? ? ? ? 50 8B 08 FF 51 0C") + 0x1));
	_valid(DirectXDevice);
}

void CSteamInterfaces::Init()
{
	Client = g_Interface.Get<ISteamClient017*>(STEAMCLIENT, STEAMCLIENT_INTERFACE_VERSION_017);
	_validS(Client);

	HSteamPipe hsNewPipe = Client->CreateSteamPipe();              _validS(hsNewPipe);
	HSteamPipe hsNewUser = Client->ConnectToGlobalUser(hsNewPipe); _validS(hsNewUser);

	Friends002 = reinterpret_cast<ISteamFriends002*>(Client->GetISteamFriends(hsNewUser, hsNewPipe, STEAMFRIENDS_INTERFACE_VERSION_002));
	_validS(Friends002);
	
	Friends015 = reinterpret_cast<ISteamFriends015*>(Client->GetISteamFriends(hsNewUser, hsNewPipe, STEAMFRIENDS_INTERFACE_VERSION_015));
	_validS(Friends015);
	
	Utils007   = reinterpret_cast<ISteamUtils007*>(Client->GetISteamUtils(hsNewUser, STEAMUTILS_INTERFACE_VERSION_007));
	_validS(Utils007);
	
	SteamApps  = reinterpret_cast<ISteamApps006*>(Client->GetISteamApps(hsNewUser, hsNewPipe, STEAMAPPS_INTERFACE_VERSION_006));
	_validS(SteamApps);
	
	UserStats  = reinterpret_cast<ISteamUserStats011*>(Client->GetISteamUserStats(hsNewUser, hsNewPipe, STEAMUSERSTATS_INTERFACE_VERSION_011));
	_validS(UserStats);
	
	User       = reinterpret_cast<ISteamUser017*>(Client->GetISteamUser(hsNewUser, hsNewPipe, STEAMUSER_INTERFACE_VERSION_017));
	_validS(User);

	// Credits to spook953 for teaching me how this works
	static auto fn = reinterpret_cast<ISteamNetworkingUtils*(__cdecl*)()>(GetProcAddress(GetModuleHandleA("steamnetworkingsockets.dll"), "SteamNetworkingUtils_LibV4"));
	NetworkingUtils = fn();
	_validS(NetworkingUtils);

	/*Networking = reinterpret_cast<ISteamNetworking004*>(Client->GetISteamNetworking(hsNewUser, hsNewPipe, _(STEAMNETWORKINGUTILS_INTERFACE_VERSION)));
	_validS(Networking);*/
}