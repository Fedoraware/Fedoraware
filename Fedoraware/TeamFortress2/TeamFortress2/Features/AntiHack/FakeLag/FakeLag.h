#pragma once
#include "../../../SDK/SDK.h"

class CFakeLag
{
	enum FakelagModes // Update this enum if you're adding/removing modes!
	{
		FL_Plain, 
		FL_Random, 
		FL_Adaptive 
	};

	int ChokeCounter = 0; // How many ticks have been choked
	int ChosenAmount = 0; // How many ticks should be choked
	std::pair<bool, int> pInAirTicks = {false, 0};	//	started on ground, ticks in air
	Vec3 vLastPosition;

	bool IsVisible(CBaseEntity* pLocal);
	bool IsAllowed(CBaseEntity* pLocal);
	bool DuckLogic(CBaseEntity* pLocal);

public:
	void OnTick(CUserCmd* pCmd, bool* pSendPacket);
};

ADD_FEATURE(CFakeLag, FakeLag)
