#pragma once
#include "../../NetVars/NetVars.h"

#include "AnimState/TFPlayerAnimState.h" //includes class C_BaseCombatWeapon

#define M_VIRTUALGET(name, type, base, fn, index) __inline type Get##name() \
{ \
	void* pBase = base; \
	return GetVFunc<fn>(pBase, index)(pBase); \
}

#define HAS_CONDITION(ent, cond) (ent->GetCond() & cond)

enum ShouldTransmitState_t {
	SHOULDTRANSMIT_START = 0,
	SHOULDTRANSMIT_END
};


// has all these in them. Left it as an enum in case we want to go back though
enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,	// indicates it was created +and+ entered the pvs
//	DATA_UPDATE_ENTERED_PVS,
DATA_UPDATE_DATATABLE_CHANGED,
//	DATA_UPDATE_LEFT_PVS,
//	DATA_UPDATE_DESTROYED,		// FIXME: Could enable this, but it's a little worrying
								// since it changes a bunch of existing code
};

class IClientNetworkable;
class CBaseEntity;
class IClientRenderable;
class ICollideable;
class IClientEntity;
class IClientThinkable;

class IClientUnknown : public IHandleEntity
{
public:
	virtual ICollideable* GetCollideable() = 0;
	virtual IClientNetworkable* GetClientNetworkable() = 0;
	virtual IClientRenderable* GetClientRenderable() = 0;
	virtual IClientEntity* GetIClientEntity() = 0;
	virtual CBaseEntity* GetBaseEntity() = 0;
	virtual IClientThinkable* GetClientThinkable() = 0;
};

class IClientNetworkable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual void Release() = 0;
	virtual CClientClass* GetClientClass() = 0;
	virtual void NotifyShouldTransmit(ShouldTransmitState_t state) = 0;
	virtual void OnPreDataChanged(DataUpdateType_t updateType) = 0;
	virtual void OnDataChanged(DataUpdateType_t updateType) = 0;
	virtual void PreDataUpdate(DataUpdateType_t updateType) = 0;
	virtual void PostDataUpdate(DataUpdateType_t updateType) = 0;
	virtual bool IsDormant(void) = 0;
	virtual int	entindex(void) const = 0;
	virtual void ReceiveMessage(int classID, bf_read& msg) = 0;
	virtual void* GetDataTableBasePtr() = 0;
	virtual void SetDestroyedOnRecreateEntities(void) = 0;
	virtual void OnDataUnchangedInPVS() = 0;
};

