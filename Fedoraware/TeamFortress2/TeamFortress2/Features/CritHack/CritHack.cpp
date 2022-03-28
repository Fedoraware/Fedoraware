#include "CritHack.h"
#define MASK_SIGNED 0x7FFFFFFF

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

/* Returns the next crit command number */
int CCritHack::NextCritTick(const CUserCmd* pCmd, int loops = 4096)
{
	const auto& pLocal = g_EntityCache.m_pLocal;
	if (!pLocal) { return -1; }

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) { return -1; }

	// Return previous crit tick if it's still good
	if (PreviousWeapon == pWeapon->GetIndex() && PreviousCrit >= pCmd->command_number) { return PreviousCrit; }

	// Find the next crit tick
	int foundTick = -1;
	const int seedBackup = MD5_PseudoRandom(pCmd->command_number) & MASK_SIGNED;
	for (int i = 0; i < loops; i++)
	{
		const int cmdNum = pCmd->command_number + i;
		*g_Interfaces.RandomSeed = MD5_PseudoRandom(cmdNum) & MASK_SIGNED;
		if (pWeapon->WillCrit())
		{
			PreviousCrit = cmdNum;
			PreviousWeapon = pWeapon->GetIndex();
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

	// TODO: Fix the crit bucket

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
}

void CCritHack::Draw()
{
	if (!IsEnabled()) { return; }

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!pLocal->IsAlive()) { return; }

		const auto& pWeapon = pLocal->GetActiveWeapon();
		if (!pWeapon) { return; }

		const stats_t stats = *reinterpret_cast<stats_t*>(pWeapon + 0xA54);
		const int bucket = static_cast<int>(stats.flCritBucket);
		
		g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 200, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, (std::string("Bucket: ") + std::to_string(bucket)).c_str());
	}
}