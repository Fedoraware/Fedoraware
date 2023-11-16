#include "Interfaces.h"

#define VALIDATE(x) if (!(x)) MessageBoxA(0, #x, "CInterfaces::Init() -> nullptr", MB_ICONERROR)
#define VALIDATE_STEAM(x) if (!(x)) MessageBoxA(0, #x, "CSteamInterfaces::Init() -> nullptr", MB_ICONERROR)

namespace S
{
	MAKE_SIGNATURE(GlobalVars_Interface, ENGINE_DLL, "68 ? ? ? ? 50 50 FF 12", 0x1);
	MAKE_SIGNATURE(ClientState_Interface, ENGINE_DLL, "68 ? ? ? ? E8 ? ? ? ? 83 C4 ? 5F 5E 5B 5D", 0x1);
	MAKE_SIGNATURE(ClientModeShared, CLIENT_DLL, "8B 0D ? ? ? ? 8B 02 D9 05", 0x2);
	MAKE_SIGNATURE(DemoPlayer_Interface, ENGINE_DLL, "8B 0D ? ? ? ? 85 C9 74 ? 8B 01 8B 40 ? FF D0 84 C0 74 ? 8B 0D ? ? ? ? 8B 01 FF 50 ? 83 F8 ? 7D ? 53", 0x2);
	MAKE_SIGNATURE(TFGCClientSystem_Interface, CLIENT_DLL, "B9 ? ? ? ? 50 E8 ? ? ? ? 8B 5D", 0x1);
	MAKE_SIGNATURE(TFInventoryManager_Interface, CLIENT_DLL, "B9 ? ? ? ? E8 ? ? ? ? B9 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05", 0x1);
	MAKE_SIGNATURE(Input_Interface, CLIENT_DLL, "8B 0D ? ? ? ? 56 8B 01 FF 50 ? 8B 45 ? 5F 5E 5B C7 40", 0x2);
	MAKE_SIGNATURE(UniformRandomStream_Interface, VSTDLIB_DLL, "B9 ? ? ? ? 85 C0 0F 45 C8 89 0D ? ? ? ? 5D C3", 0x1);
	MAKE_SIGNATURE(ViewRenderBeams_Interface, CLIENT_DLL, "8B 0D ? ? ? ? 56 8B 01 FF 50 ? 0F B7 96", 0x2);
	MAKE_SIGNATURE(TFGameRules_Interface, CLIENT_DLL, "8B 0D ? ? ? ? 56 8B 01 8B 80 ? ? ? ? FF D0 84 C0 0F 84 ? ? ? ? 80 BB", 0x2);
	MAKE_SIGNATURE(ThirdPersonManager_Interface, CLIENT_DLL, "B9 ? ? ? ? E8 ? ? ? ? 84 C0 74 ? 8B 86", 0x1);
	MAKE_SIGNATURE(ClientModeTFNormal_Interface, CLIENT_DLL, "B9 ? ? ? ? A3 ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 83 C4 ? 8B 35", 0x1);
	MAKE_SIGNATURE(HostState_Interface, ENGINE_DLL, "8B 15 ? ? ? ? C6 85", 0x1);
	MAKE_SIGNATURE(CTFGameMovement_Interface, CLIENT_DLL, "B9 ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C6 05 ? ? ? ? ? E8", 0x1);
	MAKE_SIGNATURE(MoveHelper_Interface, CLIENT_DLL, "8B 0D ? ? ? ? 8B 01 FF 50 ? 56 8B C8 8B 10 FF 52 ? 8B 13", 0x2);

	MAKE_SIGNATURE(Get_TFPartyClient, CLIENT_DLL, "A1 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC 55 8B EC 83 EC ? 53", 0x0);
	MAKE_SIGNATURE(DirectXDevice, "shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C", 0x1);
}

