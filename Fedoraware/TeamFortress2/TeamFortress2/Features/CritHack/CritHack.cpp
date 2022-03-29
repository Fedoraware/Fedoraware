#include "CritHack.h"
#define MASK_SIGNED 0x7FFFFFFF

// i hate crithack

/* Returns whether random crits are enabled on the server */
bool CCritHack::AreRandomCritsEnabled()
{
	if (static auto tf_weapon_criticals = g_Interfaces.CVars->FindVar("tf_weapon_criticals"); tf_weapon_criticals) {
		return tf_weapon_criticals->GetBool();
	}
	return true;
}

/* Returns whether the crithack should run */
bool CCritHack::IsEnabled()
{
	if (!Vars::CritHack::Active.m_Var) { return false; }
	if (!AreRandomCritsEnabled()) { return false; }
	if (!g_Interfaces.Engine->IsInGame()) { return false; }

	return true;
}

float CCritHack::GetWithdrawMult(CBaseCombatWeapon* pWeapon)
{
	const auto count = static_cast<float>(*reinterpret_cast<int*>(pWeapon + 0xa5c) + 1);
	const auto checks = static_cast<float>(*reinterpret_cast<int*>(pWeapon + 0xa58) + 1);

	float multiply = 0.5;
	if (pWeapon->GetSlot() != 2) { multiply = Math::RemapValClamped(count / checks, .1f, 1.f, 1.f, 3.f); }

	return multiply * 3.f;
}

float CCritHack::GetWithdrawAmount(CBaseCombatWeapon* pWeapon)
{
	float amount = static_cast<float>(AddedPerShot) * GetWithdrawMult(pWeapon);
	if (pWeapon->IsRapidFire()) {
		amount = TakenPerCrit * GetWithdrawMult(pWeapon);
		reinterpret_cast<int&>(amount) &= ~1;
	}
	return amount;
}

float CCritHack::GetCritCap(CBaseCombatWeapon* pWeapon)
{
	const auto& pLocal = g_EntityCache.m_pLocal;
	if (!pLocal) { return 0.f; }

	const auto critMult = static_cast<float>(pLocal->GetCritMult());
	float chance = .02f;
	if (pWeapon->GetSlot() == 2)
	{
		chance = .15f;
	}

	float multChance = Utils::ATTRIB_HOOK_FLOAT(critMult * chance, "mult_crit_chance", pWeapon, nullptr, 1);
	if (pWeapon->IsRapidFire())
	{
		const float flTotalCritChance = std::clamp(0.02f * critMult, 0.01f, 0.99f);
		constexpr float flCritDuration = 2.0f;
		const float flNonCritDuration = (flCritDuration / flTotalCritChance) - flCritDuration;
		const float flStartCritChance = 1 / flNonCritDuration;
		multChance = Utils::ATTRIB_HOOK_FLOAT(flStartCritChance, "mult_crit_chance", pWeapon, nullptr, 1);
	}

	return multChance;
}

std::pair<float, float> CCritHack::GetCritMultInfo(CBaseCombatWeapon* pWeapon)
{
	float observed = pWeapon->ObservedCritChance();
	float needed = GetCritCap(pWeapon) + .1f;
	return { observed, needed };
}

bool CCritHack::CanWithdrawFromBucket(CBaseCombatWeapon* pWeapon, bool damage = true)
{
	auto bucket = *reinterpret_cast<float*>(pWeapon + 0xA54);
	if (damage) {
		if (bucket < tf_weapon_criticals_bucket_cap->GetFloat()) {
			bucket += static_cast<float>(AddedPerShot);
			bucket = std::min(bucket, tf_weapon_criticals_bucket_cap->GetFloat());
		}
	}

	if (GetWithdrawAmount(pWeapon) > bucket) { return false; }
	return true;
}

