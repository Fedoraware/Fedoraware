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

/* Returns the next crit command number */
int CCritHack::NextCritTick(const CUserCmd* pCmd, int loops = 67)
{
	static int previousWeapon = 0;
	static int previousCrit = 0;

	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal) { return -1; }

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) { return -1; }

	// Return previous crit tick if it's still good
	if (previousWeapon == pWeapon->GetIndex() && previousCrit >= pCmd->command_number) { return previousCrit; }

	// Find the next crit tick
	int foundTick = -1;
	const int seedBackup = MD5_PseudoRandom(pCmd->command_number) & MASK_SIGNED;
	for (int i = 0; i < loops; i++)
	{
		const int cmdNum = pCmd->command_number + i;
		*I::RandomSeed = MD5_PseudoRandom(cmdNum) & MASK_SIGNED;
		if (pWeapon->WillCrit())
		{
			previousCrit = cmdNum;
			previousWeapon = pWeapon->GetIndex();
			foundTick = cmdNum;
			break;
		}
	}

	*reinterpret_cast<int*>(pWeapon + 0xA5C) = 0;

	*I::RandomSeed = seedBackup;
	return foundTick;
}

void CCritHack::Run(CUserCmd* pCmd)
{
	if (!IsEnabled()) { return; }

	const auto& pWeapon = g_EntityCache.GetWeapon();
	if (!pWeapon || !pWeapon->CanFireCriticalShot(false)) { return; }

	int nextCrit = NextCritTick(pCmd);
	if (nextCrit >= 0 && IsAttacking(pCmd, pWeapon))
	{
		if (ShouldCrit())
		{
			// Force next crit
			pCmd->command_number = nextCrit;
			pCmd->random_seed = MD5_PseudoRandom(nextCrit) & MASK_SIGNED;
		} 
		else if (Vars::CritHack::AvoidRandom.Value)
		{
			// Prevent crit
			int tries = 0;
			while (pCmd->command_number == nextCrit && tries < 5)
			{
				pCmd->command_number++;
				pCmd->random_seed = MD5_PseudoRandom(pCmd->command_number) & MASK_SIGNED;
				nextCrit = NextCritTick(pCmd, 5);
				tries++;
			}
		}
	}

	// TODO: Fix the crit bucket
}

void CCritHack::Draw()
{
	if (!Vars::CritHack::Indicators.Value) { return; }
	if (!IsEnabled() || !G::CurrentUserCmd) { return; }

	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive()) { return; }

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) { return; }

	const float bucket = *reinterpret_cast<float*>(pWeapon + 0xA54);
	const int seedRequests = *reinterpret_cast<int*>(pWeapon + 0xA5C);
	int currentY = (g_ScreenSize.h / 2) + 150;

	// Are we currently forcing crits?
	if (ShouldCrit())
	{
		g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 70, 190, 50, 255 }, ALIGN_CENTERHORIZONTAL, "Forcing crits...");
	}

	static auto tf_weapon_criticals_bucket_cap = g_ConVars.FindVar("tf_weapon_criticals_bucket_cap");
	const float bucketCap = tf_weapon_criticals_bucket_cap->GetFloat();
	const auto bucketText = tfm::format("Bucket: %s / %s", static_cast<int>(bucket), bucketCap);
	const auto seedText = tfm::format("m_nCritSeedRequests: %d", seedRequests);
	g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, bucketText.c_str());
	g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, seedText.c_str());
}
