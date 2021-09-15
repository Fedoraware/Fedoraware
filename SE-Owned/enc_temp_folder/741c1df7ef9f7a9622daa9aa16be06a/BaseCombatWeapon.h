#pragma once

#include "../BaseEntity/BaseEntity.h"



class CBaseCombatWeapon : public CBaseEntity
{
public: //Netvars
		M_DYNVARGET(Clip1, int, this, _("DT_BaseCombatWeapon"), _("LocalWeaponData"), _("m_iClip1"))
		M_DYNVARGET(Clip2, int, this, _("DT_BaseCombatWeapon"), _("LocalWeaponData"), _("m_iClip2"))
		M_DYNVARGET(ItemDefIndex, int, this, _("DT_EconEntity"), _("m_AttributeManager"), _("m_Item"), _("m_iItemDefinitionIndex"))
		M_DYNVARGET(ChargeBeginTime, float, this, _("DT_WeaponPipebombLauncher"), _("PipebombLauncherLocalData"), _("m_flChargeBeginTime"))
		M_DYNVARGET(ChargeDamage, float, this, _("DT_TFSniperRifle"), _("SniperRifleLocalData"), _("m_flChargedDamage"))
		M_DYNVARGET(LastFireTime, float, this, _("DT_TFWeaponBase"), _("LocalActiveTFWeaponData"), _("m_flLastFireTime"))
		M_DYNVARGET(NextSecondaryAttack, float, this, _("DT_BaseCombatWeapon"), _("LocalActiveWeaponData"), _("m_flNextSecondaryAttack"))
		M_DYNVARGET(NextPrimaryAttack, float, this, _("DT_BaseCombatWeapon"), _("LocalActiveWeaponData"), _("m_flNextPrimaryAttack"))
		//M_DYNVARGET(LastCritCheckTime, float, this, _("DT_TFWeaponBase"), _("LocalActiveTFWeaponData"), _("m_flLastCritCheckTime"))
		//M_DYNVARGET(ObservedCritChance, float, this, _("DT_TFWeaponBase"), _("LocalActiveTFWeaponData"), _("m_flObservedCritChance"))

		M_OFFSETGET(UberCharge, float, 0xC6C) //DT_WeaponMedigun -> NonLocalTFWeaponMedigundata -> m_flChargeLevel
		//M_OFFSETGET(HealingTarget, int, 0xC48) //DT_WeaponMedigun -> m_hHealingTarget
		M_OFFSETGET(Healing, int, 0xC51) //DT_WeaponMedigun -> m_bHealing
		M_OFFSETGET(CritBucket, float, 0xA38)
		M_OFFSETGET(WeaponSeed, int, 0xB3C)
		M_OFFSETGET(Unknown1, int, 0xB30)
		M_OFFSETGET(Unknown2, int, 0xB34)
		M_OFFSETGET(Unknown3, bool, 0xB17)
		M_OFFSETGET(Unknown4, float, 0xB40)
		M_OFFSETGET(CritAttempts, int, 0xA3C)
		M_OFFSETGET(CritCount, int, 0xA40)
		M_OFFSETGET(ObservedCritChance, int, 0xBFC)
		M_OFFSETGET(Unknown7, bool, 0xB18)
		M_OFFSETGET(WeaponMode, bool, 0xB04)
		M_OFFSETGET(WeaponDataa, bool, 0xB10)


public: //Virtuals
	M_VIRTUALGET(WeaponID, int, this, int(__thiscall*)(void*), 377)
	M_VIRTUALGET(Slot, int, this, int(__thiscall*)(void*), 327)
	M_VIRTUALGET(DamageType, int, this, int(__thiscall*)(void*), 378)
	M_VIRTUALGET(FinishReload, void, this, void(__thiscall*)(void*), 275)
	M_VIRTUALGET(BulletSpread, Vec3&, this, Vec3&(__thiscall*)(void*), 286)

public: //Everything else, lol
	__inline float GetSmackTime() {
		//credits to KGB
		static auto dwOffset = g_NetVars.get_offset("DT_TFWeaponBase", "m_nInspectStage") + 0x1C;
		return *reinterpret_cast<float*>(this + dwOffset);
	}

