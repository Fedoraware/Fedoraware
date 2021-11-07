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

			// pretty srue these are all wrong but i have no idea how to do the thing to find out what they are
		// you add 1c idiot
		/*
				*(float*)((uintptr_t)pWeapon + 0xA54) = crit_bucket;
		*(unsigned int*)((uintptr_t)pWeapon + 0xB58) = weapon_seed;
		*(unsigned int*)((uintptr_t)pWeapon + 0xB4c) = unknown1;
		*(unsigned int*)((uintptr_t)pWeapon + 0xB50) = unknown2;
		*(bool*)((uintptr_t)pWeapon + 0xB33) = unknown3;
		*(float*)((uintptr_t)pWeapon + 0xB5c) = unknown4;
		*(int*)((uintptr_t)pWeapon + 0xA58) = crit_attempts;
		*(int*)((uintptr_t)pWeapon + 0xA5c) = crit_count;
		*(float*)((uintptr_t)pWeapon + 0xC18) = observed_crit_chance;
		*(bool*)((uintptr_t)pWeapon + 0xB34) = unknown7;
		*//*
		M_OFFSETGET(CritBucket, float, 0xA54)
		M_OFFSETGET(WeaponSeed, int, 0xB58)
		M_OFFSETGET(Unknown1, int, 0xB4C)
		M_OFFSETGET(Unknown2, int, 0xB50)
		M_OFFSETGET(Unknown3, bool, 0xB33)
		M_OFFSETGET(Unknown4, float, 0xB5C)
		M_OFFSETGET(CritAttempts, int, 0xA58)
		M_OFFSETGET(CritCount, int, 0xA5C)
		M_OFFSETGET(ObservedCritChance, int, 0xC18)
		M_OFFSETGET(Unknown7, bool, 0xB34)
		M_OFFSETGET(WeaponMode, bool, 0xB20)
		M_OFFSETGET(WeaponDataa, bool, 0xB2C)*/


public: //Virtuals
	M_VIRTUALGET(WeaponID, int, this, int(__thiscall*)(void*), 377)
		M_VIRTUALGET(Slot, int, this, int(__thiscall*)(void*), 327)
		M_VIRTUALGET(DamageType, int, this, int(__thiscall*)(void*), 378)
		M_VIRTUALGET(FinishReload, void, this, void(__thiscall*)(void*), 275)
		M_VIRTUALGET(BulletSpread, Vec3&, this, Vec3& (__thiscall*)(void*), 286)

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

	__inline bool CalcIsAttackCritical() {
		typedef bool(__thiscall* OriginalFn)(CBaseCombatWeapon*);
		static DWORD dwFunc = g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 18 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B F0 83 C4 14 89 75 EC"));
		return ((OriginalFn)dwFunc)(this);
	}
	__inline bool DoSwingTrace(CGameTrace& Trace) {
		return GetVFunc<int(__thiscall*)(CGameTrace&)>(this, 453)(Trace);
	}

	__inline int LookupAttachment(const char* pAttachmentName)
	{
		const auto pRend = Renderable();
		return GetVFunc<int(__thiscall*)(void*, const char*)>(pRend, 35)(pRend, pAttachmentName);
	}

	__inline bool GetAttachment(int number, Vec3& origin) {
		return GetVFunc<bool(__thiscall*)(void*, int, Vec3&)>(this, 71)(this, number, origin);
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
		return *reinterpret_cast<int*>(this + 0xC48);
	}
};

