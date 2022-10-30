#pragma once
#include "../../SDK/SDK.h"

class CMisc
{
	void AccurateMovement(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoJump(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoStrafe(CUserCmd* pCmd, CBaseEntity* pLocal);
	void NoiseMakerSpam(CBaseEntity* pLocal);
	void ExtendFreeze(CBaseEntity* pLocal);
	void Freecam(CUserCmd* pCmd, CBaseEntity* pLocal);
	void RageRetry(CBaseEntity* pLocal);
	void AntiBackstab(CBaseEntity* pLocal, CUserCmd* pCmd);
	void LegJitter(CUserCmd* pCmd, CBaseEntity* pLocal);
	void ViewmodelFlip(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoPeek(CUserCmd* pCmd, CBaseEntity* pLocal);
	void StopMovement(CUserCmd* pCmd, bool* pSendPacket);
	void FastDeltaMove(CUserCmd* pCmd, bool* pSendPacket);

	void AntiAFK(CUserCmd* pCmd);
	void ChatSpam();
	void CheatsBypass();
	void PingReducer();
	void WeaponSway();
	void DetectChoke();
	void ChokeCheck(bool* pSendPacket);
	void DoubletapPacket(bool* pSendPacket);
	void EdgeJump(CUserCmd* pCmd, const int nOldGroundEnt);

	void PrintProjAngles(CBaseEntity* pLocal);

	// Late
	void FastStop(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoRocketJump(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoScoutJump(CUserCmd* pCmd, CBaseEntity* pLocal);
	void FastAccel(CUserCmd* pCmd, CBaseEntity* pLocal, bool* pSendPacket);
	float m_flSpinYaw = 0.f;

	bool SteamCleared = false;
	bool bMovementScuffed = false;
public:
	bool TauntControl(CUserCmd* pCmd);
	void RunPre(CUserCmd* pCmd, bool* pSendPacket);
	void RunMid(CUserCmd* pCmd, const int nOldGroundEnt);
	void RunPost(CUserCmd* pCmd, bool* pSendPacket);

	void SteamRPC();
	void UnlockAchievements();
	void LockAchievements();

	bool bAntiWarp = false;
	bool bFastAccel = false;
	bool bMovementStopped = false;

	Vec3 PeekReturnPos;
};

ADD_FEATURE(CMisc, Misc)

class CStatistics
{
public:
	int m_nHighestKillstreak = 0;
	int m_nTotalKills = 0;
	int m_nTotalDeaths = 0;
	int m_nCurrentKillstreak = 0;
	CSteamID m_SteamID;

	void Clear();
	void Submit();
	void Event(CGameEvent* pEvent, const FNV1A_t uNameHash);
};

ADD_FEATURE(CStatistics, Statistics);
