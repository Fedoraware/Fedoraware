#pragma once
#include "../../NetVars/NetVars.h"

#include "AnimState/TFPlayerAnimState.h" //includes class C_BaseCombatWeapon

#define M_VIRTUALGET(name, type, base, fn, index) __inline type Get##name() \
{ \
	void* pBase = base; \
	return GetVFunc<fn>(pBase, index)(pBase); \
}

namespace S
{
	MAKE_SIGNATURE(TeamFortress_CalculateMaxSpeed, CLIENT_DLL, "55 8B EC 83 EC ? 83 3D ? ? ? ? ? 56 8B F1 75", 0x0);

	MAKE_SIGNATURE(CBaseEntity_GetAmmoCount, CLIENT_DLL, "55 8B EC 56 8B 75 ? 57 8B F9 83 FE ? 75 ? 5F", 0x0);
	MAKE_SIGNATURE(CBaseEntity_GetVelocity, CLIENT_DLL, "55 8B EC 83 EC ? 56 8B F1 E8 ? ? ? ? 3B F0", 0x0);
	MAKE_SIGNATURE(CBaseEntity_SetAbsOrigin, CLIENT_DLL, "55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B 7D ? F3 0F 10 07", 0x0);
	MAKE_SIGNATURE(CBaseEntity_PostDataUpdate, CLIENT_DLL, "55 8B EC 83 EC ? 53 8B 5D ? 56 57 8B F9 85 DB 75 ? 8B 0D", 0x0);
	MAKE_SIGNATURE(CBaseEntity_RemoveEffect, CLIENT_DLL, "55 8B EC 56 FF 75 ? 8B F1 B8", 0x0);
	MAKE_SIGNATURE(CBaseEntity_UpdateButtonState, CLIENT_DLL, "55 8B EC 8B 81 ? ? ? ? 8B D0", 0x0);
	MAKE_SIGNATURE(CBaseEntity_SetNextThink, CLIENT_DLL, "55 8B EC F3 0F 10 45 ? 0F 2E 05 ? ? ? ? 53", 0x0);
	MAKE_SIGNATURE(CBaseEntity_GetNextThinkTick, CLIENT_DLL, "55 8B EC 8B 45 ? 56 8B F1 85 C0 75 ? 8B 86", 0x0);
	MAKE_SIGNATURE(CBaseEntity_PhysicsRunThink, CLIENT_DLL, "55 8B EC 53 8B D9 56 57 8B 83 ? ? ? ? C1 E8", 0x0);
}

