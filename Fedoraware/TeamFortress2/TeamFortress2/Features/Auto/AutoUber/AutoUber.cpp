#include "AutoUber.h"

#include "../../Vars.h"
#include "../AutoGlobal/AutoGlobal.h"

const static int changeTimer = 1; // i am lazy to change code, this should be fine.
const static int defaultResistance = 0;

int vaccChangeState = 0;
int vaccChangeTicks = 0;
int vaccIdealResist = 0;
int vaccChangeTimer = 0;

int BulletDangerValue(CBaseEntity* pPatient)
{
	bool anyZoomedSnipers = false;
	bool anyEnemies = false;

	// Find dangerous playes in other team
	for (const auto& player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!player->IsAlive())
			continue;

		if (player->GetDormant())
			continue;

		switch (player->GetClassNum()){
		case 1: if (!(Vars::Triggerbot::Uber::ReactClasses.Value & 1 << 0)) { continue; } break;	//	scout
		case 2: if (!(Vars::Triggerbot::Uber::ReactClasses.Value & 1 << 7)) { continue; } break;	//	sniper
		case 3: if (!(Vars::Triggerbot::Uber::ReactClasses.Value & 1 << 1)) { continue; } break;	//	soldier
		case 6: if (!(Vars::Triggerbot::Uber::ReactClasses.Value & 1 << 4)) { continue; } break;	//	heavy
		case 7: if (!(Vars::Triggerbot::Uber::ReactClasses.Value & 1 << 2)) { continue; } break;	//	pyro
		case 8: if (!(Vars::Triggerbot::Uber::ReactClasses.Value & 1 << 8)) { continue; } break;	//	spy
		case 9: if (!(Vars::Triggerbot::Uber::ReactClasses.Value & 1 << 5)) { continue; } break;	//	engineer
		default: { continue; }
		}

		if (HAS_CONDITION(player, (TFCond_Bonked | TFCond_Cloaked))) //is enemy cloaked/bonked?
		{
			return false;
		}
		
		if (Utils::isFeigningDeath(player)) { 
			return false;
		}

		if (player->GetActiveWeapon()->GetSlot() == SLOT_MELEE)
		{
			return false;
		}

		if (player->GetActiveWeapon()->GetClassID() == ETFClassID::CTFLunchBox || player->GetActiveWeapon()->GetClassID() == ETFClassID::CTFLunchBox_Drink || player->GetActiveWeapon()->GetClassID() == ETFClassID::CTFWeaponPDA)
		{
			return false;
		}

		// Ignore ignored players
		if (F::AutoGlobal.ShouldIgnore(player)) { continue; }

		const Vec3 vAngleTo = Math::CalcAngle(player->GetEyePosition(), pPatient->GetWorldSpaceCenter());
		const float flFOVTo = Math::CalcFov(player->GetEyeAngles(), vAngleTo);

		if (G::PlayerPriority[player->GetIndex()].Mode != 4 && Vars::Triggerbot::Uber::ReactFoV.Value) {
			if ((flFOVTo - (3.f * G::ChokeMap[player->GetIndex()])) > (float)Vars::Triggerbot::Uber::ReactFoV.Value) { continue; }	//	account for choking :D
		}	
	
		if (HAS_CONDITION(player, TFCond_Zoomed))
		{
			anyZoomedSnipers = true;
			if (Utils::VisPos(pPatient, player, pPatient->GetHitboxPos(HITBOX_HEAD), player->GetEyePosition()))
			{
				return 2;
			}
		}


		if (Utils::VisPos(pPatient, player, pPatient->GetHitboxPos(HITBOX_PELVIS),
			player->GetEyePosition()))
		{
			if (const auto& pWeapon = player->GetActiveWeapon())
			{
				if (player->GetClassNum() == CLASS_SPY && pWeapon->GetSlot() == SLOT_PRIMARY || player->GetClassNum() == CLASS_SCOUT || player->GetClassNum() == CLASS_HEAVY || player->GetClassNum() == CLASS_MEDIC || player->GetClassNum() == CLASS_SNIPER || player->GetClassNum() == CLASS_ENGINEER)
				{
					if (pPatient->GetVecOrigin().DistTo(player->GetVecOrigin()) < 350.f ||
						(pPatient->GetVecOrigin().DistTo(player->GetVecOrigin()) < 600.f && 
							(player->GetClassNum() == CLASS_SPY || player->GetClassNum() == CLASS_SCOUT || player->GetClassNum() == CLASS_HEAVY || player->GetClassNum() == CLASS_MEDIC || player->GetClassNum() == CLASS_SNIPER || player->GetClassNum() == CLASS_ENGINEER))) {
						return 2;
					}
				}
			}

			if (player->GetActiveWeapon()->GetClassID() == ETFClassID::CTFShotgun_Pyro || player->GetActiveWeapon()->GetClassID() == ETFClassID::CTFShotgun_Soldier)
			{
				{
					if (pPatient->GetVecOrigin().DistTo(player->GetVecOrigin()) < 50.f ||
						(pPatient->GetVecOrigin().DistTo(player->GetVecOrigin()) < 250.f && (
							(player->GetClassNum() == CLASS_PYRO)))) {
						return 2;
					}

					if (pPatient->GetVecOrigin().DistTo(player->GetVecOrigin()) < 50.f ||
						(pPatient->GetVecOrigin().DistTo(player->GetVecOrigin()) < 250.f && (
							(player->GetClassNum() == CLASS_SOLDIER)))) {
						return 2;
					}
					
				}
			}
			bool anyEnemies = true;
		}

	}

	bool hasHitscan = false;

	for (const auto& pProjectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
	{
		if (pProjectile->GetVelocity().IsZero())
			continue;

		if (pProjectile->GetTeamNum() == pPatient->GetTeamNum())
			continue;

		if (pProjectile->GetClassID() != ETFClassID::CTFProjectile_Arrow &&
		    pProjectile->GetClassID() != ETFClassID::CTFProjectile_EnergyBall &&
		    pProjectile->GetClassID() != ETFClassID::CTFProjectile_EnergyRing)
		     continue;

		const Vec3 vPredicted = (pProjectile->GetAbsOrigin() + pProjectile->GetVelocity());
		const float flHypPred = sqrtf(pPatient->GetVecOrigin().DistToSqr(vPredicted));
		const float flHyp = sqrtf(pPatient->GetVecOrigin().DistToSqr(pProjectile->GetVecOrigin()));
		if (flHypPred < flHyp && pPatient->GetVecOrigin().DistTo(vPredicted) < pProjectile->GetVelocity().Length())
		{
			if (pProjectile->IsCritBoosted()) { return 2; }
			hasHitscan = true;
		}

	}

	if (hasHitscan)
	{
		if (pPatient->GetHealth() < 449)
		{
			return 2;
		}
	}

	return (anyZoomedSnipers || anyEnemies) ? 1 : 0;
}

