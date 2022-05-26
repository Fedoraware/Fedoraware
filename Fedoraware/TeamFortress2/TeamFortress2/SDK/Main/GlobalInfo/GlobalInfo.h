#pragma once

#include "../BaseEntity/BaseEntity.h"

#define DT_WAIT_CALLS 26

struct VelFixRecord {
	Vec3 m_vecOrigin;
	int m_nFlags;
	float m_flSimulationTime;
};

struct DormantData {
	Vec3 Location;
	float LastUpdate = 0.f;
};

struct Priority {
	int Mode = 2;
};

namespace G
{
	inline int m_nCurrentTargetIdx = 0;
	inline int m_nCurItemDefIndex = 0;
	inline int m_nWaitForShift = 0;
	inline int m_nShifted = 0;
	inline int gNotifCounter = 0;
	inline int vEyeAngDelay = 25;
	inline bool m_bWeaponCanHeadShot = false;
	inline bool m_bWeaponCanAttack = false;
	inline bool m_bWeaponCanSecondaryAttack = false;
	inline bool m_bAAActive = false;
	inline bool m_bFakeShotPitch = false;
	inline bool m_bHitscanSilentActive = false;
	inline bool m_bAvoidingBackstab = false;
	inline bool m_bProjectileSilentActive = false; //flamethrower
	inline bool m_bAutoBackstabRunning = false;
	inline bool m_bHitscanRunning = false;
	inline bool m_bSilentTime = false;
	inline bool m_bLocalSpectated = false;
	inline bool m_bRollExploiting = false;
	inline bool m_bAttacking = false; // this is only used by aimbot, and is also set to false at the start of a lot of functions, this is not reliable
	inline bool m_bModulateWorld = true;
	inline bool m_bShouldShift = false;
	inline bool m_bRecharging = false;
	inline bool m_bRechargeQueued = false;
	inline bool m_bForceSendPacket = false; // might not actually be useful 
	inline bool m_bForceChokePacket = false; // might not actually be useful 
	inline bool m_bChoking = false; // might not actually be useful 
	inline bool unloadWndProcHook = false;
	inline float m_flCurAimFOV = 0.0f;
	inline Vec3 m_vPredictedPos = {};
	inline Vec3 m_vAimPos = {};
	inline VMatrix m_WorldToProjection = {};
	inline Vec3 m_vViewAngles = {};
	inline Vec3 m_vRealViewAngles = {};
	inline Vec3 m_vFakeViewAngles = {};
	inline Vec3 m_vPunchAngles = {};
	inline Vec3 linearPredLine = {}; //clubpenguin > tf2
	inline EWeaponType m_WeaponType = {};
	inline CUserCmd* currentUserCmd{ nullptr };
	inline CUserCmd* lateUserCmd{ nullptr };
	inline std::map < CBaseEntity*, VelFixRecord> velFixRecord;
	inline std::vector<Vec3> predBeforeLines;
	inline std::vector<Vec3> predFutureLines;
	inline bool m_bFreecamActive = false;
	inline Vec3 m_vFreecamPos = {};
	inline std::map<int, DormantData> partyPlayerESP;		// < Player-Index, DormantData >
	inline std::map<int, int> chokeMap;
	inline int tickShiftQueue = 0; // Ticks that shouls be shifted
	inline bool m_bDrawingStaticProps = false;
	inline std::map<uint32_t, Priority> PlayerPriority;

	inline bool IsIgnored(uint32_t friendsID)
	{
		return PlayerPriority[friendsID].Mode < 2;
	}
};