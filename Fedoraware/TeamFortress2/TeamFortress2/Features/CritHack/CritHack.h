#pragma once
#include "../../SDK/SDK.h"

class CCritHack {
private:
	int NextCritTick(const CUserCmd* pCmd, int pLoops);

	struct stats_t
	{
		float flCritBucket; // A54
		int iNumAttacks; // A58
		int iNumCrits; // A5C
	};

public:
	void Run(CUserCmd* pCmd);
	void Draw();
};

inline CCritHack g_CritHack;