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
};