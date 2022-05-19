#pragma once

#include "../BaseEntity/BaseEntity.h"

#ifndef TICKS_TO_TIME
#define TICKS_TO_TIME( t )	( g_Interfaces.GlobalVars->interval_per_tick * ( t ) )
#endif

class CBaseCombatWeapon : public CBaseEntity
{
public: //Netvars
	M_DYNVARGET(Clip1, int, this, _("DT_BaseCombatWeapon"), _("LocalWeaponData"), _("m_iClip1"))
		M_DYNVARGET(Clip2, int, this, _("DT_BaseCombatWeapon"), _("LocalWeaponData"), _("m_iClip2"))
		M_DYNVARGET(nViewModelIndex, int, this, _("DT_BaseCombatWeapon"), _("LocalWeaponData"), _("m_nViewModelIndex"))
		M_DYNVARGET(iViewModelIndex, int, this, _("DT_BaseCombatWeapon"), _("m_iViewModelIndex"))
		M_DYNVARGET(ItemDefIndex, int, this, _("DT_EconEntity"), _("m_AttributeManager"), _("m_Item"), _("m_iItemDefinitionIndex"))
		M_DYNVARGET(ChargeBeginTime, float, this, _("DT_WeaponPipebombLauncher"), _("PipebombLauncherLocalData"), _("m_flChargeBeginTime"))
		M_DYNVARGET(ChargeDamage, float, this, _("DT_TFSniperRifle"), _("SniperRifleLocalData"), _("m_flChargedDamage"))
		M_DYNVARGET(LastFireTime, float, this, _("DT_TFWeaponBase"), _("LocalActiveTFWeaponData"), _("m_flLastFireTime"))
		M_DYNVARGET(NextSecondaryAttack, float, this, _("DT_BaseCombatWeapon"), _("LocalActiveWeaponData"), _("m_flNextSecondaryAttack"))
		M_DYNVARGET(NextPrimaryAttack, float, this, _("DT_BaseCombatWeapon"), _("LocalActiveWeaponData"), _("m_flNextPrimaryAttack"))
		M_DYNVARGET(ChargeResistType, int, this, _("DT_WeaponMedigun"), _("m_nChargeResistType"))
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
	M_VIRTUALGET(WeaponID, int, this, int(__thiscall*)(void*), 379)
		M_VIRTUALGET(Slot, int, this, int(__thiscall*)(void*), 329)
		M_VIRTUALGET(DamageType, int, this, int(__thiscall*)(void*), 340)
		M_VIRTUALGET(FinishReload, void, this, void(__thiscall*)(void*), 275)
		M_VIRTUALGET(BulletSpread, Vec3&, this, Vec3& (__thiscall*)(void*), 286)

public: //Everything else, lol
	__inline float GetSmackTime() {
		//credits to KGB
		static auto dwOffset = g_NetVars.get_offset("DT_TFWeaponBase", "m_nInspectStage") + 0x1C;
		return *reinterpret_cast<float*>(this + dwOffset);
	}

	__inline float ObservedCritChance()
	{
		DYNVAR_RETURN(float, this, "DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flObservedCritChance");
	}
	
	//str8 outta cathook
	__inline bool AmbassadorCanHeadshot()
	{
		if (GetItemDefIndex() == Spy_m_TheAmbassador || GetItemDefIndex() == Spy_m_FestiveAmbassador)
		{
			if ((g_Interfaces.GlobalVars->curtime - GetLastFireTime()) <= 1.0)
			{
				return false;
			}
		}
		return true;
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
		return static_cast<float>(GetVFunc<int(__thiscall*)(CBaseEntity*)>(this, 453)(pLocal));
	}