void CInterfaces::Init()
{
	using namespace I;

	BaseClientDLL = g_Interface.Get<CBaseClientDLL*>(CLIENT_DLL, CLIENT_DLL_INTERFACE_VERSION);
	VALIDATE(BaseClientDLL);

	ClientDLLSharedAppSystems = g_Interface.Get<CClientDLLSharedAppSystems*>(CLIENT_DLL, CLIENT_DLL_SHARED_APPSYSTEMS);
	VALIDATE(ClientDLLSharedAppSystems);

	ClientEntityList = g_Interface.Get<CClientEntityList*>(CLIENT_DLL, VCLIENTENTITYLIST_INTERFACE_VERSION);
	VALIDATE(ClientEntityList);

	Prediction = g_Interface.Get<CPrediction*>(CLIENT_DLL, VCLIENT_PREDICTION_INTERFACE_VERSION);
	VALIDATE(Prediction);

	GameMovement = g_Interface.Get<CGameMovement*>(CLIENT_DLL, CLIENT_GAMEMOVEMENT_INTERFACE_VERSION);
	VALIDATE(GameMovement);

	Physics = g_Interface.Get<IPhysics*>(VPHYSICS_DLL, VPHYSICS_INTERFACE_VERSION);
	VALIDATE(Physics);

	PhysicsCollision = g_Interface.Get<IPhysicsCollision*>(VPHYSICS_DLL, VPHYSICS_COLLISION_INTERFACE_VERSION);
	VALIDATE(PhysicsCollision);

	CenterPrint = g_Interface.Get<ICenterPrint*>(CLIENT_DLL, VCENTERPRINT_INTERFACE_VERSION);
	VALIDATE(CenterPrint);

	ModelInfoClient = g_Interface.Get<CModelInfoClient*>(ENGINE_DLL, VMODELINFO_CLIENT_INTERFACE_VERSION);
	VALIDATE(ModelInfoClient);

	EngineClient = g_Interface.Get<CEngineClient*>(ENGINE_DLL, VENGINE_CLIENT_INTERFACE_VERSION_13);
	VALIDATE(EngineClient);

	EngineEffects = g_Interface.Get<IVEngineEffects*>(ENGINE_DLL, VENGINE_EFFECTS_INTERFACE_VERSION);
	VALIDATE(EngineEffects);

	EngineTrace = g_Interface.Get<CEngineTrace*>(ENGINE_DLL, VENGINE_TRACE_CLIENT_INTERFACE_VERSION);
	VALIDATE(EngineTrace);

	VGuiPanel = g_Interface.Get<CPanel*>(VGUI2_DLL, VGUI_PANEL_INTERFACE_VERSION);
	VALIDATE(VGuiPanel);

	VGuiSurface = g_Interface.Get<CSurface*>(MATSURFACE_DLL, VGUI_SURFACE_INTERFACE_VERSION);
	VALIDATE(VGuiSurface);

	Cvar = g_Interface.Get<ICvar*>(VSTDLIB_DLL, VENGINE_CVAR_INTERFACE_VERSION);
	VALIDATE(Cvar);

	GlobalVars = *S::GlobalVars_Interface.As<CGlobalVarsBase**>();
	VALIDATE(GlobalVars);

	ClientState = *S::ClientState_Interface.As<CClientState**>();
	VALIDATE(ClientState);

	ClientModeShared = **S::ClientModeShared.As<CClientModeShared***>();
	VALIDATE(ClientModeShared);

	EngineVGui = g_Interface.Get<CEngineVGui*>(ENGINE_DLL, VENGINE_VGUI_VERSION);
	VALIDATE(EngineVGui);

	DemoPlayer = **S::DemoPlayer_Interface.As<void***>();
	VALIDATE(DemoPlayer);

	RenderView = g_Interface.Get<IVRenderView*>(ENGINE_DLL, VENGINE_RENDERVIEW_INTERFACE_VERSION);
	VALIDATE(RenderView);

	DebugOverlay = g_Interface.Get<CDebugOverlay*>(ENGINE_DLL, VENGINE_DEBUGOVERLAY_INTERFACE_VERSION);
	VALIDATE(RenderView);

	GameEventManager = g_Interface.Get<CGameEventManager*>(ENGINE_DLL, GAMEEVENTSMANAGER_ENGINE_INTERFACE);
	VALIDATE(GameEventManager);

	ModelRender = g_Interface.Get<CModelRender*>(ENGINE_DLL, VENGINE_MODELRENDER_INTERFACE);
	VALIDATE(ModelRender);

	MaterialSystem = g_Interface.Get<CMaterialSystem*>(MATSYSTEM_DLL, VMATERIALSYSTEM_INTERFACE);
	VALIDATE(MaterialSystem);

	TFGCClientSystem = *S::TFGCClientSystem_Interface.As<CTFGCClientSystem**>();
	VALIDATE(TFGCClientSystem);

	TFPartyClient = reinterpret_cast<CTFPartyClient * (__cdecl*)()>(S::Get_TFPartyClient())();
	VALIDATE(TFPartyClient);

	TFInventoryManager = *S::TFInventoryManager_Interface.As<CTFInventoryManager**>();
	VALIDATE(TFInventoryManager);

	// TODO: This doesn't belong here
	RandomSeed = *S::RandomSeed.As<int32_t**>();
	VALIDATE(RandomSeed);

	AllowSecureServers = *S::AllowSecureServers.As<bool**>();
	VALIDATE(AllowSecureServers);

	const auto pdwClient = reinterpret_cast<PDWORD>(BaseClientDLL);
	VALIDATE(pdwClient);

	const auto pdwTable = *reinterpret_cast<PDWORD*>(pdwClient);
	VALIDATE(pdwTable);

	ViewRender = **reinterpret_cast<IViewRender***>(pdwTable[27] + 5);
	VALIDATE(ViewRender);

	Input = **S::Input_Interface.As<IInput***>();
	VALIDATE(Input);

	auto GetKeyValuesSystem = [&]() -> IKeyValuesSystem* {
		static auto fn = reinterpret_cast<IKeyValuesSystem * (__cdecl*)()>(reinterpret_cast<DWORD>(GetProcAddress(GetModuleHandleA(VSTDLIB_DLL), "KeyValuesSystem")));
		return fn();
	};

	KeyValuesSystem = GetKeyValuesSystem();
	VALIDATE(KeyValuesSystem);

	UniformRandomStream = *S::UniformRandomStream_Interface.As<IUniformRandomStream**>();
	VALIDATE(UniformRandomStream);

	StudioRender = g_Interface.Get<void*>(STUDIORENDER_DLL, "VStudioRender025");
	VALIDATE(StudioRender);

	InputSystem = g_Interface.Get<IInputSystem*>("inputsystem.dll", "InputSystemVersion001");
	VALIDATE(InputSystem);

	EffectsClient = g_Interface.Get<CEffectsClient*>(CLIENT_DLL, IEFFECTS_INTERFACE_VERSION);
	VALIDATE(EffectsClient);

	using getachievementmgr = IAchievementMgr* (*)();

	AchievementMgr = reinterpret_cast<IAchievementMgr*>(GetVFunc<getachievementmgr>(EngineClient, 115));
	VALIDATE(AchievementMgr);

	ViewRenderBeams = **S::ViewRenderBeams_Interface.As<IViewRenderBeams***>();
	VALIDATE(ViewRenderBeams);

	EngineSound = g_Interface.Get<IEngineSound*>(ENGINE_DLL, "IEngineSoundClient003");
	VALIDATE(EngineSound);

	TFGameRules = *S::TFGameRules_Interface.As<CTFGameRules**>();
	VALIDATE(TFGameRules);

	ThirdPersonManager = *S::ThirdPersonManager_Interface.As<CThirdPersonManager**>();
	VALIDATE(ThirdPersonManager);

	DirectXDevice = **S::DirectXDevice.As<IDirect3DDevice9***>();
	VALIDATE(DirectXDevice);

	ClientModeTF = *S::ClientModeTFNormal_Interface.As<ClientModeTFNormal**>();
	VALIDATE(ClientModeTF);

	Localize = g_Interface.Get<ILocalize*>(VGUI2_DLL, VGUI_LOCALIZE_INTERFACE_VERSION);
	VALIDATE(Localize);

	HostState = *S::HostState_Interface.As<CCommonHostState**>();
	VALIDATE(HostState);

	TFGameMovement = *S::CTFGameMovement_Interface.As<CTFGameMovement**>();
	VALIDATE(TFGameMovement);

	MoveHelper = **S::MoveHelper_Interface.As<CMoveHelper***>();
	VALIDATE(MoveHelper);
}

void CSteamInterfaces::Init()
{
	Client = g_Interface.Get<ISteamClient*>("steamclient.dll", STEAMCLIENT_INTERFACE_VERSION);
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
