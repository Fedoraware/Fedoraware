#pragma once
#include "../BaseEntity/BaseEntity.h"

constexpr auto DT_WAIT_CALLS = 26;

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
	int Mode = 2; // 0 - Friend, 1 - Ignore, 2 - Default, 3 - Rage, 4 - Cheater
};

namespace G
{
	inline int CurrentTargetIdx = 0; // Index of the current aimbot target
	inline int CurItemDefIndex = 0; // DefIndex of the current weapon
	inline int NotifyCounter = 0;
	inline int EyeAngDelay = 25;
	inline int LoadInCount = 0;	//	increments each time we change server / map.
	inline bool WeaponCanHeadShot = false; // Can the current weapon headshot?
	inline bool WeaponCanAttack = false; // Can the current weapon attack?
	inline bool WeaponCanSecondaryAttack = false;
	inline bool AAActive = false; // Is the Anti-Aim active?
	inline bool FakeShotPitch = false;
	inline bool HitscanSilentActive = false;
	inline bool AvoidingBackstab = false; // Are we currently avoiding a backstab? (Overwrites AA)
	inline bool ProjectileSilentActive = false; //flamethrower
	inline bool AutoBackstabRunning = false;
	inline bool LocalSpectated = false; // Is the local player being spectated?
	inline bool RollExploiting = false; // Are we performing the roll exploit?
	inline bool UnloadWndProcHook = false;

	/* Double tap / Tick shift */
	inline int WaitForShift = 0;
	inline int ShiftedTicks = 0; // Amount of ticks that are shifted
	inline bool ShouldShift = false; // Should we shift now?
	inline bool Recharging = false; // Are we currently recharging?
	inline bool RechargeQueued = false; // Queues a recharge
	inline int TickShiftQueue = 0; // Ticks that shouls be shifted

	/* Choking / Packets */
	inline bool ForceSendPacket = false; // might not actually be useful 
	inline bool ForceChokePacket = false; // might not actually be useful 
	inline bool IsChoking = false; // might not actually be useful 

	/* Aimbot */
	inline bool IsAttacking = false; // this is only used by aimbot, and is also set to false at the start of a lot of functions, this is not reliable
	inline bool HitscanRunning = false;
	inline bool SilentTime = false;
	inline float CurAimFOV = 0.0f;
	inline Vec3 AimPos = {};
	inline VMatrix WorldToProjection = {};

	/* Angles */
	inline Vec3 ViewAngles = {};
	inline Vec3 RealViewAngles = {}; // Real view angles (AA)
	inline Vec3 FakeViewAngles = {}; // Fake view angles (AA)
	inline Vec3 PunchAngles = {};

	/* Projectile prediction */
	inline Vec3 PredictedPos = {};
	inline Vec3 LinearPredLine = {};
	inline std::vector<Vec3> PredBeforeLines;
	inline std::vector<Vec3> PredFutureLines;

	inline CUserCmd* CurrentUserCmd{ nullptr }; // Unreliable! Only use this if you really have to.
	inline CUserCmd* LastUserCmd{ nullptr };

	inline EWeaponType CurWeaponType = {};
	inline std::unordered_map <CBaseEntity*, VelFixRecord> VelFixRecords;
	inline bool FreecamActive = false;
	inline Vec3 FreecamPos = {};
	inline std::unordered_map<int, DormantData> PartyPlayerESP; // <Index, DormantData>
	inline std::unordered_map<int, int> ChokeMap; // Choked packets of players <Index, Amount>
	inline bool DrawingStaticProps = false;
	inline std::unordered_map<uint32_t, Priority> PlayerPriority; // Playerlist priorities <FriendsID, Priority>

	inline bool IsIgnored(uint32_t friendsID)
	{
		return PlayerPriority[friendsID].Mode < 2;
	}
};