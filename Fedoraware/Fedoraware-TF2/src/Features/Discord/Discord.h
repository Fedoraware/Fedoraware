#pragma once
#include "../Feature.h"

class CDiscordRPC
{
	bool m_IsActive = false;
	bool m_Loaded = false;

public:
	CDiscordRPC();

	void Init();
	void Shutdown();
	void Update();

	bool IsLoaded() { return m_Loaded; }
};

ADD_FEATURE(CDiscordRPC, DiscordRPC)