#include "AutoUber.h"

#include "../../Vars.h"
#include "../AutoGlobal/AutoGlobal.h"

const static int changeTimer = 50;
const static int defaultResistance = 0;

int vaccChangeState = 0;
int vaccChangeTicks = 0;
int vaccIdealResist = 0;
int vaccChangeTimer = 0;

int BulletDangerValue(CBaseEntity* pPatient)
{
	bool anyZoomedSnipers = false;
	bool anyHeavys = false;

	// Find dangerous snipers in other team
	for (const auto& player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!player->IsAlive())
			continue;

		if (player->GetClassNum() != CLASS_SNIPER &&
			player->GetClassNum() != CLASS_HEAVY)
			continue;

		// Ignore ignored players
		PlayerInfo_t pInfo;
		g_Interfaces.Engine->GetPlayerInfo(player->GetIndex(), &pInfo);
		if (g_GlobalInfo.ignoredPlayers.find(pInfo.friendsID) != g_GlobalInfo.ignoredPlayers.end()) { continue; }

		// Check for any zoomed snipers
		if (HAS_CONDITION(player, TFCond_Zoomed))
		{
			anyZoomedSnipers = true;
			if (Utils::VisPos(pPatient, player, pPatient->GetHitboxPos(HITBOX_HEAD), player->GetHitboxPos(HITBOX_HEAD)))
			{
				return 2;
			}
		}

		// Check for any dangerous heavys
		if (HAS_CONDITION(player, TFCond_Slowed))
		{
			if (const auto& pWeapon = player->GetActiveWeapon())
			{
				if (Utils::VisPos(pPatient, player, pPatient->GetHitboxPos(HITBOX_PELVIS),
				                  player->GetHitboxPos(HITBOX_HEAD)))
				{
					if (pPatient->GetVecOrigin().DistTo(player->GetVecOrigin()) < 350.f ||
						(pPatient->GetVecOrigin().DistTo(player->GetVecOrigin()) < 600.f && (
							HAS_CONDITION(player, TFCond_Ubercharged) ||
							HAS_CONDITION(player, TFCondEx2_BulletImmune))))
					{
						return 2;
					}
					anyHeavys = true;
				}
			}
		}
	}

	return (anyZoomedSnipers || anyHeavys) ? 1 : 0;
}

int FireDangerValue(CBaseEntity* pPatient)
{
	int shouldSwitch = 0;

	for (const auto& player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!player->IsAlive())
			continue;

		if (player->GetClassNum() != CLASS_PYRO) // Pyro only
			continue;

		if (pPatient->GetVecOrigin().DistTo(player->GetVecOrigin()) > 450.f)
			continue;

		if (player->GetActiveWeapon()->GetClassID() == ETFClassID::CTFFlameThrower)
		{
			if (HAS_CONDITION(pPatient, TFCond_OnFire))
			{
				if (pPatient->GetClassNum() == CLASS_PYRO) { return 1; }
				return 2;
			}

			if (HAS_CONDITION(player, TFCondEx_PhlogUber)) { return 2; }
			shouldSwitch = 1;
		}
	}

	return shouldSwitch;
}

