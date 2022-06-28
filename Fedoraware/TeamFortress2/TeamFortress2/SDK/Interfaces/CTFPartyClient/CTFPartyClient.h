#pragma once
#include "../../Includes/Includes.h"

class CTFPartyClient
{
public:
	void LoadSavedCasualCriteria()
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"83 79 18 00 C6 81 ? ? ? ? ? 74 06 80 79 28 00 74 07 C6 81 ? ? ? ? ? FF 35 ? ? ? ? 81 C1 ? ? ? ? E8 ? ? ? ? C3"));
		return FN(this);
	}

	bool BInStandbyQueue()
	{
		static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"8A 41 40 C3"));
		return FN(this);
	}

	//CTFPartyClient::BCanQueueForMatch(ETFMatchGroup, CUtlVector<CTFPartyClient::QueueEligibilityData_t, CUtlMemory<CTFPartyClient::QueueEligibilityData_t, int>>&)
	bool BCanQueueForMatch(int eMatchGroup, void* vecQueueEligibilityData)
	{
		static auto FN = reinterpret_cast<bool(__thiscall*)(void*, int, void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 8B 45 0C 89 45 FC 8D 45 08 56 50 8B F1 E8 ? ? ? ? 83 C4 04 89 45 F4"));
		return FN(this, eMatchGroup, vecQueueEligibilityData);
	}

	bool BCanQueueForStandby()
	{
		static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 14 56 6A 00 8B F1 E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 38 46 40 0F 85 ? ? ? ? 8B 4E 18 85 C9 0F 84 ? ? ? ?"));
		return FN(this);
	}

	bool BInQueueForMatchGroup(int eMatchGroup)
	{
		static auto FN = reinterpret_cast<bool(__thiscall*)(void*, int)>(g_Pattern.Find(L"client.dll", L"55 8B EC 56 8B 75 08 57 56 8B F9 E8 ? ? ? ? 83 C4 04 84 C0 74 2C 83 FE FF 74 27"));
		return FN(this, eMatchGroup);
	}

	//CTFPartyOptions&
	bool BMakeUpdateMsg(void* pPartyOptions)
	{
		static auto FN = reinterpret_cast<bool(__thiscall*)(void*, void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 60 53 56 8B F1 33 C9 8B 86 ? ? ? ? 8D 96 ? ? ? ? 3B 86 ? ? ? ? 75 0B 8B 42 14 3B 86 ? ? ? ? 74 0F"));
		return FN(this, pPartyOptions);
	}

	void CancelOutgoingJoinRequestOrIncomingInvite(CSteamID steamID)
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*, CSteamID)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 14 56 8B F1 83 7E 18 00 74 0A 80 7E 28 00 0F 84 ? ? ? ?"));
		return FN(this, steamID);
	}

	void CheckResetSentOptions()
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 3C A1 ? ? ? ? 53 56 57 83 78 30 00 8B D9 C7 45 ? ? ? ? ? 74 0E 68 ? ? ? ? FF 15 ? ? ? ?"));
		return FN(this);
	}

	void OnInQueueChanged()
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 56 8B 75 08 57 56 8B F9 E8 ? ? ? ? 83 C4 04 84 C0 74 38 83 FE FF 74 33"));
		return FN(this);
	}

	void RequestQueueForStandby()
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 18 56 8B F1 83 7E 18 00 0F 84 ? ? ? ? 80 BE ? ? ? ? ? 0F 85 ? ? ? ?"));
		return FN(this);
	}

	bool UpdateActiveParty()
	{
		static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 8B 15 ? ? ? ? 53 56 57 33 FF 8B F1 89 BD ? ? ? ? 89 BD ? ? ? ? 8B 02 89 75 FC 89 BD ? ? ? ? 89 BD ? ? ? ? 85 C0 74 4B"));
		return FN(this);
	}

	void RequestQueueForMatch(int eMatchGroup)
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(void*, int)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 38 56 8B 75 08 57 56 8B F9 E8 ? ? ? ? 83 C4 04 84 C0 74 14 83 FE FF 74 0F 8A 84 3E ? ? ? ? 84 C0 0F 85 ? ? ? ?"));
		return FN(this, eMatchGroup);
	}
};