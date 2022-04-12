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
	M_DYNVARGET(NextNoiseMakerTime, float, this, _("DT_TFPlayer"), _("m_Shared"), _("m_flNextNoiseMakerTime"))
		M_DYNVARGET(FeignDeathReady, bool, this, _("DT_TFPlayer"), _("m_Shared"), _("m_bFeignDeathReady"))
		M_DYNVARGET(StepSize, float, this, _("DT_BasePlayer"), _("localdata"), _("m_flStepSize"))
		M_DYNVARGET(ConveyorSpeed, float, this, _("DT_FuncConveyor"), _("m_flConveyorSpeed"))
		M_DYNVARGET(MaxSpeed, float, this, _("DT_BasePlayer"), _("m_flMaxspeed"))
		M_DYNVARGET(State, int, this, _("DT_TFPlayer"), _("m_Shared"), _("m_nPlayerState"))
		M_DYNVARGET(ViewOffset, Vec3, this, _("DT_BasePlayer"), _("localdata"), _("m_vecViewOffset[0]"))
		M_DYNVARGET(CurrentCommand, CUserCmd*, (this - 0x4), _("DT_BasePlayer"), _("localdata"), _("m_hConstraintEntity"))
		M_DYNVARGET(VecOrigin, Vec3, this, _("DT_BaseEntity"), _("m_vecOrigin"))
		M_DYNVARGET(Ammo, int, (this + 0x4), _("DT_BasePlayer"), _("localdata"), _("m_iAmmo"))
		M_DYNVARGET(HitboxSet, int, this, _("DT_BaseAnimating"), _("m_nHitboxSet"))
		M_DYNVARGET(TickBase, int, this, _("DT_BasePlayer"), _("localdata"), _("m_nTickBase"))
		M_DYNVARGET(SimulationTime, float, this, _("DT_BaseEntity"), _("m_flSimulationTime"))
		M_DYNVARGET(hOwner, int, this, _("DT_BaseEntity"), _("m_hOwnerEntity"))
		M_DYNVARGET(Health, int, this, _("DT_BasePlayer"), _("m_iHealth"))
		M_DYNVARGET(TeamNum, int, this, _("DT_BaseEntity"), _("m_iTeamNum"))
		M_DYNVARGET(Flags, int, this, _("DT_BasePlayer"), _("m_fFlags"))
		M_DYNVARGET(LifeState, BYTE, this, _("DT_BasePlayer"), _("m_lifeState"))
		M_DYNVARGET(ClassNum, int, this, _("DT_TFPlayer"), _("m_PlayerClass"), _("m_iClass"))
		M_DYNVARGET(Cond, int, this, _("DT_TFPlayer"), _("m_Shared"), _("m_nPlayerCond"))
		M_DYNVARGET(ViewingCYOAPDA, bool, this, _("DT_TFPlayer"), _("m_bViewingCYOAPDA"))
		M_DYNVARGET(UsingActionSlot, bool, this, _("DT_TFPlayer"), _("m_bUsingActionSlot"))
		M_DYNVARGET(LoadoutUnavailable, bool, this, _("DT_TFPlayer"), _("m_Shared"), _("m_bLoadoutUnavailable"))
		M_DYNVARGET(CondEx, int, this, _("DT_TFPlayer"), _("m_Shared"), _("m_nPlayerCondEx"))
		M_DYNVARGET(CondEx2, int, this, _("DT_TFPlayer"), _("m_Shared"), _("m_nPlayerCondEx2"))
		M_DYNVARGET(CondEx3, int, this, _("DT_TFPlayer"), _("m_Shared"), _("m_nPlayerCondEx3"))
		M_DYNVARGET(CollideableMins, Vec3, this, _("DT_BaseEntity"), _("m_Collision"), _("m_vecMins"))
		M_DYNVARGET(CollideableMaxs, Vec3, this, _("DT_BaseEntity"), _("m_Collision"), _("m_vecMaxs"))
		M_DYNVARGET(EyeAngles, Vec3, this, _("DT_TFPlayer"), _("tfnonlocaldata"), _("m_angEyeAngles[0]"))
		M_DYNVARGET(WaterLevel, BYTE, this, _("DT_TFPlayer"), _("m_nWaterLevel"))
		M_DYNVARGET(NextAttack, float, this, _("DT_BaseCombatCharacter"), _("bcc_localdata"), _("m_flNextAttack"))
		M_DYNVARGET(Fov, int, this, _("DT_BasePlayer"), _("m_iFOV"))
		M_DYNVARGET(ObserverTarget, int, this, _("DT_BasePlayer"), _("m_hObserverTarget"))
		M_DYNVARGET(ObserverMode, int, this, _("DT_BasePlayer"), _("m_iObserverMode"))
		M_DYNVARGET(GlowEnabled, bool, this, _("DT_TFPlayer"), _("m_bGlowEnabled"))
		M_DYNVARGET(Thrower, void*, this, _("DT_BaseGrenade"), _("m_hThrower"))
		M_DYNVARGET(DamageRadius, float, this, _("DT_BaseGrenade"), _("m_DmgRadius"));

	M_OFFSETGET(PipebombType, int, 0x8FC)
		M_OFFSETGET(Touched, bool, 0x8F8)
		M_OFFSETGET(PunchAngles, Vec3, 0xE8C)
		M_OFFSETGET(VecVelocity, Vec3, 0x120)
		M_OFFSETGET(WaterJumpTime, float, 0x10FC)
		M_OFFSETGET(SurfaceFriction, float, 0x12D4)
		M_OFFSETGET(MoveType, MoveType_t, 0x1A4)

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
		M_CONDGET(Stunned, GetCond(), TFCond_Stunned)
		M_CONDGET(Slowed, GetCond(), TFCond_Slowed)
		M_CONDGET(MegaHealed, GetCond(), TFCond_MegaHeal)
		M_CONDGET(AGhost, GetCondEx2(), TFCondEx2_HalloweenGhostMode)
		M_CONDGET(InBumperKart, GetCondEx2(), TFCondEx_InKart)
		M_CONDGET(PhlogUbered, GetCondEx(), TFCondEx_PhlogUber)
		M_CONDGET(BlastImmune, GetCondEx2(), TFCondEx2_BlastImmune)
		M_CONDGET(BulletImmune, GetCondEx2(), TFCondEx2_BulletImmune)
		M_CONDGET(FireImmune, GetCondEx2(), TFCondEx2_FireImmune)

		// thanks litebase this is just better ngl
		NETVAR(m_vecOrigin, Vec3, _("CBaseEntity"), _("m_vecOrigin"))
		NETVAR(m_Local, void*, _("CBasePlayer"), _("m_Local"))
		NETVAR(m_chAreaBits, void*, _("CBasePlayer"), _("m_chAreaBits"))
		NETVAR(m_chAreaPortalBits, void*, _("CBasePlayer"), _("m_chAreaPortalBits"))
		NETVAR(m_iHideHUD, int, _("CBasePlayer"), _("m_iHideHUD"))
		NETVAR(m_flFOVRate, float, _("CBasePlayer"), _("m_flFOVRate"))
		NETVAR(m_bDucked, bool, _("CBasePlayer"), _("m_bDucked"))
		NETVAR(m_bDucking, bool, _("CBasePlayer"), _("m_bDucking"))
		NETVAR(m_bInDuckJump, bool, _("CBasePlayer"), _("m_bInDuckJump"))
		NETVAR(m_flDucktime, float, _("CBasePlayer"), _("m_flDucktime"))
		NETVAR(m_flDuckJumpTime, float, _("CBasePlayer"), _("m_flDuckJumpTime"))
		NETVAR(m_flJumpTime, float, _("CBasePlayer"), _("m_flJumpTime"))
		NETVAR(m_flFallVelocity, float, _("CBasePlayer"), _("m_flFallVelocity"))
		NETVAR(m_vecPunchAngle, Vec3, _("CBasePlayer"), _("m_vecPunchAngle"))
		NETVAR(m_vecPunchAngleVel, Vec3, _("CBasePlayer"), _("m_vecPunchAngleVel"))
		NETVAR(m_bDrawViewmodel, bool, _("CBasePlayer"), _("m_bDrawViewmodel"))
		NETVAR(m_bWearingSuit, bool, _("CBasePlayer"), _("m_bWearingSuit"))
		NETVAR(m_bPoisoned, bool, _("CBasePlayer"), _("m_bPoisoned"))
		NETVAR(m_flStepSize, float, _("CBasePlayer"), _("m_flStepSize"))
		NETVAR(m_bAllowAutoMovement, bool, _("CBasePlayer"), _("m_bAllowAutoMovement"))
		NETVAR(m_vecViewOffset, Vec3, _("CBasePlayer"), "m_vecViewOffset[0]")
		NETVAR(m_flFriction, float, _("CBasePlayer"), _("m_flFriction"))
		NETVAR(m_iAmmo, void*, _("CBasePlayer"), _("m_iAmmo"))
		NETVAR(m_fOnTarget, int, _("CBasePlayer"), _("m_fOnTarget"))
		NETVAR(m_nTickBase, int, _("CBasePlayer"), _("m_nTickBase"))
		NETVAR(m_nNextThinkTick, int, _("CBasePlayer"), _("m_nNextThinkTick"))
		NETVAR(m_hLastWeapon, int, _("CBasePlayer"), _("m_hLastWeapon"))
		NETVAR(m_hGroundEntity, int, _("CBasePlayer"), _("m_hGroundEntity"))
		NETVAR(m_vecVelocity, Vec3, _("CBasePlayer"), _("m_vecVelocity[0]"))
		NETVAR(m_vecBaseVelocity, Vec3, _("CBasePlayer"), _("m_vecBaseVelocity"))
		NETVAR(m_hConstraintEntity, int, _("CBasePlayer"), _("m_hConstraintEntity"))
		NETVAR(m_vecConstraintCenter, Vec3, _("CBasePlayer"), _("m_vecConstraintCenter"))
		NETVAR(m_flConstraintRadius, float, _("CBasePlayer"), _("m_flConstraintRadius"))
		NETVAR(m_flConstraintWidth, float, _("CBasePlayer"), _("m_flConstraintWidth"))
		NETVAR(m_flConstraintSpeedFactor, float, _("CBasePlayer"), _("m_flConstraintSpeedFactor"))
		NETVAR(m_flDeathTime, float, _("CBasePlayer"), _("m_flDeathTime"))
		NETVAR(m_nWaterLevel, int, _("CBasePlayer"), _("m_nWaterLevel"))
		NETVAR(m_flLaggedMovementValue, float, _("CBasePlayer"), _("m_flLaggedMovementValue"))
		NETVAR(m_AttributeList, void*, _("CBasePlayer"), _("m_AttributeList"))
		NETVAR(pl, void*, _("CBasePlayer"), _("pl"))
		NETVAR(deadflag, int, _("CBasePlayer"), _("deadflag"))
		NETVAR(m_iFOV, int, _("CBasePlayer"), _("m_iFOV"))
		NETVAR(m_iFOVStart, int, _("CBasePlayer"), _("m_iFOVStart"))
		NETVAR(m_flFOVTime, float, _("CBasePlayer"), _("m_flFOVTime"))
		NETVAR(m_iDefaultFOV, int, _("CBasePlayer"), _("m_iDefaultFOV"))
		NETVAR(m_hZoomOwner, int, _("CBasePlayer"), _("m_hZoomOwner"))
		NETVAR(m_hVehicle, int, _("CBasePlayer"), _("m_hVehicle"))
		NETVAR(m_hUseEntity, int, _("CBasePlayer"), _("m_hUseEntity"))
		NETVAR(m_iHealth, int, _("CBasePlayer"), _("m_iHealth"))
		NETVAR(m_lifeState, byte, _("CBasePlayer"), _("m_lifeState"))
		NETVAR(m_iBonusProgress, int, _("CBasePlayer"), _("m_iBonusProgress"))
		NETVAR(m_iBonusChallenge, int, _("CBasePlayer"), _("m_iBonusChallenge"))
		NETVAR(m_flMaxspeed, float, _("CBasePlayer"), _("m_flMaxspeed"))
		NETVAR(m_fFlags, int, _("CBasePlayer"), _("m_fFlags"))
		NETVAR(m_iObserverMode, int, _("CBasePlayer"), _("m_iObserverMode"))
		NETVAR(m_hObserverTarget, int, _("CBasePlayer"), _("m_hObserverTarget"))
		NETVAR(m_hViewModel, int, _("CBasePlayer"), _("m_hViewModel[0]"))
		NETVAR(m_szLastPlaceName, const char*, _("CBasePlayer"), _("m_szLastPlaceName"))
		NETVAR(m_flModelScale, float, _("CBaseAnimating"), _("m_flModelScale"))
		NETVAR(m_bClientSideAnimation, bool, _("CBaseAnimating"), _("m_bClientSideAnimation"))
		NETVAR(m_flPlaybackRate, float, _("CBaseAnimating"), _("m_flPlaybackRate"))
		NETVAR(m_vecMins, Vec3, _("CBaseEntity"), _("m_vecMins"))
		NETVAR(m_vecMaxs, Vec3, _("CBaseEntity"), _("m_vecMaxs"))
		NETVAR(m_nSequence, int, _("CBaseAnimating"), _("m_nSequence"))
		NETVAR(m_flCycle, float, _("CBaseAnimating"), _("m_flCycle"))
		NETVAR(m_flAnimTime, int, _("CBaseAnimating"), _("m_flAnimTime "))
		NETVAR(m_ConditionList, void*, _("CTFPlayer"), _("m_ConditionList"))

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
		static auto dwOff = g_NetVars.get_offset(_("DT_BaseCombatCharacter"), _("m_hMyWeapons"));
		return reinterpret_cast<size_t*>(this + dwOff);
	}

	__inline bool* PDAman() {
		static auto dwOff = g_NetVars.get_offset(_("DT_TFPlayer"), _("m_bViewingCYOAPDA"));
		return reinterpret_cast<bool*>(this + dwOff);
		//M_DYNVARGET(ViewingCYOAPDA, bool, this, _("DT_TFPlayer"), _("m_bViewingCYOAPDA"))
	}

	__inline float* GetHeadScale() {
		static auto dwOff = g_NetVars.get_offset(_("DT_TFPlayer"), _("m_flHeadScale"));
		return reinterpret_cast<float*>(this + dwOff);
	}
	__inline float* GetTorsoScale() {
		static auto dwOff = g_NetVars.get_offset(_("DT_TFPlayer"), _("m_flTorsoScale"));
		return reinterpret_cast<float*>(this + dwOff);
	}
	__inline float* GetHandScale() {
		static auto dwOff = g_NetVars.get_offset(_("DT_TFPlayer"), _("m_flHandScale"));
		return reinterpret_cast<float*>(this + dwOff);
	}


	__inline CBaseEntity* GetGroundEntity() {
		DYNVAR(pHandle, int, _("DT_BasePlayer"), _("m_hGroundEntity"));
		return reinterpret_cast<CBaseEntity*>(g_Interfaces.EntityList->GetClientEntityFromHandle(pHandle.GetValue(this)));
	}

	__inline CBaseEntity* FirstMoveChild() {
		return g_Interfaces.EntityList->GetClientEntity(*reinterpret_cast<int*>(this + 0x1B0) & 0xFFF);
	}

	__inline CBaseEntity* NextMovePeer() {
		return g_Interfaces.EntityList->GetClientEntity(*reinterpret_cast<int*>(this + 0x1B4) & 0xFFF);
	}

	__inline CBaseCombatWeapon* GetActiveWeapon() {
		DYNVAR(pHandle, int, _("DT_BaseCombatCharacter"), _("m_hActiveWeapon"));
		return reinterpret_cast<CBaseCombatWeapon*>(g_Interfaces.EntityList->GetClientEntityFromHandle(pHandle.GetValue(this)));
	}

	inline float TeamFortress_CalculateMaxSpeed(bool bIgnoreSpecialAbility = false)
	{
		typedef float(__thiscall* CalculateMaxSpeedFn)(CBaseEntity*, bool);
		static DWORD dwFn = g_Pattern.Find(_(L"client.dll"), _(L"E8 ? ? ? ? D9 96 ? ? ? ? D9 EE DB F1")) + 0x1;
		static DWORD dwEstimate = ((*(PDWORD)(dwFn)) + dwFn + 0x4);
		CalculateMaxSpeedFn maxSpeed = (CalculateMaxSpeedFn)dwEstimate;
		return maxSpeed(this, bIgnoreSpecialAbility);
	}

	__inline CBaseCombatWeapon* GetWeaponFromSlot(const int nSlot) {
		static DWORD dwMyWeapons = g_NetVars.get_offset(_("DT_BaseCombatCharacter"), _("m_hMyWeapons"));
		int hWeapon = *reinterpret_cast<int*>(this + (dwMyWeapons + (nSlot * 0x4)));
		return reinterpret_cast<CBaseCombatWeapon*>(g_Interfaces.EntityList->GetClientEntityFromHandle(hWeapon));
	}

	__inline bool InCond(int eCond)
	{
		using fn = bool(__thiscall*)(CBaseEntity*, int);
		static fn FN = reinterpret_cast<fn>(g_Pattern.Find(L"clientt.dll", L"55 8B EC 83 EC 08 56 57 8B 7D 08 8B F1 83 FF 20"));
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
		static DWORD dwOffset = g_NetVars.get_offset(_("DT_BaseAnimating"), _("m_flPoseParameter"));
		*reinterpret_cast<std::array<float, MAXSTUDIOPOSEPARAM>*>(this + dwOffset) = param;
	}

	__inline std::array<float, MAXSTUDIOPOSEPARAM> GetPoseParam() {
		static DWORD dwOffset = g_NetVars.get_offset(_("DT_BaseAnimating"), _("m_flPoseParameter"));
		return *reinterpret_cast<std::array<float, MAXSTUDIOPOSEPARAM>*>(this + dwOffset);
	}

	__inline bool GetHitboxMinsAndMaxsAndMatrix(const int nHitbox, Vec3& vMins, Vec3& vMaxs, matrix3x4& matrix, Vec3* vCenter) {
		if (const auto& pModel = GetModel()) {
			if (const auto& pHdr = g_Interfaces.ModelInfo->GetStudioModel(pModel)) {
				matrix3x4 BoneMatrix[128];
				if (SetupBones(BoneMatrix, 128, 0x100, g_Interfaces.GlobalVars->curtime)) {
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
			if (const auto& pHdr = g_Interfaces.ModelInfo->GetStudioModel(pModel)) {
				matrix3x4 BoneMatrix[128];
				if (SetupBones(BoneMatrix, 128, 0x100, g_Interfaces.GlobalVars->curtime)) {
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
		return !(nCond & TFCond_Ubercharged || nCond & TFCond_Bonked);
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
			nCondEx & TFCondEx_PyroCrits);
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
			nCondEx & TFCondEx_PyroCrits);
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

	__inline Vec3 GetHitboxPos(const int nHitbox) {
		if (const auto& pModel = GetModel()) {
			if (const auto& pHdr = g_Interfaces.ModelInfo->GetStudioModel(pModel)) {
				matrix3x4 BoneMatrix[128];
				if (SetupBones(BoneMatrix, 128, 0x100, g_Interfaces.GlobalVars->curtime)) {
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
		if (SetupBones(BoneMatrix, 128, 0x100, g_Interfaces.GlobalVars->curtime))
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
		static auto FN = reinterpret_cast<void(__thiscall*)(CBaseEntity*, Vec3&)>(g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC ? 56 8B F1 E8 ? ? ? ? 3B F0")));
		Vec3 v;
		FN(this, v);
		return v;
	}

	__inline Vec3 GetShootPos() {
		return  GetVecOrigin() + GetViewOffset();
	}

	__inline int GetNumOfHitboxes() {
		if (const auto& pModel = GetModel()) {
			if (const auto& pHdr = g_Interfaces.ModelInfo->GetStudioModel(pModel)) {
				if (const auto& pSet = pHdr->GetHitboxSet(GetHitboxSet()))
					return pSet->numhitboxes;
			}
		}

		return 0;
	}

	__inline int GetCritMult() {
		DYNVAR(m_iCritMult, int, _("DT_TFPlayer"), _("m_Shared"), _("m_iCritMult"));
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

	__inline const char* GetModelName() {
		return g_Interfaces.ModelInfo->GetModelName(GetModel());
	}

	__inline void SetTickBase(const int nTickBase) {
		DYNVAR_SET(int, this, nTickBase, _("DT_BasePlayer"), _("localdata"), _("m_nTickBase"));
	}

	__inline void UpTickBase() {
		static CDynamicNetvar<int> n(_("DT_BasePlayer"), _("localdata"), _("m_nTickBase"));
		n.SetValue(this, n.GetValue(this) + 1);
	}
	__inline void DownTickBase() {
		static CDynamicNetvar<int> n(_("DT_BasePlayer"), _("localdata"), _("m_nTickBase"));
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
		static DWORD dwFN = g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B 7D 08 F3 0F 10 07"));
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
		DYNVAR_SET(CUserCmd*, (this - 0x4), pCmd, _("DT_BasePlayer"), _("localdata"), _("m_hConstraintEntity"));
	}

	__inline void SetVecOrigin(const Vec3& vOrigin) {
		DYNVAR_SET(Vec3, this, vOrigin, _("DT_BaseEntity"), _("m_vecOrigin"));
	}

	__inline void RemoveEffect(const BYTE Effect) {
		*reinterpret_cast<byte*>(this + 0x7C) &= ~Effect;

		if (Effect == EF_NODRAW) {
			static auto AddToLeafSystemFn = reinterpret_cast<int(__thiscall*)(PVOID, int)>(g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 56 FF 75 08 8B F1 B8")));

			if (AddToLeafSystemFn)
				AddToLeafSystemFn(this, RENDER_GROUP_OPAQUE_ENTITY);
		}
	}

	__inline void SetGlowEnabled(const bool bState) {
		DYNVAR_SET(bool, this, bState, _("DT_TFPlayer"), _("m_bGlowEnabled"));
	}

	__inline void SetCond(const int nCond) {
		DYNVAR_SET(int, this, nCond, _("DT_TFPlayer"), _("m_Shared"), _("m_nPlayerCond"));
	}

	__inline void SetFov(const int nFov) {
		DYNVAR_SET(int, this, nFov, _("DT_BasePlayer"), _("m_iFOV"));
	}

	__inline void ForceTauntCam(const int nTo) {
		DYNVAR_SET(int, this, nTo, _("DT_TFPlayer"), _("m_nForceTauntCam"));
	}

	__inline void SetFlags(const int nFlags) {
		DYNVAR_SET(int, this, nFlags, _("DT_BasePlayer"), _("m_fFlags"));
	}

	__inline void AddCond(const int nCond) {
		static DWORD offset = g_NetVars.get_offset(_("DT_TFPlayer"), _("m_Shared"), _("m_nPlayerCond"));
		*reinterpret_cast<DWORD*>(this + offset) |= nCond;
	}

	__inline void AddCondEx(const int nCond) {
		static DWORD offset = g_NetVars.get_offset(_("DT_TFPlayer"), _("m_Shared"), _("m_nPlayerCondEx"));
		*reinterpret_cast<DWORD*>(this + offset) |= nCond;
	}

	__inline void RemoveCond(const int nCond) {
		static DWORD offset = g_NetVars.get_offset(_("DT_TFPlayer"), _("m_Shared"), _("m_nPlayerCond"));
		*reinterpret_cast<DWORD*>(this + offset) &= ~nCond;
	}

	__inline void RemoveCondEx(const int nCond) {
		static DWORD offset = g_NetVars.get_offset(_("DT_TFPlayer"), _("m_Shared"), _("m_nPlayerCondEx"));
		*reinterpret_cast<DWORD*>(this + offset) &= ~nCond;
	}

	__inline void SetEyeAngles(const Vec3& vAngles) {
		DYNVAR_SET(Vec3, this, vAngles, _("DT_TFPlayer"), _("tfnonlocaldata"), _("m_angEyeAngles[0]"));
	}

	__inline void IncreaseTickBase() {
		static CDynamicNetvar<int>n(_("DT_BasePlayer"), _("localdata"), _("m_nTickBase"));
		n.SetValue(this, n.GetValue(this) + 1);
	}

	__inline void SetAnimTime(const float flAnimTime) {
		DYNVAR_SET(float, this, flAnimTime, _("DT_BaseEntity"), _("m_flAnimTime"));
	}

	/*
	__inline CCollisionProperty* GetCollision() {
		return *reinterpret_cast<CCollisionProperty*>(this + 0x1C8);
	}
	*/ // why push this when A) It doesn't work, and B) Isn't used

};
