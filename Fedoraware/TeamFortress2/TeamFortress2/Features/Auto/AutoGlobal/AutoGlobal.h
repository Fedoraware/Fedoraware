#pragma once
#include "../../../SDK/SDK.h"

class CAutoGlobal
{
public:
	bool IsKeyDown();
	bool ShouldIgnore(CBaseEntity* pTarget);
};

ADD_FEATURE(CAutoGlobal, AutoGlobal)