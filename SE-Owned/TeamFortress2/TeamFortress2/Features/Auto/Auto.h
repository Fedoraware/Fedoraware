#pragma once
#include "../../SDK/SDK.h"

class CAuto
{
private:
	bool ShouldRun(CBaseEntity* pLocal);

public:
	void Run(CUserCmd* pCmd);
};

inline CAuto g_Auto;