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
	TickRecord(const float flSimulationTime, const Vec3& headPos, const Vec3& absOrigin,
			   BoneMatrixes boneMatrix, model_t* model, studiohdr_t* hdr,
			   int hitboxSet, const Vec3& mins, const Vec3& maxs, const Vec3& worldSpaceCenter,
			   const Vec3& eyeangles)
	{
		SimulationTime = flSimulationTime;
		HeadPosition = headPos;
		AbsOrigin = absOrigin;
		BoneMatrix = boneMatrix;
		Model = model;
		HDR = hdr;
		HitboxSet = hitboxSet;
		WorldSpaceCenter = worldSpaceCenter;
		EyeAngles = eyeangles;
	}

	float SimulationTime = -1;
	Vec3 HeadPosition = { };
	Vec3 AbsOrigin = { };
	BoneMatrixes BoneMatrix{ };
	model_t* Model = nullptr;
	studiohdr_t* HDR = nullptr;
	int  HitboxSet = 0;
	Vec3 mins = Vec3();
	Vec3 maxs = Vec3();
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

	std::vector<TickRecord>* GetPlayerRecord(int iEntityIndex);
	std::vector<TickRecord>* GetPlayerRecord(CBaseEntity* pEntity);
	int LastInSequence = 0;
	bool AllowLatency = false;
	std::array<std::vector<TickRecord>, 64> Records;
	float LatencyRampup = 0.f;
	std::deque<CIncomingSequence> Sequences;
};

ADD_FEATURE(CBacktrack, Backtrack)