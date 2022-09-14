#pragma once
#include "../../SDK/SDK.h"

class CAntiAim
{
private:
	// utils
	void FixMovement(CUserCmd* pCmd, const Vec3& vOldAngles, float fOldSideMove, float fOldForwardMove);

	// logic
	float EdgeDistance(float edgeRayYaw);
	bool FindEdge(float edgeOrigYaw);
	bool IsOverlapping(float epsilon);
	bool ShouldAntiAim(CBaseEntity* pLocal);
	
	// angles
	float CalculateCustomRealPitch(float WishPitch, bool FakeDown);
	void SetupPitch(int iMode, CUserCmd* pCmd);
	float YawIndex(int iIndex);
	float GetBaseYaw(int iMode, CBaseEntity* pLocal, CUserCmd* pCmd);

	float flBaseYaw = 0.f;
	float flLastFakeOffset = 0.f;
	float flLastRealOffset = 0.f;
	float flLastPitch = 0.f;
	bool bSendState = true;
	bool bWasHit = false;
	bool bEdge = false;	//	false - right, true - left
	bool bPitchFlip = false;
	bool bInvert = false;
	std::pair<bool, bool> p_bJitter = {false, false};

	Timer tAATimer{};
public:
	void Run(CUserCmd* pCmd, bool* pSendPacket);
	void Event(CGameEvent* pEvent, const FNV1A_t uNameHash);
};

ADD_FEATURE(CAntiAim, AntiAim)