#pragma once

#include <string>
#include <deque>
#include <unordered_map>
#include "../Vars.h"


class CCrits
{
public:
	void Tick(CUserCmd* pCommand);
	//float getObservedCritChance();
	//void fixObservedCritchance(CBaseCombatWeapon* weapon);
	void Frame();
	//void FixBucket(CBaseCombatWeapon* weapon, CUserCmd* cmd);

	std::unordered_map<int, int> new_command_number;
	float CachedDamage = 0.f;
	float RoundDamage = 0.f;
	float CritDamage = 0.f;
private:
	struct stats_t
	{
		float flCritBucket; // A54
		int   iNumAttacks;  // A58
		int   iNumCrits;    // A5C
	} stats;

	// Memory layout for C_TFWeaponBase
	struct state_t
	{
		bool  bCurrentAttackIsCrit;
		bool  bLowered;                  // NETVAR 0xB40
		int   iAltFireHint;              // 0xB44
		int   iReloadStartClipAmount;    // 0xB48
		float flCritEndTime;             // 0xB4C
		float flLastCritCheckTime;       // NETVAR // 0xB50
		int   iLastCritCheckFrame;       // 0xB54
		int   iCurrentSeed;              // 0xB58
		float flLastCritCheckTimeNoPred; // 0xB5C
	};

	state_t& GetState(CBaseCombatWeapon* pWeapon)
	{
		static int offset = g_NetVars.get_offset("DT_TFWeaponBase", "m_bLowered") - 0x4;
		return *(state_t*)(pWeapon + offset);
	}
	bool ForceCrit(CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
	void CorrectBucket(CBaseEntity* weapon, CUserCmd* pCommand);
	int FindNextCritCmdN(CBaseEntity* pWeapon, CUserCmd* pCommand);

	int CmdN = 0;
	int LastCmdN = 0;
	int LastUserCmd = 0;

	//weapon_info last_weapon_info = nullptr;


	unsigned long* LastWeapon = nullptr;

	std::string CritStatus = "None";
	std::string CritStatus2 = "None";
};

inline CCrits g_Crits;