	__inline CAttributeList* GetAttributeList() {
		static auto dwOff = g_NetVars.get_offset(_("DT_EconEntity"), _("m_AttributeManager"), _("m_AttributeList"));
		return reinterpret_cast<CAttributeList*>(this + dwOff);
	}

	__inline void SetItemDefIndex(const int nIndex) {
		static auto dwOff = g_NetVars.get_offset(_("DT_EconEntity"), _("m_AttributeManager"), _("m_Item"), _("m_iItemDefinitionIndex"));
		*reinterpret_cast<int*>(this + dwOff) = nIndex;
	}

	__inline CBaseEntity* GetHealingTarget() {
		return g_Interfaces.EntityList->GetClientEntityFromHandle(GetHealingTargetHandle());
	}

	__inline int GetHealingTargetHandle() {
		return *reinterpret_cast<int*>(this + 0xC48);
	}

	__inline WeaponData_t GetWeaponData() {
		static int offset = g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 66 8B ? ? 66 3B 05 ? ? ? ? 73"));
		static auto get_tf_weapon_data_fn = reinterpret_cast<CTFWeaponInfo * (__cdecl*)(int)>(offset);
		return get_tf_weapon_data_fn(GetWeaponID())->m_WeaponData[0];
	}

	__inline float GetSwingRange(CBaseEntity* pLocal) {
		return static_cast<float>(GetVFunc<int(__thiscall*)(CBaseEntity*)>(this, 451)(pLocal));
	}

