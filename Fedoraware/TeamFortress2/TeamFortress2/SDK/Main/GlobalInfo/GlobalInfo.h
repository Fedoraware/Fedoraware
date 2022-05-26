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
	inline int CurrentTargetIdx = 0;
	inline int CurItemDefIndex = 0;
	inline int WaitForShift = 0;
	inline int ShiftedTicks = 0;
	inline int NotifyCounter = 0;
	inline int EyeAngDelay = 25;
	inline bool WeaponCanHeadShot = false;
	inline bool WeaponCanAttack = false;
	inline bool WeaponCanSecondaryAttack = false;
	inline bool AAActive = false;
	inline bool FakeShotPitch = false;
	inline bool HitscanSilentActive = false;
	inline bool AvoidingBackstab = false;
	inline bool ProjectileSilentActive = false; //flamethrower
	inline bool AutoBackstabRunning = false;
	inline bool HitscanRunning = false;
	inline bool SilentTime = false;
	inline bool LocalSpectated = false;
	inline bool RollExploiting = false;
	inline bool IsAttacking = false; // this is only used by aimbot, and is also set to false at the start of a lot of functions, this is not reliable
	inline bool ShouldShift = false;
	inline bool Recharging = false;
	inline bool RechargeQueued = false;
	inline bool ForceSendPacket = false; // might not actually be useful 
	inline bool ForceChokePacket = false; // might not actually be useful 
	inline bool IsChoking = false; // might not actually be useful 
	inline bool UnloadWndProcHook = false;
	inline float CurAimFOV = 0.0f;
	inline Vec3 PredictedPos = {};
	inline Vec3 AimPos = {};
	inline VMatrix WorldToProjection = {};
	inline Vec3 ViewAngles = {};
	inline Vec3 RealViewAngles = {};
	inline Vec3 FakeViewAngles = {};
	inline Vec3 PunchAngles = {};
	inline Vec3 LinearPredLine = {}; //clubpenguin > tf2
	inline EWeaponType CurWeaponType = {};
	inline CUserCmd* CurrentUserCmd{ nullptr };
	inline CUserCmd* LastUserCmd{ nullptr };
	inline std::map < CBaseEntity*, VelFixRecord> VelFixRecords;
	inline std::vector<Vec3> PredBeforeLines;
	inline std::vector<Vec3> PredFutureLines;
	inline bool FreecamActive = false;
	inline Vec3 FreecamPos = {};
	inline std::map<int, DormantData> PartyPlayerESP;		// < Player-Index, DormantData >
	inline std::map<int, int> ChokeMap;
	inline int TickShiftQueue = 0; // Ticks that shouls be shifted
	inline bool DrawingStaticProps = false;
	inline std::map<uint32_t, Priority> PlayerPriority;

	inline bool IsIgnored(uint32_t friendsID)
	{
		return PlayerPriority[friendsID].Mode < 2;
	}
};