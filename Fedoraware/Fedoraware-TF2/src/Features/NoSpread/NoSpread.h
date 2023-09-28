#pragma once
#include "../Feature.h"

class CNoSpread {
public:
	void CreateMove(CUserCmd* cmd);
};
ADD_FEATURE(CNoSpread, NoSpread)
