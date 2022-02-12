#pragma once
#include "../../SDK/SDK.h"

typedef struct BoneMatrixes {
	float BoneMatrix[128][3][4];
};

struct TickRecord {
	TickRecord( const float flSimulationTime, const Vec3& headPos, const Vec3& absOrigin, BoneMatrixes boneMatrix) {
		SimulationTime = flSimulationTime;
		HeadPosition = headPos;
		AbsOrigin = absOrigin;
		BoneMatrix = boneMatrix;
	}

	float SimulationTime = -1;
	Vec3 HeadPosition;
	Vec3 AbsOrigin;
	BoneMatrixes BoneMatrix;
};

class CBacktrack {
public:
	std::vector<TickRecord> Record[64];
	bool IsGoodTick(int tick) const;
	void Start(CUserCmd* pCmd);
	void Calculate(CUserCmd* pCmd);
	void Run(CUserCmd* pCmd);
};

inline CBacktrack g_Backtrack;