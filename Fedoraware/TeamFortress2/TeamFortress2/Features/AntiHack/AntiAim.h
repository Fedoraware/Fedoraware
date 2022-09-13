#pragma once
#include "../../SDK/SDK.h"

class CAntiAim
{
private:
	void FixMovement(CUserCmd* pCmd, const Vec3& vOldAngles, float fOldSideMove, float fOldForwardMove);
	float EdgeDistance(float edgeRayYaw);
	bool FindEdge(float edgeOrigYaw);
	bool IsOverlapping(float a, float b, float epsilon);
	float CalculateCustomRealPitch(float WishPitch, bool FakeDown);
	
	void SetupPitch(int iMode, CUserCmd* pCmd);
	float YawIndex(int iIndex);

	float GetBaseYaw(int iMode);

	float flBaseYaw = 0.f;
	float flLastFakeOffset = 0.f;
	float flLastRealOffset = 0.f;
	float flLastPitch = 0.f;
	bool bSendState = true;
	bool bWasHit = false;
	bool bEdge = false;	//	false - right, true - left
	bool bPitchFlip = false;

	Timer tAATimer{};
public:
	void Run(CUserCmd* pCmd, bool* pSendPacket);
	void Event(CGameEvent* pEvent, const FNV1A_t uNameHash);
};

ADD_FEATURE(CAntiAim, AntiAim)