	__inline float GetWeaponSpread() {
		static auto GetWeaponSpreadFn = reinterpret_cast<float(__thiscall*)(decltype(this))>(g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 08 56 8B F1 57 6A 01 6A 00 8B 96 ? ? ? ? 8B 86 ? ? ? ? C1 E2 06 56 68 ? ? ? ? 51")));
		return GetWeaponSpreadFn(this);
	}

	__inline bool WillCrit() {
		static auto dwCalcIsAttackCritical = g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 18 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B F0 83 C4 14 89 75 EC"));
		return reinterpret_cast<bool(__thiscall*)(decltype(this))>(dwCalcIsAttackCritical);
	}

	__inline bool DoSwingTrace(CGameTrace& Trace) {
		return GetVFunc<int(__thiscall*)(CGameTrace&)>(this, 453)(Trace);
	}

	__inline bool CanFireCriticalShot(const bool bHeadShot) {
		bool bResult = false;
		if (const auto& pOwner = g_Interfaces.EntityList->GetClientEntityFromHandle(GethOwner())) {
			const int nOldFov = pOwner->GetFov(); pOwner->SetFov(70);
			bResult = GetVFunc<bool(__thiscall*)(decltype(this), bool, CBaseEntity*)>(this, 421)(this, bHeadShot, nullptr);
			pOwner->SetFov(nOldFov);
		} return bResult;
	}

	__inline bool CanFireRandomCriticalShot(const float flCritChance) {
		return GetVFunc<bool(__thiscall*)(decltype(this), float)>(this, 422)(this, flCritChance);
	}

	__inline bool CanWeaponHeadShot() {
		return ((GetDamageType() & DMG_USE_HITLOCATIONS) && CanFireCriticalShot(true)); //credits to bertti
	}

	__inline bool CanShoot(CBaseEntity* pLocal) {
		if (!pLocal->IsAlive() || pLocal->IsTaunting() || pLocal->IsBonked() || pLocal->IsAGhost() || pLocal->IsInBumperKart())
			return false;

		if (pLocal->GetClassNum() == CLASS_SPY)
		{
			{ //DR
				static float flTimer = 0.0f;

				if (pLocal->GetFeignDeathReady()) {
					flTimer = 0.0f;
					return false;
				}
				else {
					if (!flTimer)
						flTimer = g_Interfaces.GlobalVars->curtime;

					if ((g_Interfaces.GlobalVars->curtime - flTimer) < 0.4f)
						return false;
				}
			}

			{ //Invis
				static float flTimer = 0.0f;

				if (pLocal->IsCloaked()) {
					flTimer = 0.0f;
					return false;
				}
				else {
					if (!flTimer)
						flTimer = g_Interfaces.GlobalVars->curtime;

					if ((g_Interfaces.GlobalVars->curtime - flTimer) < 2.0f)
						return false;
				}
			}
		}

		float flCurTime = static_cast<float>(pLocal->GetTickBase()) * g_Interfaces.GlobalVars->interval_per_tick;

		return GetNextPrimaryAttack() <= flCurTime && pLocal->GetNextAttack() <= flCurTime;
	}

	__inline bool CanSecondaryAttack(CBaseEntity* pLocal) {
		if (!pLocal->IsAlive() || pLocal->IsTaunting() || pLocal->IsBonked() || pLocal->IsAGhost() || pLocal->IsInBumperKart())
			return false;

		float flCurTime = static_cast<float>(pLocal->GetTickBase()) * g_Interfaces.GlobalVars->interval_per_tick;

		return GetNextSecondaryAttack() <= flCurTime && pLocal->GetNextAttack() <= flCurTime;
	}

	__inline bool IsInReload() {
		static DWORD dwNextPrimaryAttack = g_NetVars.get_offset(_("DT_BaseCombatWeapon"), _("LocalActiveWeaponData"), _("m_flNextPrimaryAttack"));
		bool m_bInReload = *reinterpret_cast<bool*>(this + (dwNextPrimaryAttack + 0xC));
		int m_iReloadMode = *reinterpret_cast<int*>(this + 0xB28);
		return (m_bInReload || m_iReloadMode != 0);
	}

	__inline void GetSpreadAngles(Vec3& vOut) {
		static auto GetSpreadAnglesFn = reinterpret_cast<void(__thiscall*)(decltype(this), Vec3&)>(g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 18 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B F0 83 C4 14 85 F6 74 10 8B 06 8B CE 8B 80 ? ? ? ? FF D0 84 C0 75 02")));
		GetSpreadAnglesFn(this, vOut);
	}

	__inline void GetProjectileFireSetup(CBaseEntity* pPlayer, Vec3 vOffset, Vec3* vSrc, Vec3* vForward, bool bHitTeam, float flEndDist) {
		typedef void(__thiscall* EstimateAbsVelocityFn)(decltype(this), CBaseEntity*, Vec3, Vec3*, Vec3*, bool, float);
		static DWORD dwFn = g_Pattern.Find(_(L"client.dll"), _(L"E8 ? ? ? ? F3 0F 10 46 ? F3 0F 5C 05 ? ? ? ? F3 0F 11 46 ? 5E 5D C2 20 00")) + 0x1;
		static DWORD dwOffset = ((*(PDWORD)(dwFn)) + dwFn + 0x4);
		EstimateAbsVelocityFn fn = (EstimateAbsVelocityFn)dwOffset;	
		fn(this, pPlayer, vOffset, vSrc, vForward, bHitTeam, flEndDist);
	}

	__inline bool CalcIsAttackCriticalHelper(CBaseEntity* pWeapon)
	{
		typedef bool (*fn_t)(CBaseEntity*);
		return GetVFunc<fn_t>(pWeapon, 461, 0)(pWeapon);
	}
	
	__inline bool CalcIsAttackCriticalHelperNoCrits(CBaseEntity* pWeapon)
	{
		typedef bool (*fn_t)(CBaseEntity*);
		return GetVFunc<fn_t>(pWeapon, 462, 0)(pWeapon);
	}

	__inline bool CanFireCriticalShot(CBaseEntity* pWeapon)
	{
		typedef bool (*fn_t)(CBaseEntity*);
		return GetVFunc<fn_t>(pWeapon, 490, 0)(pWeapon);
	}

	/*__inline bool CalcIsAttackCriticalHelper() {
		return false;
	}*/

	__inline Vec3 GetSpreadAngles() {
		Vec3 vOut; GetSpreadAngles(vOut); return vOut;
	}

	__inline int GetMinigunState() {
		return *reinterpret_cast<int *>(this + 0xC48);
	}
};

class weapon_info
{
public:
	float crit_bucket{};
	unsigned int weapon_seed{};
	unsigned unknown1{};
	unsigned unknown2{};
	bool unknown3{};
	float unknown4{};
	int crit_attempts{};
	int crit_count{};
	float observed_crit_chance{};
	bool unknown7{};
	int weapon_mode{};
	int weapon_data{};
	void Load(CBaseCombatWeapon* pWeapon)
	{
		crit_bucket = pWeapon->GetCritBucket();
		weapon_seed = pWeapon->GetWeaponSeed();
		unknown1 = pWeapon->GetUnknown1();
		unknown2 = pWeapon->GetUnknown2();
		unknown3 = pWeapon->GetUnknown3();
		unknown4 = pWeapon->GetUnknown4();
		crit_attempts = pWeapon->GetCritAttempts();
		crit_count = pWeapon->GetCritCount();
		observed_crit_chance = pWeapon->GetObservedCritChance();
		unknown7 = pWeapon->GetUnknown7();

		weapon_mode = pWeapon->GetWeaponMode();
		weapon_data = pWeapon->GetWeaponDataa();
	}
	weapon_info(CBaseCombatWeapon* pWeapon)
	{
		Load(pWeapon);
	}
	void restore_data(CBaseCombatWeapon* pWeapon)
	{
		/*DYNVAR_SET(float, pWeapon, crit_bucket, 0xA38);
		DYNVAR_SET(int, pWeapon, weapon_seed, 0xA38);
		DYNVAR_SET(int, pWeapon, unknown1, 0xA38);
		DYNVAR_SET(int, pWeapon, unknown2, 0xA38);
		DYNVAR_SET(bool, pWeapon, unknown3, 0xA38);
		DYNVAR_SET(float, pWeapon, unknown4, 0xA38);
		DYNVAR_SET(int, pWeapon, crit_attempts, 0xA38);
		DYNVAR_SET(int, pWeapon, crit_count, 0xA38);
		DYNVAR_SET(int, pWeapon, observed_crit_chance, 0xA38);
		DYNVAR_SET(bool, pWeapon, unknown7, 0xA38);
		DYNVAR_SET(bool, pWeapon, weapon_mode, 0xA38);
		DYNVAR_SET(bool, pWeapon, weapon_data, 0xA38);*/

		*(float*)((uintptr_t)pWeapon + 0xa38) = crit_bucket;
		*(unsigned int*)((uintptr_t)pWeapon + 0xb3c) = weapon_seed;
		*(unsigned int*)((uintptr_t)pWeapon + 0xb30) = unknown1;
		*(unsigned int*)((uintptr_t)pWeapon + 0xb34) = unknown2;
		*(bool*)((uintptr_t)pWeapon + 0xb17) = unknown3;
		*(float*)((uintptr_t)pWeapon + 0xb40) = unknown4;
		*(int*)((uintptr_t)pWeapon + 0xa3c) = crit_attempts;
		*(int*)((uintptr_t)pWeapon + 0xa40) = crit_count;
		*(float*)((uintptr_t)pWeapon + 0xbfc) = observed_crit_chance;
		*(bool*)((uintptr_t)pWeapon + 0xb18) = unknown7;
	}
};

/*		M_OFFSETGET(CritBucket, float, 0xA38)
M_OFFSETGET(WeaponSeed, int, 0xB3C)
M_OFFSETGET(Unknown1, int, 0xB30)
M_OFFSETGET(Unknown2, int, 0xB34)
M_OFFSETGET(Unknown3, bool, 0xB17)
M_OFFSETGET(Unknown4, float, 0xB40)
M_OFFSETGET(CritAttempts, int, 0xA3C)
M_OFFSETGET(CritCount, int, 0xA40)
M_OFFSETGET(ObservedCritChance, int, 0xBFC)
M_OFFSETGET(Unknown7, bool, 0xB18)
M_OFFSETGET(WeaponMode, bool, 0xB04)
M_OFFSETGET(WeaponDataa, bool, 0xB10)*/