#pragma once
#include "../Feature.h"

class CNoSpread {
private:
	float ServerCurTime();
	bool IsPerfectShot(CBaseCombatWeapon* weapon, float flProvidedTime = 0.0f);

	bool bWaitingPerfData = false;
	bool bWaitingForPostSNM = false;
	bool bResyncNeeded = false;
	bool bLastWasPlayerPerf = false;
	bool bShouldUpdateTime = true;
	bool bIsSyncing = false;
	bool bFirstUserCmd = false;
	bool bShouldUpdateUserCmdCorrection = false;
	bool bResyncedThisDeath = false;

	float flPredictionSeed = 0.0f;
	float flMantissaStep = 0.0f;
	float flCurrentWeaponSpread = 0.0f;

	double dFloatTimeDelta = 0.0;
	double dLastSyncDeltaTime = 0.0;
	double dServerTime = 0.0;
	double dTimeStart = 0.0;
	double dWriteUserCmdCorrection = 0.0;
	double dPingAtSend = 0.0;
	double dLastCorrection = 0.0;
	double dLastPingAtSend = 0.0;

	int iNewPackets = 0;

	CUserCmd UserCmdBackup = {};

public:
	float CalcMantissaStep(float flValue);
	void Reset();

	bool SendNetMessage(INetMessage* data);
	void SendNetMessagePost();
	bool DispatchUserMessage(bf_read* buf, int iType);

	void CreateMoveProjectile(CUserCmd* pCmd);
	void ClSendMove();
	void ClSendMovePost();
	void CreateMoveHitscan(CUserCmd* pCmd);

	void ApplySpreadCorrection(Vec3& vAngles, int iSeed, float flSpread);
};
ADD_FEATURE(CNoSpread, NoSpread)
