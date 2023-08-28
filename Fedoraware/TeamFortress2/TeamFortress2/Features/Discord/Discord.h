#pragma once
#include "../Feature.h"

class CDiscordRPC
{
public:
	void Init();
	void Shutdown();

	void Update();
};

ADD_FEATURE(CDiscordRPC, DiscordRPC)