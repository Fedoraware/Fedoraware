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
	bool IsGoodTick(float simTime);
	void UpdateRecords(const CUserCmd* pCmd);

	void UpdateDatagram();
	float GetLatency();

	float LatencyRampup = 0.f;
	int LastInSequence = 0;
	std::deque<CIncomingSequence> Sequences;

public:
	void Run(const CUserCmd* pCmd);
	void AdjustPing(INetChannel* netChannel);
	void ResetLatency();

	std::deque<TickRecord>* GetPlayerRecords(int iEntityIndex);
	std::deque<TickRecord>* GetPlayerRecords(CBaseEntity* pEntity);

	bool AllowLatency = false;
	std::array<std::deque<TickRecord>, 64> Records;
};

ADD_FEATURE(CBacktrack, Backtrack)