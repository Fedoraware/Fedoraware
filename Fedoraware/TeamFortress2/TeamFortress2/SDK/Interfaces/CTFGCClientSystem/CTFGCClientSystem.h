#pragma once
#include "../../Includes/Includes.h"
#include "../CTFPartyClient/CTFPartyClient.h"

class CTFGCClientSystem
{
public:
	bool Init()
	{
		static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"56 57 8B F9 8D B7 ? ? ? ? C6 46 04 01 8B 0D ? ? ? ? 85 C9 74 0D 8B 01 6A 00 68 ? ? ? ? 56 FF 50 0C C6 46 04 01 8B 0D ? ? ? ?"));
		return FN(this);
	}

	void PreInitGC()
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"56 8B F1 80 BE ? ? ? ? ? 0F 85 ? ? ? ? 68 ? ? ? ?"));
		return FN(this);
	}

	void PostInit()
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC 55 8B EC 83 EC 08 56 68 ? ? ? ?"));
		return FN(this);
	}

	void PostInitGC()
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 08 56 68 ? ? ? ? 8B F1 FF 15 ? ? ? ? A1 ? ? ? ? 83 C4 04 85 C0 74 4E 83 78 04 00 74 48 68 ? ? ? ? FF 15 ? ? ? ?"));
		return FN(this);
	}

	void ReceivedClientWelcome(const int& msg)
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*, const int&)>(g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 56 57 FF 75 08 8B F9 E8 ? ? ? ? 68 ? ? ? ?"));
		return FN(this, msg);
	}

	void Shutdown()
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 08 56 68 ? ? ? ? 8B F1 FF 15 ? ? ? ? A1 ? ? ? ? 83 C4 04 85 C0 74 5B"));
		return FN(this);
	}

	void Update(float frametime)
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*, float)>(g_Pattern.Find(L"client.dll", L"55 8B EC D9 45 08 83 EC 08"));
		return FN(this, frametime);
	}

	CTFPartyClient* GetParty()
	{
		static auto FN = reinterpret_cast<CTFPartyClient * (__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"8B 89 ? ? ? ? 85 C9 74 1D 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 0F 8B 48 18 85 C9 74 08 8B 40 04 8B 44 88 FC C3"));
		return FN(this);
	}

	bool BHaveLiveMatch()
	{
		static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"56 8B F1 8D 8E ? ? ? ? E8 ? ? ? ? 84 C0 74 0D 80 BE ? ? ? ? ? 75 04 B0 01 5E C3"));
		return FN(this);
	}

	int GetNumMatchInvites()
	{
		static auto FN = reinterpret_cast<int(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"8B 89 ? ? ? ? 85 C9 74 12 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 04 8B 40 18 C3"));
		return FN(this);
	}

	bool JoinMMMatch()
	{
		static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"56 8B F1 57 8D 8E ? ? ? ? E8 ? ? ? ? 84 C0 74 32"));
		return FN(this);
	}

	bool BConnectedToMatchServer(bool bLiveMatch)
	{
		static auto FN = reinterpret_cast<bool(__thiscall*)(void*, bool)>(g_Pattern.Find(L"client.dll", L"55 8B EC 53 8A 5D 08 56 8B F1 84 DB 74 20 8D 8E ? ? ? ? E8 ? ? ? ? 84 C0 74 09 80 BE ? ? ? ? ? 74 08"));
		return FN(this, bLiveMatch);
	}

	bool BGetLocalPlayerBadgeInfoForTour(int iTourIndex, uint32* pnBadgeLevel, uint32* pnCompletedChallenges)
	{
		static auto FN = reinterpret_cast<bool(__thiscall*)(void*, int, uint32*, uint32*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 08 8B 45 0C 56 C7 00 ? ? ? ? 8B 45 10 C7 00 ? ? ? ? E8 ? ? ? ? 8B C8 8B 10 FF 52 58 8B F0 89 75 F8 85 F6 75 07 32 C0 5E 8B E5 5D C3"));
		return FN(this, iTourIndex, pnBadgeLevel, pnCompletedChallenges);
	}

	bool BHasCompetitiveAccess()
	{
		static auto FN = reinterpret_cast<bool(__cdecl*)()>(g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? 8B C8 8B 10 FF 52 58 85 C0 75 03 32 C0 C3 8B 48 68 85 C9 74 F6 6A 07 E8 ? ? ? ? 85 C0 74 25 83 78 18 01 75 1F 8B 40 04 8B 08 85 C9 74 16 8B 41 0C C1 E8 0A A8 01 74 0C 80 79 48 00 74 06 B8"));
		return FN();
	}

	bool BIsMatchGroupDisabled(int eMatchGroup)
	{
		static auto FN = reinterpret_cast<bool(__thiscall*)(void*, int)>(g_Pattern.Find(L"client.dll", L"55 8B EC 51 56 8B B1 ? ? ? ? 33 D2 C7 45 ? ? ? ? ? 85 F6 7E 15 8B 81 ? ? ? ? 8B 4D 08 39 08 74 11 42 83 C0 04 3B D6 7C F4"));
		return FN(this, eMatchGroup);
	}

	void ConnectToServer(const char* connect)
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*, const char*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 51 57 FF 75 08 8B F9 68 ? ? ? ? FF 15 ? ? ? ? FF 75 08 8D 45 FC C7 45 ? ? ? ? ? 68 ? ? ? ? 50 E8 ? ? ? ? 83 C4 14 83 3D ? ? ? ? ?"));
		return FN(this, connect);
	}

	void PingThink()
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 53 56 8B F1 89 75 CC FF 15 ? ? ? ? 8B D8 85 DB 0F 84 ? ? ? ? 80 BE ? ? ? ? ? 75 3D"));
		return FN(this);
	}

	void DumpPing()
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 20 57 8B F9 C7 45 ? ? ? ? ? 8B 87 ? ? ? ? 85 C0 75 23 0F B6 87 ? ? ? ? 50 0F B6 87 ? ? ? ? 50 68 ? ? ? ? FF 15 ? ? ? ?"));
		return FN(this);
	}

	void FireGameEvent(CGameEvent* pEvent)
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*, CGameEvent*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 53 8B 5D 08 56 8B F1 8B CB 57 8B 03 FF 50 04 8B F8 68 ? ? ? ? 57 E8 ? ? ? ? 83 C4 08 85 C0 75 65"));
		return FN(this, pEvent);
	}

	void* GetLobby()
	{
		static auto FN = reinterpret_cast<void* (__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"8B 89 ? ? ? ? 85 C9 74 33 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 25 8B 48 18 85 C9 74 1E"));
		return FN(this);
	}

	void* GetMatchInvite(void* pUnknown1, void* pUnknown2)
	{
		static auto FN = reinterpret_cast<void* (__thiscall*)(void*, void*, void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 10 56 57 8B F9 8B 8F ? ? ? ? 85 C9 74 13 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 07 8B 48 18 EB 04"));
		return FN(this, pUnknown1, pUnknown2);
	}

	void DumpLobby()
	{
		static auto FN = reinterpret_cast<void(__cdecl*)()>(g_Pattern.Find(L"client.dll", L"8B 0D ? ? ? ? 85 C9 74 3D 68 ? ? ? ?"));
		return FN();
	}

	void DumpParty()
	{
		static auto FN = reinterpret_cast<void(__cdecl*)()>(g_Pattern.Find(L"client.dll", L"8B 0D ? ? ? ? 85 C9 74 24 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 16 8B 48 18 85 C9 74 0F"));
		return FN();
	}
};