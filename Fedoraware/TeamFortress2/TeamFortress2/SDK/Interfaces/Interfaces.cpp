#include "Interfaces.h"

#define VALIDATE(x) if (!(x)) { MessageBoxA(0, #x, "CInterfaces::Init() -> nullptr", MB_ICONERROR); };
#define VALIDATE_STEAM(x) if (!(x)) { MessageBoxA(0, #x, "CSteamInterfaces::Init() -> nullptr", MB_ICONERROR); }

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

	BaseClientDLL = g_Interface.Get<CBaseClientDLL*>(CLIENT, CLIENT_DLL_INTERFACE_VERSION);
	VALIDATE(BaseClientDLL)

	ClientDLLSharedAppSystems = g_Interface.Get<CClientDLLSharedAppSystems*>(CLIENT, CLIENT_DLL_SHARED_APPSYSTEMS);
	VALIDATE(ClientDLLSharedAppSystems)

	ClientEntityList = g_Interface.Get<CClientEntityList*>(CLIENT, VCLIENTENTITYLIST_INTERFACE_VERSION);
	VALIDATE(ClientEntityList)

	Prediction = g_Interface.Get<CPrediction*>(CLIENT, VCLIENT_PREDICTION_INTERFACE_VERSION);
	VALIDATE(Prediction)

	GameMovement = g_Interface.Get<CGameMovement*>(CLIENT, CLIENT_GAMEMOVEMENT_INTERFACE_VERSION);
	VALIDATE(GameMovement)

	CenterPrint = g_Interface.Get<ICenterPrint*>(CLIENT, VCENTERPRINT_INTERFACE_VERSION);
	VALIDATE(CenterPrint)

	ModelInfoClient = g_Interface.Get<CModelInfoClient*>(ENGINE, VMODELINFO_CLIENT_INTERFACE_VERSION);
	VALIDATE(ModelInfoClient)

	EngineClient = g_Interface.Get<CEngineClient*>(ENGINE, VENGINE_CLIENT_INTERFACE_VERSION_13);
	VALIDATE(EngineClient)

	EngineEffects = g_Interface.Get<IVEngineEffects*>(ENGINE, VENGINE_EFFECTS_INTERFACE_VERSION);
	VALIDATE(EngineEffects)

	EngineTrace = g_Interface.Get<CEngineTrace*>(ENGINE, VENGINE_TRACE_CLIENT_INTERFACE_VERSION);
	VALIDATE(EngineTrace)

	VGuiPanel = g_Interface.Get<CPanel*>(VGUI2, VGUI_PANEL_INTERFACE_VERSION);
	VALIDATE(VGuiPanel)

	VGuiSurface = g_Interface.Get<CSurface*>(MATSURFACE, VGUI_SURFACE_INTERFACE_VERSION);
	VALIDATE(VGuiSurface)

	Cvar = g_Interface.Get<ICvar*>(VSTDLIB, VENGINE_CVAR_INTERFACE_VERSION);
	VALIDATE(Cvar)

	GlobalVars = *reinterpret_cast<CGlobalVarsBase**>(g_Pattern.Find(ENGINE, L"A1 ? ? ? ? 8B 11 68") + 0x8);
	VALIDATE(GlobalVars)

	ClientState = *reinterpret_cast<CClientState**>(g_Pattern.Find(ENGINE, L"68 ? ? ? ? E8 ? ? ? ? 83 C4 08 5F 5E 5B 5D C3") + 0x1);
	VALIDATE(ClientState)

	void* ClientTable = reinterpret_cast<void*>(g_Pattern.Find(CLIENT, L"8B 0D ? ? ? ? 8B 02 D9 05"));
	VALIDATE(ClientTable)

	ClientModeShared = **reinterpret_cast<CClientModeShared***>(reinterpret_cast<DWORD>(ClientTable) + 2);
	VALIDATE(ClientModeShared)

	EngineVGui = g_Interface.Get<CEngineVGui*>(ENGINE, VENGINE_VGUI_VERSION);
	VALIDATE(EngineVGui)

	DemoPlayer = **reinterpret_cast<void***>(g_Pattern.Find(ENGINE, L"8B 0D ? ? ? ? 85 C9 74 3B 8B 01 8B 40 18 FF D0 84 C0 74 30") + 0x2);
	VALIDATE(DemoPlayer)

	RenderView = g_Interface.Get<IVRenderView*>(ENGINE, VENGINE_RENDERVIEW_INTERFACE_VERSION);
	VALIDATE(RenderView)

	DebugOverlay = g_Interface.Get<CDebugOverlay*>(ENGINE, VENGINE_DEBUGOVERLAY_INTERFACE_VERSION);
	VALIDATE(RenderView)

	GameEventManager = g_Interface.Get<CGameEventManager*>(ENGINE, GAMEEVENTSMANAGER_ENGINE_INTERFACE);
	VALIDATE(GameEventManager)

	ModelRender = g_Interface.Get<CModelRender*>(ENGINE, VENGINE_MODELRENDER_INTERFACE);
	VALIDATE(ModelRender)

	MaterialSystem = g_Interface.Get<CMaterialSystem*>(MATSYSTEM, VMATERIALSYSTEM_INTERFACE);
	VALIDATE(MaterialSystem)

	TFGCClientSystem = *reinterpret_cast<CTFGCClientSystem**>(g_Pattern.Find(CLIENT, L"B9 ? ? ? ? 50 E8 ? ? ? ? 8B 5D F8") + 0x1);
	VALIDATE(TFGCClientSystem)

	TFPartyClient = reinterpret_cast<CTFPartyClient * (__cdecl*)()>(g_Pattern.E8(CLIENT, L"E8 ? ? ? ? FF 70 24"))();
	VALIDATE(TFPartyClient)

	TFInventoryManager = *reinterpret_cast<CTFInventoryManager**>(g_Pattern.Find(CLIENT, L"B9 ? ? ? ? E8 ? ? ? ? B9 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ?") + 0x1);
	VALIDATE(TFInventoryManager)

	RandomSeed = *reinterpret_cast<int32_t**>(g_Pattern.Find(CLIENT, L"C7 05 ? ? ? ? ? ? ? ? 5D C3 8B 40 34") + 0x2);
	VALIDATE(RandomSeed)

	AllowSecureServers = *reinterpret_cast<bool**>(g_Pattern.Find(ENGINE, L"C6 05 ? ? ? ? ? 8A C3") + 0x2);
	VALIDATE(AllowSecureServers)

	const auto pdwClient = reinterpret_cast<PDWORD>(BaseClientDLL);     VALIDATE(pdwClient);
	const auto pdwTable = *reinterpret_cast<PDWORD*>(pdwClient); VALIDATE(pdwTable);

	if (pdwTable)
		ViewRender = **reinterpret_cast<IViewRender***>(static_cast<DWORD>(pdwTable[27]) + 5);
	VALIDATE(ViewRender)

	Input = **reinterpret_cast<IInput***>(g_Pattern.Find(CLIENT, L"8B 0D ? ? ? ? 56 8B 01 FF 50 24 8B 45 FC") + 0x2);
	VALIDATE(Input)

	auto GetKeyValuesSystem = [&]() -> IKeyValuesSystem*
	{
		static auto fn = reinterpret_cast<IKeyValuesSystem * (__cdecl*)()>(reinterpret_cast<DWORD>(GetProcAddress(GetModuleHandleW(VSTDLIB), "KeyValuesSystem")));
		return fn();
	};

	KeyValuesSystem = GetKeyValuesSystem();
	VALIDATE(KeyValuesSystem)

	UniformRandomStream = *reinterpret_cast<IUniformRandomStream**>(g_Pattern.Find(VSTDLIB, L"B9 ? ? ? ? 85 C0 0F 45 C8 89 0D ? ? ? ? 5D C3") + 0x1);
	VALIDATE(UniformRandomStream)

	StudioRender = g_Interface.Get<void*>(L"studiorender.dll", "VStudioRender025");
	VALIDATE(StudioRender)

	InputSystem = g_Interface.Get<IInputSystem*>(L"inputsystem.dll", "InputSystemVersion001");
	VALIDATE(InputSystem)

	EffectsClient = g_Interface.Get<CEffectsClient*>(CLIENT, IEFFECTS_INTERFACE_VERSION);
	VALIDATE(EffectsClient)

	typedef IAchievementMgr* (*getachievementmgr)();

	AchievementMgr = reinterpret_cast<IAchievementMgr*>(GetVFunc<getachievementmgr>(I::EngineClient, 115));
	VALIDATE(AchievementMgr)

	ViewRenderBeams = **reinterpret_cast<IViewRenderBeams***>(g_Pattern.Find(L"client.dll", L"8B 0D ? ? ? ? 56 8B 01 FF 50 18 0F B7 96 ? ? ? ?") + 0x2);
	VALIDATE(ViewRenderBeams)

	EngineSound = g_Interface.Get<IEngineSound*>(L"engine.dll", "IEngineSoundClient003");
	VALIDATE(EngineSound)

	TFGameRules = *reinterpret_cast<CTFGameRules**>(g_Pattern.Find(L"client.dll", L"8B 0D ? ? ? ? 56 8B 01 8B 80 ? ? ? ? FF D0 84 C0 0F 84 ? ? ? ? 80 BB ? ? ? ? ?") + 0x1);
	VALIDATE(TFGameRules)

	ThirdPersonManager = *reinterpret_cast<CThirdPersonManager**>(g_Pattern.Find(L"client.dll", L"B9 ? ? ? ? E8 ? ? ? ? 84 C0 74 42 8B 86") + 0x1);
	VALIDATE(ThirdPersonManager)

	// Forgive the double cast but this was annoying meeeeee
	DirectXDevice = reinterpret_cast<IDirect3DDevice9*>(**reinterpret_cast<DWORD**>(g_Pattern.Find(L"shaderapidx9.dll", L"A1 ? ? ? ? 50 8B 08 FF 51 0C") + 0x1));
	VALIDATE(DirectXDevice)

	ClientModeTF = *reinterpret_cast<ClientModeTFNormal**>(g_Pattern.Find(L"client.dll", L"B9 ? ? ? ? A3 ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 83 C4 04 8B 35 ? ? ? ?") + 0x1);
	VALIDATE(ClientModeTF)

	Localize = g_Interface.Get<ILocalize*>(VGUI2, VGUI_LOCALIZE_INTERFACE_VERSION);
	VALIDATE(Localize)

	HostState = *reinterpret_cast<CCommonHostState**>(g_Pattern.Find(L"engine.dll", L"8B 15 ? ? ? ? C6 85 ? ? ? ? ? C6 85 ? ? ? ? ? C6 85 ? ? ? ? ? C6 85 ? ? ? ? ? C6 85") + 0x1);
	VALIDATE(HostState)
}

