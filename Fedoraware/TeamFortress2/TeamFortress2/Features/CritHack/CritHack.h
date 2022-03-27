#pragma once
#include "../../SDK/SDK.h"

class CCritHack {
private:
	int NextCritTick(CUserCmd* pCmd, int pLoops);

public:
	void Run(CUserCmd* pCmd);
	void Draw();
};

inline CCritHack g_CritHack;