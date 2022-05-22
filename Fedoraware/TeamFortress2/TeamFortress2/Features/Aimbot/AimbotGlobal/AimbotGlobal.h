#pragma once
#include "../../../SDK/SDK.h"

namespace SandvichAimbot
{
	extern bool bIsSandvich;
	extern void IsSandvich();
	extern void RunSandvichAimbot(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, CBaseEntity* pTarget);
}

enum struct ETargetType {
	UNKNOWN,
	PLAYER,
	BUILDING,
	STICKY
};

enum struct ESortMethod {
	UNKNOWN,
	FOV,
	DISTANCE
};

struct Target_t {
	CBaseEntity* m_pEntity = nullptr;
	ETargetType m_TargetType = ETargetType::UNKNOWN;
	Vec3 m_vPos = {};
	Vec3 m_vAngleTo = {};
	float m_flFOVTo = std::numeric_limits<float>::max();
	float m_flDistTo = std::numeric_limits<float>::max();
	int m_nAimedHitbox = -1;
	bool m_bHasMultiPointed = false;
	Priority n_Priority = {};
};

class CAimbotGlobal {
public:
	bool IsKeyDown();
	void SortTargets(const ESortMethod& Method);
	const Target_t& GetBestTarget(const ESortMethod& Method);
	bool ShouldIgnore(CBaseEntity* pTarget, bool hasMedigun = false);

	std::vector<Target_t> m_vecTargets = {};
};

inline CAimbotGlobal g_AimbotGlobal;
