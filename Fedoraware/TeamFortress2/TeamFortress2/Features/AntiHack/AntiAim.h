#pragma once
#include "../../SDK/SDK.h"

class CAntiAim
{
	void FixMovement(CUserCmd* pCmd, const Vec3& vOldAngles, float fOldSideMove, float fOldForwardMove);
	float EdgeDistance(float edgeRayYaw);
	bool FindEdge(float edgeOrigYaw);
	bool IsOverlapping(float a, float b, float epsilon);
public:
	void Run(CUserCmd* pCmd, bool* pSendPacket);
	void Event(CGameEvent* pEvent, const FNV1A_t uNameHash);
};

inline CAntiAim g_AntiAim;
