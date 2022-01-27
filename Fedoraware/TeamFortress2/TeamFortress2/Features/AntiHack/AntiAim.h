#pragma once
#include "../../SDK/SDK.h"

class CAntiAim
{
private:
	void FixMovement(CUserCmd* pCmd, Vec3 vOldAngles, float fOldSideMove, float fOldForwardMove);

public:
	void Run(CUserCmd* pCmd, bool* pSendPacket);
};

inline CAntiAim g_AntiAim;