#pragma once
#include "../../Includes/Includes.h"

namespace S
{
	MAKE_SIGNATURE(CTFPartyClient_LoadSavedCasualCriteria, CLIENT_DLL, "83 79 ? ? C6 81 ? ? ? ? ? 74 ? 80 79 ? ? 74 ? C6 81 ? ? ? ? ? FF 35", 0x0);
	MAKE_SIGNATURE(CTFPartyClient_BInStandbyQueue, CLIENT_DLL, "8A 41 ? C3 CC CC CC CC CC CC CC CC CC CC CC CC 55 8B EC 81 EC ? ? ? ? 56", 0x0);
	MAKE_SIGNATURE(CTFPartyClient_BCanQueueForMatch, CLIENT_DLL, "55 8B EC 81 EC ? ? ? ? 8B 45 ? 89 45 ? 8D 45", 0x0);
	MAKE_SIGNATURE(CTFPartyClient_BCanQueueForStandby, CLIENT_DLL, "55 8B EC 83 EC ? 56 6A ? 8B F1 E8 ? ? ? ? 84 C0", 0x0);
	MAKE_SIGNATURE(CTFPartyClient_BInQueueForMatchGroup, CLIENT_DLL, "55 8B EC 56 8B 75 ? 57 56 8B F9 E8 ? ? ? ? 83 C4 ? 84 C0 74 ? 83 FE ? 74 ? 8B 57 ? 33 C0 85 D2 7E ? 8B 4F ? 39 31 74 ? 40 83 C1 ? 3B C2 7C ? 83 C8 ? 83 F8 ? 5F", 0x0);

	MAKE_SIGNATURE(CTFPartyClient_BMakeUpdateMsg, CLIENT_DLL, "55 8B EC 83 EC ? 53 56 8B F1 33 C9 8B 86", 0x0);
	MAKE_SIGNATURE(CTFPartyClient_CancelOutgoingJoinRequestOrIncomingInvite, CLIENT_DLL, "55 8B EC 83 EC ? 56 8B F1 83 7E ? ? 74 ? 80 7E", 0x0);
	MAKE_SIGNATURE(CTFPartyClient_CheckResetSentOptions, CLIENT_DLL, "55 8B EC 83 EC ? A1 ? ? ? ? 53 56 57 83 78 ? ? 8B D9", 0x0);
	MAKE_SIGNATURE(CTFPartyClient_OnInQueueChanged, CLIENT_DLL, "55 8B EC 56 8B 75 ? 57 56 8B F9 E8 ? ? ? ? 83 C4 ? 84 C0 74 ? 83 FE ? 74 ? 8B 57 ? 33 C0 85 D2 7E ? 8B 4F ? 39 31 74 ? 40 83 C1 ? 3B C2 7C ? 83 C8 ? 83 F8 ? 0F 95 C0", 0x0);
	MAKE_SIGNATURE(CTFPartyClient_RequestQueueForStandby, CLIENT_DLL, "55 8B EC 83 EC ? 56 8B F1 83 7E ? ? 0F 84 ? ? ? ? 80 BE", 0x0);

	MAKE_SIGNATURE(CTFPartyClient_UpdateActiveParty, CLIENT_DLL, "55 8B EC 81 EC ? ? ? ? 8B 15 ? ? ? ? 53 56 57 33 FF 8B F1", 0x0);
	MAKE_SIGNATURE(CTFPartyClient_RequestQueueForMatch, CLIENT_DLL, "55 8B EC 83 EC ? 56 8B 75 ? 57 56 8B F9 E8 ? ? ? ? 83 C4 ? 84 C0", 0x0);
}

class CTFPartyClient
{
public:
	void LoadSavedCasualCriteria()
	{
		static auto FN = S::CTFPartyClient_LoadSavedCasualCriteria.As<void(__thiscall*)(void*)>();
		return FN(this);
	}

	bool BInStandbyQueue()
	{
		static auto FN = S::CTFPartyClient_BInStandbyQueue.As<bool(__thiscall*)(void*)>();
		return FN(this);
	}

	//CTFPartyClient::BCanQueueForMatch(ETFMatchGroup, CUtlVector<CTFPartyClient::QueueEligibilityData_t, CUtlMemory<CTFPartyClient::QueueEligibilityData_t, int>>&)
	bool BCanQueueForMatch(int eMatchGroup, void* vecQueueEligibilityData)
	{
		static auto FN = S::CTFPartyClient_BCanQueueForMatch.As<bool(__thiscall*)(void*, int, void*)>();
		return FN(this, eMatchGroup, vecQueueEligibilityData);
	}

	bool BCanQueueForStandby()
	{
		static auto FN = S::CTFPartyClient_BCanQueueForStandby.As<bool(__thiscall*)(void*)>();
		return FN(this);
	}

	bool BInQueueForMatchGroup(int eMatchGroup)
	{
		static auto FN = S::CTFPartyClient_BInQueueForMatchGroup.As<bool(__thiscall*)(void*, int)>();
		return FN(this, eMatchGroup);
	}

	//CTFPartyOptions&
	bool BMakeUpdateMsg(void* pPartyOptions)
	{
		static auto FN = S::CTFPartyClient_BMakeUpdateMsg.As<bool(__thiscall*)(void*, void*)>();
		return FN(this, pPartyOptions);
	}

	void CancelOutgoingJoinRequestOrIncomingInvite(CSteamID steamID)
	{
		static auto FN = S::CTFPartyClient_CancelOutgoingJoinRequestOrIncomingInvite.As<void(__thiscall*)(void*, CSteamID)>();
		return FN(this, steamID);
	}

	void CheckResetSentOptions()
	{
		static auto FN = S::CTFPartyClient_CheckResetSentOptions.As<void(__thiscall*)(void*)>();
		return FN(this);
	}

	void OnInQueueChanged()
	{
		static auto FN = S::CTFPartyClient_OnInQueueChanged.As<void(__thiscall*)(void*)>();
		return FN(this);
	}

	void RequestQueueForStandby()
	{
		static auto FN = S::CTFPartyClient_RequestQueueForStandby.As<void(__thiscall*)(void*)>();
		return FN(this);
	}

	bool UpdateActiveParty()
	{
		static auto FN = S::CTFPartyClient_UpdateActiveParty.As<bool(__thiscall*)(void*)>();
		return FN(this);
	}

	void RequestQueueForMatch(int eMatchGroup)
	{
		static auto FN = S::CTFPartyClient_RequestQueueForMatch.As<void(__thiscall*)(void*, int)>();
		return FN(this, eMatchGroup);
	}
};