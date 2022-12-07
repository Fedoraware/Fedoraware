#pragma once
#include "../../SDK/SDK.h"

struct ResolveData{
	//config data
	bool bEnabled = false;			//	should we resolve this player

	//bruteforce data
	int iYawIndex = 0;

	//logical data
	std::pair<int, float> pLastSniperPitch = {0, 0.f};
	float flPitchNoise = 0.f;		//	noise around sniper dot pitch
	int iPitchNoiseSteps = 0;

	//historical data
	std::pair<std::pair<int, bool>, Vec3> pLastFireAngles = { { 0, false }, {} };
	Vec2 vOriginalAngles = {0.f, 0.f};
};

class PResolver
{
private:
	//sniper dots
	void UpdateSniperDots();
	std::optional<float> GetPitchForSniperDot(CBaseEntity* pEntity);

	//logic
	std::optional<float> PredictBaseYaw(CBaseEntity* pEntity);

	//misc
	bool ShouldRun();
	bool ShouldRunEntity(CBaseEntity* pEntity);
	bool KeepOnShot(CBaseEntity* pEntity);
	bool IsOnShotPitchReliable(const float flPitch);
	float GetRealPitch(const float flPitch);
	void SetAngles(const Vec3 vAngles, CBaseEntity* pEntity);
	int GetPitchMode(CBaseEntity* pEntity);
	int GetYawMode(CBaseEntity* pEntity);
	void OnDormancy(CBaseEntity* pEntity);
	

	//data
	std::unordered_map<CBaseEntity*, CBaseEntity*> mSniperDots;
	std::unordered_map<CBaseEntity*, ResolveData> mResolverData;
	std::pair<int, std::pair<CBaseEntity*, bool>> pWaiting = {0, {nullptr, false}};
public:
	void Aimbot(CBaseEntity* pEntity, const bool bHeadshot);
	void FrameStageNotify();
	void CreateMove();
	void FXFireBullet(int iIndex, const Vec3 vAngles);
	void OnPlayerHurt(CGameEvent* pEvent);
	std::unordered_map<uint32_t, std::pair<int, int>> mResolverMode;	//	pitch, yaw
};

ADD_FEATURE(PResolver, Resolver)