void CSteamInterfaces::Init()
{
	Client = g_Interface.Get<ISteamClient*>(STEAMCLIENT, STEAMCLIENT_INTERFACE_VERSION);
	VALIDATE_STEAM(Client)

	const HSteamPipe hsNewPipe = Client->CreateSteamPipe();              VALIDATE_STEAM(hsNewPipe)
	const HSteamPipe hsNewUser = Client->ConnectToGlobalUser(hsNewPipe); VALIDATE_STEAM(hsNewUser)

	Friends002 = reinterpret_cast<ISteamFriends*>(Client->GetISteamFriends(hsNewUser, hsNewPipe, STEAMFRIENDS_INTERFACE_VERSION));
	VALIDATE_STEAM(Friends002)

	Friends015 = reinterpret_cast<ISteamFriends*>(Client->GetISteamFriends(hsNewUser, hsNewPipe, STEAMFRIENDS_INTERFACE_VERSION));
	VALIDATE_STEAM(Friends015)

	Utils007 = reinterpret_cast<ISteamUtils*>(Client->GetISteamUtils(hsNewUser, STEAMUTILS_INTERFACE_VERSION));
	VALIDATE_STEAM(Utils007)

	SteamApps = reinterpret_cast<ISteamApps*>(Client->GetISteamApps(hsNewUser, hsNewPipe, STEAMAPPS_INTERFACE_VERSION));
	VALIDATE_STEAM(SteamApps)

	UserStats = reinterpret_cast<ISteamUserStats*>(Client->GetISteamUserStats(hsNewUser, hsNewPipe, STEAMUSERSTATS_INTERFACE_VERSION));
	VALIDATE_STEAM(UserStats)

	User = reinterpret_cast<ISteamUser*>(Client->GetISteamUser(hsNewUser, hsNewPipe, STEAMUSER_INTERFACE_VERSION));
	VALIDATE_STEAM(User)

	// Credits to spook953 for teaching me how this works
	static auto fn = reinterpret_cast<ISteamNetworkingUtils * (__cdecl*)()>(GetProcAddress(GetModuleHandleA("steamnetworkingsockets.dll"), "SteamNetworkingUtils_LibV4"));
	NetworkingUtils = fn();
	VALIDATE_STEAM(NetworkingUtils)
}