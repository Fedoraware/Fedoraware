#pragma once
#include "../../Includes/Includes.h"
#include "../CTFPartyClient/CTFPartyClient.h"

class CTFGCClientSystem
{
public:
    void PreInitGC()
    {
        static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"56 8B F1 80 BE ? ? ? ? ? 0F 85 ? ? ? ? 68 ? ? ? ?"));
        return FN(this);
    }

    CTFPartyClient* GetParty()
    {
        static auto FN = reinterpret_cast<CTFPartyClient*(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"8B 89 ? ? ? ? 85 C9 74 1D 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 0F 8B 48 18 85 C9 74 08 8B 40 04 8B 44 88 FC C3"));
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
};