enum ShouldTransmitState_t
{
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
		M_DYNVARGET(CondEx, int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCondEx")
		M_DYNVARGET(CondEx2, int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCondEx2")
		M_DYNVARGET(CondEx3, int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCondEx3")
		M_DYNVARGET(CondEx4, int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCondEx4")
		M_DYNVARGET(CondBits, int, this, "DT_TFPlayer", "m_Shared", "m_ConditionList", "_condition_bits")
		M_DYNVARGET(ViewingCYOAPDA, bool, this, "DT_TFPlayer", "m_bViewingCYOAPDA")
		M_DYNVARGET(UsingActionSlot, bool, this, "DT_TFPlayer", "m_bUsingActionSlot")
		M_DYNVARGET(LoadoutUnavailable, bool, this, "DT_TFPlayer", "m_Shared", "m_bLoadoutUnavailable")
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
		M_DYNVARGET(TargetPlayer, int, this, "DT_CHalloweenGiftPickup", "m_hTargetPlayer")

		M_OFFSETGET(PipebombType, int, 0x8F8)												
		M_OFFSETGET(PipebombPulsed, bool, 0x908)	//	this is incredibly fucking lazy.
		M_OFFSETGET(Touched, bool, 0x8FC)	
		M_OFFSETGET(PunchAngles, Vec3, 0xE8C)
		M_OFFSETGET(VecVelocity, Vec3, 0x120)
		M_OFFSETGET(WaterJumpTime, float, 0x10FC)
		M_OFFSETGET(SurfaceFriction, float, 0x12D4)
		M_OFFSETGET(MoveType, MoveType_t, 0x1A4)

		M_FLAGGET(OnGround, GetFlags(), FL_ONGROUND)
		M_FLAGGET(InWater, GetFlags(), FL_INWATER)
		M_FLAGGET(Ducking, GetFlags(), FL_DUCKING)
		M_CONDGET(Charging, InCond, TF_COND_SHIELD_CHARGE)
		M_CONDGET(Scoped, InCond, TF_COND_ZOOMED)
		M_CONDGET(Ubered, InCond, TF_COND_INVULNERABLE)
		M_CONDGET(Bonked, InCond, TF_COND_PHASE)
		M_CONDGET(InMilk, InCond, TF_COND_MAD_MILK)
		M_CONDGET(InJarate, InCond, TF_COND_URINE)
		M_CONDGET(Bleeding, InCond, TF_COND_BLEEDING)
		M_CONDGET(Disguised, InCond, TF_COND_DISGUISED)
		M_CONDGET(Cloaked, InCond, TF_COND_STEALTHED)
		M_CONDGET(Taunting, InCond, TF_COND_TAUNTING)
		M_CONDGET(OnFire, InCond, TF_COND_BURNING)
		M_CONDGET(Stunned, InCond, TF_COND_STUNNED)
		M_CONDGET(Slowed, InCond, TF_COND_AIMING)
		M_CONDGET(MegaHealed, InCond, TF_COND_MEGAHEAL)
		M_CONDGET(AGhost, InCond, TF_COND_HALLOWEEN_GHOST_MODE)
		M_CONDGET(InBumperKart, InCond, TF_COND_HALLOWEEN_KART)
		M_CONDGET(PhlogUbered, InCond, TF_COND_CRITBOOSTED_RAGE_BUFF)
		M_CONDGET(BlastImmune, InCond, TF_COND_BLAST_IMMUNE)
		M_CONDGET(BulletImmune, InCond, TF_COND_BULLET_IMMUNE)
		M_CONDGET(FireImmune, InCond, TF_COND_FIRE_IMMUNE)
		M_CONDGET(StrengthRune, InCond, TF_COND_RUNE_STRENGTH)
		M_CONDGET(HasteRune, InCond, TF_COND_RUNE_HASTE)
		M_CONDGET(RegenRune, InCond, TF_COND_RUNE_REGEN)
		M_CONDGET(ResistRune, InCond, TF_COND_RUNE_RESIST)
		M_CONDGET(VampireRune, InCond, TF_COND_RUNE_VAMPIRE)
		M_CONDGET(ReflectRune, InCond, TF_COND_RUNE_REFLECT)
		M_CONDGET(PrecisionRune, InCond, TF_COND_RUNE_PRECISION)
		M_CONDGET(AgilityRune, InCond, TF_COND_RUNE_AGILITY)
		M_CONDGET(KnockoutRune, InCond, TF_COND_RUNE_KNOCKOUT)
		M_CONDGET(ImbalanceRune, InCond, TF_COND_RUNE_IMBALANCE)
		M_CONDGET(CritTempRune, InCond, TF_COND_CRITBOOSTED_RUNE_TEMP)
		M_CONDGET(KingRune, InCond, TF_COND_RUNE_KING)
		M_CONDGET(PlagueRune, InCond, TF_COND_RUNE_PLAGUE)
		M_CONDGET(SupernovaRune, InCond, TF_COND_RUNE_SUPERNOVA)
		M_CONDGET(BuffedByKing, InCond, TF_COND_KING_BUFFED)
		M_CONDGET(BlastResist, InCond, TF_COND_MEDIGUN_UBER_BLAST_RESIST)
		M_CONDGET(BulletResist, InCond, TF_COND_MEDIGUN_UBER_BULLET_RESIST)
		M_CONDGET(FireResist, InCond, TF_COND_MEDIGUN_UBER_FIRE_RESIST)

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
		NETVAR(m_ubInterpolationFrame, byte, "CBasePlayer", "m_ubInterpolationFrame")
		NETVAR(m_ubOldInterpolationFrame, byte, "CBasePlayer", "m_ubInterpolationFrame" + 0x1)
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

		__inline void GetRenderBounds(Vec3& vMins, Vec3& vMaxs)
	{
		const auto pRend = Renderable();
		GetVFunc<void(__thiscall*)(void*, Vec3&, Vec3&)>(pRend, 20)(pRend, vMins, vMaxs);
	}

	__inline bool SetupBones(matrix3x4* pOut, int nMax, int nMask, float flTime)
	{
		const auto pRend = Renderable();
		return GetVFunc<bool(__thiscall*)(void*, matrix3x4*, int, int, float)>(pRend, 16)(pRend, pOut, nMax, nMask, flTime);
	}

	__inline int DrawModel(int nFlags)
	{
		const auto pRend = Renderable();
		return GetVFunc<int(__thiscall*)(void*, int)>(pRend, 10)(pRend, nFlags);
	}

public: //Virtuals from networkable
	__inline IClientNetworkable* Networkable() { return reinterpret_cast<IClientNetworkable*>((reinterpret_cast<uintptr_t>(this) + 0x8)); }

	M_VIRTUALGET(ClientClass, CClientClass*, Networkable(), CClientClass* (__thiscall*)(void*), 2)
		M_VIRTUALGET(Dormant, bool, Networkable(), bool(__thiscall*)(void*), 8)
		M_VIRTUALGET(Index, int, Networkable(), int(__thiscall*)(void*), 9)

public: //Everything else, lol.
	//Credits to KGB
	__inline bool InCond(const ETFCond cond)
	{
		const int iCond = static_cast<int>(cond);

		switch (iCond / 32)
		{
		case 0:
		{
			const int bit = (1 << iCond);
			if ((GetCond() & bit) == bit)
			{
				return true;
			}

			if ((GetCondBits() & bit) == bit)
			{
				return true;
			}

			break;
		}
		case 1:
		{
			const int bit = 1 << (iCond - 32);
			if ((GetCondEx() & bit) == bit)
			{
				return true;
			}

			break;
		}
		case 2:
		{
			const int bit = 1 << (iCond - 64);
			if ((GetCondEx2() & bit) == bit)
			{
				return true;
			}

			break;
		}
		case 3:
		{
			const int bit = 1 << (iCond - 96);
			if ((GetCondEx3() & bit) == bit)
			{
				return true;
			}

			break;
		}
		case 4:
		{
			const int bit = 1 << (iCond - 128);
			if ((GetCondEx4() & bit) == bit)
			{
				return true;
			}

			break;
		}
		default:
			break;
		}

		return false;
	}

	__inline size_t* GetMyWeapons()
	{
		static auto dwOff = g_NetVars.get_offset("DT_BaseCombatCharacter", "m_hMyWeapons");
		return reinterpret_cast<size_t*>(this + dwOff);
	}

	__inline bool* PDAman()
	{
		static auto dwOff = g_NetVars.get_offset("DT_TFPlayer", "m_bViewingCYOAPDA");
		return reinterpret_cast<bool*>(this + dwOff);
		//M_DYNVARGET(ViewingCYOAPDA, bool, this, "DT_TFPlayer", "m_bViewingCYOAPDA")
	}

	__inline float* GetHeadScale()
	{
		static auto dwOff = g_NetVars.get_offset("DT_TFPlayer", "m_flHeadScale");
		return reinterpret_cast<float*>(this + dwOff);
	}
	__inline float* GetTorsoScale()
	{
		static auto dwOff = g_NetVars.get_offset("DT_TFPlayer", "m_flTorsoScale");
		return reinterpret_cast<float*>(this + dwOff);
	}
	__inline float* GetHandScale()
	{
		static auto dwOff = g_NetVars.get_offset("DT_TFPlayer", "m_flHandScale");
		return reinterpret_cast<float*>(this + dwOff);
	}

	int GetAmmoCount(int iAmmoIndex)
	{
		static auto fnGetAmmoCount = S::CBaseEntity_GetAmmoCount.As<int(__thiscall*)(CBaseEntity*, int)>();
		return fnGetAmmoCount(this, iAmmoIndex);
	}


	__inline CBaseEntity* GetGroundEntity()
	{
		DYNVAR(pHandle, int, "DT_BasePlayer", "m_hGroundEntity");
		return I::ClientEntityList->GetClientEntityFromHandle(pHandle.GetValue(this));
	}

	__inline CBaseEntity* FirstMoveChild()
	{
		return I::ClientEntityList->GetClientEntity(*reinterpret_cast<int*>(this + 0x1B0) & 0xFFF);
	}

	__inline CBaseEntity* NextMovePeer()
	{
		return I::ClientEntityList->GetClientEntity(*reinterpret_cast<int*>(this + 0x1B4) & 0xFFF);
	}

	__inline CBaseCombatWeapon* GetActiveWeapon()
	{
		DYNVAR(pHandle, int, "DT_BaseCombatCharacter", "m_hActiveWeapon");
		return reinterpret_cast<CBaseCombatWeapon*>(I::ClientEntityList->GetClientEntityFromHandle(pHandle.GetValue(this)));
	}

	inline float TeamFortress_CalculateMaxSpeed(bool bIgnoreSpecialAbility = false)
	{
		using FN = float(__thiscall*)(CBaseEntity*, bool);
		static FN fnMaxSpeed = S::TeamFortress_CalculateMaxSpeed.As<FN>();
		return fnMaxSpeed(this, bIgnoreSpecialAbility);
	}

	__inline CBaseCombatWeapon* GetWeaponFromSlot(const int nSlot)
	{
		static DWORD dwMyWeapons = g_NetVars.get_offset("DT_BaseCombatCharacter", "m_hMyWeapons");
		const int hWeapon = *reinterpret_cast<int*>(this + (dwMyWeapons + (nSlot * 0x4)));
		return reinterpret_cast<CBaseCombatWeapon*>(I::ClientEntityList->GetClientEntityFromHandle(hWeapon));
	}

	__inline ETFClassID GetClassID()
	{
		const auto& pCC = GetClientClass();
		return pCC ? ETFClassID(pCC->m_ClassID) : ETFClassID(0);
	}

	__inline CTFPlayerAnimState* GetAnimState()
	{
		return *reinterpret_cast<CTFPlayerAnimState**>(this + 0x1D88);
	}

	__inline void SetPoseParam(std::array<float, MAXSTUDIOPOSEPARAM> param)
	{
		static DWORD dwOffset = g_NetVars.get_offset("DT_BaseAnimating", "m_flPoseParameter");
		*reinterpret_cast<std::array<float, MAXSTUDIOPOSEPARAM>*>(this + dwOffset) = param;
	}

	__inline std::array<float, MAXSTUDIOPOSEPARAM> GetPoseParam()
	{
		static DWORD dwOffset = g_NetVars.get_offset("DT_BaseAnimating", "m_flPoseParameter");
		return *reinterpret_cast<std::array<float, MAXSTUDIOPOSEPARAM>*>(this + dwOffset);
	}

	__inline bool GetHitboxMinsAndMaxsAndMatrix(const int nHitbox, Vec3& vMins, Vec3& vMaxs, matrix3x4& matrix, Vec3* vCenter)
	{
		if (const auto& pModel = GetModel())
		{
			if (const auto& pHdr = I::ModelInfoClient->GetStudioModel(pModel))
			{
				matrix3x4 BoneMatrix[128];
				if (SetupBones(BoneMatrix, 128, 0x100, I::GlobalVars->curtime))
				{
					if (const auto& pSet = pHdr->GetHitboxSet(GetHitboxSet()))
					{
						if (const auto& pBox = pSet->hitbox(nHitbox))
						{
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

	__inline bool GetHitboxMinsAndMaxsFromMatrix(const int nHitbox, Vec3& vMins, Vec3& vMaxs, matrix3x4* matrix, Vec3* vCenter)
	{
		if (const auto& pModel = GetModel())
		{
			if (const auto& pHdr = I::ModelInfoClient->GetStudioModel(pModel))
			{
				if (const auto& pSet = pHdr->GetHitboxSet(GetHitboxSet()))
				{
					if (const auto& pBox = pSet->hitbox(nHitbox))
					{
						vMins = pBox->bbmin; vMaxs = pBox->bbmax;
						if (vCenter) Math::VectorTransform(((pBox->bbmin + pBox->bbmax) * 0.5f), matrix[pBox->bone], *vCenter);
						return true;
					}
				}
			}
		}
		return false;
	}

	__inline bool GetHitboxMinsAndMaxs(const int nHitbox, Vec3& vMins, Vec3& vMaxs, Vec3* vCenter)
	{
		if (const auto& pModel = GetModel())
		{
			if (const auto& pHdr = I::ModelInfoClient->GetStudioModel(pModel))
			{
				matrix3x4 BoneMatrix[128];
				if (SetupBones(BoneMatrix, 128, 0x100, I::GlobalVars->curtime))
				{
					if (const auto& pSet = pHdr->GetHitboxSet(GetHitboxSet()))
					{
						if (const auto& pBox = pSet->hitbox(nHitbox))
						{
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

	__inline bool IsClass(const int nClass)
	{
		return (GetClassNum() == nClass);
	}

	__inline bool IsSentrygun()
	{
		return GetClassID() == ETFClassID::CObjectSentrygun;
	}

	__inline bool IsDispenser()
	{
		return GetClassID() == ETFClassID::CObjectDispenser;
	}

	__inline bool IsTeleporter()
	{
		return GetClassID() == ETFClassID::CObjectTeleporter;
	}

	__inline bool IsInValidTeam()
	{
		const int nTeam = GetTeamNum();
		return (nTeam == 2 || nTeam == 3);
	}

	__inline bool IsSwimming()
	{
		return (GetWaterLevel() > 1);
	}

	__inline bool IsAlive()
	{
		return (GetLifeState() == LIFE_ALIVE);
	}

	__inline bool IsBaseCombatWeapon()
	{
		return (GetClassID() == ETFClassID::CBaseCombatWeapon);
	}

	__inline bool IsWearable()
	{
		return (GetClassID() == ETFClassID::CTFWearable);
	}

	__inline bool IsVulnerable()
	{
		return !(InCond(TF_COND_INVULNERABLE)
			|| InCond(TF_COND_INVULNERABLE_CARD_EFFECT)
			|| InCond(TF_COND_INVULNERABLE_HIDE_UNLESS_DAMAGED)
			|| InCond(TF_COND_INVULNERABLE_USER_BUFF)
			|| InCond(TF_COND_PHASE));
	}

	__inline bool IsVisible()
	{
		if (InCond(TF_COND_MAD_MILK) || InCond(TF_COND_URINE) || InCond(TF_COND_BURNING) || InCond(TF_COND_STEALTHED_BLINK) || InCond(TF_COND_BLEEDING))
			return false;

		return InCond(TF_COND_STEALTHED);
	}

	__inline bool IsCritBoosted()
	{
		return InCond(TF_COND_CRITBOOSTED)
			|| InCond(TF_COND_CRITBOOSTED_BONUS_TIME)
			|| InCond(TF_COND_CRITBOOSTED_CARD_EFFECT)
			|| InCond(TF_COND_CRITBOOSTED_CTF_CAPTURE)
			|| InCond(TF_COND_CRITBOOSTED_FIRST_BLOOD)
			|| InCond(TF_COND_CRITBOOSTED_ON_KILL)
			|| InCond(TF_COND_CRITBOOSTED_PUMPKIN)
			|| InCond(TF_COND_CRITBOOSTED_RAGE_BUFF)
			|| InCond(TF_COND_CRITBOOSTED_RUNE_TEMP)
			|| InCond(TF_COND_CRITBOOSTED_USER_BUFF)
			|| InCond(TF_COND_MINICRITBOOSTED_ON_KILL)
			|| InCond(TF_COND_NOHEALINGDAMAGEBUFF)
			|| InCond(TF_COND_ENERGY_BUFF);
	}

	__inline bool IsCritBoostedNoMini()
	{
		return InCond(TF_COND_CRITBOOSTED)
			|| InCond(TF_COND_CRITBOOSTED_BONUS_TIME)
			|| InCond(TF_COND_CRITBOOSTED_CARD_EFFECT)
			|| InCond(TF_COND_CRITBOOSTED_CTF_CAPTURE)
			|| InCond(TF_COND_CRITBOOSTED_FIRST_BLOOD)
			|| InCond(TF_COND_CRITBOOSTED_ON_KILL)
			|| InCond(TF_COND_CRITBOOSTED_PUMPKIN)
			|| InCond(TF_COND_CRITBOOSTED_RAGE_BUFF)
			|| InCond(TF_COND_CRITBOOSTED_RUNE_TEMP)
			|| InCond(TF_COND_CRITBOOSTED_USER_BUFF);
	}

	__inline bool IsMiniCritBoosted()
	{
		return InCond(TF_COND_MINICRITBOOSTED_ON_KILL)
			|| InCond(TF_COND_NOHEALINGDAMAGEBUFF)
			|| InCond(TF_COND_ENERGY_BUFF);
	}

	__inline const wchar_t* GetRune()
	{
		if (IsStrengthRune()) { return (L"Strength Rune"); }
		if (IsHasteRune()) { return (L"Haste Rune"); }
		if (IsRegenRune()) { return (L"Regen Rune"); }
		if (IsResistRune()) { return (L"Resist Rune"); }
		if (IsVampireRune()) { return (L"Vampire Rune"); }
		if (IsReflectRune()) { return (L"Reflect Rune"); }
		if (IsPrecisionRune()) { return (L"Precision Rune"); }
		if (IsAgilityRune()) { return (L"Agility Rune"); }
		if (IsKnockoutRune()) { return (L"Knockout Rune"); }
		if (IsImbalanceRune()) { return (L"Imbalance Rune"); }
		if (IsKingRune()) { return (L"King"); }
		if (IsPlagueRune()) { return (L"Plague Rune"); }
		if (IsSupernovaRune()) { return (L"Supernova Rune"); }
		return nullptr;
	}

	__inline bool IsKingBuffed()
	{
		return IsBuffedByKing();
	}

	__inline bool IsPlayer()
	{
		return (GetClassID() == ETFClassID::CTFPlayer);
	}

	__inline bool IsBuilding()
	{
		switch (GetClassID())
		{
			case ETFClassID::CObjectDispenser:
			case ETFClassID::CObjectSentrygun:
			case ETFClassID::CObjectTeleporter: return true;
			default: return false;
		}
	}

	__inline bool IsPickup()
	{
		switch (GetClassID())
		{
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

	__inline bool IsCredits()
	{
		return GetClassID() == ETFClassID::CCurrencyPack;
	}

	__inline bool IsGargoyle()
	{
		return GetClassID() == ETFClassID::CHalloweenGiftPickup;
	}

	__inline Vec3 GetHitboxPos(const int nHitbox)
	{
		if (const auto& pModel = GetModel())
		{
			if (const auto& pHdr = I::ModelInfoClient->GetStudioModel(pModel))
			{
				matrix3x4 BoneMatrix[128];
				if (SetupBones(BoneMatrix, 128, 0x100, I::GlobalVars->curtime))
				{
					if (const auto& pSet = pHdr->GetHitboxSet(GetHitboxSet()))
					{
						if (const auto& pBox = pSet->hitbox(nHitbox))
						{
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


	__inline Vec3 GetHitboxPosMatrix(const int nHitbox, matrix3x4 BoneMatrix[128])
	{
		if (const auto& pModel = GetModel())
		{
			if (const auto& pHdr = I::ModelInfoClient->GetStudioModel(pModel))
			{
				if (const auto& pSet = pHdr->GetHitboxSet(GetHitboxSet()))
				{
					if (const auto& pBox = pSet->hitbox(nHitbox))
					{
						Vec3 vPos = (pBox->bbmin + pBox->bbmax) * 0.5f, vOut;
						Math::VectorTransform(vPos, BoneMatrix[pBox->bone], vOut);
						return vOut;
					}
				}
			}
		}

		return Vec3();
	}

	__inline Vec3 GetBonePos(const int nBone)
	{
		matrix3x4 BoneMatrix[128];
		if (SetupBones(BoneMatrix, 128, 0x100, I::GlobalVars->curtime))
			return Vec3(BoneMatrix[nBone][0][3], BoneMatrix[nBone][1][3], BoneMatrix[nBone][2][3]);

		return Vec3(0.0f, 0.0f, 0.0f);
	}

	__inline Vec3 GetEyePosition()
	{
		return (GetViewOffset() + GetAbsOrigin());
	}

	__inline Vec3 GetWorldSpaceCenter()
	{
		const Vec3 vMax = this->GetCollideableMaxs();
		Vec3 vWorldSpaceCenter = GetAbsOrigin();
		vWorldSpaceCenter.z += (vMax.z / 2.0f);
		return vWorldSpaceCenter;
	}

	// TODO: This is a duplicate of EstimateAbsVelocity...
	__inline Vec3 GetVelocity()
	{
		static auto fnGetVelocity = S::CBaseEntity_GetVelocity.As<void(__thiscall*)(CBaseEntity*, Vec3&)>();
		Vec3 out;
		fnGetVelocity(this, out);
		return out;
	}

	// This does not return your actual shoot pos for projectile weapons! Use Utils::GetRealShootPos(...) instead
	__inline Vec3 GetShootPos()
	{
		return  GetVecOrigin() + GetViewOffset();
	}

	__inline int GetNumOfHitboxes()
	{
		if (const auto& pModel = GetModel())
		{
			if (const auto& pHdr = I::ModelInfoClient->GetStudioModel(pModel))
			{
				if (const auto& pSet = pHdr->GetHitboxSet(GetHitboxSet()))
					return pSet->numhitboxes;
			}
		}

		return 0;
	}

	__inline int GetCritMult()
	{
		DYNVAR(m_iCritMult, int, "DT_TFPlayer", "m_Shared", "m_iCritMult");
		return Math::RemapValClamped(m_iCritMult.GetValue(this), 0.0f, 255.0f, 1.0f, 4.0f);
	}

	__inline float GetPlayerMaxVelocity()
	{
		switch (GetClassNum())
		{
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

	__inline bool OnSolid()
	{
		return m_hGroundEntity() >= 0 || IsOnGround();
	}

	__inline const char* GetModelName()
	{
		return I::ModelInfoClient->GetModelName(GetModel());
	}

	__inline void SetTickBase(const int nTickBase)
	{
		DYNVAR_SET(int, this, nTickBase, "DT_BasePlayer", "localdata", "m_nTickBase");
	}

	__inline void UpTickBase()
	{
		static CDynamicNetvar<int> n("DT_BasePlayer", "localdata", "m_nTickBase");
		n.SetValue(this, n.GetValue(this) + 1);
	}
	__inline void DownTickBase()
	{
		static CDynamicNetvar<int> n("DT_BasePlayer", "localdata", "m_nTickBase");
		n.SetValue(this, n.GetValue(this) - 1);
	}

	__inline void ClearPunchAngle()
	{ //m_vecPunchAngle
		*reinterpret_cast<Vec3*>(this + 0xE8C) = Vec3(0.0f, 0.0f, 0.0f);
	}

	__inline void SetRenderAngles(const Vec3& vAngles)
	{
		Vec3* pRenderAngles = const_cast<Vec3*>(&this->GetRenderAngles());
		*pRenderAngles = vAngles;
	}

	__inline void SetAbsOrigin(const Vec3& vOrigin)
	{
		using FN = void(__thiscall*)(CBaseEntity*, const Vec3&);
		static FN fnSetAbsOrigin = S::CBaseEntity_SetAbsOrigin.As<FN>();
		fnSetAbsOrigin(this, vOrigin);
	}

	__inline void PostDataUpdate(int updateType)
	{
		using FN = void(__thiscall*)(CBaseEntity*, int);
		static FN fnPostDataUpdate = S::CBaseEntity_PostDataUpdate.As<FN>();
		fnPostDataUpdate(this, updateType);
	}

	__inline void SetAbsAngles(const Vec3& vAngles)
	{
		Vec3* pAbsAngles = const_cast<Vec3*>(&this->GetAbsAngles());
		*pAbsAngles = vAngles;
	}

	__inline void SetCurrentCmd(CUserCmd* pCmd)
	{
		DYNVAR_SET(CUserCmd*, (this - 0x4), pCmd, "DT_BasePlayer", "localdata", "m_hConstraintEntity");
	}

	__inline void SetVecOrigin(const Vec3& vOrigin)
	{
		DYNVAR_SET(Vec3, this, vOrigin, "DT_BaseEntity", "m_vecOrigin");
	}

	__inline void RemoveEffect(const BYTE Effect)
	{
		*reinterpret_cast<byte*>(this + 0x7C) &= ~Effect;

		if (Effect == EF_NODRAW)
		{
			static auto fnAddToLeafSystem = S::CBaseEntity_RemoveEffect.As<int(__thiscall*)(PVOID, int)>();

			if (fnAddToLeafSystem)
			{
				fnAddToLeafSystem(this, RENDER_GROUP_OPAQUE_ENTITY);
			}
		}
	}

	__inline void SetGlowEnabled(const bool bState)
	{
		DYNVAR_SET(bool, this, bState, "DT_TFPlayer", "m_bGlowEnabled");
	}

	__inline void SetCond(const int nCond)
	{
		DYNVAR_SET(int, this, nCond, "DT_TFPlayer", "m_Shared", "m_nPlayerCond");
	}

	__inline void SetFov(const int nFov)
	{
		DYNVAR_SET(int, this, nFov, "DT_BasePlayer", "m_iFOV");
	}

	__inline void ForceTauntCam(const int nTo)
	{
		DYNVAR_SET(int, this, nTo, "DT_TFPlayer", "m_nForceTauntCam");
	}

	__inline void SetFlags(const int nFlags)
	{
		DYNVAR_SET(int, this, nFlags, "DT_BasePlayer", "m_fFlags");
	}

	__inline void AddCond(const int nCond)
	{
		static DWORD offset = g_NetVars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCond");
		*reinterpret_cast<DWORD*>(this + offset) |= nCond;
	}

	__inline void AddCondEx(const int nCond)
	{
		static DWORD offset = g_NetVars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx");
		*reinterpret_cast<DWORD*>(this + offset) |= nCond;
	}

	__inline void RemoveCond(const int nCond)
	{
		static DWORD offset = g_NetVars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCond");
		*reinterpret_cast<DWORD*>(this + offset) &= ~nCond;
	}

	__inline void RemoveCondEx(const int nCond)
	{
		static DWORD offset = g_NetVars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCondEx");
		*reinterpret_cast<DWORD*>(this + offset) &= ~nCond;
	}

	__inline void SetEyeAngles(const Vec3& vAngles)
	{
		DYNVAR_SET(Vec3, this, vAngles, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]");
	}

	__inline void IncreaseTickBase()
	{
		static CDynamicNetvar<int>n("DT_BasePlayer", "localdata", "m_nTickBase");
		n.SetValue(this, n.GetValue(this) + 1);
	}

	__inline void SetAnimTime(const float flAnimTime)
	{
		DYNVAR_SET(float, this, flAnimTime, "DT_BaseEntity", "m_flAnimTime");
	}

	__inline void EstimateAbsVelocity(Vec3& vVel)
	{
		static auto fnEstimateABSVelocity = S::CBaseEntity_GetVelocity.As<void(__thiscall*)(void*, Vec3&)>();
		fnEstimateABSVelocity(this, vVel);
	}

	__inline float TickVelocity2D()
	{	//	bad
		const int iDivide = floor(1000.f / I::GlobalVars->interval_per_tick);
		const float flVel = this->m_vecVelocity().Length2D();
		return flVel / iDivide;
	}

	__inline CCollisionProperty* GetCollision() {
		return reinterpret_cast<CCollisionProperty*>(this + 0x1C8);
	}

	__inline void UpdateButtonState(int nUserCmdButtonMask)
	{
		static auto fnUpdateButtonState = S::CBaseEntity_UpdateButtonState.As<void(__thiscall*)(void*, int)>();
		fnUpdateButtonState(this, nUserCmdButtonMask);
	}

	__inline void SetNextThink(float thinkTime, const char* szContext)
	{
		static auto fnSetNextThink = S::CBaseEntity_SetNextThink.As<void(__thiscall*)(void*, float, const char*)>();
		fnSetNextThink(this, thinkTime, szContext);
	}

	__inline int GetNextThinkTick(const char* szContext)
	{
		static auto fnGetNextThinkTick = S::CBaseEntity_GetNextThinkTick.As<int(__thiscall*)(void*, const char*)>();
		return fnGetNextThinkTick(this, szContext);
	}

	__inline bool PhysicsRunThink(int thinkMethod = 0)
	{
		static auto fnPhysicsRunThink = S::CBaseEntity_PhysicsRunThink.As<bool(__thiscall*)(void*, int)>();
		return fnPhysicsRunThink(this, thinkMethod);
	}

	__inline void UpdateClientSideAnimation() {
		static auto fnUpdateClientSideAnimation = S::CBaseAnimating_UpdateClientSideAnimation.As<void(__thiscall*)(CBaseEntity*)>();
		return fnUpdateClientSideAnimation(this);
	}

	__inline float FrameAdvance(float flInterval) {
		static auto fnFrameAdvance = S::CBaseAnimating_FrameAdvance.As<float(__thiscall*)(CBaseEntity*, float)>();
		return fnFrameAdvance(this, flInterval);
	}

	__inline void SelectItem(const char* pstr, int iSubType) {
		typedef bool(__thiscall* FN)(PVOID, const char*, int);
		GetVFunc<FN>(this, 270)(this, pstr, iSubType);
	}

	__inline void PreThink() {
		typedef void(__thiscall* FN)(PVOID);
		GetVFunc<FN>(this, 260)(this);
	}

	__inline void Think() {
		typedef void(__thiscall* FN)(PVOID);
		GetVFunc<FN>(this, 174)(this);
	}

	__inline void PostThink() {
		typedef void(__thiscall* FN)(PVOID);
		GetVFunc<FN>(this, 261)(this);
	}
};
