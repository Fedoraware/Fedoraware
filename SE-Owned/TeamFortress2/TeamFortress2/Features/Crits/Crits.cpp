//#include "Crits.h"
//
//// YES
//// I
//// AM
//// PASTING
//// FROM
//// CATHOOK
//// AND
//// THIS
//// IS
//// NOT 
//// DONE
//// YET
//
//static float added_per_shot = 0.0f;
//// How much is taken in the case of a fastfire weapon?
//static float taken_per_crit = 0.0f;
//// Needed to calculate observed crit chance properly
//static int cached_damage = 0;
//static int crit_damage = 0;
//static int melee_damage = 0;
//static int round_damage = 0;
//static bool is_out_of_sync = false;
//// Optimization
//static int shots_to_fill_bucket = 0;
//
//bool IsRapidFire(CBaseCombatWeapon* pWeapon)
//{
//	weapon_info info(pWeapon);
//	bool ret = pWeapon->GetWeaponData().m_bUseRapidFireCrits;
//	// Minigun changes mode once revved, so fix that
//	return ret || pWeapon->GetClientClass()->iClassID == (int)ETFClassID::CTFMinigun;
//}
//
//static float GetBucketCap()
//{
//	static ConVar* tf_weapon_criticals_bucket_cap = g_Interfaces.CVars->FindVar("tf_weapon_criticals_bucket_cap");
//	return tf_weapon_criticals_bucket_cap->GetFloat();
//}
//
//static float GetWithdrawMult(CBaseCombatWeapon* pWeapon) {
//	weapon_info info(pWeapon);
//
//	int call_count = info.crit_count + 1;
//
//	int crit_checks = info.crit_attempts + 1;
//	float flMultiply = 0.5;
//	
//	if (g_GlobalInfo.m_WeaponType != EWeaponType::MELEE) {
//		flMultiply = Math::RemapValClamped(((float)call_count / (float)crit_checks), 0.1f, 1.f, 1.f, 3.f);
//	}
//
//	float flToRemove = flMultiply * 3.0f;
//	return flToRemove;
//}
//
//static float GetWithdrawAmount(CBaseCombatWeapon* pWeapon) {
//	float amount = added_per_shot * GetWithdrawMult(pWeapon);
//
//	if (IsRapidFire(pWeapon)) {
//		amount = taken_per_crit * GetWithdrawMult(pWeapon);
//		reinterpret_cast<int&>(amount) &= ~1;
//	}
//	return amount;
//}
//
//static bool IsAllowedToWithdrawFromBucket(CBaseCombatWeapon* pWeapon, bool add_damage = true) {
//	weapon_info info(pWeapon);
//	if (add_damage)
//	{
//		if (info.crit_bucket < GetBucketCap()) {
//			info.crit_bucket += added_per_shot;
//			info.crit_bucket = std::min(info.crit_bucket, GetBucketCap());
//		}
//	}
//
//	if (GetWithdrawMult(pWeapon) > info.crit_bucket)
//	{
//		return false;
//	}
//	return true;
//}
//
//static void SimulateNormalShot(CBaseCombatWeapon* pWeapon, float flDamage)
//{
//	weapon_info info(pWeapon);
//
//	if (info.crit_count < GetBucketCap())
//	{
//		info.crit_bucket += flDamage;
//		info.crit_bucket = std::min(info.crit_bucket, GetBucketCap());
//	}
//	info.crit_attempts++;
//	info.restore_data(pWeapon);
//}
//
//static int ShotsUntilCrit(CBaseCombatWeapon* pWeapon) {
//	weapon_info info(pWeapon);
//
//	int shots = 0;
//
//	for (shots = 0; shots < shots_to_fill_bucket + 1; shots++) {
//		if (IsAllowedToWithdrawFromBucket(pWeapon, true)) {
//			break;
//		}
//		SimulateNormalShot(pWeapon, added_per_shot);
//	}
//
//	info.restore_data(pWeapon);
//	return shots;
//}
//
//float Clampf(float value, float min, float max) {
//	const float t = value < min ? min : value;
//	return t > max ? max : t;
//}
//
//static float GetCritCap(CBaseCombatWeapon* pWeapon) {
//	float crit_mult = pWeapon->GetCritMult();
//
//	float chance = 0.02f;
//	if (g_GlobalInfo.m_WeaponType == EWeaponType::MELEE) {
//		chance = 0.15f;
//	}
//	float flMultCritChance = Utils::ATTRIB_HOOK_FLOAT(crit_mult * chance, "mult_crit_chance", pWeapon, 0, 1);
//
//	if (IsRapidFire(pWeapon)) {
//		float flTotalCritChance = Clampf(0.02f * crit_mult, 0.01f, 0.99f);
//
//		float flCritDuration = 2.0f;
//
//		float flNonCritDuration = (flCritDuration / flTotalCritChance) - flCritDuration;
//
//		float flStartCritChance = 1 / flNonCritDuration;
//		flMultCritChance = Utils::ATTRIB_HOOK_FLOAT(flStartCritChance, "mult_crit_chance", pWeapon, 0, 1);
//	}
//	return flMultCritChance;
//}
//
//static float getObservedCritChance()
//{
//	if (!(cached_damage - round_damage))
//		return 0.0f;
//	// Same is used by server
//	float normalized_damage = (float)crit_damage / 3.0f;
//	return normalized_damage / (normalized_damage + (float)((cached_damage - round_damage) - crit_damage));
//}
//
//static std::pair<float, float> critMultInfo(CBaseCombatWeapon* pWeapon)
//{
//	float cur_crit = GetCritCap(pWeapon);
//	float observed_chance = pWeapon->GetObservedCritChance();
//	float needed_chance = cur_crit + 0.1f;
//	return std::pair<float, float>(observed_chance, needed_chance);
//}
//
//static int damageUntilToCrit(CBaseCombatWeapon* pWeapon)
//{
//	// First check if we even need to deal damage at all
//	auto crit_info = critMultInfo(pWeapon);
//	if (crit_info.first <= crit_info.second || g_GlobalInfo.m_WeaponType == EWeaponType::MELEE)
//		return 0;
//
//	float target_chance = critMultInfo(pWeapon).second;
//	// Formula taken from TotallyNotElite
//	int damage = std::ceil(crit_damage * (2.0f * target_chance + 1.0f) / (3.0f * target_chance));
//	return damage - (cached_damage - round_damage);
//}
//
//static int nextCritTick(int loops = 4096)
//{
//	static int previous_crit = -1;
//	static int previous_weapon = -1;
//
//	CBaseCombatWeapon* pWeapon = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer())->GetActiveWeapon();
//
//	if (previous_weapon == pWeapon->GetIndex() && previous_crit >= g_GlobalInfo.lateUserCmd->command_number) {
//		return previous_crit;
//	}
//
//	int old_seed = MD5_PseudoRandom(g_GlobalInfo.lateUserCmd->command_number) & 0x7FFFFFFF;
//
//	for (int i = 0; i < loops; i++) {
//		int cmd_number = g_GlobalInfo.lateUserCmd->command_number + i;
//
//		*g_Interfaces.RandomSeed = MD5_PseudoRandom(cmd_number) & 0x7FFFFFFF;
//
//		weapon_info info(pWeapon);
//		bool is_crit = pWeapon->CalcIsAttackCriticalHelper(pWeapon);
//
//		info.restore_data(pWeapon);
//
//		if (is_crit)
//		{
//			*g_Interfaces.RandomSeed = old_seed;
//			previous_crit = cmd_number;
//			previous_weapon = pWeapon->GetIndex();
//			return cmd_number;
//		}
//	}
//	*g_Interfaces.RandomSeed = old_seed;
//	return -1;
//}
//
//static bool randomCritEnabled()
//{
//	static ConVar* tf_weapon_criticals = g_Interfaces.CVars->FindVar("tf_weapon_criticals");
//	return tf_weapon_criticals->GetInt();
//}
//
//bool force_crit_this_tick = false;
//
//bool isEnabled()
//{
//	// No crits without random crits
//	if (!randomCritEnabled()) {
//		return false;
//	}
//
//	return true;
//}
//
//bool shouldMeleeCrit()
//{
//	if (g_GlobalInfo.m_WeaponType != EWeaponType::MELEE) {
//		return false;
//	}
//
//	return true;
//}
//
//bool shouldCrit()
//{
//	if (shouldMeleeCrit()) {
//		return true;
//	}
//
//	if (force_crit_this_tick)
//		return true;
//
//	return true;
//}
//
//static bool can_beggars_crit = false;
//static bool attacked_last_tick = false;
//
//bool canWeaponCrit(bool draw = false)
//{
//	CBaseCombatWeapon *pWeapon = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer())->GetActiveWeapon();
//	if (!pWeapon->IsBaseCombatWeapon())
//		return false;
//	if (randomCritEnabled() || !added_per_shot)
//		return false;
//	if (pWeapon->CanFireCriticalShot(pWeapon))
//		return false;
//	if (!added_per_shot)
//		return false;
//	if (!GetCritCap(pWeapon))
//		return false;
//
//	if (!IsAllowedToWithdrawFromBucket(pWeapon))
//		return false;
//
//	auto crit_mult_info = critMultInfo(pWeapon);
//	if (crit_mult_info.first > crit_mult_info.second && g_GlobalInfo.m_WeaponType != EWeaponType::MELEE)
//		return false;
//	return true;
//}
//
//size_t current_index = 0;
//// Cache Weapons
//std::map<int, std::vector<int>> crit_cmds;
//
//struct player_status
//{
//	int health{};
//	int clazz{};
//	// To make Player resource not be a hindrance
//	bool just_updated{};
//};
//static std::array<player_status, 32> player_status_list{};
//
//bool prevent_crit()
//{
//	int tries = 0;
//	// Only check one tick
//	while (nextCritTick(5) == g_GlobalInfo.lateUserCmd->command_number && tries < 5)
//	{
//		g_GlobalInfo.lateUserCmd->command_number++;
//		g_GlobalInfo.lateUserCmd->random_seed = MD5_PseudoRandom(g_GlobalInfo.lateUserCmd->command_number);
//		tries++;
//	}
//	// Failed
//	if (nextCritTick(5) == g_GlobalInfo.lateUserCmd->command_number)
//		return false;
//	// Suceeded
//	return true;
//}
//
//void force_crit()
//{
//	CBaseCombatWeapon* pWeapon = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer())->GetActiveWeapon();
//	if (g_GlobalInfo.m_WeaponType != EWeaponType::MELEE && pWeapon->GetClassID() != ETFClassID::CTFPipebombLauncher)
//	{
//		if (crit_cmds.size() && crit_cmds.find(pWeapon->GetIndex()) != crit_cmds.end())
//		{
//			if (current_index >= crit_cmds.at(pWeapon->GetIndex()).size())
//			{
//				current_index = 0;
//			}
//			g_GlobalInfo.lateUserCmd->command_number = crit_cmds.at(pWeapon->GetIndex()).at(current_index);
//			g_GlobalInfo.lateUserCmd->random_seed = MD5_PseudoRandom(g_GlobalInfo.lateUserCmd->command_number) & 0x7FFFFFFF;
//			current_index++;
//		}
//	}
//
//	else
//	{
//		int next_crit = nextCritTick();
//		if (next_crit != -1)
//		{
//			if (pWeapon->GetClassID() == ETFClassID::CTFPipebombLauncher)
//			{
//
//			}
//			g_GlobalInfo.lateUserCmd->command_number = next_crit;
//			g_GlobalInfo.lateUserCmd->random_seed = MD5_PseudoRandom(next_crit) & 0x7FFFFFFF;
//		}
//	}
//	force_crit_this_tick = true;
//}
//
//static void updateCmds()
//{
//	CBaseCombatWeapon* pWeapon = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer())->GetActiveWeapon();
//	static int previous_weapon = 0;
//
//	int cur_cmdnum = g_GlobalInfo.lateUserCmd->command_number;
//
//	bool old_cmds = false;
//
//	auto weapon_cmds = crit_cmds.find(pWeapon->GetIndex());
//
//	if (weapon_cmds == crit_cmds.end()) 
//	{
//		old_cmds = true;
//	} 
//	else 
//	{
//		for (auto& cmd : weapon_cmds->second) {
//			if (cmd < cur_cmdnum) {
//				old_cmds = true;
//			}
//		}
//	}
//
//	if (old_cmds)
//	{
//		int xor_dat = (pWeapon->GetIndex() << 8 | g_EntityCache.m_pLocal->GetIndex());
//		if (g_GlobalInfo.m_WeaponType != EWeaponType::MELEE) {
//			xor_dat = (pWeapon->GetIndex() << 16 | g_EntityCache.m_pLocal->GetIndex() << 8);
//		}
//
//		crit_cmds[pWeapon->GetIndex()].clear();
//		added_per_shot = 0.0f;
//
//		for (int i = cur_cmdnum + 200, j = 3; i <= cur_cmdnum + 100000 + 200 && j > 0; i++) {
//			int iSeed = MD5_PseudoRandom(i) & 0x7FFFFFFF;
//			int tempSeed = iSeed & (xor_dat);
//			Utils::RandomSeed(tempSeed);
//
//			int iResult = Utils::RandInt(0, 9999);
//
//			if (iResult == 0 && i > cur_cmdnum + 200) {
//				crit_cmds[pWeapon->GetIndex()].push_back(i);
//				j--;
//			}
//		}
//	}
//
//	if (added_per_shot = 0.0f || previous_weapon != pWeapon->GetIndex())
//	{
//		weapon_info info(pWeapon);
//		int nProjectilesPerShot = pWeapon->GetWeaponData().m_nBulletsPerShot;
//		if (nProjectilesPerShot >= 1) {
//			nProjectilesPerShot = Utils::ATTRIB_HOOK_FLOAT(nProjectilesPerShot, "mult_bullets_per_shot", pWeapon, 0x0, true);
//		}
//		else {
//			nProjectilesPerShot = 1;
//		}
//
//		added_per_shot = pWeapon->GetWeaponData().m_nDamage;
//		added_per_shot = Utils::ATTRIB_HOOK_FLOAT(added_per_shot, "mult_dmg", pWeapon, 0x0, true);
//		added_per_shot *= nProjectilesPerShot;
//		shots_to_fill_bucket = GetBucketCap() / added_per_shot;
//		// Special boi
//		if (IsRapidFire(pWeapon))
//		{
//			taken_per_crit = added_per_shot;
//			taken_per_crit *= 2.0f / pWeapon->GetWeaponData().m_flTimeFireDelay;
//
//			// Yes this looks dumb but i want to ensure that it matches with valve code
//			int bucket_cap_recasted = (int)GetBucketCap();
//			// Never try to drain more than cap
//			if (taken_per_crit * 3.0f > bucket_cap_recasted)
//				taken_per_crit = (float)bucket_cap_recasted / 3.0f;
//		}
//	}
//
//	previous_weapon = pWeapon->GetIndex();
//}
//
//// Fix observed crit chance
//static void fixObservedCritchance(CBaseCombatWeapon* pWeapon)
//{
//	weapon_info info(pWeapon);
//	info.observed_crit_chance = getObservedCritChance();
//	info.restore_data(pWeapon);
//}
//
//static std::vector<float> crit_mult_storage;
//static float last_bucket_fix = -1;
//
//void CCrits::RunInCreateMove() {
//	//if (	)
//}
//
//void CCrits::DrawInfo() {
//	CBaseCombatWeapon* activeWeapon = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer())->GetActiveWeapon();
//	weapon_info wep(activeWeapon);
//	int x = 0;
//	int y = (g_ScreenSize.h / 2);
//	g_Draw.String(FONT_DEBUG, x, y, { 255,255,255,255 }, ALIGN_DEFAULT, L"Crit bucket: %.2f", wep.crit_bucket);
//	g_Draw.String(FONT_DEBUG, x, y + 20, { 255,255,255,255 }, ALIGN_DEFAULT, L"weapon seed: %d", wep.weapon_seed);
//	g_Draw.String(FONT_DEBUG, x, y + 40, { 255,255,255,255 }, ALIGN_DEFAULT, L"Unknown 1: %d", wep.unknown1);
//	g_Draw.String(FONT_DEBUG, x, y + 60, { 255,255,255,255 }, ALIGN_DEFAULT, L"Unknown 2: %d", wep.unknown2);
//	g_Draw.String(FONT_DEBUG, x, y + 80, { 255,255,255,255 }, ALIGN_DEFAULT, L"Unknown 3: %d", wep.unknown3);
//	g_Draw.String(FONT_DEBUG, x, y + 100, { 255,255,255,255 }, ALIGN_DEFAULT, L"Unknown 4: %.2f", wep.unknown4);
//	g_Draw.String(FONT_DEBUG, x, y + 120, { 255,255,255,255 }, ALIGN_DEFAULT, L"Crit attempts: %d", wep.crit_attempts);
//	g_Draw.String(FONT_DEBUG, x, y + 140, { 255,255,255,255 }, ALIGN_DEFAULT, L"Crit count: %d", wep.crit_count);
//	g_Draw.String(FONT_DEBUG, x, y + 160, { 255,255,255,255 }, ALIGN_DEFAULT, L"Observed chance: %d", wep.observed_crit_chance);
//	g_Draw.String(FONT_DEBUG, x, y + 180, { 255,255,255,255 }, ALIGN_DEFAULT, L"Unknown 7: %d", wep.unknown7);
//	g_Draw.String(FONT_DEBUG, x, y + 200, { 255,255,255,255 }, ALIGN_DEFAULT, L"Weapon mode: %d", wep.weapon_mode);
//	g_Draw.String(FONT_DEBUG, x, y + 220, { 255,255,255,255 }, ALIGN_DEFAULT, L"Weapon data: %d", wep.weapon_data);
//	g_Draw.String(FONT_DEBUG, x, y + 240, { 255,255,255,255 }, ALIGN_DEFAULT, L"Shots till crit: %d", ShotsUntilCrit(activeWeapon));
//}


#include "Crits.h"
#include "../Vars.h"


Crits g_Crits;

#define me g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer())

// Game keeps track of these stats
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

void Crits::CorrectBucket(CBaseEntity* pWeapon, CUserCmd* pCommand)
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

int Crits::FindNextCritCmdN(CBaseEntity* pWeapon, CUserCmd* pCommand)
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

bool Crits::ForceCrit(CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
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
		CNetChannel* net = g_Interfaces.Engine->GetNetChannelInfo();
		net->m_nOutSequenceNr = CmdN - 1;
	}

	LastWeapon = (unsigned long*)pWeapon->GetModel();
	LastCmdN = CmdN;

	return CmdN != 0;
}

#include "../Aimbot/Aimbot.h"

void Crits::Tick(CUserCmd* pCommand)
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

void Crits::Frame()
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

		g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) - 10, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, CritStatus2.c_str());
		g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 30, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, (std::string("Bucket: ") + std::to_string(bucket)).c_str());
		g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 50, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, (std::string("cmdnum: ") + std::to_string(LastUserCmd)).c_str());
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