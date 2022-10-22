#pragma once
#include "../AimbotGlobal/AimbotGlobal.h"

class CAimbotProjectile
{
	struct ProjectileInfo_t
	{
		float m_flVelocity = 0.0f;
		float m_flGravity = 0.0f;
		float m_flMaxTime = 0.0f;
	};

	struct Predictor_t
	{
		CBaseEntity* m_pEntity = nullptr;
		Vec3 m_vPosition = {};
		Vec3 m_vVelocity = {};
		Vec3 m_vAcceleration = {};


		Vec3 Extrapolate(float time);
	};

	struct Solution_t
	{
		float m_flPitch = 0.0f;
		float m_flYaw = 0.0f;
		float m_flTime = 0.0f;
	};

	bool GetProjectileInfo(CBaseCombatWeapon* pWeapon, ProjectileInfo_t& out);
	bool CalcProjAngle(const Vec3& vLocalPos, const Vec3& vTargetPos, const ProjectileInfo_t& projInfo,
					   Solution_t& out);
	bool SolveProjectile(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Predictor_t& predictor,
						 const ProjectileInfo_t& projInfo, Solution_t& out);

	Vec3 GetAimPos(CBaseEntity* pLocal, CBaseEntity* pEntity, const Vec3& targetPredPos);
	Vec3 GetAimPosBuilding(CBaseEntity* pLocal, CBaseEntity* pEntity);
	bool WillProjectileHit(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, const Vec3& vPredictedPos, Solution_t& out, const ProjectileInfo_t& projInfo, const Predictor_t& predictor);
	std::vector<Target_t> GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	bool VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Target_t& target);
	bool GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Target_t& outTarget);
	void Aim(CUserCmd* pCmd, CBaseCombatWeapon* pWeapon, Vec3& vAngle);
	bool ShouldFire(CUserCmd* pCmd);
	bool IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon);
	bool GetSplashTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Target_t& outTarget);
	float m_flTravelTime = 0.0f;
	float m_flTravelTimeStart = 0.0f;
	bool IsFlameThrower = false;
	bool IsBoosted = false;

public:

	bool Flippy = false;
	bool running = false;
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
};

ADD_FEATURE(CAimbotProjectile, AimbotProjectile)