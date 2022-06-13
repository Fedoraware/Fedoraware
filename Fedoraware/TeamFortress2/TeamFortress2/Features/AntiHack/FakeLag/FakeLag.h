#pragma once
#include "../../../SDK/SDK.h"

class CFakeLag {

	// Update this enum if you're adding/removing modes!
	enum FakelagModes {
		FL_Plain, FL_Random, FL_Velocity
	};

	int ChokeCounter = 0; // How many ticks have been choked
	int ChosenAmount = 0; // How many ticks should be choked

public:
	bool IsAllowed(CBaseEntity* pLocal);
	void OnTick(CUserCmd* pCmd, bool* pSendPacket);
};

ADD_FEATURE(CFakeLag, FakeLag)
