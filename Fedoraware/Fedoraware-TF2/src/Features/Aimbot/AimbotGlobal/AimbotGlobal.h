#pragma once
#include "../../Feature.h"

namespace SandvichAimbot
{
	extern bool bIsSandvich;
	extern bool IsSandvich();
	extern void RunSandvichAimbot(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, CBaseEntity* pTarget);
}

enum struct ETargetType
{
	UNKNOWN,
	PLAYER,
	BUILDING,
	STICKY,
	NPC,
	BOMBS
};

enum struct ESortMethod
{
	FOV,
	DISTANCE
};

enum Ignored
{
	INVUL = 1 << 0,
	CLOAKED = 1 << 1,
	DEADRINGER = 1 << 2,
	FRIENDS = 1 << 3,
	TAUNTING = 1 << 4,
	VACCINATOR = 1 << 5,
	UNSIMULATED = 1 << 6,
	DISGUISED = 1 << 7
};

struct Target_t
{
	CBaseEntity* m_pEntity = nullptr;
	ETargetType m_TargetType = ETargetType::UNKNOWN;
	Vec3 m_vPos = {};
	Vec3 m_vAngleTo = {};
	float m_flFOVTo = std::numeric_limits<float>::max();
	float m_flDistTo = std::numeric_limits<float>::max();
	int m_nAimedHitbox = -1;
	bool m_bHasMultiPointed = false;
	Priority n_Priority = {};

	// Backtrack
	bool ShouldBacktrack = false;
	float SimTime = 0.f;
};

struct AimInfo_t
{
	Vec3 vAngleTo{};
	Vec3 vPos{};
	float flFoVTo{};
	float flDist{};
};

class CAimbotGlobal
{
	bool m_Attacking = false;

public:
	AimInfo_t GetInfo(CBaseEntity* pTarget, const Vec3 vFrom, const Vec3 vAng, const bool bDist);
	bool IsKeyDown();
	void SortTargets(std::vector<Target_t>*, const ESortMethod& method);
	bool ShouldIgnore(CBaseEntity* pTarget, bool hasMedigun = false);
	Priority GetPriority(int targetIdx);

	bool IsAttacking() { return m_Attacking; }
	void SetAttacking(bool bState) { m_Attacking = bState; }
};

ADD_FEATURE(CAimbotGlobal, AimbotGlobal)