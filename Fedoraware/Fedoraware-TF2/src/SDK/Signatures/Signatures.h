#pragma once
#include <Windows.h>

static constexpr auto CLIENT_DLL = "client.dll";
static constexpr auto ENGINE_DLL = "engine.dll";
static constexpr auto SERVER_DLL = "server.dll";
static constexpr auto VSTDLIB_DLL = "vstdlib.dll";
static constexpr auto VGUI2_DLL = "vgui2.dll";
static constexpr auto MATSURFACE_DLL = "vguimatsurface.dll";
static constexpr auto MATSYSTEM_DLL = "MaterialSystem.dll";
static constexpr auto STUDIORENDER_DLL = "studiorender.dll";
static constexpr auto TIER0_DLL = "tier0.dll";
static constexpr auto VPHYSICS_DLL = "vphysics.dll";

class CSignature
{
	DWORD m_Address = 0;
	LPCSTR m_Module = nullptr;
	LPCSTR m_Pattern = nullptr;
	int m_Offset = 0;

	void Find();

public:
	CSignature(LPCSTR szModule, LPCSTR szPattern, int offset = 0)
		: m_Module(szModule), m_Pattern(szPattern), m_Offset(offset) { }

	// Return the address
	DWORD operator()()
	{
		if (m_Address == 0) { Find(); }
		return m_Address;
	}

	template <typename T> T As() { return reinterpret_cast<T>(this->operator()()); }
};

#define MAKE_SIGNATURE(name, module, pattern, offset) inline CSignature name{ module, pattern, offset }

