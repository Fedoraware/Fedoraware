#pragma once
#include "../../SDK/SDK.h"

#pragma warning ( disable : 4091 )

class CIncomingSequence
{
public:
	int IsReliableState;
	int SequenceNr;
	float CurTime;

	CIncomingSequence(int instate, int seqnr, float time)
	{
		IsReliableState = instate;
		SequenceNr = seqnr;
		CurTime = time;
	}
};

typedef struct BoneMatrixes {
	float BoneMatrix[128][3][4];
};

struct TickRecord {
	TickRecord( const float flSimulationTime, const Vec3& headPos, const Vec3& absOrigin, 
		BoneMatrixes boneMatrix, model_t* model, studiohdr_t* hdr,
		int hitboxSet) {
		SimulationTime = flSimulationTime;
		HeadPosition = headPos;
		AbsOrigin = absOrigin;
		BoneMatrix = boneMatrix;
		Model = model;
		HDR = hdr;
		HitboxSet = hitboxSet;
	}

	float SimulationTime = -1;
	Vec3 HeadPosition;
	Vec3 AbsOrigin;
	BoneMatrixes BoneMatrix;
	model_t* Model;
	studiohdr_t* HDR;
	int  HitboxSet;
	bool AimedAt = false;
};

class CBacktrack {
public:
	bool IsGoodTick(int tick);
	void Start(const CUserCmd* pCmd);
	void Calculate(CUserCmd* pCmd);
	void Run(CUserCmd* pCmd);

	std::vector<TickRecord> Record[64];
};

inline CBacktrack g_Backtrack;