#pragma once
#include "../Feature.h"

class CNoSpread {
private:
	float PrevServerTime = 0.0f;
	float AskTime = 0.0f;
	float GuessTime = 0.0f;
	bool WaitingForPP = false;
	float GuessDelta = 0.0f;
	float ResponseTime = 0.0f;

public:
	bool Synced = false;
	float ServerTime = 0.0f;
	float SyncOffset = 0.0f;

	void CreateMoveProjectile(CUserCmd* pCmd);
	void AskForPlayerPerf();
	bool ParsePlayerPerf(bf_read& msg_data);
	int GetSeed();
	void Reset();
	void CreateMoveHitscan(CUserCmd* cmd);
};
ADD_FEATURE(CNoSpread, NoSpread)
