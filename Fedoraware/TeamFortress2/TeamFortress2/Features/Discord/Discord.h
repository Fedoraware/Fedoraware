#pragma once
#include "../../SDK/Discord/include/discord_register.h"
#include "../../SDK/Discord/include/discord_rpc.h"

class CDiscordRPC
{
public:
	void Update();
};

inline CDiscordRPC g_DiscordRPC;