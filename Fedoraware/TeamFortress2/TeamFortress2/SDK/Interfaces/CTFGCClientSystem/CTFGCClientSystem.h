#pragma once
#include "../../Includes/Includes.h"

class CTFParty
{
public:
    // 7 Dec client.so:sub_B986E0
    static CTFParty* GetParty();

    inline static int state_(CTFParty* party)
    {
        return *(reinterpret_cast<int*>(party) + 19);
    }
};

class CTFGCClientSystem
{
public:
    CTFGCClientSystem() = delete;
    static CTFGCClientSystem* GTFGCClientSystem();
    void AbandonCurrentMatch();
    bool BConnectedToMatchServer(bool flag);
    bool BHaveLiveMatch();
    CTFParty* GetParty();
    int JoinMMMatch();
};