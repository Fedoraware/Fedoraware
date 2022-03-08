#pragma once
#include "../Hooks.h"

namespace FireBullets
{
	inline SEOHook::Func Func;

	struct FireBulletsInfo_t
	{
		FireBulletsInfo_t()
		{
			m_iShots = 1;
			m_vecSpread.Clear();
			m_flDistance = 8192;
			m_iTracerFreq = 4;
			m_flDamage = 0;
			m_iPlayerDamage = 0;
			m_pAttacker = nullptr;
			m_nFlags = 0;
			m_pAdditionalIgnoreEnt = nullptr;
			m_flDamageForceScale = 1.0f;
			m_bPrimaryAttack = true;
			m_bUseServerRandomSeed = false;
		}

		FireBulletsInfo_t(int nShots, const Vec3& vecSrc, const Vec3& vecDir, const Vec3& vecSpread, float flDistance,
		                  int nAmmoType, bool bPrimaryAttack = true)
		{
			m_iShots = nShots;
			m_vecSrc = vecSrc;
			m_vecDirShooting = vecDir;
			m_vecSpread = vecSpread;
			m_flDistance = flDistance;
			m_iAmmoType = nAmmoType;
			m_iTracerFreq = 4;
			m_flDamage = 0;
			m_iPlayerDamage = 0;
			m_pAttacker = nullptr;
			m_nFlags = 0;
			m_pAdditionalIgnoreEnt = nullptr;
			m_flDamageForceScale = 1.0f;
			m_bPrimaryAttack = bPrimaryAttack;
			m_bUseServerRandomSeed = false;
		}

		int m_iShots = 0;
		Vec3 m_vecSrc = {};
		Vec3 m_vecDirShooting = {};
		Vec3 m_vecSpread = {};
		float m_flDistance = 0.f;
		int m_iAmmoType = 0;
		int m_iTracerFreq = 0;
		float m_flDamage = 0.f;
		int m_iPlayerDamage = 0; // Damage to be used instead of m_flDamage if we hit a player
		int m_nFlags = 0; // See FireBulletsFlags_t
		float m_flDamageForceScale = 0.f;
		CBaseEntity* m_pAttacker = nullptr;
		CBaseEntity* m_pAdditionalIgnoreEnt = nullptr;
		bool m_bPrimaryAttack = false;
		bool m_bUseServerRandomSeed = false;
	};

	using fn = void(__fastcall*)(void*, void*, CBaseCombatWeapon*, const FireBulletsInfo_t&, bool, int, int);
	using LookupAttachment_t = int(*)(CBaseEntity*, const char*);
	void __fastcall Hook(void* ecx, void* edx, CBaseCombatWeapon* pWeapon, const FireBulletsInfo_t& info,
	                     bool bDoEffects, int nDamageType, int nCustomDamageType);

	void Init();
}