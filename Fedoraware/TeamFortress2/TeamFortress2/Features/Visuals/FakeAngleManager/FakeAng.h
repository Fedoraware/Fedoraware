#pragma once
#include "../../../SDK/SDK.h"

typedef struct FakeMatrixes {
	float BoneMatrix[128][3][4];
};

class CFakeAng {
public:
	FakeMatrixes BoneMatrix;
	void Run(CUserCmd* pCmd);
};

inline CFakeAng g_FakeAng;