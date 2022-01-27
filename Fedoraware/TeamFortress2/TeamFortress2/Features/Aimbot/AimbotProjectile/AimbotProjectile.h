#pragma once
#include "../AimbotGlobal/AimbotGlobal.h"

class CAimbotProjectile
{
private:
	struct ProjectileInfo_t
	{
		float m_flVelocity = 0.0f;
		float m_flGravity = 0.0f;
		float m_flMaxTime = 2.0f;
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
	bool CalcProjAngle(const Vec3& vLocalPos, const Vec3& vTargetPos, const ProjectileInfo_t& ProjInfo, Solution_t& out);
	bool SolveProjectile(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Predictor_t& Predictor, const ProjectileInfo_t& ProjInfo, Solution_t& out);

private:
	Vec3 GetAimPos(CBaseEntity* pLocal, CBaseEntity* pEntity);
	ESortMethod GetSortMethod();
	bool GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	bool VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Target_t& Target);
	bool GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Target_t& Out);
	void Aim(CUserCmd* pCmd, CBaseCombatWeapon* pWeapon, Vec3& vAngle);
	bool ShouldFire(CUserCmd* pCmd);
	bool IsAttacking(CUserCmd* pCmd, CBaseCombatWeapon* pWeapon);
	bool m_bIsFlameThrower;

public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
};

inline CAimbotProjectile g_AimbotProjectile;