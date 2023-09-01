#pragma once
#include "../Feature.h"

class CAuto
{
	bool ShouldRun(CBaseEntity* pLocal);

public:
	void Run(CUserCmd* pCmd);
};

ADD_FEATURE(CAuto, Auto)