bool CCritHack::CanWeaponCrit(CBaseCombatWeapon* pWeapon, bool draw)
{
	if (!AreRandomCritsEnabled() || !AddedPerShot) { return false; }
	// if (!pWeapon->CanFireCriticalShot(false)) { return false; }
	if (GetCritCap(pWeapon) == 0.f) { return false; }
	if (!CanWithdrawFromBucket(pWeapon)) { return false; }
	if (!draw && !pWeapon->IsReadyToFire() && !pWeapon->IsRapidFire()) { return false; }

	auto [observed, needed] = GetCritMultInfo(pWeapon);
	if (observed > needed && pWeapon->GetSlot() != 2) { return false; }

	return true;
}

int CCritHack::GetShotsUntilCrit(CBaseCombatWeapon* pWeapon)
{
	// Backup weapon stats
	const auto backupBucket = *reinterpret_cast<float*>(pWeapon + 0xa54);
	const auto backupAttempts = *reinterpret_cast<float*>(pWeapon + 0xa58);

	int shots;
	for (shots = 0; shots < ShotsToFill + 1; shots++)
	{
		if (CanWithdrawFromBucket(pWeapon, true)) { break; }

		auto bucket = *reinterpret_cast<float*>(pWeapon + 0xa54);
		auto attempts = *reinterpret_cast<float*>(pWeapon + 0xa58);

		if (bucket < tf_weapon_criticals_bucket_cap->GetFloat())
		{
			bucket += static_cast<float>(AddedPerShot);
			bucket = std::min(bucket, tf_weapon_criticals_bucket_cap->GetFloat());
		}

		attempts++;

		*reinterpret_cast<float*>(pWeapon + 0xa54) = bucket;
		*reinterpret_cast<float*>(pWeapon + 0xa58) = attempts;
	}

	// Restore backup
	*reinterpret_cast<float*>(pWeapon + 0xa54) = backupBucket;
	*reinterpret_cast<float*>(pWeapon + 0xa58) = backupAttempts;
	return shots;
}

/* Returns the next crit command number */
int CCritHack::NextCritTick(const CUserCmd* pCmd, int loops = 4096)
{
	static int previousWeapon = 0;
	static int previousCrit = 0;

	const auto& pLocal = g_EntityCache.m_pLocal;
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
		*g_Interfaces.RandomSeed = MD5_PseudoRandom(cmdNum) & MASK_SIGNED;
		if (pWeapon->WillCrit())
		{
			previousCrit = cmdNum;
			previousWeapon = pWeapon->GetIndex();
			foundTick = cmdNum;
			break;
		}
	}

	*g_Interfaces.RandomSeed = seedBackup;
	return foundTick;
}