class weapon_info
{
public:
	float flCritBucket{};
	int iCurrentSeed{};
	float flCritEndTime{};
	float flLastCritCheckTime{};
	float iLastCritCheckFrame{};
	int iNumAttacks{};
	int iNumCrits{};
	float m_flObservedCritChance{};
	bool unknown7{};
	//int weapon_mode{};
	//int weapon_data{};
	/*
	
// Game keeps track of these stats
struct stats_t
{
	float flCritBucket; // A54
	int   iNumAttacks;  // A58
	int   iNumCrits;    // A5C
} stats;

// Memory layout for C_TFWeaponBase
struct state_t
{
	bool  bCurrentAttackIsCrit;
	bool  bLowered;                  // NETVAR 0xB40
	int   iAltFireHint;              // 0xB44
	int   iReloadStartClipAmount;    // 0xB48
	float flCritEndTime;             // 0xB4C
	float flLastCritCheckTime;       // NETVAR // 0xB50
	int   iLastCritCheckFrame;       // 0xB54
	int   iCurrentSeed;              // 0xB58
	float flLastCritCheckTimeNoPred; // 0xB5C
};*/

	void Load(CBaseCombatWeapon* pWeapon)
	{
		flCritBucket = *(float*)((uintptr_t)pWeapon + 0xA54);//pWeapon->GetCritBucket();
		iCurrentSeed = *(int*)((uintptr_t)pWeapon + 0xB58);//pWeapon->GetWeaponSeed();
		flCritEndTime = *(float*)((uintptr_t)pWeapon + 0xB4C);//pWeapon->GetUnknown1();
		flLastCritCheckTime = *(float*)((uintptr_t)pWeapon + 0xB50); //pWeapon->GetUnknown2();
		iLastCritCheckFrame = *(int*)((uintptr_t)pWeapon + 0xB54);//pWeapon->GetUnknown4();
		iNumAttacks = *(int*)((uintptr_t)pWeapon + 0xB58);//pWeapon->GetCritAttempts();
		iNumCrits = *(int*)((uintptr_t)pWeapon + 0xA5C);//pWeapon->GetCritCount();
		m_flObservedCritChance = *(float*)((uintptr_t)pWeapon + 0xC1C); // pWeapon->GetObservedCritChance();
		unknown7 = *(bool*)((uintptr_t)pWeapon + 0xB34);

		//weapon_mode = pWeapon->GetWeaponMode();
		//weapon_data = pWeapon->GetWeaponDataa();
	}
	weapon_info(CBaseCombatWeapon* pWeapon)
	{
		Load(pWeapon);
	}
	void RestoreData(CBaseCombatWeapon* pWeapon)
	{
		*(float*)((uintptr_t)pWeapon + 0xA54) = flCritBucket;
		*(int*)((uintptr_t)pWeapon + 0xB58) = iCurrentSeed;
		*(float*)((uintptr_t)pWeapon + 0xB4C) = flCritEndTime;
		*(float*)((uintptr_t)pWeapon + 0xB50) = flLastCritCheckTime;
		*(float*)((uintptr_t)pWeapon + 0xB5C) = iLastCritCheckFrame;
		*(int*)((uintptr_t)pWeapon + 0xA58) = iNumAttacks;
		*(int*)((uintptr_t)pWeapon + 0xA5C) = iNumCrits;
		*(float*)((uintptr_t)pWeapon + 0xC18) = m_flObservedCritChance;
		*(bool*)((uintptr_t)pWeapon + 0xB34) = unknown7;
	}

	bool operator==(const weapon_info& b) const
	{
		return (
			flCritBucket == b.flCritBucket &&
			iCurrentSeed == b.iCurrentSeed &&
			flCritEndTime == b.flCritEndTime &&
			flLastCritCheckTime == b.flLastCritCheckTime &&
			iLastCritCheckFrame == b.iLastCritCheckFrame &&
			iNumAttacks == b.iNumAttacks &&
			iNumCrits == b.iNumCrits &&
			m_flObservedCritChance == b.m_flObservedCritChance &&
			unknown7 == b.unknown7
			);
		//return  == B.crit_bucket && weapon_seed == B.weapon_seed && unknown1 == B.unknown1 && unknown2 == B.unknown2 && unknown3 == B.unknown3 && critTime == B.critTime && crit_attempts == B.crit_attempts && crit_count == B.crit_count && observed_crit_chance == B.observed_crit_chance && unknown7 == B.unknown7;
	}
	bool operator!=(const weapon_info& B) const
	{
		return !(*this == B);
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