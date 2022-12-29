#pragma once
#include "../../SDK/SDK.h"

class CTickshiftHandler
{
	// logic
	void Speedhack(CUserCmd* pCmd);
	void Recharge(CUserCmd* pCmd);
	void Teleport(CUserCmd* pCmd);
	void Doubletap(const CUserCmd* pCmd, CBaseEntity* pLocal);

	// utils
	void CLMoveFunc(float accumulated_extra_samples, bool bFinalTick);

	//
	bool bSpeedhack = false;
	bool bTeleport = false;
	bool bRecharge = false;
	bool bDoubletap = false;
	int iAvailableTicks = 0;	//	should be equal to G::ShiftedTicks
	int iNextPassiveTick = 0;
	int iTickRate = 0;

public:
	int iDeficit = 0;
	int iPredicted = 0;	//	DEBUG
	bool MeleeDoubletapCheck(CBaseEntity* pLocal);	//	checks if we WILL doubletap, used by melee aimbot from AimbotMelee.cpp
	void CLMove(float accumulated_extra_samples, bool bFinalTick);	//	to be run from CL_Move.cpp
	void CreateMove(CUserCmd* pCmd);								//	to be run from ClientModeShared_CreateMove.cpp
	void Reset();
	void DrawDebug();
};

ADD_FEATURE(CTickshiftHandler, Ticks)