void CCritHack::Run(CUserCmd* pCmd)
{
	if (!IsEnabled()) { return; }

	const auto& pWeapon = g_EntityCache.m_pLocalWeapon;
	if (!pWeapon) { return; }

	int nextCrit = NextCritTick(pCmd);
	if (nextCrit >= 0 && (pCmd->buttons & IN_ATTACK))
	{
		if (GetAsyncKeyState(Vars::CritHack::CritKey.m_Var) & 0x8000)
		{
			// Force next crit
			pCmd->command_number = nextCrit;
			pCmd->random_seed = MD5_PseudoRandom(nextCrit) & MASK_SIGNED;
		} else
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

	// Update stats
	static int previousWeapon = 0;
	if (AddedPerShot == 0 || previousWeapon != pWeapon->GetIndex())
	{
		const auto& weaponData = pWeapon->GetWeaponData();
		const auto cap = tf_weapon_criticals_bucket_cap->GetFloat();
		int projectilesPerShot = weaponData.m_nBulletsPerShot;
		if (projectilesPerShot >= 1)
		{
			projectilesPerShot = Utils::ATTRIB_HOOK_FLOAT(projectilesPerShot, "mult_bullets_per_shot", pWeapon, nullptr, true);
		}
		else
		{
			projectilesPerShot = 1;
		}

		AddedPerShot = weaponData.m_nDamage;
		AddedPerShot = static_cast<int>(Utils::ATTRIB_HOOK_FLOAT(static_cast<float>(AddedPerShot), "mult_dmg", pWeapon, nullptr, true));
		AddedPerShot *= std::max(1, projectilesPerShot);
		ShotsToFill = static_cast<int>(cap / static_cast<float>(AddedPerShot));

		if (pWeapon->IsRapidFire())
		{
			TakenPerCrit = AddedPerShot;
			TakenPerCrit *= static_cast<int>(2.f / weaponData.m_flTimeFireDelay);
			if (TakenPerCrit * 3 > static_cast<int>(cap))
			{
				TakenPerCrit = static_cast<int>(cap / 3.f);
			}
		}
	}
	previousWeapon = pWeapon->GetIndex();
}

void CCritHack::Draw()
{
	if (!IsEnabled()) { return; }
	if (!g_GlobalInfo.currentUserCmd) { return; }

	const auto& pLocal = g_EntityCache.m_pLocal;
	if (!pLocal) { return; }
	if (!pLocal->IsAlive()) { return; }

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) { return; }

	const bool canCrit = CanWeaponCrit(pWeapon, true);
	if (!canCrit) { ShotsUntilCrit = GetShotsUntilCrit(pWeapon); }

	// This is dumb code pasted from cathook
	if (!ShotsUntilCrit && AddedPerShot && (pWeapon->GetIndex() != LastWeapon || LastCritTick - g_GlobalInfo.currentUserCmd->command_number < 0 || (LastCritTick - g_GlobalInfo.currentUserCmd->command_number) * g_Interfaces.GlobalVars->interval_per_tick > 30))
	{
		LastCritTick = NextCritTick(g_GlobalInfo.currentUserCmd);
	}

	const float bucket = *reinterpret_cast<float*>(pWeapon + 0xA54);
	int currentY = (g_ScreenSize.h / 2) + 150;

	// Are we currently forcing crits?
	if (GetAsyncKeyState(Vars::CritHack::CritKey.m_Var) & 0x8000)
	{
		g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 70, 190, 50, 255 }, ALIGN_CENTERHORIZONTAL, "Forcing crits...");
	}

	if (pWeapon->GetSlot() != SLOT_MELEE)
	{
		if (canCrit)
		{
			const auto bucketText = tfm::format("Bucket: %s / %s", static_cast<int>(bucket), static_cast<int>(tf_weapon_criticals_bucket_cap->GetFloat()));
			g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, bucketText.c_str());

			const int withdrawAmount = GetWithdrawAmount(pWeapon);
			const int potentialCrits = (bucket + AddedPerShot) / withdrawAmount;
			const auto critText = tfm::format("Estimated crits: %s", potentialCrits);
			g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, critText.c_str());
		} else
		{
			g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 195, 50, 20, 255 }, ALIGN_CENTERHORIZONTAL, "CRIT BANNED");

			auto [observed, needed] = GetCritMultInfo(pWeapon);
			const auto critText = tfm::format("%.3f < %.3f", observed, needed);
			g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, critText.c_str());
		}
	}

	LastBucket = bucket;
	LastWeapon = pWeapon->GetIndex();
}

void CCritHack::FireEvent(CGameEvent* pEvent, const FNV1A_t uNameHash)
{
	switch (uNameHash)
	{
		case FNV1A::HashConst("player_hurt"):
		{
			// TODO: This
			break;
		}

		case FNV1A::HashConst("teamplay_round_start"):
		case FNV1A::HashConst("client_disconnect"):
		case FNV1A::HashConst("client_beginconnect"):
		case FNV1A::HashConst("game_newmap"):
		{
			// TODO: Clear CritCmds
			LastCritTick = -1;
			LastBucket = -1.f;

			ShotsUntilCrit = 0;
			AddedPerShot = 0;
			ShotsToFill = 0;
			TakenPerCrit = 0;
			break;
		}
	}
}

void CCritHack::Init()
{
	tf_weapon_criticals = g_Interfaces.CVars->FindVar("tf_weapon_criticals");
	tf_weapon_criticals_bucket_cap = g_Interfaces.CVars->FindVar("tf_weapon_criticals_bucket_cap");
}