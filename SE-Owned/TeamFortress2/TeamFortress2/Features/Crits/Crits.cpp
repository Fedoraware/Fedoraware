#include "Crits.h"

#define me g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer())

void CCrits::CorrectBucket(CBaseEntity* pWeapon, CUserCmd* pCommand)
{
	static bool changed;
	static float last_bucket;
	static int last_weapon;

	if (pCommand->command_number)
		changed = false;

	stats_t stats = *(stats_t*)(pWeapon + 0xA54);

	float& bucket = stats.flCritBucket;

	if (bucket != last_bucket)
	{
		if (changed && pWeapon->GetIndex() == last_weapon)
		{
			bucket = last_bucket;
		}
		changed = true;
	}
	last_weapon = pWeapon->GetIndex();
	last_bucket = bucket;
}

int CCrits::FindNextCritCmdN(CBaseEntity* pWeapon, CUserCmd* pCommand)
{
	int tries = 0, number = pCommand->command_number, found = 0, seed_backup;

	state_t state;
	stats_t stats;

	seed_backup = *g_Interfaces.RandomSeed;

	memcpy(&state, &GetState((CBaseCombatWeapon*)pWeapon), sizeof(state_t));
	memcpy(&stats, &*(stats_t*)(pWeapon + 0xA54), sizeof(stats_t));
	seed_backup = *g_Interfaces.RandomSeed;
	while (!found && tries < 4096)
	{
		*g_Interfaces.RandomSeed = MD5_PseudoRandom(number) & 0x7FFFFFFF;
		found = me->GetActiveWeapon()->CalcIsAttackCritical();

		if (found)
		{
			break;
		}

		++tries;
		++number;
	}

	*g_Interfaces.RandomSeed = seed_backup;

	memcpy(&GetState((CBaseCombatWeapon*)pWeapon), &state, sizeof(state_t));
	memcpy(&*(stats_t*)(pWeapon + 0xA54), &stats, sizeof(stats_t));

	if (found)
		return number;

	return 0;
}

struct cached_calculation_s
{
	int command_number;
	int init_command;
	int weapon_entity;
};



static cached_calculation_s cached_calculation{};

bool CCrits::ForceCrit(CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	auto command_number = pCmd->command_number;

	if (LastCmdN < command_number || LastWeapon != (unsigned long*)pWeapon->GetModel() || LastCmdN - command_number > 1000)
	{
		/*if (cached_calculation.init_command > command_number || command_number - cached_calculation.init_command > 50 || (command_number && (cached_calculation.command_number < command_number)))
			cached_calculation.weapon_entity = 0;
		if (cached_calculation.weapon_entity == pWeapon->GetIndex())
			return bool(cached_calculation.command_number);*/

		CmdN = FindNextCritCmdN(pWeapon, pCmd);
	}
	else {
		CmdN = LastCmdN;
	}

	CritStatus = "Found crit at: " + std::to_string(CmdN) + "(" + std::to_string(LastUserCmd - CmdN) + ")";


	if (CmdN && CmdN != command_number && me->GetActiveWeapon()->GetSlot() != 2)
	{
		new_command_number[command_number] = CmdN;
		CNetChannel* net = g_Interfaces.Engine->GetNetChannelInfo();
		net->m_nOutSequenceNr = CmdN - 1;
	}

	LastWeapon = (unsigned long*)pWeapon->GetModel();
	//last_weapon_info = weapon_info(pWeapon);
	LastCmdN = CmdN;

	return CmdN != 0;
}

#include "../Aimbot/Aimbot.h"

