#pragma once

#include <string>
#include <deque>
#include <unordered_map>
#include "../Vars.h"
#include <array>
#include "../PlayerResource/PlayerResource.h"
class CCrits
{
public:
	void Tick(CUserCmd* pCommand);
	//float getObservedCritChance();
	//void fixObservedCritchance(CBaseCombatWeapon* weapon);
	void Frame();
	//void FixBucket(CBaseCombatWeapon* weapon, CUserCmd* cmd);

	std::unordered_map<int, int> new_command_number;
	int cachedDamage = 0;
	int critDamage = 0;
	int meleeDamage = 0;
	int roundDamage = 0;

	// We need to store a bunch of data for when we kill someone with a crit
	struct player_status
	{
		int health{};
		int clazz{};
		// To make Player resource not be a hindrance
		bool just_updated{};
	};
	std::array<player_status, 32> player_status_list;
	void ObservedCritChance_NetHook(const CRecvProxyData* data, void* pWeapon, void* out);

private:
	struct stats_t
	{
		float flCritBucket; // A54
		int iNumAttacks; // A58
		int iNumCrits; // A5C
	} stats;

	// Memory layout for C_TFWeaponBase
	struct state_t
	{
		bool bCurrentAttackIsCrit;
		bool bLowered; // NETVAR 0xB40
		int iAltFireHint; // 0xB44
		int iReloadStartClipAmount; // 0xB48
		float flCritEndTime; // 0xB4C
		float flLastCritCheckTime; // NETVAR // 0xB50
		int iLastCritCheckFrame; // 0xB54
		int iCurrentSeed; // 0xB58
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

namespace Argh {
	inline void ObservedCritChance_NetHook(const CRecvProxyData* data, void* pWeapon, void* out)
	{
		auto flObservedCritChance = reinterpret_cast<float*>(out);
		*flObservedCritChance = data->m_Value.m_Float;
		if (!g_EntityCache.m_pLocalWeapon) {
			return;
		}

		if (pWeapon != g_EntityCache.m_pLocalWeapon) {
			return;
		}

		float sentChance = data->m_Value.m_Float;
		if (sentChance) {
			// Before fix
			// Sync our chance, Player ressource is guranteed to be working, melee damage not, but it's fairly reliable
			int rangedDamage = g_PR->GetDamageByIndex(g_Interfaces.Engine->GetLocalPlayer()) - g_Crits.meleeDamage;
			// We need to do some checks for our sync process
			if (rangedDamage != 0 && 2.0f * sentChance + 1 != 0.0f)
			{
				g_Crits.cachedDamage = rangedDamage - g_Crits.roundDamage;
				// Powered by math
				g_Crits.critDamage = (3.0f * rangedDamage * sentChance) / (2.0f * sentChance + 1);
			}
		}
	}

}