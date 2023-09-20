#pragma once

#include "../AimbotGlobal/AimbotGlobal.h"
#include "../../Vars.h"
#include "../MovementSimulation/MovementSimulation.h"
#include "../../TickHandler/TickHandler.h"
#include "../../Backtrack/Backtrack.h"

class CAimbotMelee
{
	inline EGroupType GetGroupType(CBaseCombatWeapon* pWeapon);
	inline bool IsPlayerValid(CBaseEntity* pTarget);
	inline bool TargetTeamBuilding(CBaseObject* pBuilding, CBaseCombatWeapon* pWeapon);
	inline bool TargetTeamBuildings(CBaseCombatWeapon* pWeapon);
	inline bool CanBacktrack(CBaseEntity* pEntity);
	inline bool IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon);
	inline bool ShouldAttack();
	inline float GetSwingRange(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	inline float GetSwingVec(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	bool CanMeleeHit(CBaseEntity* pLocal, CBaseEntity* pEntity, CBaseCombatWeapon* pWeapon, const Vec3& vAngles);
	bool FillCache(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	std::vector<Target_t> GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	bool VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& target);
	bool GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	void Aim(CUserCmd* pCmd, Vec3& vAngle);

	float flSwingTime{};
	Vec3 vCache{};
	Target_t tTarget{};
public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
};

ADD_FEATURE(CAimbotMelee, AimbotMelee)