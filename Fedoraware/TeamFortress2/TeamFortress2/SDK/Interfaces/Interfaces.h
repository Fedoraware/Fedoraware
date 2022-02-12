#pragma once

#include "EngineClient/EngineClient.h"
#include "EngineEffects/EngineEffects.h"
#include "BaseClientDLL/BaseClientDLL.h"
#include "ClientDLLSharedAppSystems/ClientDLLSharedAppSystems.h"
#include "ClientModeShared/ClientModeShared.h"
#include "GlobalVarsBase/GlobalVarsBase.h"
#include "ClientEntityList/ClientEntityList.h"
#include "ModelInfo/ModelInfo.h"
#include "DebugOverlay/DebugOverlay.h"
#include "EngineTrace/EngineTrace.h"
#include "GameMovement/GameMovement.h"
#include "MoveHelper/MoveHelper.h"
#include "ConVar/ConVar.h"
#include "Prediction/Prediction.h"
#include "Panel/Panel.h"
#include "Surface/Surface.h"
#include "EngineVGui/EngineVGui.h"
#include "NetChannel/NetChannel.h"
#include "Steam/Steamworks.h"
#include "GameEvent/GameEvent.h"
#include "Material/Material.h"
#include "ModelRender/ModelRender.h"
#include "ViewRender/ViewRender.h"
#include "RenderView/RenderView.h"
#include "Input/Input.h"
#include "KeyValuesSystem/KeyValuesSystem.h"
#include "UniformRandomStream/UniformRandomStream.h"
#include "InputSystem/InputSystem.h"

class CClockDriftMgr
{
private:
	enum
	{
		// This controls how much it smoothes out the samples from the server.
		NUM_CLOCKDRIFT_SAMPLES = 16
	};

	// This holds how many ticks the client is ahead each time we get a server tick.
	// We average these together to get our estimate of how far ahead we are.
	float m_ClockOffsets[NUM_CLOCKDRIFT_SAMPLES];
	int m_iCurClockOffset;

	int m_nServerTick;		// Last-received tick from the server.
	int	m_nClientTick;		// The client's own tick counter (specifically, for interpolation during rendering).
							// The server may be on a slightly different tick and the client will drift towards it.
};

class CClientState {
private:
	char pad_0000[8]; //0x0000
public:
	void* thisptr = nullptr; //0x0008
private:
	char pad_000C[4]; //0x000C
public:
	CNetChannel* m_NetChannel = nullptr; //0x0010
private:
	char pad_0014[320]; //0x0014
public:
	CClockDriftMgr m_ClockDriftMgr = { }; //0x0154
	int m_nDeltaTick = 0; //0x01A0
private:
	char pad_01A4[16]; //0x01A4
public:
	char m_szLevelFileName[128]; //0x01B4
	char m_szLevelBaseName[128]; //0x0234
	int m_nMaxClients = 0; //0x02B4
private:
	char pad_02B8[18528]; //0x02B8
public:
	int	oldtickcount = 0;		//0x4B18
	float m_tickRemainder = 0.f;	//0x4B1C
	float m_frameTime = 0.f; //0x4B20
	int lastoutgoingcommand = 0; //0x4B24
	int chokedcommands = 0; //0x4B28
	int last_command_ack = 0; //0x4B2C
};

class CSteamInterfaces
{
public:
	ISteamFriends002* Friends002 = nullptr;
	ISteamFriends015* Friends015 = nullptr;
	ISteamApps006* SteamApps = nullptr;
	ISteamUserStats011* UserStats = nullptr;
	ISteamClient017* Client = nullptr;
	ISteamUser017* User = nullptr;
	ISteamUtils007* Utils007 = nullptr;

	void Init();
};


class CInterfaces
{
public: 
	CBaseClientDLL*             Client						= nullptr;
	CClientDLLSharedAppSystems* ClientShared				= nullptr;
	CClientState*				ClientState					= nullptr;
	CClientModeShared*          ClientMode					= nullptr;
	CEngineClient*              Engine						= nullptr;
	IVEngineEffects*			EngineEffects				= nullptr;
	CPanel*                     Panel						= nullptr;
	CSurface*                   Surface						= nullptr;
	CClientEntityList*          EntityList					= nullptr;
	CModelInfoClient*           ModelInfo					= nullptr;
	CEngineTrace*               EngineTrace					= nullptr;
	CPrediction*                Prediction					= nullptr;
	CGameMovement*              GameMovement				= nullptr;
	CMoveHelper*                MoveHelper					= nullptr;
	ICvar*                      CVars						= nullptr;
	CGlobalVarsBase*            GlobalVars					= nullptr;
	CEngineVGui*                EngineVGui					= nullptr;
	int32_t*                    RandomSeed					= nullptr;
	void*                       DemoPlayer					= nullptr;
	IVRenderView*               RenderView					= nullptr;
	IViewRender*                ViewRender					= nullptr;
	CDebugOverlay*              DebugOverlay				= nullptr;
	CGameEventManager*          GameEvent					= nullptr;
	CModelRender*               ModelRender					= nullptr;
	CMaterialSystem*            MatSystem					= nullptr;
	IInput*						Input						= nullptr;
	IKeyValuesSystem*			KeyValuesSystem				= nullptr;
	IUniformRandomStream*		UniformRandomStream			= nullptr;
	void *						StudioRender				= nullptr;
	IInputSystem*				InputSystem					= nullptr;
	void*						CHud						= nullptr;
	void*						CTFGameMovement				= nullptr;
	void Init();
};

inline CInterfaces g_Interfaces;
inline CSteamInterfaces g_SteamInterfaces;
