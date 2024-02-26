#pragma once
#include "../Feature.h"

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
	float flSimTime = 0.f;
	float flCreateTime = 0.f;
	int iTickCount = 0;
	bool bOnShot = false;
	BoneMatrixes BoneMatrix{};
	Vec3 vOrigin = {};
	Vec3 vAngles = {};
};

enum class BacktrackMode
{
	ALL, //	iterates through every tick (slow probably)
	PREFERONSHOT, // prefers on shot records.
	FORCEONSHOT,	//	forces on shot records.
};

class CBacktrack
{
	const Color_t BT_LOG_COLOUR{ 150, 0, 212, 255};

//	logic
	bool IsSimulationReliable(CBaseEntity* pEntity);
	inline bool IsEarly(CBaseEntity* pEntity);
	bool WithinRewindEx(const TickRecord& record, const float flCompTime);
	//bool IsBackLagComped(CBaseEntity* pEntity);

	//	records
	inline std::optional<TickRecord> GetFirstRecord(CBaseEntity* pEntity, CBaseEntity* pLocal, const int nHitbox);

	//	utils
	void CleanRecords();
	void MakeRecords();
	void UpdateAnimations();
	std::optional<TickRecord> GetHitRecord(CUserCmd* pCmd, CBaseEntity* pEntity, Vec3 vAngles, Vec3 vPos);
	//	utils - fake latency
	void UpdateDatagram();

	//	data
	std::unordered_map<CBaseEntity*, std::deque<TickRecord>> mRecords;
	std::unordered_map<int, bool> mDidShoot;
	int iLastCreationTick = 0;
	int iTickCount = 0;

	//	data - fake latency
	std::deque<CIncomingSequence> dSequences;
	float flLatencyRampup = 0.f;
	int iLastInSequence = 0;

public:
	float GetLatency();
	inline bool IsTracked(const TickRecord& record, const float flDelay = I::GlobalVars->interval_per_tick);
	bool WithinRewind(const TickRecord& record, const float flDelay = I::GlobalVars->interval_per_tick);
	bool CanHitOriginal(CBaseEntity* pEntity);
	void PlayerHurt(CGameEvent* pEvent); //	called on player_hurt event
	void ResolverUpdate(CBaseEntity* pEntity);	//	omfg
	void Restart(); //	called whenever lol
	void FrameStageNotify(); //	called in FrameStageNotify
	void ReportShot(int iIndex);
	std::deque<TickRecord>* GetRecords(CBaseEntity* pEntity);
	std::optional<TickRecord> Aimbot(CBaseEntity* pEntity, BacktrackMode iMode, int nHitbox);
	std::optional<TickRecord> GetLastRecord(CBaseEntity* pEntity);
	std::optional<TickRecord> GetFirstRecord(CBaseEntity* pEntity);
	std::optional<TickRecord> Run(CUserCmd* pCmd); //	returns a valid record
	void AdjustPing(INetChannel* netChannel); //	blurgh
	bool bFakeLatency = false;
	bool bUpdatingAnims = false;
};

ADD_FEATURE(CBacktrack, Backtrack)
