#include "CritHack.h"
#define MASK_SIGNED 0x7FFFFFFF

// i hate crithack

/* Returns whether random crits are enabled on the server */
bool CCritHack::AreRandomCritsEnabled()
{
	if (static auto tf_weapon_criticals = g_ConVars.FindVar("tf_weapon_criticals"); tf_weapon_criticals) {
		return tf_weapon_criticals->GetBool();
	}
	return true;
}

/* Returns whether the crithack should run */
bool CCritHack::IsEnabled()
{
	if (!Vars::CritHack::Active.Value) { return false; }
	if (!AreRandomCritsEnabled()) { return false; }
	if (!I::EngineClient->IsInGame()) { return false; }

	return true;
}

bool CCritHack::IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon)
{
	if (G::CurItemDefIndex == Soldier_m_TheBeggarsBazooka)
	{
		static bool bLoading = false;

		if (pWeapon->GetClip1() > 0)
		{
			bLoading = true;
		}

		if (!(pCmd->buttons & IN_ATTACK) && bLoading)
		{
			bLoading = false;
			return true;
		}
	}

	else
	{
		if (pWeapon->GetWeaponID() == TF_WEAPON_COMPOUND_BOW || pWeapon->GetWeaponID() == TF_WEAPON_PIPEBOMBLAUNCHER)
		{
			static bool bCharging = false;

			if (pWeapon->GetChargeBeginTime() > 0.0f)
			{
				bCharging = true;
			}

			if (!(pCmd->buttons & IN_ATTACK) && bCharging)
			{
				bCharging = false;
				return true;
			}
		}

		//pssst..
		//Dragon's Fury has a gauge (seen on the weapon model) maybe it would help for pSilent hmm..
		/*
		if (pWeapon->GetWeaponID() == 109) {
		}*/

		else
		{
			if ((pCmd->buttons & IN_ATTACK) && G::WeaponCanAttack)
			{
				return true;
			}
		}
	}

	return false;
}

bool CCritHack::ShouldCrit()
{
	static KeyHelper critKey{ &Vars::CritHack::CritKey.Value };
	if (critKey.Down()) { return true; }
	if (G::CurWeaponType == EWeaponType::MELEE && Vars::CritHack::AlwaysMelee.Value) { return true; }

	return false;
}

int CCritHack::LastGoodCritTick(const CUserCmd* pCmd) {
	int retVal = -1;
	bool pop = false;
	for (int i = 0; i < critTicks.size(); i++) {
		if (critTicks.at(i) >= pCmd->command_number) {
			retVal = critTicks.at(i);
		}
		else {
			pop = true;
		}
	}

	if (pop) {
		critTicks.pop_back();
	}

	return retVal;
}

void CCritHack::ScanForCrits(const CUserCmd* pCmd, int loops)
{
	static int previousWeapon = 0;
	static int previousCrit = 0;
	static int startingNum = pCmd->command_number;

	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal) { return; }

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) { return; }

	if (G::IsAttacking || IsAttacking(pCmd, pWeapon) || pCmd->buttons & IN_ATTACK) {
		return;
	}

	if (startingNum - pCmd->command_number > 60) {
		return;	//	apparently we can't go further into the future than 60 (according to cathook :D)
	}

	const bool bRescanRequired = previousWeapon != pWeapon->GetIndex();
	if (bRescanRequired) {
		startingNum = pCmd->command_number;
		previousWeapon = pWeapon->GetIndex();
		critTicks.clear();
	}

	if (startingNum > 132) {	//	2 seconds into the future we stop scanning
		return;
	}


	//CritBucketBP = *reinterpret_cast<float*>(pWeapon + 0xA54);
	bProtectData = true;	//	stop shit that interferes with our crit bucket because it will BREAK it
	const int seedBackup = MD5_PseudoRandom(pCmd->command_number) & MASK_SIGNED;
	for (int i = 0; i < loops; i++)
	{
		const int cmdNum = startingNum + i;
		*I::RandomSeed = MD5_PseudoRandom(cmdNum) & MASK_SIGNED;
		if (pWeapon->WillCrit())
		{
			critTicks.push_back(cmdNum);	//	store our wish command number for later reference
		}
	}
	startingNum += loops;
	bProtectData = false;	//	we no longer need to be protecting important crit data
	
	//*reinterpret_cast<float*>(pWeapon + 0xA54) = CritBucketBP;
	*reinterpret_cast<int*>(pWeapon + 0xA5C) = 0;	//	dont comment this out, makes sure our crit mult stays as low as possible
													//	crit mult can reach a maximum value of 3!! which means we expend 3 crits WORTH from our bucket
													//	by forcing crit mult to be its minimum value of 1, we can crit more without directly fucking our bucket
													//	yes bProtectData stops this value from changing artificially, but it still changes when you fire and this is worth it imo.

	*I::RandomSeed = seedBackup;
}

