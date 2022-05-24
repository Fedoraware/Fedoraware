#pragma once
#include "../../../SDK/SDK.h"

class FakeLag
{
public:
	void onTick(CUserCmd* pCmd, CBaseEntity* pLocal, bool* pSendPacket);
private:
	bool flgLogic(int logic, CBaseEntity* pLocal);
	bool isflgAllowed(KeyHelper fakelagKey);
	int chokeCounter = 0;
	int chosenAmount = 0;
};

inline FakeLag g_FLGHandler;