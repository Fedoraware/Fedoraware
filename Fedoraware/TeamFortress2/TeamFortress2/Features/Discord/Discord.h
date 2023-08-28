#pragma once
#include "../../SDK/SDK.h"
#include "../../SDK/Discord/include/discord_register.h"
#include "../../SDK/Discord/include/discord_rpc.h"

class CDiscordRPC
{
public:
	void Init();
	void Shutdown();

	void Update();
};

ADD_FEATURE(CDiscordRPC, DiscordRPC)