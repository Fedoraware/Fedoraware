#pragma once
#include "../../Includes/Includes.h"
#include "../CTFPartyClient/CTFPartyClient.h"

namespace S
{
	MAKE_SIGNATURE(CTFGCClientSystem_Init, CLIENT_DLL, "56 57 8B F9 8D B7 ? ? ? ? C6 46", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_PreInitGC, CLIENT_DLL, "56 8B F1 80 BE ? ? ? ? ? 0F 85 ? ? ? ? 68", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_PostInit, CLIENT_DLL, "E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC 55 8B EC 83 EC ? 56 68", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_PostInitGC, CLIENT_DLL, "55 8B EC 83 EC ? 56 68 ? ? ? ? 8B F1 FF 15 ? ? ? ? A1 ? ? ? ? 83 C4 ? 85 C0 74 ? 83 78 ? ? 74 ? 68 ? ? ? ? FF 15 ? ? ? ? A1 ? ? ? ? 8D 55 ? 83 C4 ? 8B 48 ? 52 8B 01 FF 50 ? 8D 86", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_ReceivedClientWelcome, CLIENT_DLL, "55 8B EC 81 EC ? ? ? ? 56 57 FF 75 ? 8B F9 E8 ? ? ? ? 68", 0x0);

	MAKE_SIGNATURE(CTFGCClientSystem_Shutdown, CLIENT_DLL, "55 8B EC 83 EC ? 56 68 ? ? ? ? 8B F1 FF 15 ? ? ? ? A1 ? ? ? ? 83 C4 ? 85 C0 74 ? 83 78 ? ? 74 ? 68 ? ? ? ? FF 15 ? ? ? ? A1 ? ? ? ? 8D 55 ? 83 C4 ? 8B 48 ? 52 8B 01 FF 50 ? 8D 45", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_Update, CLIENT_DLL, "55 8B EC D9 45 ? 83 EC ? 53 56", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_GetParty, CLIENT_DLL, "8B 89 ? ? ? ? 85 C9 74 ? 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 ? 8B 48 ? 85 C9 74 ? 8B 40 ? 8B 44 88", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_BHaveLiveMatch, CLIENT_DLL, "56 8B F1 8D 8E ? ? ? ? E8 ? ? ? ? 84 C0 74", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_GetNumMatchInvites, CLIENT_DLL, "8B 89 ? ? ? ? 85 C9 74 ? 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 ? 8B 40", 0x0);

	MAKE_SIGNATURE(CTFGCClientSystem_JoinMMMatch, CLIENT_DLL, "56 8B F1 57 8D 8E ? ? ? ? E8 ? ? ? ? 84 C0 74 ? 80 BE", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_BConnectedToMatchServer, CLIENT_DLL, "55 8B EC 53 8A 5D ? 56 8B F1 84 DB 74 ? 8D 8E", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_BGetLocalPlayerBadgeInfoForTour, CLIENT_DLL, "55 8B EC 83 EC ? 8B 45 ? 56 C7 00", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_BHasCompetitiveAccess, CLIENT_DLL, "E8 ? ? ? ? 8B C8 8B 10 FF 52 ? 85 C0 75 ? 32 C0 C3 8B 48 ? 85 C9 74 ? 6A ? E8 ? ? ? ? 85 C0 74 ? 83 78 ? ? 75 ? 8B 40 ? 8B 08 85 C9 74 ? 8B 41 ? C1 E8 ? A8 ? 74 ? 80 79 ? ? 74 ? B8 ? ? ? ? C3 33 C0 C3 CC CC CC 56", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_BIsMatchGroupDisabled, CLIENT_DLL, "55 8B EC 51 56 8B B1 ? ? ? ? 33 D2 C7 45", 0x0);

	MAKE_SIGNATURE(CTFGCClientSystem_ConnectToServer, CLIENT_DLL, "55 8B EC 51 57 FF 75 ? 8B F9 68", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_PingThink, CLIENT_DLL, "55 8B EC 81 EC ? ? ? ? 53 56 8B F1 89 75 ? FF 15", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_DumpPing, CLIENT_DLL, "55 8B EC 83 EC ? 57 8B F9 C7 45 ? ? ? ? ? 8B 87", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_FireGameEvent, CLIENT_DLL, "55 8B EC 53 8B 5D ? 56 8B F1 8B CB 57 8B 03 FF 50 ? 8B F8 68", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_GetLobby, CLIENT_DLL, "8B 89 ? ? ? ? 85 C9 74 ? 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 ? 8B 48 ? 85 C9 74 ? 8B 40 ? 6A", 0x0);

	MAKE_SIGNATURE(CTFGCClientSystem_GetMatchInvite, CLIENT_DLL, "55 8B EC 83 EC ? 56 57 8B F9 8B 8F ? ? ? ? 85 C9 74 ? 68", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_DumpLobby, CLIENT_DLL, "8B 0D ? ? ? ? 85 C9 74 ? 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 ? 8B 48 ? 85 C9 74 ? 8B 40 ? 6A", 0x0);
	MAKE_SIGNATURE(CTFGCClientSystem_DumpParty, CLIENT_DLL, "8B 0D ? ? ? ? 85 C9 74 ? 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 ? 8B 48 ? 85 C9 74 ? 8B 40 ? 8B 4C 88", 0x0);
}

