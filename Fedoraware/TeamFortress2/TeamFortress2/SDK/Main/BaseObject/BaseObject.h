#pragma once

#include "../BaseEntity/BaseEntity.h"

class CBaseObject : public CBaseEntity
{
public: //Netvars
	M_DYNVARGET(Controlled, bool, this, _("DT_ObjectSentrygun"), _("m_bPlayerControlled"))
		M_DYNVARGET(Constructing, bool, this, _("DT_BaseObject"), _("m_bBuilding"))
		M_DYNVARGET(MiniBuilding, bool, this, _("DT_BaseObject"), _("m_bMiniBuilding"))
		M_DYNVARGET(ObjectMode, int, this, _("DT_BaseObject"), _("m_iObjectMode"))
		M_DYNVARGET(MapPlaced, bool, this, _("DT_BaseObject"), _("m_bWasMapPlaced"))
		M_DYNVARGET(Sapped, bool, this, _("DT_BaseObject"), _("m_bHasSapper"))
		M_DYNVARGET(PlasmaDisabled, bool, this, _("DT_BaseObject"), _("m_bPlasmaDisable")) //cowmangler's secondary attack I think
		M_DYNVARGET(Disabled, bool, this, _("DT_BaseObject"), _("m_bDisabled"))
		M_DYNVARGET(Carried, bool, this, _("DT_BaseObject"), _("m_bCarried"))
		M_DYNVARGET(hBuilder, int, this, _("DT_BaseObject"), _("m_hBuilder"))
		M_DYNVARGET(Level, int, this, _("DT_BaseObject"), _("m_iUpgradeLevel"))
		M_DYNVARGET(Health, int, this, _("DT_BaseObject"), _("m_iHealth"))
		M_DYNVARGET(MaxHealth, int, this, _("DT_BaseObject"), _("m_iMaxHealth"))
		M_DYNVARGET(Rockets, int, this, _("DT_ObjectSentrygun"), _("m_iAmmoRockets"))
		M_DYNVARGET(HighestLevel, int, this, _("DT_BaseObject"), _("m_iHighestUpgradeLevel"))
		M_DYNVARGET(Type, int, this, _("DT_BaseObject"), _("m_iObjectType"))
		M_DYNVARGET(Metal, int, this, _("DT_ObjectDispenser"), _("m_iAmmoMetal"))
		M_DYNVARGET(Ammo, int, this, _("DT_ObjectSentrygun"), _("m_iAmmoShells"))
		M_DYNVARGET(Constructed, float, this, _("DT_BaseObject"), _("m_flPercentageConstructed"))

		M_OFFSETGET(RechargeTime, float, 0xFC0)
		M_OFFSETGET(CurrentChargeDuration, float, 0xFC4)
		M_OFFSETGET(YawToExit, float, 0xFCC)

public: //Everything else, lol
	__inline CBaseEntity* GetOwner() {
		DYNVAR(hBuilder, int, _("DT_BaseObject"), _("m_hBuilder"));
		return g_Interfaces.EntityList->GetClientEntityFromHandle(hBuilder.GetValue(this));
	}
};