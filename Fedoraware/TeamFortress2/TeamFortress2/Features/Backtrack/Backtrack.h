#pragma once
#include "../../SDK/SDK.h"

#pragma warning ( disable : 4091 )

class CIncomingSequence
{
public:
	int InReliableState;
	int SequenceNr;
	float CurTime;

	CIncomingSequence(int inState, int seqNr, float time)
	{
		InReliableState = inState;
		SequenceNr = seqNr;
		CurTime = time;
	}
};

using BoneMatrixes = struct
{
	float BoneMatrix[128][3][4];
};

struct TickRecord
{
	float SimulationTime = -1;
	Vec3 HeadPosition = { };
	Vec3 AbsOrigin = { };
	BoneMatrixes BoneMatrix{ };
	model_t* Model = nullptr;
	studiohdr_t* HDR = nullptr;
	int  HitboxSet = 0;
	Vec3 Mins = Vec3();
	Vec3 Maxs = Vec3();
	Vec3 WorldSpaceCenter = { };
	Vec3 EyeAngles = { };
};

class CBacktrack
{
public:
	bool IsGoodTick(float simTime);
	void Start(const CUserCmd* pCmd);
	void Run(const CUserCmd* pCmd);

	// Latency
	void UpdateDatagram();
	float GetLatency();
	void AdjustPing(INetChannel* netChannel);

	std::deque<TickRecord>* GetPlayerRecord(int iEntityIndex);
	std::deque<TickRecord>* GetPlayerRecord(CBaseEntity* pEntity);
	int LastInSequence = 0;
	bool AllowLatency = false;
	std::array<std::deque<TickRecord>, 64> Records;
	float LatencyRampup = 0.f;
	std::deque<CIncomingSequence> Sequences;
};

ADD_FEATURE(CBacktrack, Backtrack)