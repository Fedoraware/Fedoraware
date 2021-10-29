#pragma once

#include "../BaseEntity/BaseEntity.h"

#define MAX_NEW_COMMANDS 20
#define MAX_NEW_COMMANDS_HEAVY 24
#define DT_WAIT_CALLS 26

struct GlobalInfo_t
{
	int m_nCurrentTargetIdx				= 0;
	int m_nCurItemDefIndex              = 0;
	int m_nWaitForShift                 = 0;
	int m_nShifted                      = 0;
	int dtTicks = MAX_NEW_COMMANDS; // how many to shift
	int tickCounter = 0;
	bool m_bWeaponCanHeadShot			= false;
	bool m_bWeaponCanAttack				= false;
	bool m_bWeaponCanSecondaryAttack	= false;
	bool m_bAAActive					= false;
	bool m_bHitscanSilentActive			= false;
	bool m_bProjectileSilentActive		= false; //flamethrower
	bool m_bAutoBackstabRunning			= false;
	bool m_bHitscanRunning				= false;
	bool m_bSilentTime					= false;
	bool m_bLocalSpectated				= false;
	bool m_bRollExploiting			    = false;
	bool m_bAttacking			 	    = false;
	bool fast_stop						= false;
	bool m_bModulateWorld				= true;
	bool m_bShouldShift                 = false;
	bool m_bRecharging                  = false;
	bool unloadWndProcHook = false;
	float m_flCurAimFOV					= 0.0f;
	Vec3 m_vPredictedPos				= {};
	Vec3 m_vAimPos						= {};
	VMatrix m_WorldToProjection			= {};
	Vec3 m_vViewAngles					= {};
	Vec3 m_vRealViewAngles				= {};
	Vec3 m_vFakeViewAngles				= {};
	Vec3 m_vPunchAngles					= {};
	Vec3 m_vClubPenguinClubPenguinClubPenguinClubPenguinClubPenguinClubPenguinClubPenguinClubPenguin = {}; //clubpenguin > tf2
	EWeaponType m_WeaponType			= {};
	CUserCmd* lateUserCmd{nullptr};
	std::map<int, bool> ignoredPlayers;
};

inline GlobalInfo_t g_GlobalInfo;