void CCrits::Tick(CUserCmd* pCommand)
{
	if (!Vars::Crits::Active.m_Var) {
		return;
	}
	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		if (!pLocal->IsAlive()) {
			return;
		}
		LastUserCmd = pCommand->command_number;

		if (g_Interfaces.Prediction->InPrediction())
			return;

		if (!g_GlobalInfo.m_bWeaponCanAttack)
			return;

		CBaseCombatWeapon* pActiveWep = pLocal->GetActiveWeapon();
		CorrectBucket(pActiveWep, pCommand);
		if (GetAsyncKeyState(Vars::Crits::CritKey.m_Var) && (pCommand->buttons & IN_ATTACK || g_GlobalInfo.m_bHitscanRunning || g_GlobalInfo.m_bAttacking))
		{
			if (pCommand->command_number != CmdN && CmdN) {
				CritStatus2 = "Waiting...";
			}
			else {
				CritStatus2 = "Fired!";
			}
			ForceCrit(pActiveWep, pCommand);
		}
	}
}

//// Server gives us garbage so let's just calc our own
//float Crits::getObservedCritChance()
//{
//	if (!(CachedDamage - RoundDamage))
//		return 0.0f;
//	// Same is used by server
//	float normalized_damage = (float)CritDamage / 3.0f;
//	return normalized_damage / (normalized_damage + (float)((CachedDamage - RoundDamage) - CritDamage));
//}
//
//void Crits::fixObservedCritchance(CBaseCombatWeapon* weapon)
//{
//	weapon_info info(weapon);
//	info.m_flObservedCritChance = getObservedCritChance();
//	info.RestoreData(weapon);
//}
//
//void Crits::FixBucket(CBaseCombatWeapon* weapon, CUserCmd* cmd)
//{
//	INetChannel* ch = (INetChannel*)g_Interfaces.Engine->GetNetChannelInfo();
//	if (!ch)
//		return;
//
//	static int last_weapon;
//	// This tracks only when bucket is updated
//	static int last_update_command;
//
//	fixObservedCritchance(weapon);
//
//	weapon_info original_info(weapon);
//
//	weapon_info info(weapon);
//
//	// Changed bucket more than once this tick, this is wrong and needs to be corrected.
//	if (weapon->GetIndex() == last_weapon && info != last_weapon_info && last_update_command == cmd->command_number)
//		info = last_weapon_info;
//
//	last_weapon = weapon->GetIndex();
//	// Bucket changed, update
//	if (last_weapon_info.flCritBucket != original_info.flCritBucket)
//		last_update_command = cmd->command_number;
//
//	last_weapon_info = info;
//
//	info.RestoreData(weapon);
//}

void CCrits::Frame()
{
	if (!Vars::Crits::Active.m_Var) {
		return;
	}

	if (!g_Interfaces.Engine->IsInGame())
		return;

	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		if (!pLocal->IsAlive()) {
			return;
		}

		CBaseCombatWeapon* pActiveWep = me->GetActiveWeapon();

		if (!pActiveWep)
			return;

		stats_t stats = *(stats_t*)(pActiveWep + 0xA54);

		int bucket = (int)(stats.flCritBucket);

		int wide = (bucket / 3.75 - bucket * .1) - 1;


		g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 200, { 84, 168, 255, 127 }, ALIGN_CENTERHORIZONTAL, (std::string("Bucket: ") + std::to_string(bucket)).c_str());
		if (CmdN == 0) {
			g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 220, { 255, 84, 84, 255 }, ALIGN_CENTERHORIZONTAL, "Crit banned");
		}
		else {
			g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 220, { 100, 255, 118, 127 }, ALIGN_CENTERHORIZONTAL, CritStatus.c_str());
		}
		if (LastUserCmd) {
			if (CmdN > LastUserCmd)
			{
				float nextcrit = ((float)CmdN - (float)LastUserCmd) / (float)90;
				if (nextcrit > 0.0f)
				{
					g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 240, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, (std::string("Wait: ") + std::to_string(nextcrit)).c_str());
				}
			}
			else {
				g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 240, { 230, 180, 80, 127 }, ALIGN_CENTERHORIZONTAL, CritStatus2.c_str());
			}
		}
	}
}