class CBaseEntity
{
public: //Netvars & conditions
	M_DYNVARGET(NextNoiseMakerTime, float, this, "DT_TFPlayer", "m_Shared", "m_flNextNoiseMakerTime")
		M_DYNVARGET(FeignDeathReady, bool, this, "DT_TFPlayer", "m_Shared", "m_bFeignDeathReady")
		M_DYNVARGET(StepSize, float, this, "DT_BasePlayer", "localdata", "m_flStepSize")
		M_DYNVARGET(ConveyorSpeed, float, this, "DT_FuncConveyor", "m_flConveyorSpeed")
		M_DYNVARGET(MaxSpeed, float, this, "DT_BasePlayer", "m_flMaxspeed")
		M_DYNVARGET(State, int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerState")
		M_DYNVARGET(ViewOffset, Vec3, this, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]")
		M_DYNVARGET(CurrentCommand, CUserCmd*, (this - 0x4), "DT_BasePlayer", "localdata", "m_hConstraintEntity")
		M_DYNVARGET(VecOrigin, Vec3, this, "DT_BaseEntity", "m_vecOrigin")
		M_DYNVARGET(Ammo, int, (this + 0x4), "DT_BasePlayer", "localdata", "m_iAmmo")
		M_DYNVARGET(HitboxSet, int, this, "DT_BaseAnimating", "m_nHitboxSet")
		M_DYNVARGET(TickBase, int, this, "DT_BasePlayer", "localdata", "m_nTickBase")
		M_DYNVARGET(SimulationTime, float, this, "DT_BaseEntity", "m_flSimulationTime")
		M_DYNVARGET(OldSimulationTime, float, (this + 0x4), "DT_BaseEntity", "m_flSimulationTime");
		M_DYNVARGET(hOwner, int, this, "DT_BaseEntity", "m_hOwnerEntity")
		M_DYNVARGET(Health, int, this, "DT_BasePlayer", "m_iHealth")
		M_DYNVARGET(TeamNum, int, this, "DT_BaseEntity", "m_iTeamNum")
		M_DYNVARGET(Flags, int, this, "DT_BasePlayer", "m_fFlags")
		M_DYNVARGET(LifeState, BYTE, this, "DT_BasePlayer", "m_lifeState")
		M_DYNVARGET(ClassNum, int, this, "DT_TFPlayer", "m_PlayerClass", "m_iClass")
		M_DYNVARGET(Cond, int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCond")
		M_DYNVARGET(ViewingCYOAPDA, bool, this, "DT_TFPlayer", "m_bViewingCYOAPDA")
		M_DYNVARGET(UsingActionSlot, bool, this, "DT_TFPlayer", "m_bUsingActionSlot")
		M_DYNVARGET(LoadoutUnavailable, bool, this, "DT_TFPlayer", "m_Shared", "m_bLoadoutUnavailable")
		M_DYNVARGET(CondEx, int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCondEx")
		M_DYNVARGET(CondEx2, int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCondEx2")
		M_DYNVARGET(CondEx3, int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCondEx3")
		M_DYNVARGET(CollideableMins, Vec3, this, "DT_BaseEntity", "m_Collision", "m_vecMins")
		M_DYNVARGET(CollideableMaxs, Vec3, this, "DT_BaseEntity", "m_Collision", "m_vecMaxs")
		M_DYNVARGET(EyeAngles, Vec3, this, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]")
		M_DYNVARGET(WaterLevel, BYTE, this, "DT_TFPlayer", "m_nWaterLevel")
		M_DYNVARGET(NextAttack, float, this, "DT_BaseCombatCharacter", "bcc_localdata", "m_flNextAttack")
		M_DYNVARGET(Fov, int, this, "DT_BasePlayer", "m_iFOV")
		M_DYNVARGET(ObserverTarget, int, this, "DT_BasePlayer", "m_hObserverTarget")
		M_DYNVARGET(ObserverMode, int, this, "DT_BasePlayer", "m_iObserverMode")
		M_DYNVARGET(GlowEnabled, bool, this, "DT_TFPlayer", "m_bGlowEnabled")
		M_DYNVARGET(Thrower, void*, this, "DT_BaseGrenade", "m_hThrower")
		M_DYNVARGET(DamageRadius, float, this, "DT_BaseGrenade", "m_DmgRadius")
		M_DYNVARGET(Streaks, int*, this, "DT_TFPlayer", "m_Shared", "m_nStreaks")
		M_DYNVARGET(Crits, int, this, "DT_TFPlayer", "m_Shared", "tfsharedlocaldata", "m_ScoreData", "m_iCrits")

	M_OFFSETGET(PipebombType, int, 0x8FC)
		M_OFFSETGET(Touched, bool, 0x8F8)
		M_OFFSETGET(PunchAngles, Vec3, 0xE8C)
		M_OFFSETGET(VecVelocity, Vec3, 0x120)
		M_OFFSETGET(WaterJumpTime, float, 0x10FC)
		M_OFFSETGET(SurfaceFriction, float, 0x12D4)
		M_OFFSETGET(MoveType, MoveType_t, 0x1A4)
		M_OFFSETGET(m_ubInterpolationFrame, byte, 0x78)
		M_OFFSETGET(m_ubOldInterpolationFrame, byte, 0x79)

		M_CONDGET(OnGround, GetFlags(), FL_ONGROUND)
		M_CONDGET(InWater, GetFlags(), FL_INWATER)
		M_CONDGET(Ducking, GetFlags(), FL_DUCKING)
		M_CONDGET(Charging, GetCond(), TFCond_Charging)
		M_CONDGET(Scoped, GetCond(), TFCond_Zoomed)
		M_CONDGET(Ubered, GetCond(), TFCond_Ubercharged)
		M_CONDGET(Bonked, GetCond(), TFCond_Bonked)
		M_CONDGET(InMilk, GetCond(), TFCond_Milked)
		M_CONDGET(InJarate, GetCond(), TFCond_Jarated)
		M_CONDGET(Bleeding, GetCond(), TFCond_Bleeding)
		M_CONDGET(Disguised, GetCond(), TFCond_Disguised)
		M_CONDGET(Cloaked, GetCond(), TFCond_Cloaked)
		M_CONDGET(Taunting, GetCond(), TFCond_Taunting)
		M_CONDGET(OnFire, GetCond(), TFCond_OnFire)
		M_CONDGET(Stunned, GetCond(), TFCond_Stunned)
		M_CONDGET(Slowed, GetCond(), TFCond_Slowed)
		M_CONDGET(MegaHealed, GetCond(), TFCond_MegaHeal)
		M_CONDGET(AGhost, GetCondEx2(), TFCondEx2_HalloweenGhostMode)
		M_CONDGET(InBumperKart, GetCondEx2(), TFCondEx_InKart)
		M_CONDGET(PhlogUbered, GetCondEx(), TFCondEx_PhlogUber)
		M_CONDGET(BlastImmune, GetCondEx2(), TFCondEx2_BlastImmune)
		M_CONDGET(BulletImmune, GetCondEx2(), TFCondEx2_BulletImmune)
		M_CONDGET(FireImmune, GetCondEx2(), TFCondEx2_FireImmune)
		M_CONDGET(StrengthRune, GetCondEx2(), TFCondEx2_StrengthRune)
		M_CONDGET(HasteRune, GetCondEx2(), TFCondEx2_HasteRune)
		M_CONDGET(RegenRune, GetCondEx2(), TFCondEx2_RegenRune)
		M_CONDGET(ResistRune, GetCondEx2(), TFCondEx2_ResistRune)
		M_CONDGET(VampireRune, GetCondEx2(), TFCondEx2_VampireRune)
		M_CONDGET(ReflectRune, GetCondEx2(), TFCondEx2_ReflectRune)
		M_CONDGET(PrecisionRune, GetCondEx3(), TFCondEx3_PrecisionRune)
		M_CONDGET(AgilityRune, GetCondEx3(), TFCondEx3_AgilityRune)
		M_CONDGET(KnockoutRune, GetCondEx3(), TFCondEx3_KnockoutRune)
		M_CONDGET(ImbalanceRune, GetCondEx3(), TFCondEx3_ImbalanceRune)
		M_CONDGET(CritTempRune, GetCondEx3(), TFCondEx3_CritboostedTempRune)
		M_CONDGET(KingRune, GetCondEx3(), TFCondEx3_KingRune)
		M_CONDGET(PlagueRune, GetCondEx3(), TFCondEx3_PlagueRune)
		M_CONDGET(SupernovaRune, GetCondEx3(), TFCondEx3_SupernovaRune)
		M_CONDGET(BuffedByKing, GetCondEx3(), TFCondEx3_KingBuff)
		M_CONDGET(BlastResist, GetCondEx(), TFCondEx_ExplosiveCharge)
		M_CONDGET(BulletResist, GetCondEx(), TFCondEx_BulletCharge)
		M_CONDGET(FireResist, GetCondEx(), TFCondEx_FireCharge)

		// thanks litebase this is just better ngl
		NETVAR(m_vecOrigin, Vec3, "CBaseEntity", "m_vecOrigin")
		NETVAR(m_Local, void*, "CBasePlayer", "m_Local")
		NETVAR(m_chAreaBits, void*, "CBasePlayer", "m_chAreaBits")
		NETVAR(m_chAreaPortalBits, void*, "CBasePlayer", "m_chAreaPortalBits")
		NETVAR(m_iHideHUD, int, "CBasePlayer", "m_iHideHUD")
		NETVAR(m_flFOVRate, float, "CBasePlayer", "m_flFOVRate")
		NETVAR(m_bDucked, bool, "CBasePlayer", "m_bDucked")
		NETVAR(m_bDucking, bool, "CBasePlayer", "m_bDucking")
		NETVAR(m_bInDuckJump, bool, "CBasePlayer", "m_bInDuckJump")
		NETVAR(m_flDucktime, float, "CBasePlayer", "m_flDucktime")
		NETVAR(m_flDuckJumpTime, float, "CBasePlayer", "m_flDuckJumpTime")
		NETVAR(m_flJumpTime, float, "CBasePlayer", "m_flJumpTime")
		NETVAR(m_flFallVelocity, float, "CBasePlayer", "m_flFallVelocity")
		NETVAR(m_vecPunchAngle, Vec3, "CBasePlayer", "m_vecPunchAngle")
		NETVAR(m_vecPunchAngleVel, Vec3, "CBasePlayer", "m_vecPunchAngleVel")
		NETVAR(m_bDrawViewmodel, bool, "CBasePlayer", "m_bDrawViewmodel")
		NETVAR(m_bWearingSuit, bool, "CBasePlayer", "m_bWearingSuit")
		NETVAR(m_bPoisoned, bool, "CBasePlayer", "m_bPoisoned")
		NETVAR(m_flStepSize, float, "CBasePlayer", "m_flStepSize")
		NETVAR(m_bAllowAutoMovement, bool, "CBasePlayer", "m_bAllowAutoMovement")
		NETVAR(m_vecViewOffset, Vec3, "CBasePlayer", "m_vecViewOffset[0]")
		NETVAR(m_flFriction, float, "CBasePlayer", "m_flFriction")
		NETVAR(m_iAmmo, void*, "CBasePlayer", "m_iAmmo")
		NETVAR(m_fOnTarget, int, "CBasePlayer", "m_fOnTarget")
		NETVAR(m_nTickBase, int, "CBasePlayer", "m_nTickBase")
		NETVAR(m_nNextThinkTick, int, "CBasePlayer", "m_nNextThinkTick")
		NETVAR(m_hLastWeapon, int, "CBasePlayer", "m_hLastWeapon")
		NETVAR(m_hGroundEntity, int, "CBasePlayer", "m_hGroundEntity")
		NETVAR(m_vecVelocity, Vec3, "CBasePlayer", "m_vecVelocity[0]")
		NETVAR(m_vecBaseVelocity, Vec3, "CBasePlayer", "m_vecBaseVelocity")
		NETVAR(m_hConstraintEntity, int, "CBasePlayer", "m_hConstraintEntity")
		NETVAR(m_vecConstraintCenter, Vec3, "CBasePlayer", "m_vecConstraintCenter")
		NETVAR(m_flConstraintRadius, float, "CBasePlayer", "m_flConstraintRadius")
		NETVAR(m_flConstraintWidth, float, "CBasePlayer", "m_flConstraintWidth")
		NETVAR(m_flConstraintSpeedFactor, float, "CBasePlayer", "m_flConstraintSpeedFactor")
		NETVAR(m_flDeathTime, float, "CBasePlayer", "m_flDeathTime")
		NETVAR(m_nWaterLevel, int, "CBasePlayer", "m_nWaterLevel")
		NETVAR(m_flLaggedMovementValue, float, "CBasePlayer", "m_flLaggedMovementValue")
		NETVAR(m_AttributeList, void*, "CBasePlayer", "m_AttributeList")
		NETVAR(pl, void*, "CBasePlayer", "pl")
		NETVAR(deadflag, int, "CBasePlayer", "deadflag")
		NETVAR(m_iFOV, int, "CBasePlayer", "m_iFOV")
		NETVAR(m_iFOVStart, int, "CBasePlayer", "m_iFOVStart")
		NETVAR(m_flFOVTime, float, "CBasePlayer", "m_flFOVTime")
		NETVAR(m_iDefaultFOV, int, "CBasePlayer", "m_iDefaultFOV")
		NETVAR(m_hZoomOwner, int, "CBasePlayer", "m_hZoomOwner")
		NETVAR(m_hVehicle, int, "CBasePlayer", "m_hVehicle")
		NETVAR(m_hUseEntity, int, "CBasePlayer", "m_hUseEntity")
		NETVAR(m_iHealth, int, "CBasePlayer", "m_iHealth")
		NETVAR(m_ubInterpolationFrame, int, "CBasePlayer", "m_ubInterpolationFrame")
		NETVAR(m_fEffects, int, "CBasePlayer", "m_fEffects")
		NETVAR(m_lifeState, byte, "CBasePlayer", "m_lifeState")
		NETVAR(m_iBonusProgress, int, "CBasePlayer", "m_iBonusProgress")
		NETVAR(m_iBonusChallenge, int, "CBasePlayer", "m_iBonusChallenge")
		NETVAR(m_flMaxspeed, float, "CBasePlayer", "m_flMaxspeed")
		NETVAR(m_fFlags, int, "CBasePlayer", "m_fFlags")
		NETVAR(m_iObserverMode, int, "CBasePlayer", "m_iObserverMode")
		NETVAR(m_hObserverTarget, int, "CBasePlayer", "m_hObserverTarget")
		NETVAR(m_hViewModel, int, "CBasePlayer", "m_hViewModel[0]")
		NETVAR(m_szLastPlaceName, const char*, "CBasePlayer", "m_szLastPlaceName")
		NETVAR(m_flModelScale, float, "CBaseAnimating", "m_flModelScale")
		NETVAR(m_bClientSideAnimation, bool, "CBaseAnimating", "m_bClientSideAnimation")
		NETVAR(m_flPlaybackRate, float, "CBaseAnimating", "m_flPlaybackRate")
		NETVAR(m_vecMins, Vec3, "CBaseEntity", "m_vecMins")
		NETVAR(m_vecMaxs, Vec3, "CBaseEntity", "m_vecMaxs")
		NETVAR(m_nSequence, int, "CBaseAnimating", "m_nSequence")
		NETVAR(m_flCycle, float, "CBaseAnimating", "m_flCycle")
		NETVAR(m_flAnimTime, int, "CBaseAnimating", "m_flAnimTime")
		NETVAR(m_hOwnerEntity, int, "CBaseAnimating", "m_hOwnerEntity")
		NETVAR(m_ConditionList, void*, "CTFPlayer", "m_ConditionList")
		NETVAR(m_nStreaks, void*, "CTFPlayer", "m_nStreaks");

public: //Virtuals
	M_VIRTUALGET(UpdateGlowEffect, void, this, void(__thiscall*)(void*), 226)
		M_VIRTUALGET(MaxHealth, int, this, int(__thiscall*)(void*), 107)
		M_VIRTUALGET(AbsAngles, const Vec3&, this, Vec3& (__thiscall*)(void*), 10)
		M_VIRTUALGET(AbsOrigin, const Vec3&, this, Vec3& (__thiscall*)(void*), 9)

public: //Virtuals from renderable
	__inline void* Renderable() { return reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(this) + 0x4)); }

	M_VIRTUALGET(UpdateClientSideAnimation, void, Renderable(), void(__thiscall*)(void*), 3)
		M_VIRTUALGET(RenderAngles, const Vec3&, Renderable(), const Vec3& (__thiscall*)(void*), 2)
		M_VIRTUALGET(Model, model_t*, Renderable(), model_t* (__thiscall*)(void*), 9)
		M_VIRTUALGET(RgflCoordinateFrame, matrix3x4&, Renderable(), matrix3x4& (__thiscall*)(void*), 34)

		__inline void GetRenderBounds(Vec3& vMins, Vec3& vMaxs) {
		const auto pRend = Renderable();
		GetVFunc<void(__thiscall*)(void*, Vec3&, Vec3&)>(pRend, 20)(pRend, vMins, vMaxs);
	}

	__inline bool SetupBones(matrix3x4* pOut, int nMax, int nMask, float flTime) {
		const auto pRend = Renderable();
		return GetVFunc<bool(__thiscall*)(void*, matrix3x4*, int, int, float)>(pRend, 16)(pRend, pOut, nMax, nMask, flTime);
	}

	__inline int DrawModel(int nFlags) {
		const auto pRend = Renderable();
		return GetVFunc<int(__thiscall*)(void*, int)>(pRend, 10)(pRend, nFlags);
	}

public: //Virtuals from networkable
	__inline IClientNetworkable* Networkable() { return reinterpret_cast<IClientNetworkable*>((reinterpret_cast<uintptr_t>(this) + 0x8)); }

	M_VIRTUALGET(ClientClass, CClientClass*, Networkable(), CClientClass* (__thiscall*)(void*), 2)
		M_VIRTUALGET(Dormant, bool, Networkable(), bool(__thiscall*)(void*), 8)
		M_VIRTUALGET(Index, int, Networkable(), int(__thiscall*)(void*), 9)

public: //Everything else, lol.
	__inline size_t* GetMyWeapons() {
		static auto dwOff = g_NetVars.get_offset("DT_BaseCombatCharacter", "m_hMyWeapons");
		return reinterpret_cast<size_t*>(this + dwOff);
	}

	__inline bool* PDAman() {
		static auto dwOff = g_NetVars.get_offset("DT_TFPlayer", "m_bViewingCYOAPDA");
		return reinterpret_cast<bool*>(this + dwOff);
		//M_DYNVARGET(ViewingCYOAPDA, bool, this, "DT_TFPlayer", "m_bViewingCYOAPDA")
	}

	__inline float* GetHeadScale() {
		static auto dwOff = g_NetVars.get_offset("DT_TFPlayer", "m_flHeadScale");
		return reinterpret_cast<float*>(this + dwOff);
	}
	__inline float* GetTorsoScale() {
		static auto dwOff = g_NetVars.get_offset("DT_TFPlayer", "m_flTorsoScale");
		return reinterpret_cast<float*>(this + dwOff);
	}
	__inline float* GetHandScale() {
		static auto dwOff = g_NetVars.get_offset("DT_TFPlayer", "m_flHandScale");
		return reinterpret_cast<float*>(this + dwOff);
	}

	int GetAmmoCount(int iAmmoIndex)
	{
		static auto FN = reinterpret_cast<int(__thiscall*)(CBaseEntity*, int)>(g_Pattern.Find(L"client.dll", L"55 8B EC 56 8B 75 08 57 8B F9 83 FE FF 75 08 5F 33 C0 5E 5D C2 04 00"));
		return FN(this, iAmmoIndex);
	}


	__inline CBaseEntity* GetGroundEntity() {
		DYNVAR(pHandle, int, "DT_BasePlayer", "m_hGroundEntity");
		return reinterpret_cast<CBaseEntity*>(I::ClientEntityList->GetClientEntityFromHandle(pHandle.GetValue(this)));
	}

	__inline CBaseEntity* FirstMoveChild() {
		return I::ClientEntityList->GetClientEntity(*reinterpret_cast<int*>(this + 0x1B0) & 0xFFF);
	}

	__inline CBaseEntity* NextMovePeer() {
		return I::ClientEntityList->GetClientEntity(*reinterpret_cast<int*>(this + 0x1B4) & 0xFFF);
	}

	__inline CBaseCombatWeapon* GetActiveWeapon() {
		DYNVAR(pHandle, int, "DT_BaseCombatCharacter", "m_hActiveWeapon");
		return reinterpret_cast<CBaseCombatWeapon*>(I::ClientEntityList->GetClientEntityFromHandle(pHandle.GetValue(this)));
	}

	inline float TeamFortress_CalculateMaxSpeed(bool bIgnoreSpecialAbility = false)
	{
		typedef float(__thiscall* CalculateMaxSpeedFn)(CBaseEntity*, bool);
		static DWORD dwFn = g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? D9 96 ? ? ? ? D9 EE DB F1") + 0x1;
		static DWORD dwEstimate = ((*(PDWORD)(dwFn)) + dwFn + 0x4);
		CalculateMaxSpeedFn maxSpeed = (CalculateMaxSpeedFn)dwEstimate;
		return maxSpeed(this, bIgnoreSpecialAbility);
	}

	__inline CBaseCombatWeapon* GetWeaponFromSlot(const int nSlot) {
		static DWORD dwMyWeapons = g_NetVars.get_offset("DT_BaseCombatCharacter", "m_hMyWeapons");
		int hWeapon = *reinterpret_cast<int*>(this + (dwMyWeapons + (nSlot * 0x4)));
		return reinterpret_cast<CBaseCombatWeapon*>(I::ClientEntityList->GetClientEntityFromHandle(hWeapon));
	}

	__inline bool InCond(int eCond)
	{
		using fn = bool(__thiscall*)(CBaseEntity*, int);
		static fn FN = reinterpret_cast<fn>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 08 56 57 8B 7D 08 8B F1 83 FF 20"));
		return FN(this, eCond);
	}

	__inline ETFClassID GetClassID() {
		const auto& pCC = GetClientClass();
		return pCC ? ETFClassID(pCC->m_ClassID) : ETFClassID(0);
	}

	__inline CTFPlayerAnimState* GetAnimState() {
		return *reinterpret_cast<CTFPlayerAnimState**>(this + 0x1BF8);
	}

	__inline void SetPoseParam(std::array<float, MAXSTUDIOPOSEPARAM> param) {
		static DWORD dwOffset = g_NetVars.get_offset("DT_BaseAnimating", "m_flPoseParameter");
		*reinterpret_cast<std::array<float, MAXSTUDIOPOSEPARAM>*>(this + dwOffset) = param;
	}

	__inline std::array<float, MAXSTUDIOPOSEPARAM> GetPoseParam() {
		static DWORD dwOffset = g_NetVars.get_offset("DT_BaseAnimating", "m_flPoseParameter");
		return *reinterpret_cast<std::array<float, MAXSTUDIOPOSEPARAM>*>(this + dwOffset);
	}

	__inline bool GetHitboxMinsAndMaxsAndMatrix(const int nHitbox, Vec3& vMins, Vec3& vMaxs, matrix3x4& matrix, Vec3* vCenter) {
		if (const auto& pModel = GetModel()) {
			if (const auto& pHdr = I::ModelInfoClient->GetStudioModel(pModel)) {
				matrix3x4 BoneMatrix[128];
				if (SetupBones(BoneMatrix, 128, 0x100, I::GlobalVars->curtime)) {
					if (const auto& pSet = pHdr->GetHitboxSet(GetHitboxSet())) {
						if (const auto& pBox = pSet->hitbox(nHitbox)) {
							vMins = pBox->bbmin; vMaxs = pBox->bbmax;
							memcpy(matrix, BoneMatrix[pBox->bone], sizeof(matrix3x4));
							if (vCenter) Math::VectorTransform(((pBox->bbmin + pBox->bbmax) * 0.5f), BoneMatrix[pBox->bone], *vCenter);
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	__inline bool GetHitboxMinsAndMaxs(const int nHitbox, Vec3& vMins, Vec3& vMaxs, Vec3* vCenter) {
		if (const auto& pModel = GetModel()) {
			if (const auto& pHdr = I::ModelInfoClient->GetStudioModel(pModel)) {
				matrix3x4 BoneMatrix[128];
				if (SetupBones(BoneMatrix, 128, 0x100, I::GlobalVars->curtime)) {
					if (const auto& pSet = pHdr->GetHitboxSet(GetHitboxSet())) {
						if (const auto& pBox = pSet->hitbox(nHitbox)) {
							vMins = pBox->bbmin; vMaxs = pBox->bbmax;
							if (vCenter) Math::VectorTransform(((pBox->bbmin + pBox->bbmax) * 0.5f), BoneMatrix[pBox->bone], *vCenter);
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	__inline bool IsClass(const int nClass) {
		return (GetClassNum() == nClass);
	}

	__inline bool IsSentrygun() {
		return GetClassID() == ETFClassID::CObjectSentrygun;
	}

	__inline bool IsDispenser() {
		return GetClassID() == ETFClassID::CObjectDispenser;
	}

	__inline bool IsTeleporter() {
		return GetClassID() == ETFClassID::CObjectTeleporter;
	}

	__inline bool IsInValidTeam() {
		const int nTeam = GetTeamNum();
		return (nTeam == 2 || nTeam == 3);
	}

	__inline bool IsSwimming() {
		return (GetWaterLevel() > 1);
	}

	__inline bool IsAlive() {
		return (GetLifeState() == LIFE_ALIVE);
	}

	__inline bool IsBaseCombatWeapon() {
		return (GetClassID() == ETFClassID::CBaseCombatWeapon);
	}

	__inline bool IsWearable() {
		return (GetClassID() == ETFClassID::CTFWearable);
	}

	__inline bool IsVulnerable() {
		int nCond = GetCond();
		int nCondEx = GetCondEx();
		return !(nCond & TFCond_Ubercharged || nCond & TFCond_Bonked || nCondEx & TFCondEx_PhlogUber || nCondEx & TFCondEx_UberchargedHidden || nCondEx & TFCondEx_UberchargedCanteen);
	}

	__inline bool IsVisible() {
		int nCond = GetCond();
		int nCondEx = GetCondEx();

		if (nCond & (TFCond_Milked | TFCond_Jarated | TFCond_OnFire | TFCond_CloakFlicker | TFCond_Bleeding)) {
			return false;
		}

		return (nCond & TFCond_Cloaked);
	}

	__inline bool IsCritBoosted() {
		int nCond = GetCond(), nCondEx = GetCondEx();

		return (nCond & TFCond_Kritzkrieged ||
			nCond & TFCond_MiniCrits ||
			nCondEx & TFCondEx_CritCanteen ||
			nCondEx & TFCondEx_CritOnFirstBlood ||
			nCondEx & TFCondEx_CritOnWin ||
			nCondEx & TFCondEx_CritOnKill ||
			nCondEx & TFCondEx_CritDemoCharge ||
			nCondEx & TFCondEx_CritOnFlagCapture ||
			nCondEx & TFCondEx_HalloweenCritCandy ||
			nCondEx & TFCondEx_PyroCrits ||
			IsCritTempRune());
	}

	__inline bool IsCritBoostedNoMini() {
		int nCond = GetCond(), nCondEx = GetCondEx();

		return (nCond & TFCond_Kritzkrieged ||
			nCondEx & TFCondEx_CritCanteen ||
			nCondEx & TFCondEx_CritOnFirstBlood ||
			nCondEx & TFCondEx_CritOnWin ||
			nCondEx & TFCondEx_CritOnKill ||
			nCondEx & TFCondEx_CritDemoCharge ||
			nCondEx & TFCondEx_CritOnFlagCapture ||
			nCondEx & TFCondEx_HalloweenCritCandy ||
			nCondEx & TFCondEx_PyroCrits ||
			IsCritTempRune());
	}

	__inline const wchar_t* GetRune() {
		if (IsStrengthRune())	{ return (L"Strength Rune"); }
		if (IsHasteRune())		{ return (L"Haste Rune"); }
		if (IsRegenRune())		{ return (L"Regen Rune"); }
		if (IsResistRune())		{ return (L"Resist Rune"); }
		if (IsVampireRune())	{ return (L"Vampire Rune"); }
		if (IsReflectRune())	{ return (L"Reflect Rune"); }
		if (IsPrecisionRune())	{ return (L"Precision Rune"); }
		if (IsAgilityRune())	{ return (L"Agility Rune"); }
		if (IsKnockoutRune())	{ return (L"Knockout Rune"); }
		if (IsImbalanceRune())	{ return (L"Imbalance Rune"); }
		if (IsKingRune())		{ return (L"King"); }
		if (IsPlagueRune())		{ return (L"Plague Rune"); }
		if (IsSupernovaRune())	{ return (L"Supernova Rune"); }
		return nullptr;
	}

	__inline bool IsKingBuffed() {
		return IsBuffedByKing();
	}

	__inline bool IsPlayer() {
		return (GetClassID() == ETFClassID::CTFPlayer);
	}

	__inline bool IsBuilding() {
		switch (GetClassID()) {
		case ETFClassID::CObjectDispenser:
		case ETFClassID::CObjectSentrygun:
		case ETFClassID::CObjectTeleporter: return true;
		default: return false;
		}
	}

	__inline bool IsPickup() {
		switch (GetClassID()) {
		case ETFClassID::CBaseAnimating: return GetModelName()[24] != 'h';
		case ETFClassID::CTFAmmoPack: return true;
		default: return false;
		}
	}

	__inline bool IsNPC()
	{
		switch (GetClassID())
		{
		case ETFClassID::CHeadlessHatman:
		case ETFClassID::CTFTankBoss:
		case ETFClassID::CMerasmus:
		case ETFClassID::CZombie:
		case ETFClassID::CEyeballBoss:
			return true;
		default: return false;
		}
	}

	__inline bool IsBomb()
	{
		switch (GetClassID())
		{
		case ETFClassID::CTFPumpkinBomb:
		case ETFClassID::CTFGenericBomb:
			return true;
		default: return false;
		}
	}

	__inline Vec3 GetHitboxPos(const int nHitbox) {
		if (const auto& pModel = GetModel()) {
			if (const auto& pHdr = I::ModelInfoClient->GetStudioModel(pModel)) {
				matrix3x4 BoneMatrix[128];
				if (SetupBones(BoneMatrix, 128, 0x100, I::GlobalVars->curtime)) {
					if (const auto& pSet = pHdr->GetHitboxSet(GetHitboxSet())) {
						if (const auto& pBox = pSet->hitbox(nHitbox)) {
							Vec3 vPos = (pBox->bbmin + pBox->bbmax) * 0.5f, vOut;
							Math::VectorTransform(vPos, BoneMatrix[pBox->bone], vOut);
							return vOut;
						}
					}
				}
			}
		}

		return Vec3();
	}

	__inline Vec3 GetBonePos(const int nBone) {
		matrix3x4 BoneMatrix[128];
		if (SetupBones(BoneMatrix, 128, 0x100, I::GlobalVars->curtime))
			return Vec3(BoneMatrix[nBone][0][3], BoneMatrix[nBone][1][3], BoneMatrix[nBone][2][3]);

		return Vec3(0.0f, 0.0f, 0.0f);
	}

	__inline Vec3 GetEyePosition() {
		return (GetViewOffset() + GetAbsOrigin());
	}

	__inline Vec3 GetWorldSpaceCenter() {
		Vec3 vMin, vMax; GetRenderBounds(vMin, vMax);
		Vec3 vWorldSpaceCenter = GetAbsOrigin();
		vWorldSpaceCenter.z += (vMin.z + vMax.z) / 2.0f;
		return vWorldSpaceCenter;
	}

	__inline Vec3 GetVelocity() {
		static auto FN = reinterpret_cast<void(__thiscall*)(CBaseEntity*, Vec3&)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC ? 56 8B F1 E8 ? ? ? ? 3B F0"));
		Vec3 v;
		FN(this, v);
		return v;
	}

	// This does not return your actual shoot pos for projectile weapons! Use Utils::GetRealShootPos(...) instead
	__inline Vec3 GetShootPos() {
		return  GetVecOrigin() + GetViewOffset();
	}

	__inline int GetNumOfHitboxes() {
		if (const auto& pModel = GetModel()) {
			if (const auto& pHdr = I::ModelInfoClient->GetStudioModel(pModel)) {
				if (const auto& pSet = pHdr->GetHitboxSet(GetHitboxSet()))
					return pSet->numhitboxes;
			}
		}

		return 0;
	}

	__inline int GetCritMult() {
		DYNVAR(m_iCritMult, int, "DT_TFPlayer", "m_Shared", "m_iCritMult");
		return Math::RemapValClamped(m_iCritMult.GetValue(this), 0.0f, 255.0f, 1.0f, 4.0f);
	}

	__inline float GetPlayerMaxVelocity() {
		switch (GetClassNum()) {
		case ETFClass::CLASS_SCOUT: return 400.0f;
		case ETFClass::CLASS_DEMOMAN: return 280.0f;
		case ETFClass::CLASS_ENGINEER: return 300.0f;
		case ETFClass::CLASS_HEAVY: return 230.0f; //110 when spinning minigun
		case ETFClass::CLASS_MEDIC: return 320.0f;
		case ETFClass::CLASS_PYRO: return 300.0f;
		case ETFClass::CLASS_SNIPER: return 300.0f;
		case ETFClass::CLASS_SOLDIER: return 240.0f;
		case ETFClass::CLASS_SPY: return 320.0f;
		default: return 1.0f;
		}
	}

	__inline bool OnSolid() {
		return m_hGroundEntity() >= 0 || IsOnGround();
	}

	__inline const char* GetModelName() {
		return I::ModelInfoClient->GetModelName(GetModel());
	}

	__inline void SetTickBase(const int nTickBase) {
		DYNVAR_SET(int, this, nTickBase, "DT_BasePlayer", "localdata", "m_nTickBase");
	}

	__inline void UpTickBase() {
		static CDynamicNetvar<int> n("DT_BasePlayer", "localdata", "m_nTickBase");
		n.SetValue(this, n.GetValue(this) + 1);
	}
	__inline void DownTickBase() {
		static CDynamicNetvar<int> n("DT_BasePlayer", "localdata", "m_nTickBase");
		n.SetValue(this, n.GetValue(this) - 1);
	}

	__inline void ClearPunchAngle() { //m_vecPunchAngle
		*reinterpret_cast<Vec3*>(this + 0xE8C) = Vec3(0.0f, 0.0f, 0.0f);
	}

	__inline void SetRenderAngles(const Vec3& vAngles) {
		Vec3* pRenderAngles = const_cast<Vec3*>(&this->GetRenderAngles());
		*pRenderAngles = vAngles;
	}

	__inline void SetAbsOrigin(const Vec3& vOrigin) {
		typedef void(__thiscall* FN)(CBaseEntity*, const Vec3&);
		static DWORD dwFN = g_Pattern.Find(L"client.dll", L"55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B 7D 08 F3 0F 10 07");
		FN func = (FN)dwFN;
		func(this, vOrigin);
	}

	__inline void PostDataUpdate(int updateType) {
		typedef void(__thiscall* FN)(CBaseEntity*, int);
		static DWORD dwFn = g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 18 53 8B 5D 08 56 57 8B F9 85 DB");
		FN func = (FN)dwFn;
		func(this, updateType);
	}

	__inline void SetAbsAngles(const Vec3& vAngles) {
		Vec3* pAbsAngles = const_cast<Vec3*>(&this->GetAbsAngles());
		*pAbsAngles = vAngles;
	}

	__inline void SetCurrentCmd(CUserCmd* pCmd) {
		DYNVAR_SET(CUserCmd*, (this - 0x4), pCmd, "DT_BasePlayer", "localdata", "m_hConstraintEntity");
	}

	__inline void SetVecOrigin(const Vec3& vOrigin) {
		DYNVAR_SET(Vec3, this, vOrigin, "DT_BaseEntity", "m_vecOrigin");
	}

	__inline void RemoveEffect(const BYTE Effect) {
		*reinterpret_cast<byte*>(this + 0x7C) &= ~Effect;

		if (Effect == EF_NODRAW) {
			static auto AddToLeafSystemFn = reinterpret_cast<int(__thiscall*)(PVOID, int)>(g_Pattern.Find(L"client.dll", L"55 8B EC 56 FF 75 08 8B F1 B8"));

			if (AddToLeafSystemFn)
				AddToLeafSystemFn(this, RENDER_GROUP_OPAQUE_ENTITY);
		}
	}

	__inline void SetGlowEnabled(const bool bState) {
		DYNVAR_SET(bool, this, bState, "DT_TFPlayer", "m_bGlowEnabled");
	}

	__inline void SetCond(const int nCond) {
		DYNVAR_SET(int, this, nCond, "DT_TFPlayer", "m_Shared", "m_nPlayerCond");
	}

	__inline void SetFov(const int nFov) {
		DYNVAR_SET(int, this, nFov, "DT_BasePlayer", "m_iFOV");
	}

	__inline void ForceTauntCam(const int nTo) {
		DYNVAR_SET(int, this, nTo, "DT_TFPlayer", "m_nForceTauntCam");
	}

	__inline void SetFlags(const int nFlags) {
		DYNVAR_SET(int, this, nFlags, "DT_BasePlayer", "m_fFlags");
	}

	__inline void AddCond(const int nCond) {
		static DWORD offset = g_NetVars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCond");
		*reinterpret_cast<DWORD*>(this + offset) |= nCond;
	}

	__inline void AddCondEx(const int nCond) {
		static DWORD offset = g_NetVars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx");
		*reinterpret_cast<DWORD*>(this + offset) |= nCond;
	}

	__inline void RemoveCond(const int nCond) {
		static DWORD offset = g_NetVars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCond");
		*reinterpret_cast<DWORD*>(this + offset) &= ~nCond;
	}

	__inline void RemoveCondEx(const int nCond) {
		static DWORD offset = g_NetVars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx");
		*reinterpret_cast<DWORD*>(this + offset) &= ~nCond;
	}

	__inline void SetEyeAngles(const Vec3& vAngles) {
		DYNVAR_SET(Vec3, this, vAngles, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]");
	}

	__inline void IncreaseTickBase() {
		static CDynamicNetvar<int>n("DT_BasePlayer", "localdata", "m_nTickBase");
		n.SetValue(this, n.GetValue(this) + 1);
	}

	__inline void SetAnimTime(const float flAnimTime) {
		DYNVAR_SET(float, this, flAnimTime, "DT_BaseEntity", "m_flAnimTime");
	}

	/*
	__inline CCollisionProperty* GetCollision() {
		return *reinterpret_cast<CCollisionProperty*>(this + 0x1C8);
	}
	*/ // why push this when A) It doesn't work, and B) Isn't used

};
