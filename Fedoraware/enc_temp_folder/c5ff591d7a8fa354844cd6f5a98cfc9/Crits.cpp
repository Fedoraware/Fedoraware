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

	CritStatus = "Found crit at: " + std::to_string(CmdN);


	if (CmdN && CmdN != command_number && me->GetActiveWeapon()->GetSlot() != 2)
	{
		new_command_number[command_number] = CmdN;
		CNetChannelpasted* net = (CNetChannelpasted*)g_Interfaces.Engine->GetNetChannelInfo();
		net->m_nOutSequenceNr = CmdN - 1;
	}

	LastWeapon = (unsigned long*)pWeapon->GetModel();
	LastCmdN = CmdN;

	return CmdN != 0;
}
#include "../Aimbot/Aimbot.h"





void CCrits::Tick(CUserCmd* pCommand)
{
	if (g_PR->GetDamageByIndex(g_Interfaces.Engine->GetLocalPlayer()) < roundDamage) {
		roundDamage = g_PR->GetDamageByIndex(g_Interfaces.Engine->GetLocalPlayer());
	}

	cachedDamage = g_PR->GetDamageByIndex(g_Interfaces.Engine->GetLocalPlayer()) - meleeDamage;

	for (int i = 1; i <= g_Interfaces.Engine->GetMaxClients(); i++)
	{
		CBaseEntity* ent = g_Interfaces.EntityList->GetClientEntity(i);
		// no valid check needed, GetHealth only uses m_IDX
		if (g_PR->GetHealth(ent))
		{
			auto& status = player_status_list[i - 1];
			// Only sync if not updated recently in player_hurt
			// new health is bigger,
			// or they changed classes. We do the rest in player_hurt
			if (!status.just_updated && (status.clazz != g_PR->GetClass(ent) || status.health < g_PR->GetHealth(ent)))
			{
				status.clazz = g_PR->GetClass(ent);
				status.health = g_PR->GetHealth(ent);
			}
			status.just_updated = false;
		}
	}

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

		g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 300, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, CritStatus2.c_str());
		g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 320, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, (std::string("Bucket: ") + std::to_string(bucket)).c_str());
		g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 340, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, (std::string("cmdnum: ") + std::to_string(LastUserCmd)).c_str());
		if (LastUserCmd) {
			if (CmdN > LastUserCmd)
			{
				float nextcrit = ((float)CmdN - (float)LastUserCmd) / (float)90;
				if (nextcrit > 0.0f)
				{
					g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 70, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, (std::string("Wait: ") + std::to_string(nextcrit)).c_str());
				}
			}
		}
	}


}