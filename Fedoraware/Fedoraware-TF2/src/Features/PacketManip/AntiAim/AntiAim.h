#pragma once
#include "../../Feature.h"
#include "../../Aimbot/AimbotGlobal/AimbotGlobal.h"
#include "../../Misc/Misc.h"

class CCAntiAim {
private:
	// utils
	inline void FakeShotAngles(CUserCmd* pCmd);
	inline void Keybinds();

	//	logic
	float EdgeDistance(const float flEdgeRayYaw, CBaseEntity* pEntity);

	// angles
	inline float CalculateCustomRealPitch(const float flWishPitch, const bool bFakeDown);
	inline float GetPitch(const int iFake, const int iReal, const float flCurPitch);
	inline float GetYawOffset(const int iIndex, const bool bFake);
	float GetBaseYaw(int iMode, CBaseEntity* pLocal, CUserCmd* pCmd);

	float flBaseYaw = 0.f;
	float flFakeOffset = 0.f;
	float flRealOffset = 0.f;
	bool bInvert = false;
	
public:
	Vec2 vFakeAngles{};
	Vec2 vRealAngles{};
	bool bSendingReal = false;	//	for leg jitter

	bool GetEdge(const float flEdgeOrigYaw, CBaseEntity* pEntity);
	void RunReal(CUserCmd* pCmd);
	void RunFake(CUserCmd* pCmd);
	inline bool ShouldAntiAim(CBaseEntity* pLocal);
};

ADD_FEATURE(CCAntiAim, AntiAim)