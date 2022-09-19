#pragma once
#include "../../SDK/SDK.h"
#include "../../Hooks/HookManager.h"
#include "../../Hooks/Hooks.h"

class CTickshiftHandler {
private:
	// logic
	void Speedhack(CUserCmd* pCmd);
	void Recharge(CUserCmd* pCmd);
	void Teleport(CUserCmd* pCmd);
	void Doubletap(CUserCmd* pCmd, CBaseEntity* pLocal);
	
	// utils
	void CLMoveFunc(float accumulated_extra_samples, bool bFinalTick);

	//
	bool bSpeedhack = false;
	bool bTeleport = false;
	bool bRecharge = false;
	bool bDoubletap = false;
	int iAvailableTicks = 0;	//	should be equal to G::ShiftedTicks

public:
	void CLMove(float accumulated_extra_samples, bool bFinalTick);	//	to be run from CL_Move.cpp
	void CreateMove(CUserCmd* pCmd);								//	to be run from ClientModeShared_CreateMove.cpp
};

ADD_FEATURE(CTickshiftHandler, Ticks)