int FireDangerValue(CBaseEntity* pPatient)
{
		bool shouldSwitch = false;
	for (const auto& player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!player->IsAlive()) //just incase..
			continue; 
		
		if (HAS_CONDITION(pPatient, TFCond_OnFire))
		{
			if (pPatient->GetClassNum() == CLASS_PYRO) { return 1; }
			return 2;
			shouldSwitch = true;
		}
	}

	return shouldSwitch;
}

int BlastDangerValue(CBaseEntity* pPatient)
{
	bool hasRockets = false;

	for (const auto& pProjectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
	{
		if (hasRockets && !pProjectile->IsCritBoosted())
			continue;

		if (pProjectile->GetVelocity().IsZero())
			continue;

		if (pProjectile->GetTouched()) // Ignore landed Stickies
			continue;

		if (pProjectile->GetTeamNum() == pPatient->GetTeamNum())
			continue;

		if (pProjectile->GetClassID() != ETFClassID::CTFProjectile_Rocket &&
		    pProjectile->GetClassID() != ETFClassID::CTFProjectile_SentryRocket &&
		    pProjectile->GetClassID() != ETFClassID::CTFFlameRocket && //cowmangler i think..
		    pProjectile->GetClassID() != ETFClassID::CTFGrenadePipebombProjectile)
			continue;

		// Projectile is getting closer
		/*const Vec3 vPredicted = (pProjectile->GetAbsOrigin() + pProjectile->GetVelocity());
		const float flHypPred = sqrtf(pPatient->GetVecOrigin().DistToSqr(vPredicted));
		const float flHyp = sqrtf(pPatient->GetVecOrigin().DistToSqr(pProjectile->GetVecOrigin()));
		if ( flHypPred < flHyp && pPatient->GetVecOrigin().DistTo(vPredicted) < pProjectile->GetVelocity().Length())	//	this is way too sensitive
		*/
		Vec3 vPredicted = (pProjectile->GetAbsOrigin() + pProjectile->GetVelocity());
		if (pPatient->GetAbsOrigin().DistTo(pProjectile->GetAbsOrigin()) <= 275.f) {
			hasRockets = true;
		}
	}

	if (hasRockets)
	{
		if (pPatient->GetHealth() < 235)
		{
			return 2;
		}
		return 1;
	}

	return 0;
}

int CurrentResistance()
{
	if (const auto& pWeapon = g_EntityCache.GetWeapon())
	{
		return pWeapon->GetChargeResistType();
	}
	return 0;
}

int ChargeCount()
{
	if (const auto& pWeapon = g_EntityCache.GetWeapon())
	{
		if (G::CurItemDefIndex == Medic_s_TheVaccinator) { return pWeapon->GetUberCharge() / 0.25f; }
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
		if (bulletDanger > 1 && Vars::Triggerbot::Uber::BulletRes.Value) { *pShouldPop = true; }
		if (fireDanger > 1 && Vars::Triggerbot::Uber::FireRes.Value) { *pShouldPop = true; }
		if (blastDanger > 1 && Vars::Triggerbot::Uber::BlastRes.Value) { *pShouldPop = true; }
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
	if (!Vars::Triggerbot::Uber::Active.Value || //Not enabled, return
		pWeapon->GetWeaponID() != TF_WEAPON_MEDIGUN || //Not medigun, return
		G::CurItemDefIndex == Medic_s_TheKritzkrieg || //Kritzkrieg,  return
		ChargeCount() < 1) //Not charged
		return;

	//Check local status, if enabled. Don't pop if local already is not vulnerable
	if (Vars::Triggerbot::Uber::PopLocal.Value && pLocal->IsVulnerable())
	{
		m_flHealth = static_cast<float>(pLocal->GetHealth());
		m_flMaxHealth = static_cast<float>(pLocal->GetMaxHealth());

		if (Vars::Triggerbot::Uber::AutoVacc.Value && G::CurItemDefIndex == Medic_s_TheVaccinator)
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
			if (((m_flHealth / m_flMaxHealth) * 100.0f) <= Vars::Triggerbot::Uber::HealthLeft.Value)
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
		if (Vars::Triggerbot::Uber::OnlyFriends.Value && !g_EntityCache.IsFriend(pTarget->GetIndex()))
			return;

		//Check target's status
		m_flHealth = static_cast<float>(pTarget->GetHealth());
		m_flMaxHealth = static_cast<float>(pTarget->GetMaxHealth());

		int iTargetIndex = pTarget->GetIndex();

		if (Vars::Triggerbot::Uber::VoiceCommand.Value)
		{
			for (auto& iEntity : G::MedicCallers)
			{
				if (iEntity == iTargetIndex)
				{
					pCmd->buttons |= IN_ATTACK2;
					break;
				}
			}
		}
		G::MedicCallers.clear();


		if (Vars::Triggerbot::Uber::AutoVacc.Value && G::CurItemDefIndex == Medic_s_TheVaccinator)
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
			if (((m_flHealth / m_flMaxHealth) * 100.0f) <= Vars::Triggerbot::Uber::HealthLeft.Value)
			{
				pCmd->buttons |= IN_ATTACK2; //Target under wanted health percentage, pop
			}
		}
	}
}
