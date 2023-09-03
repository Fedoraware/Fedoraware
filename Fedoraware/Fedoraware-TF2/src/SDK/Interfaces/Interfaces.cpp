#include "Interfaces.h"
#include "../Signatures.h"

#define VALIDATE(x) if (!(x)) MessageBoxA(0, #x, "CInterfaces::Init() -> nullptr", MB_ICONERROR)
#define VALIDATE_STEAM(x) if (!(x)) MessageBoxA(0, #x, "CSteamInterfaces::Init() -> nullptr", MB_ICONERROR)

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
	VALIDATE(BaseClientDLL);

	ClientDLLSharedAppSystems = g_Interface.Get<CClientDLLSharedAppSystems*>(CLIENT, CLIENT_DLL_SHARED_APPSYSTEMS);
	VALIDATE(ClientDLLSharedAppSystems);

	ClientEntityList = g_Interface.Get<CClientEntityList*>(CLIENT, VCLIENTENTITYLIST_INTERFACE_VERSION);
	VALIDATE(ClientEntityList);

	Prediction = g_Interface.Get<CPrediction*>(CLIENT, VCLIENT_PREDICTION_INTERFACE_VERSION);
	VALIDATE(Prediction);

	GameMovement = g_Interface.Get<CGameMovement*>(CLIENT, CLIENT_GAMEMOVEMENT_INTERFACE_VERSION);
	VALIDATE(GameMovement);

	CenterPrint = g_Interface.Get<ICenterPrint*>(CLIENT, VCENTERPRINT_INTERFACE_VERSION);
	VALIDATE(CenterPrint);

	ModelInfoClient = g_Interface.Get<CModelInfoClient*>(ENGINE, VMODELINFO_CLIENT_INTERFACE_VERSION);
	VALIDATE(ModelInfoClient);

	EngineClient = g_Interface.Get<CEngineClient*>(ENGINE, VENGINE_CLIENT_INTERFACE_VERSION_13);
	VALIDATE(EngineClient);

	EngineEffects = g_Interface.Get<IVEngineEffects*>(ENGINE, VENGINE_EFFECTS_INTERFACE_VERSION);
	VALIDATE(EngineEffects);

	EngineTrace = g_Interface.Get<CEngineTrace*>(ENGINE, VENGINE_TRACE_CLIENT_INTERFACE_VERSION);
	VALIDATE(EngineTrace);

	VGuiPanel = g_Interface.Get<CPanel*>(VGUI2, VGUI_PANEL_INTERFACE_VERSION);
	VALIDATE(VGuiPanel);

	VGuiSurface = g_Interface.Get<CSurface*>(MATSURFACE, VGUI_SURFACE_INTERFACE_VERSION);
	VALIDATE(VGuiSurface);

	Cvar = g_Interface.Get<ICvar*>(VSTDLIB, VENGINE_CVAR_INTERFACE_VERSION);
	VALIDATE(Cvar);

	GlobalVars = *S::GlobalVars_Interface.As<CGlobalVarsBase*>();
	VALIDATE(GlobalVars);

	ClientState = *S::ClientState_Interface.As<CClientState*>();
	VALIDATE(ClientState);

	auto ClientTable = S::ClientTable.As<void>();
	VALIDATE(ClientTable);

	ClientModeShared = **reinterpret_cast<CClientModeShared***>(reinterpret_cast<DWORD>(ClientTable) + 2);
	VALIDATE(ClientModeShared);

	EngineVGui = g_Interface.Get<CEngineVGui*>(ENGINE, VENGINE_VGUI_VERSION);
	VALIDATE(EngineVGui);

	DemoPlayer = **S::DemoPlayer_Interface.As<void**>();
	VALIDATE(DemoPlayer);

	RenderView = g_Interface.Get<IVRenderView*>(ENGINE, VENGINE_RENDERVIEW_INTERFACE_VERSION);
	VALIDATE(RenderView);

	DebugOverlay = g_Interface.Get<CDebugOverlay*>(ENGINE, VENGINE_DEBUGOVERLAY_INTERFACE_VERSION);
	VALIDATE(RenderView);

	GameEventManager = g_Interface.Get<CGameEventManager*>(ENGINE, GAMEEVENTSMANAGER_ENGINE_INTERFACE);
	VALIDATE(GameEventManager);

	ModelRender = g_Interface.Get<CModelRender*>(ENGINE, VENGINE_MODELRENDER_INTERFACE);
	VALIDATE(ModelRender);

	MaterialSystem = g_Interface.Get<CMaterialSystem*>(MATSYSTEM, VMATERIALSYSTEM_INTERFACE);
	VALIDATE(MaterialSystem);

	TFGCClientSystem = *S::TFGCClientSystem_Interface.As<CTFGCClientSystem*>();
	VALIDATE(TFGCClientSystem);

	TFPartyClient = reinterpret_cast<CTFPartyClient * (__cdecl*)()>(g_Pattern.E8(CLIENT_DLL, "E8 ? ? ? ? FF 70 24"))();
	VALIDATE(TFPartyClient);

	TFInventoryManager = *S::TFInventoryManager_Interface.As<CTFInventoryManager*>();
	VALIDATE(TFInventoryManager);

	// TODO: This doesn't belong here
	RandomSeed = *S::RandomSeed.As<int32_t*>();
	VALIDATE(RandomSeed);

	AllowSecureServers = *S::AllowSecureServers.As<bool*>();
	VALIDATE(AllowSecureServers);

	const auto pdwClient = reinterpret_cast<PDWORD>(BaseClientDLL);
	VALIDATE(pdwClient);

	const auto pdwTable = *reinterpret_cast<PDWORD*>(pdwClient);
	VALIDATE(pdwTable);

	ViewRender = **reinterpret_cast<IViewRender***>(pdwTable[27] + 5);
	VALIDATE(ViewRender);

	Input = **S::Input_Interface.As<IInput**>();
	VALIDATE(Input);

	auto GetKeyValuesSystem = [&]() -> IKeyValuesSystem* {
		static auto fn = reinterpret_cast<IKeyValuesSystem * (__cdecl*)()>(reinterpret_cast<DWORD>(GetProcAddress(GetModuleHandleW(VSTDLIB), "KeyValuesSystem")));
		return fn();
	};

	KeyValuesSystem = GetKeyValuesSystem();
	VALIDATE(KeyValuesSystem);

	UniformRandomStream = *S::UniformRandomStream_Interface.As<IUniformRandomStream*>();
	VALIDATE(UniformRandomStream);

	StudioRender = g_Interface.Get<void*>(L"studiorender.dll", "VStudioRender025");
	VALIDATE(StudioRender);

	InputSystem = g_Interface.Get<IInputSystem*>(L"inputsystem.dll", "InputSystemVersion001");
	VALIDATE(InputSystem);

	EffectsClient = g_Interface.Get<CEffectsClient*>(CLIENT, IEFFECTS_INTERFACE_VERSION);
	VALIDATE(EffectsClient);

	using getachievementmgr = IAchievementMgr* (*)();

	AchievementMgr = reinterpret_cast<IAchievementMgr*>(GetVFunc<getachievementmgr>(EngineClient, 115));
	VALIDATE(AchievementMgr);

	ViewRenderBeams = **S::ViewRenderBeams_Interface.As<IViewRenderBeams**>();
	VALIDATE(ViewRenderBeams);

	EngineSound = g_Interface.Get<IEngineSound*>(L"engine.dll", "IEngineSoundClient003");
	VALIDATE(EngineSound);

	TFGameRules = *S::TFGameRules_Interface.As<CTFGameRules*>();
	VALIDATE(TFGameRules);

	ThirdPersonManager = *S::ThirdPersonManager_Interface.As<CThirdPersonManager*>();
	VALIDATE(ThirdPersonManager);

	// Forgive the double cast but this was annoying meeeeee
	DirectXDevice = **S::DirectXDevice.As<IDirect3DDevice9**>();
	VALIDATE(DirectXDevice);

	ClientModeTF = *S::ClientModeTFNormal_Interface.As<ClientModeTFNormal*>();
	VALIDATE(ClientModeTF);

	Localize = g_Interface.Get<ILocalize*>(VGUI2, VGUI_LOCALIZE_INTERFACE_VERSION);
	VALIDATE(Localize);

	HostState = *S::HostState_Interface.As<CCommonHostState*>();
	VALIDATE(HostState);
}