// Signatures
namespace S
{
	// Hooks
	MAKE_SIGNATURE(CBaseAnimating_FrameAdvance, CLIENT_DLL, "55 8B EC 83 EC ? 56 8B F1 57 80 BE ? ? ? ? ? 0F 85", 0x0);
	MAKE_SIGNATURE(CBaseAnimating_Interpolate, CLIENT_DLL, "55 8B EC 83 EC ? 56 8B F1 83 BE ? ? ? ? ? 0F 85 ? ? ? ? F3 0F 10 86", 0x0);
	MAKE_SIGNATURE(CBaseAnimating_MaintainSequenceTransitions, CLIENT_DLL, "55 8B EC 83 EC ? 56 8B 75 ? 57 8B F9 8B CE E8 ? ? ? ? 85 C0", 0x0);
	MAKE_SIGNATURE(CBaseAnimating_UpdateClientSideAnimation, CLIENT_DLL, "55 8B EC 83 EC 0C 56 57 8B F1 E8 ? ? ? ? 8B F8 85 FF 74 10 8B 17 8B CF 8B 92 ? ? ? ? FF D2", 0x0);
	MAKE_SIGNATURE(CBaseCombatWeapon_AddToCritBucket, CLIENT_DLL, "55 8B EC A1 ? ? ? ? F3 0F 10 81 ? ? ? ? F3 0F 10 48", 0x0);
	MAKE_SIGNATURE(CBaseCombatWeapon_IsAllowedToWithdrawFromCritBucket, CLIENT_DLL, "55 8B EC 56 8B F1 0F B7 86", 0x0);
	MAKE_SIGNATURE(CBaseEntity_FireBullets, CLIENT_DLL, "55 8B EC 81 EC ? ? ? ? 53 56 57 8B F9 8B 5D", 0x0);
	MAKE_SIGNATURE(CBaseEntity_Interpolate, CLIENT_DLL, "55 8B EC 83 EC ? 53 57 8D 45", 0x0);
	MAKE_SIGNATURE(CBaseEntity_SetAbsVelocity, CLIENT_DLL, "55 8B EC 83 EC ? 56 57 8B 7D ? 8B F1 F3 0F", 0x0);
	MAKE_SIGNATURE(CBasePlayer_CalcViewModelView, CLIENT_DLL, "55 8B EC 83 EC ? 8B 55 ? 53 8B 5D ? 89 4D ? 8B 02", 0x0);
	MAKE_SIGNATURE(CBaseViewModel_ShouldFlipViewModel, CLIENT_DLL, "8B 89 ? ? ? ? 85 C9 74 ? BA ? ? ? ? 83 F9 ? 74 ? 0F B7 D1 A1 ? ? ? ? C1 E2 ? 83 C0 ? 03 C2 74 ? C1 E9 ? 39 48 ? 75 ? 8B 08 85 C9 74 ? A1", 0x0);
	MAKE_SIGNATURE(COP_RenderSprites_RenderSpriteCard, CLIENT_DLL, "55 8B EC 83 EC ? 56 8B 75 ? 57 8B 7D ? 8B C7", 0x0);
	MAKE_SIGNATURE(CTFPlayer_AvoidPlayers, CLIENT_DLL, "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 57 8B F9 89 7D ? 83 78 ? ? 0F 84 ? ? ? ? A1", 0x0);
	MAKE_SIGNATURE(CTFPlayer_FireEvent, CLIENT_DLL, "55 8B EC 8B 45 ? 81 EC ? ? ? ? 53 56 57 8B F9 3D", 0x0);
	MAKE_SIGNATURE(CTFRagdoll_CreateTFRagdoll, CLIENT_DLL, "55 8B EC B8 ? ? ? ? E8 ? ? ? ? 53 56 57 8B F9 8B 8F", 0x0);
	MAKE_SIGNATURE(CTFWeaponBase_CalcIsAttackCritical, CLIENT_DLL, "53 57 6A ? 68 ? ? ? ? 68 ? ? ? ? 6A ? 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B D8 83 C4 ? 85 DB 0F 84", 0x0);
	MAKE_SIGNATURE(CAchievementMgr_CheckAchievementsEnabled, CLIENT_DLL, "55 8B EC A1 ? ? ? ? 83 EC ? 56 8B F1 8B 48", 0x0);
	MAKE_SIGNATURE(CBaseClient_Connect, ENGINE_DLL, "55 8B EC 53 56 57 68 ? ? ? ? 8B F1", 0x0);
	MAKE_SIGNATURE(CBaseClient_Disconnect, ENGINE_DLL, "55 8B EC 81 EC ? ? ? ? 57 8B 7D ? 83 BF ? ? ? ? ? 0F 84 ? ? ? ? 56", 0x0);
	MAKE_SIGNATURE(CBaseClient_SendSignonData, ENGINE_DLL, "55 8B EC 83 EC ? 56 68 ? ? ? ? 8B F1", 0x0);
	MAKE_SIGNATURE(CEconItemSchema_GetItemDefinition, CLIENT_DLL, "55 8B EC 56 8B F1 8D 45 ? 57 50 8D 8E", 0x0);
	MAKE_SIGNATURE(CEconNotification_HasNewItems, CLIENT_DLL, "55 8B EC 83 EC ? 56 8B F1 89 75 ? E8 ? ? ? ? C7 06", 0x0);
	MAKE_SIGNATURE(CheckSimpleMaterial, ENGINE_DLL, "55 8B EC 56 8B 75 ? 85 F6 75 ? 32 C0 5E 5D C3", 0x0);
	MAKE_SIGNATURE(CHudCrosshair_GetDrawPosition, CLIENT_DLL, "55 8B EC 81 EC ? ? ? ? 53 57 E8", 0x0);
	MAKE_SIGNATURE(CInterpolatedVarArrayBase_Interpolate, CLIENT_DLL, "55 8B EC 83 EC 1C D9 45 0C 8D 45 FC 56 50 83 EC 08 C7 45 ? ? ? ? ? 8D 45 E4 8B F1 D9 5C 24 04 D9 45 08 D9 1C 24 50 E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? F6 46 2C 01 53 57 74 31 83 7D FC 00 B9 ? ? ? ? D9 45 08 B8 ? ? ? ? 0F 44 C1 8B CE 50 8B 06 83 EC 08 DD 1C 24 FF 50 28 50 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 14 80 7D E4 00 74 60 0F B7 5E 0E 8B 55 F0 0F B7 4E 0C 03", 0x0);
	MAKE_SIGNATURE(CInventoryManager_ShowItemsPickedUp, CLIENT_DLL, "55 8B EC 83 EC ? 80 7D ? ? 53 8B D9 75", 0x0);
	MAKE_SIGNATURE(CL_LoadWhitelist, ENGINE_DLL, "55 8B EC 83 EC ? 56 8B 75 ? 8B CE FF 75", 0x0);
	MAKE_SIGNATURE(CL_Move, ENGINE_DLL, "55 8B EC 83 EC ? 83 3D ? ? ? ? 02 0F 8C ? ? 00 00 E8 ? ? ? 00 84 C0", 0x0);
	MAKE_SIGNATURE(CL_SendMove, ENGINE_DLL, "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 8D", 0x0);
	MAKE_SIGNATURE(CL_ReadPackets, ENGINE_DLL, "55 8B EC 83 EC ? 8B 0D ? ? ? ? 53 33 DB 89 5D ? 89 5D ? 8B 41 ? 89 5D ? 89 5D ? 85 C0 74 ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 53 53 53 53 8D 4D ? 51 50 8B 40 ? FF D0 8B 45 ? 83 C4 ? 8B 0D ? ? ? ? 89 45 ? 8B 45 ? 89 45 ? 8B 59 ? 8B 0D ? ? ? ? 89 5D ? FF 15 ? ? ? ? 88 45 ? 84 C0 74 ? 8B 0D ? ? ? ? 6A ? 6A ? 68 ? ? ? ? 6A ? 68 ? ? ? ? FF 15 ? ? ? ? 8B 0D", 0x0);
	MAKE_SIGNATURE(ClientModeTFNormal_UpdateSteamRichPresence, CLIENT_DLL, "55 8B EC 81 EC ? ? ? ? 8B 15 ? ? ? ? 53 33 DB 89 4D", 0x0);
	MAKE_SIGNATURE(ClientState_GetClientInterpAmount, ENGINE_DLL, "55 8B EC 8B 0D ? ? ? ? 83 EC 10 85 C9 75 26 8B 0D ? ? ? ? 68 ? ? ? ? 8B 01 FF 50 34", 0x0);
	MAKE_SIGNATURE(ClientState_ProcessFixAngle, ENGINE_DLL, "55 8B EC 8B 45 08 83 EC 08 F3 0F 10 15 ? ? ? ?", 0x0);
	MAKE_SIGNATURE(CMatchInviteNotification_OnTick, CLIENT_DLL, "55 8B EC 83 EC 0C 56 8B F1 E8 ? ? ? ? 8B 86 ? ? ? ? 85 C0 74 09 83 F8 02 0F 85", 0x0);
	MAKE_SIGNATURE(CNetChan_SendNetMsg, ENGINE_DLL, "55 8B EC 57 8B F9 8D 8F ? ? ? ? E8 ? ? ? ? 85 C0 75 07 B0 01 5F 5D C2 0C 00", 0x0);
	MAKE_SIGNATURE(CNewParticleEffect_DrawModel, CLIENT_DLL, "55 8B EC 83 EC 30 53 8B D9 8B 0D ? ? ? ? 56 33 F6 89 75 DC", 0x0);
	MAKE_SIGNATURE(COcclusionSystem_IsOccluded, ENGINE_DLL, "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 57", 0x0);
	MAKE_SIGNATURE(CRendering3dView_EnableWorldFog, CLIENT_DLL, "55 8B EC 8B 0D ? ? ? ? 83 EC 0C 8B 01 53 56 FF 90 ? ? ? ? 8B F0 85 F6 74 07 8B 06 8B CE FF 50 08", 0x0);
	MAKE_SIGNATURE(CSequenceTransitioner_CheckForSequenceChange, CLIENT_DLL, "55 8B EC 53 8B 5D ? 57 8B F9 85 DB 0F 84", 0x0);
	MAKE_SIGNATURE(CSkyboxView_Enable3dSkyboxFog, CLIENT_DLL, "55 8B EC 83 EC ? 57 E8 ? ? ? ? 8B F8 85 FF 74", 0x0);
	MAKE_SIGNATURE(CSoundEmitterSystem_EmitSound, CLIENT_DLL, "55 8B EC 53 56 8B 75 ? 8B D9 57 8B 56", 0x0);
	MAKE_SIGNATURE(CStaticPropMgr_ComputePropOpacity, ENGINE_DLL, "55 8B EC 83 EC ? A1 ? ? ? ? 53 56 57 8B 7D ? 8B D9", 0x0);
	MAKE_SIGNATURE(CStaticPropMgr_DrawStaticProps, ENGINE_DLL, "55 8B EC 83 EC ? 8B 15 ? ? ? ? 53 56 57 33 F6", 0x0);
	MAKE_SIGNATURE(CTFGameRules_ModifySentChat, CLIENT_DLL, "55 8B EC 80 B9 ? ? ? ? ? 56 8B 75 08 74 26", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_UpdateAssignedLobby, CLIENT_DLL, "55 8B EC 83 EC 28 53 8B D9 56 57 89 5D F0 8B 8B ? ? ? ? 85 C9 0F 84 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 77", 0x0);
	MAKE_SIGNATURE(CTFPartyClient_BCanRequestToJoinPlayer, CLIENT_DLL, "55 8B EC 83 EC 08 8B 55 08 56 8B 75 0C 57 8B F9 89 55 F8 89 75 FC 8B 4F 18 85 C9 74 1E", 0x0);
	MAKE_SIGNATURE(CTFPlayerInventory_GetMaxItemCount, CLIENT_DLL, "8B 49 68 56", 0x0);
	MAKE_SIGNATURE(CTFPlayerShared_InCond, CLIENT_DLL, "55 8B EC 83 EC 08 56 57 8B 7D 08 8B F1 83 FF 20", 0x0);
	MAKE_SIGNATURE(CTFPlayerShared_IsPlayerDominated, CLIENT_DLL, "55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B F8 85 FF 75 08 5F 32 C0 5E 5D C2 04 00", 0x0);
	MAKE_SIGNATURE(CViewRender_DrawUnderwaterOverlay, CLIENT_DLL, "55 8B EC 83 EC 2C 53 8B 99 ? ? ? ? 89 5D E0 85 DB 0F 84 ? ? ? ? 8B 0D ? ? ? ? 57 33 FF 89", 0x0);
	MAKE_SIGNATURE(DataTable_Warning, ENGINE_DLL, "55 8B EC B8 ? ? ? ? E8 ? ? ? ? 8D 45 ? 50 FF 75 ? 8D 85 ? ? ? ? 68 ? ? ? ? 50 E8 ? ? ? ? 8D 85 ? ? ? ? 50 68 ? ? ? ? FF 15", 0x0);
	MAKE_SIGNATURE(DoEnginePostProcessing, CLIENT_DLL, "55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 33 DB 89 5D ? 89 5D ? 8B 01 89 5D ? 89 5D ? 56 57", 0x0);
	MAKE_SIGNATURE(DSP_Process, ENGINE_DLL, "55 8B EC 51 56 8B 75 ? 83 FE ? 0F 87", 0x0);
	MAKE_SIGNATURE(FX_FireBullets, CLIENT_DLL, "55 8B EC 81 EC ? ? ? ? 53 8B 5D ? 56 53", 0x0);
	MAKE_SIGNATURE(GetClientInterpAmount, CLIENT_DLL, "55 8B EC A1 ? ? ? ? 83 EC ? 56 A8", 0x0);
	MAKE_SIGNATURE(IsLocalPlayerUsingVisionFilterFlags, CLIENT_DLL, "55 8B EC 8A 45 ? 56 8B 35", 0x0);
	MAKE_SIGNATURE(KeyValues_SetInt, CLIENT_DLL, "55 8B EC 6A 01 FF 75 08 E8 ? ? ? ? 85 C0 74 0A 8B 4D 0C 89 48 0C C6 40 10 02", 0x0);
	MAKE_SIGNATURE(NetChannel_SendDatagram, ENGINE_DLL, "55 8B EC B8 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 53 56 8B D9", 0x0);
	MAKE_SIGNATURE(NotificationQueue_Add, CLIENT_DLL, "55 8B EC 8B 0D ? ? ? ? 56 57 8B 01 8B 40 68 FF D0 8B 75 08 84 C0 75 09 80 BE ? ? ? ? ? 75 29", 0x0);
	MAKE_SIGNATURE(S_StartDynamicSound, ENGINE_DLL, "55 8B EC 8B 0D ? ? ? ? 81 EC ? ? ? ? 85 C9", 0x0);
	MAKE_SIGNATURE(UTIL_TraceLine, CLIENT_DLL, "53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B ? 89 6C 24 ? 8B EC 83 EC ? 8D 4D ? 56 FF 73 ? FF 73 ? E8 ? ? ? ? 6A", 0x0);
	MAKE_SIGNATURE(ViewRender_PerformScreenSpaceEffects, CLIENT_DLL, "55 8B EC 83 EC ? 8B 0D ? ? ? ? 53 56 57 33 F6 33 FF 89 75 ? 89 7D ? 8B 01 85 C0 74 ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 57 57 57 57 8D 4D ? 51 50 8B 40 ? FF D0 8B 7D ? 83 C4 ? 8B 75 ? 8B 0D ? ? ? ? 8B 19 8B 0D", 0x0);
	MAKE_SIGNATURE(CMaterial_Uncache, MATSYSTEM_DLL, "55 8B EC 81 EC ? ? ? ? 56 8B F1 8B 0D ? ? ? ? 57", 0x0);
	MAKE_SIGNATURE(CTFMatchSummary_OnTick, CLIENT_DLL, "55 8B EC 83 EC ? 53 57 8B F9 E8 ? ? ? ? E8", 0x0);

	// Functions
	MAKE_SIGNATURE(CMatchInviteNotification_AcceptMatch, CLIENT_DLL, "55 8B EC 83 EC 10 56 8B F1 8B 86 ? ? ? ? 83 E8 00", 0x0);
	MAKE_SIGNATURE(CTFKnife_IsBehindAndFacingTarget, CLIENT_DLL, "53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B ? 89 6C 24 ? 8B EC 83 EC ? 56 57 E8 ? ? ? ? 8B F0", 0x0);

	// Values
	MAKE_SIGNATURE(RandomSeed, CLIENT_DLL, "C7 05 ? ? ? ? ? ? ? ? 5D C3 8B 40 34", 0x2);
	MAKE_SIGNATURE(AllowSecureServers, ENGINE_DLL, "C6 05 ? ? ? ? ? 8A C3", 0x2);
}
