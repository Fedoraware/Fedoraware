#pragma once
#include "../Feature.h"

class CDiscordRPC
{
	bool m_IsActive = false;

public:
	void Init();
	void Shutdown();

	void Update();
};

ADD_FEATURE(CDiscordRPC, DiscordRPC)