int BlastDangerValue(CBaseEntity* pPatient)
{
	bool hasRockets = false;

	for (const auto& pProjectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
	{
		if (pProjectile->GetVelocity().IsZero())
			continue;

		if (pProjectile->GetTouched()) // Ignore landed Stickies
			continue;

		if (pProjectile->GetTeamNum() == pPatient->GetTeamNum())
			continue;

		if (pProjectile->GetClassID() != ETFClassID::CTFProjectile_Rocket &&
			pProjectile->GetClassID() != ETFClassID::CTFGrenadePipebombProjectile)
			continue;

		// Projectile is getting closer
		Vec3 vPredicted = (pProjectile->GetAbsOrigin() + pProjectile->GetVelocity());
		if (pPatient->GetVecOrigin().DistToSqr(pProjectile->GetVecOrigin()) > pPatient->GetVecOrigin().
			DistToSqr(vPredicted) &&
			pPatient->GetVecOrigin().DistTo(vPredicted) < 200.f)
		{
			if (pProjectile->IsCritBoosted()) { return 2; }
			hasRockets = true;
		}
	}

	if (hasRockets)
	{
		if (pPatient->GetHealth() < 80)
		{
			return 2;
		}
		return 1;
	}

	return 0;
}

int CurrentResistance()
{
	if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
	{
		return pWeapon->GetChargeResistType();
	}
	return 0;
}

int ChargeCount()
{
	if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
	{
		if (g_GlobalInfo.m_nCurItemDefIndex == Medic_s_TheVaccinator) { return pWeapon->GetUberCharge() / 0.25f; }
		return pWeapon->GetUberCharge() / 1.f;
	}
	return 1;
}

int OptimalResistance(CBaseEntity* pPatient, bool* pShouldPop)
{
	int bulletDanger = BulletDangerValue(pPatient);
	int fireDanger = FireDangerValue(pPatient);
	int blastDanger = BlastDangerValue(pPatient);
	if (pShouldPop)
	{
		int charges = ChargeCount();
		if (bulletDanger > 1) { *pShouldPop = true; }
		if (fireDanger > 1) { *pShouldPop = true; }
		if (blastDanger > 1) { *pShouldPop = true; }
	}

	if (!(bulletDanger || fireDanger || blastDanger))
	{
		return -1;
	}

	vaccChangeTimer = changeTimer;

	// vaccinator_change_timer = (int) change_timer;
	if (bulletDanger >= fireDanger && bulletDanger >= blastDanger) { return 0; }
	if (blastDanger >= fireDanger && blastDanger >= bulletDanger) { return 1; }
	if (fireDanger >= bulletDanger && fireDanger >= blastDanger) { return 2; }
	return -1;
}

void SetResistance(int pResistance)
{
	Math::Clamp(pResistance, 0, 2);
	vaccChangeTimer = changeTimer;
	vaccIdealResist = pResistance;

	int curResistance = CurrentResistance();
	if (pResistance == curResistance) { return; }
	if (pResistance > curResistance)
	{
		vaccChangeState = pResistance - curResistance;
	}
	else
	{
		vaccChangeState = 3 - curResistance + pResistance;
	}
}

void DoResistSwitching(CUserCmd* pCmd)
{
	if (vaccChangeTimer > 0)
	{
		if (vaccChangeTimer == 1 && false)
		{
			SetResistance(defaultResistance - 1);
		}
		vaccChangeTimer--;
	}
	else
	{
		vaccChangeTimer = changeTimer;
	}

	if (!vaccChangeState) { return; }
	if (CurrentResistance() == vaccIdealResist)
	{
		vaccChangeTicks = 0;
		vaccChangeState = 0;
		return;
	}
	if (pCmd->buttons & IN_RELOAD)
	{
		vaccChangeTicks = 8;
		return;
	}
	if (vaccChangeTicks <= 0)
	{
		pCmd->buttons |= IN_RELOAD;
		vaccChangeState--;
		vaccChangeTicks = 8;
	}
	else
	{
		vaccChangeTicks--;
	}
}

void CAutoUber::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!Vars::Triggerbot::Uber::Active.m_Var || //Not enabled, return
		pWeapon->GetWeaponID() != TF_WEAPON_MEDIGUN || //Not medigun, return
		g_GlobalInfo.m_nCurItemDefIndex == Medic_s_TheKritzkrieg || //Kritzkrieg,  return
		ChargeCount() < 1) //Not charged
		return;

	//Check local status, if enabled. Don't pop if local already is not vulnerable
	if (Vars::Triggerbot::Uber::PopLocal.m_Var && pLocal->IsVulnerable())
	{
		m_flHealth = static_cast<float>(pLocal->GetHealth());
		m_flMaxHealth = static_cast<float>(pLocal->GetMaxHealth());

		if (Vars::Triggerbot::Uber::AutoVacc.m_Var && g_GlobalInfo.m_nCurItemDefIndex == Medic_s_TheVaccinator)
		{
			// Auto vaccinator
			bool shouldPop = false;
			DoResistSwitching(pCmd);

			int optResistance = OptimalResistance(pLocal, &shouldPop);
			if (optResistance >= 0 && optResistance != CurrentResistance())
			{
				SetResistance(optResistance);
			}

			if (shouldPop && CurrentResistance() == optResistance)
			{
				pCmd->buttons |= IN_ATTACK2;
			}
		}
		else
		{
			// Default medigun
			if (((m_flHealth / m_flMaxHealth) * 100.0f) <= Vars::Triggerbot::Uber::HealthLeft.m_Var)
			{
				pCmd->buttons |= IN_ATTACK2; //We under the wanted health percentage, pop
				return; //Popped, no point checking our target's status
			}
		}
	}

	//Will be null as long as we aren't healing anyone
	if (const auto& pTarget = pWeapon->GetHealingTarget())
	{
		//Ignore if target is somehow dead, or already not vulnerable
		if (!pTarget->IsAlive() || !pTarget->IsVulnerable())
			return;

		//Dont waste if not a friend, fuck off scrub
		if (Vars::Triggerbot::Uber::OnlyFriends.m_Var && !g_EntityCache.Friends[pTarget->GetIndex()])
			return;

		//Check target's status
		m_flHealth = static_cast<float>(pTarget->GetHealth());
		m_flMaxHealth = static_cast<float>(pTarget->GetMaxHealth());

		if (Vars::Triggerbot::Uber::AutoVacc.m_Var && g_GlobalInfo.m_nCurItemDefIndex == Medic_s_TheVaccinator)
		{
			// Auto vaccinator
			bool shouldPop = false;
			DoResistSwitching(pCmd);

			int optResistance = OptimalResistance(pTarget, &shouldPop);
			if (optResistance >= 0 && optResistance != CurrentResistance())
			{
				SetResistance(optResistance);
			}

			if (shouldPop && CurrentResistance() == optResistance)
			{
				pCmd->buttons |= IN_ATTACK2;
			}
		}
		else
		{
			// Default mediguns
			if (((m_flHealth / m_flMaxHealth) * 100.0f) <= Vars::Triggerbot::Uber::HealthLeft.m_Var)
			{
				pCmd->buttons |= IN_ATTACK2; //Target under wanted health percentage, pop
			}
		}
	}
}