void CSteamInterfaces::Init()
{
	Client = g_Interface.Get<ISteamClient*>(STEAMCLIENT, STEAMCLIENT_INTERFACE_VERSION);
	VALIDATE_STEAM(Client);

	const HSteamPipe hsNewPipe = Client->CreateSteamPipe();
	VALIDATE_STEAM(hsNewPipe);

	const HSteamPipe hsNewUser = Client->ConnectToGlobalUser(hsNewPipe);
	VALIDATE_STEAM(hsNewUser);

	Friends = Client->GetISteamFriends(hsNewUser, hsNewPipe, STEAMFRIENDS_INTERFACE_VERSION);
	VALIDATE_STEAM(Friends);

	Utils = Client->GetISteamUtils(hsNewUser, STEAMUTILS_INTERFACE_VERSION);
	VALIDATE_STEAM(Utils);

	Apps = Client->GetISteamApps(hsNewUser, hsNewPipe, STEAMAPPS_INTERFACE_VERSION);
	VALIDATE_STEAM(Apps);

	UserStats = Client->GetISteamUserStats(hsNewUser, hsNewPipe, STEAMUSERSTATS_INTERFACE_VERSION);
	VALIDATE_STEAM(UserStats);

	User = Client->GetISteamUser(hsNewUser, hsNewPipe, STEAMUSER_INTERFACE_VERSION);
	VALIDATE_STEAM(User);

	// Credits to spook953 for teaching me how this works
	static auto fn = reinterpret_cast<ISteamNetworkingUtils * (__cdecl*)()>(GetProcAddress(GetModuleHandleA("steamnetworkingsockets.dll"), "SteamNetworkingUtils_LibV4"));
	NetworkingUtils = fn();
	VALIDATE_STEAM(NetworkingUtils);
}
