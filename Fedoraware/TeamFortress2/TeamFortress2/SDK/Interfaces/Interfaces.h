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
#include "Steam/SteamTypes.h"
#include "GameEvent/GameEvent.h"
#include "Material/Material.h"
#include "ModelRender/ModelRender.h"
#include "ViewRender/ViewRender.h"
#include "RenderView/RenderView.h"
#include "Input/Input.h"
#include "KeyValuesSystem/KeyValuesSystem.h"
#include "UniformRandomStream/UniformRandomStream.h"
#include "InputSystem/InputSystem.h"
#include "EffectsClient/EffectsClient.h"
#include "AchievementMgr/AchievementMgr.h"
#include "ViewRenderBeams/ViewRenderBeams.h"

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

class CClientState
{
public:
	byte pad0[0x10];
	INetChannel* m_NetChannel;			// 0x10
	byte pad1[0x140];
	CClockDriftMgr	m_ClockDriftMgr;		// 0x154
	int				m_nDeltaTick;			// 0x1A0
	byte pad2[0x110];
	int				m_nMaxClients;			// 0x2B4	
	byte pad3[0x486C];
	int				lastoutgoingcommand;	// 0x4B24
	int				chokedcommands;			// 0x4B28
	int				last_command_ack;		// 0x4B2C
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
	ISteamNetworking004* Networking = nullptr;

	void Init();
};

namespace I
{
	inline CBaseClientDLL* Client = nullptr;
	inline CClientDLLSharedAppSystems* ClientShared = nullptr;
	inline CClientState* ClientState = nullptr;
	inline CClientModeShared* ClientMode = nullptr;
	inline CEngineClient* Engine = nullptr;
	inline IVEngineEffects* EngineEffects = nullptr;
	inline CPanel* Panel = nullptr;
	inline CSurface* Surface = nullptr;
	inline CClientEntityList* EntityList = nullptr;
	inline IEffects* EffectsClient = nullptr;
	inline CModelInfoClient* ModelInfo = nullptr;
	inline CEngineTrace* EngineTrace = nullptr;
	inline CPrediction* Prediction = nullptr;
	inline CGameMovement* GameMovement = nullptr;
	inline CMoveHelper* MoveHelper = nullptr;
	inline ICvar* CVars = nullptr;
	inline CGlobalVarsBase* GlobalVars = nullptr;
	inline CEngineVGui* EngineVGui = nullptr;
	inline int32_t* RandomSeed = nullptr;
	inline void* DemoPlayer = nullptr;
	inline IVRenderView* RenderView = nullptr;
	inline IViewRender* ViewRender = nullptr;
	inline CDebugOverlay* DebugOverlay = nullptr;
	inline CGameEventManager* GameEvent = nullptr;
	inline CModelRender* ModelRender = nullptr;
	inline CMaterialSystem* MatSystem = nullptr;
	inline IInput* Input = nullptr;
	inline IKeyValuesSystem* KeyValuesSystem = nullptr;
	inline IUniformRandomStream* UniformRandomStream = nullptr;
	inline void* StudioRender = nullptr;
	inline IInputSystem* InputSystem = nullptr;
	inline void* CHud = nullptr;
	inline void* CTFGameMovement = nullptr;
	inline IAchievementMgr* AchievementMgr = nullptr;
	inline IViewRenderBeams* ILOVEBEAMS = nullptr;
}

class CInterfaces
{
public: 
	void Init();
};

inline CInterfaces g_Interfaces;
inline CSteamInterfaces g_SteamInterfaces;
