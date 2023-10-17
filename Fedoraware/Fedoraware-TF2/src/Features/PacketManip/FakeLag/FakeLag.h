#pragma once
#include "../../Feature.h"
#include "../../Aimbot/AimbotGlobal/AimbotGlobal.h"

class CFakelag {
private:
	enum FakelagModes { // Update this enum if you're adding/removing modes!
		FL_Plain,
		FL_Random,
		FL_Adaptive
	};

	int iChokeAmount = 0;
	int iChokeGoal = 0;
	int iAirTicks = 0;	//	if this goes above 14 we need to stop fakelag from going further or else the player will rubberband badly, this can be fixed i just don't know how lol

	Vec3 vLastPosition{};

	bool bPreservingBlast = false;

	inline bool IsAllowed(CBaseEntity* pLocal);
	inline void Prediction(const int nOldGroundInt, const int nOldFlags);
	inline void PreserveBlastJump(const int nOldGroundInt, const int nOldFlags);

public:
	void Run(CUserCmd* pCmd, bool* pSendPacket, const int nOldGround, const int nOldFlags);
};

ADD_FEATURE(CFakelag, FakeLag)