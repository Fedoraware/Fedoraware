#include "CritHack.h"

#include "../Aimbot/AimbotGlobal/AimbotGlobal.h"
#define MASK_SIGNED 0x7FFFFFFF

// i hate crithack

/* Returns whether random crits are enabled on the server */
bool CCritHack::AreRandomCritsEnabled()
{
	if (static auto tf_weapon_criticals = g_ConVars.FindVar("tf_weapon_criticals"); tf_weapon_criticals)
	{
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
		if (pWeapon->GetWeaponID() == TF_WEAPON_PIPEBOMBLAUNCHER)
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
		else if (pWeapon->GetWeaponID() == TF_WEAPON_CANNON)
		{
			static bool Charging = false;

			if (pWeapon->GetDetonateTime() > 0.0f)
			{
				Charging = true;
			}

			if (!(pCmd->buttons & IN_ATTACK) && Charging)
			{
				Charging = false;
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

bool CCritHack::NoRandomCrits(CBaseCombatWeapon* pWeapon)
{
	float CritChance = Utils::ATTRIB_HOOK_FLOAT(1, "mult_crit_chance", pWeapon, 0, true);
	if (CritChance == 0)
	{
		return true;
	}
	else 
	return false;
	//list of weapons that cant random crit, but dont have the attribute for it
	switch (pWeapon->GetWeaponID())
	{
		//scout
		case TF_WEAPON_JAR_MILK:
		//soldier
		case TF_WEAPON_BUFF_ITEM:
		//pyro
		case TF_WEAPON_JAR_GAS:
		case TF_WEAPON_FLAME_BALL:
		case TF_WEAPON_ROCKETPACK:
		//demo
		case TF_WEAPON_PARACHUTE: //also for soldier
		//heavy
		case TF_WEAPON_LUNCHBOX:
		//engineer
		case TF_WEAPON_PDA_ENGINEER_BUILD:
		case TF_WEAPON_PDA_ENGINEER_DESTROY:
		case TF_WEAPON_LASER_POINTER:
		//medic
		case TF_WEAPON_MEDIGUN:
		//sniper
		case TF_WEAPON_SNIPERRIFLE:
		case TF_WEAPON_SNIPERRIFLE_CLASSIC:
		case TF_WEAPON_SNIPERRIFLE_DECAP:
		case TF_WEAPON_COMPOUND_BOW:
		case TF_WEAPON_JAR:
		//spy
		case TF_WEAPON_KNIFE:
		case TF_WEAPON_PDA_SPY_BUILD:
		case TF_WEAPON_PDA_SPY:
			return true;
			break;
		default: return false; break;
	}
}

bool CCritHack::ShouldCrit()
{
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!pLocal) { return false; }	//	will never hit
	static KeyHelper critKey{ &Vars::CritHack::CritKey.Value };
	if (critKey.Down() || !Vars::CritHack::CritKey.Value) { return true; }
	if (G::CurWeaponType == EWeaponType::MELEE && Vars::CritHack::AlwaysMelee.Value) { return true; }

	//Check if auto melee crit is enabled and player is using melee
	if (Vars::CritHack::AutoMeleeCrit.Value && G::CurWeaponType == EWeaponType::MELEE)
	{
		//Base melee damage
		int MeleeDamage = pLocal->GetClassNum() == ETFClass::CLASS_SCOUT ? 35 : 65;

		//Could be missing wepaons or reskins
		switch (G::CurItemDefIndex)
		{
		case Scout_t_SunonaStick:
		{
			//The Sun on a Stick has a -25% melee damage stat
			MeleeDamage = 26;
			break;
		}
		case Scout_t_TheFanOWar:
		{
			//The Fan O'War has a -75% melee damage stat
			MeleeDamage = 9;
			break;
		}
		case Scout_t_TheWrapAssassin:
		{
			//The Wrap Assassin has a -65% melee damage stat
			MeleeDamage = 12;
			break;
		}
		case Soldier_t_TheDisciplinaryAction:
		case Engi_t_TheJag:
		{
			//The Disciplinary Action and The Jag have a -25% melee damage stat
			MeleeDamage = 49;
			break;
		}
		case Soldier_t_TheEqualizer:
		{
			MeleeDamage = std::ceil(107.25 - (0.37295 * pLocal->GetHealth()));
			//The Equalizer does more damage the lower the local player's health is
			break;
		}
		case Pyro_t_HotHand:
		{
			//The Hot Hand has a -20% melee damage stat (and attacks twice)
			MeleeDamage = 56;
			break;
		}
		case Pyro_t_SharpenedVolcanoFragment:
		case Medic_t_Amputator:
		{
			//The Sharpened Volcano Fragment and The Amputator have a -20% melee damage stat
			MeleeDamage = 52;
			break;
		}
		case Pyro_t_TheBackScratcher:
		{
			//The Back Scratcher has a +25% melee damage stat
			MeleeDamage = 81;
			break;
		}
		case Demoman_t_TheScotsmansSkullcutter:
		{
			//The Scotsmans Skullcutter has a +20% melee damage stat
			MeleeDamage = 78;
			break;
		}
		case Heavy_t_WarriorsSpirit:
		{
			//The Warriors Spirit has a +30% melee damage stat
			MeleeDamage = 85;
			break;
		}
		case Sniper_t_TheTribalmansShiv:
		{
			//The Tribalmans Shiv has a -50% melee damage stat
			MeleeDamage = 37;
			break;
		}
		case Sniper_t_TheShahanshah:
		{
			const int iHalfHealth = pLocal->GetMaxHealth() / 2;
			MeleeDamage = pLocal->GetHealth() > iHalfHealth ? 49 : 81;
			//The Shahanshah has a -25% melee damage stat when above half health and a +25% when below half health
			//81 if below half health
			//49 if above half health
			break;
		}
		default: break;
		}

		CBaseEntity* pTarget = I::ClientEntityList->GetClientEntity(G::CurrentTargetIdx);
		if (pTarget)
		{
			if (G::CurItemDefIndex == Heavy_t_TheHolidayPunch)
			{
				if ((MeleeDamage >= pTarget->GetHealth()) && pTarget->IsVulnerable()) {
					return false;
				}
				else if (pTarget->OnSolid() && !pTarget->IsTaunting() && !pTarget->GetViewingCYOAPDA()) {
					return true;
				}
				else {
					return false;
				}
			}

			if (MeleeDamage <= pTarget->GetHealth())
				return true;
		}
	}

	return false;
}

int CCritHack::LastGoodCritTick(const CUserCmd* pCmd)
{
	int retVal = -1;
	bool popBack = false;

	for (const auto& tick : CritTicks)
	{
		if (tick >= pCmd->command_number)
		{
			retVal = tick;
		}
		else
		{
			popBack = true;
		}
	}

	if (popBack)
	{
		CritTicks.pop_back();
	}

	return retVal;
}

void CCritHack::ScanForCrits(const CUserCmd* pCmd, int loops)
{
	static int previousWeapon = 0;
	static int previousCrit = 0;

	const auto& pLocal = g_EntityCache.GetLocal();
	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pLocal || !pWeapon) { return; }

	const int seedBackup = *I::RandomSeed;

	if (F::AimbotGlobal.IsAttacking() || IsAttacking(pCmd, pWeapon)/* || pCmd->buttons & IN_ATTACK*/)
	{
		return;
	}

	if (previousWeapon != pWeapon->GetIndex())
	{
		previousWeapon = pWeapon->GetIndex();
		CritTicks.clear();
	}

	if (CritTicks.size() >= 256)
	{
		return;
	}

	//CritBucketBP = *reinterpret_cast<float*>(pWeapon + 0xA54);
	ProtectData = true; //	stop shit that interferes with our crit bucket because it will BREAK it
	for (int i = 0; i < loops; i++)
	{
		*I::RandomSeed = MD5_PseudoRandom(pCmd->command_number + i) & MASK_SIGNED;
		if (pWeapon->WillCrit())
		{
			CritTicks.push_back(pCmd->command_number + i); //	store our wish command number for later reference
			*I::RandomSeed = seedBackup;
			ProtectData = false;
		}
	}
	*I::RandomSeed = seedBackup;
	ProtectData = false; //	we no longer need to be protecting important crit data
	*reinterpret_cast<int*>(pWeapon + 0xA5C) = 0; //	dont comment this out, makes sure our crit mult stays as low as possible
	//	crit mult can reach a maximum value of 3!! which means we expend 3 crits WORTH from our bucket
	//	by forcing crit mult to be its minimum value of 1, we can crit more without directly fucking our bucket
	//	yes ProtectData stops this value from changing artificially, but it still changes when you fire and this is worth it imo.
}

void CCritHack::Run(CUserCmd* pCmd)
{
	if (!IsEnabled()) { return; }

	const auto& pWeapon = g_EntityCache.GetWeapon();
	if (!pWeapon || !pWeapon->CanFireCriticalShot(false)) { return; }

	ScanForCrits(pCmd, 100); //	fill our vector slowly.

	auto OldSeed = *I::RandomSeed;

	const int closestGoodTick = LastGoodCritTick(pCmd); //	retrieve our wish
	if (IsAttacking(pCmd, pWeapon)) //	is it valid & should we even use it
	{
		if (ShouldCrit())
		{
			if (closestGoodTick < 0) { return; }
			pCmd->command_number = closestGoodTick; //	set our cmdnumber to our wish
			pCmd->random_seed = MD5_PseudoRandom(closestGoodTick) & MASK_SIGNED; //	trash poopy whatever who cares
		}
		else if (Vars::CritHack::AvoidRandom.Value) //	we don't want to crit
		{
			for (int tries = 1; tries < 10; tries++)
			{
				*I::RandomSeed = MD5_PseudoRandom(pCmd->command_number + tries) & MASK_SIGNED;
				if (!pWeapon->WillCrit())
				{
					pCmd->command_number += tries;
					break;
				}
			}
			*I::RandomSeed = OldSeed;
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

	const int x = Vars::CritHack::IndicatorPos.Value.c;
	int currentY = Vars::CritHack::IndicatorPos.Value.y;

	const float bucket = *reinterpret_cast<float*>(pWeapon + 0xA54);
	const int seedRequests = *reinterpret_cast<int*>(pWeapon + 0xA5C);

	int longestW = 40;
	const auto& FONT= g_Draw.GetFont(FONT_INDICATORS);

	if (Vars::Debug::DebugInfo.Value)
	{
		//const void* ptr = static_cast<void*>(pWeapon + 0xA54);
		g_Draw.String(FONT, x, currentY += 15, { 255, 255, 255, 255, }, ALIGN_CENTERHORIZONTAL, std::format("{:#x}", reinterpret_cast<uintptr_t>(pWeapon + 0xA54)).c_str());
	}
	//Can this weapon do random crits?
	if (NoRandomCrits(pWeapon) == true)
	{
		g_Draw.String(FONT, x, currentY += 15, { 255, 95, 95, 255 }, ALIGN_CENTERHORIZONTAL, L"No Random Crits");
	}
	// Are we currently forcing crits?
	if (ShouldCrit() && NoRandomCrits(pWeapon) == false)
	{
		g_Draw.String(FONT, x, currentY += 15, { 70, 190, 50, 255 }, ALIGN_CENTERHORIZONTAL, "Forcing crits...");
	}
	//crit banned check
	if (CritTicks.size() == 0 && NoRandomCrits(pWeapon) == false)
	{
		g_Draw.String(FONT, x, currentY += 15, { 255,0,0,255 }, ALIGN_CENTERHORIZONTAL, L"Crit Banned");
	}
	static auto tf_weapon_criticals_bucket_cap = g_ConVars.FindVar("tf_weapon_criticals_bucket_cap");
	const float bucketCap = tf_weapon_criticals_bucket_cap->GetFloat();
	const std::wstring bucketstr = L"Bucket: " + std::to_wstring(static_cast<int>(bucket)) + L"/" + std::to_wstring(static_cast<int>(bucketCap));
	// crit bucket (this sucks)
	if (NoRandomCrits(pWeapon) == false)	
	{
			g_Draw.String(FONT, x, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, bucketstr.c_str()); 
	}
	int w, h;
	I::VGuiSurface->GetTextSize(FONT.dwFont, bucketstr.c_str(), w, h);
	if (w > longestW)
	{
		longestW = w;
	}
	if (Vars::Debug::DebugInfo.Value)
	{
		const std::wstring seedText = L"m_nCritSeedRequests: " + std::to_wstring(seedRequests);
		const std::wstring FoundCrits = L"Found Crit Ticks: " + std::to_wstring(CritTicks.size());
		const std::wstring commandNumber = L"cmdNumber: " + std::to_wstring(G::CurrentUserCmd->command_number);
		g_Draw.String(FONT, x, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, seedText.c_str());
		I::VGuiSurface->GetTextSize(FONT.dwFont, seedText.c_str(), w, h);
		if (w > longestW)
		{
			longestW = w;
		}
		g_Draw.String(FONT, x, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, FoundCrits.c_str());
		I::VGuiSurface->GetTextSize(FONT.dwFont, FoundCrits.c_str(), w, h);
		if (w > longestW)
		{
			longestW = w;
		}
		g_Draw.String(FONT, x, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, commandNumber.c_str());
		I::VGuiSurface->GetTextSize(FONT.dwFont, commandNumber.c_str(), w, h);
		if (w > longestW)
		{
			longestW = w;
		}
	}
	IndicatorW = longestW * 2;
	IndicatorH = currentY;
}
