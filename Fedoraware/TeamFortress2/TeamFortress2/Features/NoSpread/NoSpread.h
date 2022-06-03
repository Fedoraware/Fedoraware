#pragma once
#include "../../SDK/SDK.h"

class CNoSpread {
public:
	bool ShouldRun(bool isProjectile);
	void RunLate(CUserCmd* pCmd);
};

ADD_FEATURE(CNoSpread, NoSpread);