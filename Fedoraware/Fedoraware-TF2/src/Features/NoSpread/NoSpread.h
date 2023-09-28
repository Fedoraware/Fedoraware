#pragma once
#include "../Feature.h"

class CNoSpread {
public:
	void CreateMoveProjectile(CUserCmd* cmd);
};
ADD_FEATURE(CNoSpread, NoSpread)
