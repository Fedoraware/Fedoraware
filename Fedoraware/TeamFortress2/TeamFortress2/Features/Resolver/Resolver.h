#pragma once
#include "../../SDK/SDK.h"

struct ResolveMode {
	int m_Pitch = 4; // Default to AUTO
	int m_Yaw = 0;
};

struct ResolverData {
	float LastShot = 0.f;
	float LastHit = 0.f;
	int Mode = 0;
	bool RequiresUpdate = false;
};

struct BruteData {
	int BruteNum = 0;
	int HitStreak = 0;
};

class CResolver
{
public:
	void Run();
	void Update(CUserCmd* pCmd);
	void OnPlayerHurt(CGameEvent* pEvent);

	std::map<int, ResolveMode> ResolvePlayers;
	std::map<int, ResolverData> ResolveData;
};

inline CResolver g_Resolver;
