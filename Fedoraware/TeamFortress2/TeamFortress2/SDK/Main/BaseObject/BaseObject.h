#pragma once

#include "../BaseEntity/BaseEntity.h"

class CBaseObject : public CBaseEntity
{
public: //Netvars
	M_DYNVARGET(Controlled, bool, this, "DT_ObjectSentrygun", "m_bPlayerControlled")
		M_DYNVARGET(Constructing, bool, this, "DT_BaseObject", "m_bBuilding")
		M_DYNVARGET(MiniBuilding, bool, this, "DT_BaseObject", "m_bMiniBuilding")
		M_DYNVARGET(ObjectMode, int, this, "DT_BaseObject", "m_iObjectMode")
		M_DYNVARGET(MapPlaced, bool, this, "DT_BaseObject", "m_bWasMapPlaced")
		M_DYNVARGET(Sapped, bool, this, "DT_BaseObject", "m_bHasSapper")
		M_DYNVARGET(PlasmaDisabled, bool, this, "DT_BaseObject", "m_bPlasmaDisable") //cowmangler's secondary attack I think
		M_DYNVARGET(Disabled, bool, this, "DT_BaseObject", "m_bDisabled")
		M_DYNVARGET(Carried, bool, this, "DT_BaseObject", "m_bCarried")
		M_DYNVARGET(hBuilder, int, this, "DT_BaseObject", "m_hBuilder")
		M_DYNVARGET(Level, int, this, "DT_BaseObject", "m_iUpgradeLevel")
		M_DYNVARGET(Health, int, this, "DT_BaseObject", "m_iHealth")
		M_DYNVARGET(MaxHealth, int, this, "DT_BaseObject", "m_iMaxHealth")
		M_DYNVARGET(Rockets, int, this, "DT_ObjectSentrygun", "m_iAmmoRockets")
		M_DYNVARGET(HighestLevel, int, this, "DT_BaseObject", "m_iHighestUpgradeLevel")
		M_DYNVARGET(Type, int, this, "DT_BaseObject", "m_iObjectType")
		M_DYNVARGET(Metal, int, this, "DT_ObjectDispenser", "m_iAmmoMetal")
		M_DYNVARGET(Ammo, int, this, "DT_ObjectSentrygun", "m_iAmmoShells")
		M_DYNVARGET(Constructed, float, this, "DT_BaseObject", "m_flPercentageConstructed")

		M_OFFSETGET(RechargeTime, float, 0xFC0)
		M_OFFSETGET(CurrentChargeDuration, float, 0xFC4)
		M_OFFSETGET(YawToExit, float, 0xFCC)

public: //Everything else, lol
	__inline CBaseEntity* GetOwner()
	{
		DYNVAR(hBuilder, int, "DT_BaseObject", "m_hBuilder");
		return I::ClientEntityList->GetClientEntityFromHandle(hBuilder.GetValue(this));
	}

	//Pasted from https://github.com/Lak3/tf2-internal-base
	inline int MaxAmmoShells()
	{
		if ((GetLevel() == 1) || GetMiniBuilding())
			return 150;
		else
			return 200;
	}
	inline void GetAmmoCount(int& iShells, int& iMaxShells, int& iRockets, int& iMaxRockets)
	{
		const bool bIsMini = GetMiniBuilding();

		iShells = GetAmmo();
		iMaxShells = MaxAmmoShells();
		iRockets = bIsMini ? 0 : GetRockets();
		iMaxRockets = (bIsMini || (GetLevel() < 3)) ? 0 : 20;
	}
};