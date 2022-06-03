#include "NoSpread.h"

/*
 * Credits:
 * - cathook
 * - https://www.unknowncheats.me/forum/team-fortress-2-a/139094-projectile-nospread.html
 */

bool CNoSpread::ShouldRun(bool isProjectile)
{
	if (!Vars::Aimbot::Projectile::NoSpread.Value) { return false; }

	return isProjectile;
}

void CNoSpread::RunLate(CUserCmd* pCmd)
{
	const auto& pLocal = g_EntityCache.GetLocal();
	const auto& pWeapon = g_EntityCache.GetWeapon();
	if (!pLocal || !pWeapon || !ShouldRun(true)) { return; }

	// Set up Random Seed
	const int cmdNum = pCmd->command_number;
	Utils::RandomSeed(MD5_PseudoRandom(cmdNum) & 0x7FFFFFFF);
	Utils::SharedRandomInt(MD5_PseudoRandom(cmdNum) & 0x7FFFFFFF, "SelectWeightedSequence", 0, 0, 0);
	for (int i = 0; i < 6; ++i)
	{
		Utils::RandomFloat();
	}

	// Projectile
	if (G::CurWeaponType != EWeaponType::PROJECTILE)
	{
		return;
	}

	// Beggars check
	if (pWeapon->GetItemDefIndex() == Soldier_m_TheBeggarsBazooka)
	{
		// Player has 0 loaded rockets and reload mode is not 2 (reloading and ready to release)
		const bool noLoadedRockets = pWeapon->GetClip1() == 0 && pWeapon->GetReloadMode() != 2;
		// Player is attacking and reload is not 0 (not reloading)
		const bool loadingRockets = pCmd->buttons & IN_ATTACK && pWeapon->GetReloadMode() != 0;

		if (noLoadedRockets || loadingRockets) { return; }
	}

	// Huntsman check
	else if (pWeapon->GetClassID() == ETFClassID::CTFCompoundBow)
	{
		if (pCmd->buttons & IN_ATTACK || pWeapon->GetChargeBeginTime() == 0)
		{
			return;
		}
	}

	// Rest of weapons
	else if (!(pCmd->buttons & IN_ATTACK))
	{
		return;
	}

	// Run Nospread depending on the weapon
	switch (pWeapon->GetClassID())
	{
		case ETFClassID::CTFSyringeGun:
		{
			constexpr float spread = 1.5f;
			pCmd->viewangles.x -= Utils::RandomFloat(-spread, spread);
			pCmd->viewangles.y -= Utils::RandomFloat(-spread, spread);
			Math::ClampAngles(pCmd->viewangles);
			break;
		}

		default:
		{
			const Vec3 view = pLocal->GetEyeAngles();
			Vec3 spread = pWeapon->GetSpreadAngles();

			if (!spread.IsZero())
			{
				spread -= view;
				pCmd->viewangles -= spread;
				Math::ClampAngles(pCmd->viewangles);
			}
			break;
		}
	}
}