class CTFGCClientSystem
{
public:
	bool Init()
	{
		static auto FN = S::CTFGCClientSystem_Init.As<bool(__thiscall*)(void*)>();
		return FN(this);
	}

	void PreInitGC()
	{
		static auto FN = S::CTFGCClientSystem_PreInitGC.As<void(__thiscall*)(void*)>();
		return FN(this);
	}

	void PostInit()
	{
		static auto FN = S::CTFGCClientSystem_PostInit.As<void(__thiscall*)(void*)>();
		return FN(this);
	}

	void PostInitGC()
	{
		static auto FN = S::CTFGCClientSystem_PostInitGC.As<void(__thiscall*)(void*)>();
		return FN(this);
	}

	void ReceivedClientWelcome(const int& msg)
	{
		static auto FN = S::CTFGCClientSystem_ReceivedClientWelcome.As<void(__thiscall*)(void*, const int&)>();
		return FN(this, msg);
	}

	void Shutdown()
	{
		static auto FN = S::CTFGCClientSystem_Shutdown.As<void(__thiscall*)(void*)>();
		return FN(this);
	}

	void Update(float frametime)
	{
		static auto FN = S::CTFGCClientSystem_Update.As<void(__thiscall*)(void*, float)>();
		return FN(this, frametime);
	}

	CTFPartyClient* GetParty()
	{
		static auto FN = S::CTFGCClientSystem_GetParty.As<CTFPartyClient*(__thiscall*)(void*)>();
		return FN(this);
	}

	bool BHaveLiveMatch()
	{
		static auto FN = S::CTFGCClientSystem_BHaveLiveMatch.As<bool(__thiscall*)(void*)>();
		return FN(this);
	}

	int GetNumMatchInvites()
	{
		static auto FN = S::CTFGCClientSystem_GetNumMatchInvites.As<int(__thiscall*)(void*)>();
		return FN(this);
	}

	bool JoinMMMatch()
	{
		static auto FN = S::CTFGCClientSystem_JoinMMMatch.As<bool(__thiscall*)(void*)>();
		return FN(this);
	}

	bool BConnectedToMatchServer(bool bLiveMatch)
	{
		static auto FN = S::CTFGCClientSystem_BConnectedToMatchServer.As<bool(__thiscall*)(void*, bool)>();
		return FN(this, bLiveMatch);
	}

	bool BGetLocalPlayerBadgeInfoForTour(int iTourIndex, uint32* pnBadgeLevel, uint32* pnCompletedChallenges)
	{
		static auto FN = S::CTFGCClientSystem_BGetLocalPlayerBadgeInfoForTour.As<bool(__thiscall*)(void*, int, uint32*, uint32*)>();
		return FN(this, iTourIndex, pnBadgeLevel, pnCompletedChallenges);
	}

	bool BHasCompetitiveAccess()
	{
		static auto FN = S::CTFGCClientSystem_BHasCompetitiveAccess.As<bool(__cdecl*)()>();
		return FN();
	}

	bool BIsMatchGroupDisabled(int eMatchGroup)
	{
		static auto FN = S::CTFGCClientSystem_BIsMatchGroupDisabled.As<bool(__thiscall*)(void*, int)>();
		return FN(this, eMatchGroup);
	}

	void ConnectToServer(const char* connect)
	{
		static auto FN = S::CTFGCClientSystem_ConnectToServer.As<void(__thiscall*)(void*, const char*)>();
		return FN(this, connect);
	}

	void PingThink()
	{
		static auto FN = S::CTFGCClientSystem_PingThink.As<void(__thiscall*)(void*)>();
		return FN(this);
	}

	void DumpPing()
	{
		static auto FN = S::CTFGCClientSystem_DumpPing.As<void(__thiscall*)(void*)>();
		return FN(this);
	}

	void FireGameEvent(CGameEvent* pEvent)
	{
		static auto FN = S::CTFGCClientSystem_FireGameEvent.As<void(__thiscall*)(void*, CGameEvent*)>();
		return FN(this, pEvent);
	}

	void* GetLobby()
	{
		static auto FN = S::CTFGCClientSystem_GetLobby.As<void* (__thiscall*)(void*)>();
		return FN(this);
	}

	void* GetMatchInvite(void* pUnknown1, void* pUnknown2)
	{
		static auto FN = S::CTFGCClientSystem_GetMatchInvite.As<void* (__thiscall*)(void*, void*, void*)>();
		return FN(this, pUnknown1, pUnknown2);
	}

	void DumpLobby()
	{
		static auto FN = S::CTFGCClientSystem_DumpLobby.As<void(__cdecl*)()>();
		return FN();
	}

	void DumpParty()
	{
		static auto FN = S::CTFGCClientSystem_DumpParty.As<void(__cdecl*)()>();
		return FN();
	}
};