	__inline float GetWeaponSpread() {
		static auto GetWeaponSpreadFn = reinterpret_cast<float(__thiscall*)(decltype(this))>(g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 08 56 8B F1 57 6A 01 6A 00 8B 96 ? ? ? ? 8B 86 ? ? ? ? C1 E2 06 56 68 ? ? ? ? 51")));
		return GetWeaponSpreadFn(this);
	}

	/*__inline bool WillCrit() {
		static auto dwCalcIsAttackCritical = g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 18 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B F0 83 C4 14 89 75 EC"));
		return reinterpret_cast<bool(__thiscall*)(decltype(this))>(dwCalcIsAttackCritical);
	}*/

	/*__inline bool CalcIsAttackCritical() {
		typedef bool(__thiscall* OriginalFn)(CBaseCombatWeapon*);
		static DWORD dwFunc = g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 18 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B F0 83 C4 14 89 75 EC"));
		return ((OriginalFn)dwFunc)(this);
	}*/

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
			bResult = GetVFunc<bool(__thiscall*)(decltype(this), bool, CBaseEntity*)>(this, 423)(this, bHeadShot, nullptr);
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
		static auto FN = reinterpret_cast< void( __thiscall* )( CBaseEntity*, CBaseEntity*, Vec3, Vec3*, Vec3*, bool, float ) >( g_Pattern.Find( _( L"client.dll" ), _( L"53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B ? 89 6C ? ? 8B EC 81 EC ? ? ? ? 56 8B F1 57 8B 06 8B 80 ? ? ? ? FF D0 84 C0" ) ) );
		FN(this, pPlayer, vOffset, vSrc, vForward, bHitTeam, flEndDist);
	}

	__inline bool IsRapidFire()
	{
		const bool ret = GetWeaponData().m_bUseRapidFireCrits;
		return ret || this->GetClientClass()->m_ClassID == static_cast<int>(ETFClassID::CTFMinigun);
	}

	__inline bool WillCrit()
	{
		return this->GetSlot() == SLOT_MELEE ? this->CalcIsAttackCriticalHelperMelee() : this->CalcIsAttackCriticalHelper();
	}

	__inline bool CalcIsAttackCritical()
	{
		static auto func = g_Pattern.Find(_(L"client.dll"), _(L"53 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B D8 83 C4 14 85 DB 0F 84 ? ? ? ?"));
		typedef bool(__thiscall* fn)(void*);
		return reinterpret_cast<fn>(func)(this);
	}

	__inline bool CalcIsAttackCriticalHelper()
	{
		using FN = bool(__thiscall*)(CBaseCombatWeapon*);
		static FN pCalcIsAttackCriticalHelper = reinterpret_cast<FN>(g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 18 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B F0 83 C4 14 89 75 EC")));
		return pCalcIsAttackCriticalHelper(this);
	}
	__inline bool CalcIsAttackCriticalHelperMelee()
	{
		using FN = bool(__thiscall*)(CBaseCombatWeapon*);
		static FN pCalcIsAttackCriticalHelper = reinterpret_cast<FN>(g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC A1 ? ? ? ? 83 EC 08 83 78 30 00 57")));
		return pCalcIsAttackCriticalHelper(this);
	}

	__inline bool CalcIsAttackCriticalHelperNoCrits(CBaseEntity* pWeapon)
	{
		typedef bool (*fn_t)(CBaseEntity*);
		return GetVFunc<fn_t>(pWeapon, 463, 0)(pWeapon);
	}

	__inline bool CanFireCriticalShot(CBaseEntity* pWeapon)
	{
		typedef bool (*fn_t)(CBaseEntity*);
		return GetVFunc<fn_t>(pWeapon, 491, 0)(pWeapon);
	}

	__inline Vec3 GetSpreadAngles() {
		Vec3 vOut; GetSpreadAngles(vOut); return vOut;
	}

	__inline int GetMinigunState() {
		return *reinterpret_cast<int*>(this + 0xC48);
	}

	__inline bool IsReadyToFire()
	{
		static float lastFire = 0, nextAttack = 0;

		if (lastFire != GetLastFireTime())
		{
			lastFire = GetLastFireTime();
			nextAttack = GetNextPrimaryAttack();
		}

		if (GetClip1() == 0)
			return false;
		return (nextAttack <= (TICKS_TO_TIME(g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer())->GetTickBase())));
	}


	CHudTexture* GetWeaponIcon();
};