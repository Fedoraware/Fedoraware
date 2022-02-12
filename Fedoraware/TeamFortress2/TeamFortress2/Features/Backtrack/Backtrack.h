#pragma once
#include "../../SDK/SDK.h"

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
	std::vector<TickRecord> Record[64];
	bool IsGoodTick(int tick) const;
	void Start(CUserCmd* pCmd);
	void Calculate(CUserCmd* pCmd);
	void Run(CUserCmd* pCmd);
	void UpdateDatagram();
	void UpdateDatagrams(CUserCmd* pCmd);
	void AdjustPing(INetChannel* ch);
	float GetLatency();
};

class CIncomingSequence
{
public:
	int inreliablestate;
	int sequencenr;
	float curtime;

	CIncomingSequence(int instate, int seqnr, float time)
	{
		inreliablestate = instate;
		sequencenr = seqnr;
		curtime = time;
	}
};

inline CBacktrack g_Backtrack;