void CCritHack::Run(CUserCmd* pCmd)
{
	if (!IsEnabled()) { return; }

	const auto& pWeapon = g_EntityCache.GetWeapon();
	if (!pWeapon || !pWeapon->CanFireCriticalShot(false)) { return; }

	ScanForCrits(pCmd, 33);	//	fill our vector slowly.

	int closestGoodTick = LastGoodCritTick(pCmd);	//	retrieve our wish
	if (IsAttacking(pCmd, pWeapon))	//	is it valid & should we even use it
	{
		if (ShouldCrit())
		{
			if (closestGoodTick < 0) { return; }
			pCmd->command_number = closestGoodTick;		//	set our cmdnumber to our wish
			pCmd->random_seed = MD5_PseudoRandom(closestGoodTick) & MASK_SIGNED;	//	trash poopy whatever who cares
		} 
		else if (Vars::CritHack::AvoidRandom.Value)	//	we don't want to crit
		{
			for (int tries = 0; tries < 5; tries++)
			{
				if (std::find(critTicks.begin(), critTicks.end(), pCmd->command_number + tries) != critTicks.end()) {
					continue;	//	what a useless attempt
				}
				pCmd->command_number += tries;
				pCmd->random_seed = MD5_PseudoRandom(pCmd->command_number) & MASK_SIGNED;
				break;	//	we found a seed that we can use to avoid a crit and have skipped to it, woohoo
			}
		}
	}
}

void CCritHack::Draw()
{
	if (!Vars::CritHack::Indicators.Value) { return; }
	if (!IsEnabled() || !G::CurrentUserCmd) { return; }

	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive()) { return; }

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) { return; }
	int currentY = (g_ScreenSize.h / 2) + 150;
	const float bucket = *reinterpret_cast<float*>(pWeapon + 0xA54);
	
	const int seedRequests = *reinterpret_cast<int*>(pWeapon + 0xA5C);
	
	g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 255,255,255,255, }, ALIGN_CENTERHORIZONTAL, tfm::format("%x", reinterpret_cast<float*>(pWeapon + 0xA54)).c_str());
	// Are we currently forcing crits?
	if (ShouldCrit())
	{
		g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 70, 190, 50, 255 }, ALIGN_CENTERHORIZONTAL, "Forcing crits...");
	}

	static auto tf_weapon_criticals_bucket_cap = g_ConVars.FindVar("tf_weapon_criticals_bucket_cap");
	const float bucketCap = tf_weapon_criticals_bucket_cap->GetFloat();
	const auto bucketText = tfm::format("Bucket: %s / %s", static_cast<int>(bucket), bucketCap);
	const auto seedText = tfm::format("m_nCritSeedRequests: %d", seedRequests);
	const auto FoundCrits = tfm::format("Found Crit Ticks: %d", critTicks.size());
	const auto commandNumber = tfm::format("cmdNumber: %d", G::CurrentUserCmd->command_number);
	g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, bucketText.c_str());
	g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, seedText.c_str());
	g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, FoundCrits.c_str());
	g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, commandNumber.c_str());
}
