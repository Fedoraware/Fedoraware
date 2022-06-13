#pragma once
#include "../../SDK/SDK.h"

class CAntiAim
{
private:
	void FixMovement(CUserCmd* pCmd, const Vec3& vOldAngles, float fOldSideMove, float fOldForwardMove);
	float EdgeDistance(float edgeRayYaw);
	bool FindEdge(float edgeOrigYaw);
	bool IsOverlapping(float a, float b, float epsilon);
	void setAngle(CUserCmd* pCmd, const bool* pSendPacket, bool& bPitchSet, bool& bYawSet);
public:
	void Run(CUserCmd* pCmd, bool* pSendPacket);
	void Event(CGameEvent* pEvent, const FNV1A_t uNameHash);
};

ADD_FEATURE(CAntiAim, AntiAim)