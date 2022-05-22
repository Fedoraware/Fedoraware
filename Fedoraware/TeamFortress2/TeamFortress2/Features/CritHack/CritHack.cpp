#include "CritHack.h"
#define MASK_SIGNED 0x7FFFFFFF

// i hate crithack

/* Returns whether random crits are enabled on the server */
bool CCritHack::AreRandomCritsEnabled()
{
	if (static auto tf_weapon_criticals = I::CVars->FindVar("tf_weapon_criticals"); tf_weapon_criticals) {
		return tf_weapon_criticals->GetBool();
	}
	return true;
}

/* Returns whether the crithack should run */
bool CCritHack::IsEnabled()
{
	if (!Vars::CritHack::Active.m_Var) { return false; }
	if (!AreRandomCritsEnabled()) { return false; }
	if (!I::Engine->IsInGame()) { return false; }

	return true;
}

bool CCritHack::ShouldCrit()
{
	static KeyHelper critKey{ &Vars::CritHack::CritKey.m_Var };
	if (critKey.Down()) { return true; }
	if (g_GlobalInfo.m_WeaponType == EWeaponType::MELEE && Vars::CritHack::AlwaysMelee.m_Var) { return true; }

	return false;
}

/* Returns the next crit command number */
int CCritHack::NextCritTick(const CUserCmd* pCmd, int loops = 16)
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
		*I::RandomSeed = MD5_PseudoRandom(cmdNum) & MASK_SIGNED;
		if (pWeapon->WillCrit())
		{
			previousCrit = cmdNum;
			previousWeapon = pWeapon->GetIndex();
			foundTick = cmdNum;
			break;
		}
	}

	*I::RandomSeed = seedBackup;
	return foundTick;
}

void CCritHack::Run(CUserCmd* pCmd)
{
	if (!IsEnabled()) { return; }

	const auto& pWeapon = g_EntityCache.m_pLocalWeapon;
	if (!pWeapon || !pWeapon->CanFireCriticalShot(false)) { return; }

	int nextCrit = NextCritTick(pCmd);
	if (nextCrit >= 0 && (pCmd->buttons & IN_ATTACK))
	{
		if (ShouldCrit())
		{
			// Force next crit
			pCmd->command_number = nextCrit;
			pCmd->random_seed = MD5_PseudoRandom(nextCrit) & MASK_SIGNED;
		} 
		else if (Vars::CritHack::avoidrandom.m_Var)
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
	if (!IsEnabled()) { return; }
	if (!g_GlobalInfo.currentUserCmd) { return; }
	if (!Vars::CritHack::indicators.m_Var) { return; }

	const auto& pLocal = g_EntityCache.m_pLocal;
	if (!pLocal) { return; }
	if (!pLocal->IsAlive()) { return; }

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) { return; }

	const float bucket = *reinterpret_cast<float*>(pWeapon + 0xA54);
	int currentY = (g_ScreenSize.h / 2) + 150;

	// Are we currently forcing crits?
	if (ShouldCrit())
	{
		g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 70, 190, 50, 255 }, ALIGN_CENTERHORIZONTAL, "Forcing crits...");
	}

	const float bucketCap = I::CVars->FindVar("tf_weapon_criticals_bucket_cap")->GetFloat();
	const auto bucketText = tfm::format("Bucket: %s / %s", static_cast<int>(bucket), bucketCap);
	g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, bucketText.c_str());
}
