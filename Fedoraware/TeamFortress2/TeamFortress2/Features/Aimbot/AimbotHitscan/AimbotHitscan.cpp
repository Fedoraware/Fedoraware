#include "AimbotHitscan.h"
#include "../../Vars.h"
#include "../../Backtrack/Backtrack.h"
#include "../../Resolver/Resolver.h"

bool IsHitboxValid(int nHitbox, int index, bool bStatic = false)
{
	const int iStatic = Vars::Aimbot::Hitscan::StaticHitboxes.Value;
	switch (nHitbox)
	{
		case -1: return true;
		case HITBOX_HEAD: return (index & (1 << 0) && !(!bStatic && iStatic & (1 << 0)));
		case HITBOX_PELVIS: return (index & (1 << 1) && !(!bStatic && iStatic & (1 << 1)));
		case HITBOX_SPINE_0:
		case HITBOX_SPINE_1:
		case HITBOX_SPINE_2:
		case HITBOX_SPINE_3: return (index & (1 << 2) && !(!bStatic && iStatic & (1 << 2)));
		case HITBOX_UPPERARM_L:
		case HITBOX_LOWERARM_L:
		case HITBOX_HAND_L:
		case HITBOX_UPPERARM_R:
		case HITBOX_LOWERARM_R:
		case HITBOX_HAND_R: return (index & (1 << 3) && !(!bStatic && iStatic & (1 << 3)));
		case HITBOX_HIP_L:
		case HITBOX_KNEE_L:
		case HITBOX_FOOT_L:
		case HITBOX_HIP_R:
		case HITBOX_KNEE_R:
		case HITBOX_FOOT_R: return (index & (1 << 4) && !(!bStatic && iStatic & (1 << 4)));
	}
	return false;
};

int CAimbotHitscan::GetHitbox(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	switch (Vars::Aimbot::Hitscan::AimHitbox.Value)
	{
		case 0: { return HITBOX_HEAD; }
		case 1: { return HITBOX_SPINE_1; }
		case 2:
		{
			const int nClassNum = pLocal->GetClassNum();

			if (nClassNum == CLASS_SNIPER)
			{
				if (G::CurItemDefIndex != Sniper_m_TheSydneySleeper)
				{
					return (pLocal->IsScoped() ? HITBOX_HEAD : HITBOX_SPINE_1);
				}

				return HITBOX_SPINE_1;
			}
			if (nClassNum == CLASS_SPY)
			{
				const bool bIsAmbassador = (G::CurItemDefIndex == Spy_m_TheAmbassador || G::
											CurItemDefIndex == Spy_m_FestiveAmbassador);
				return (bIsAmbassador ? HITBOX_HEAD : HITBOX_SPINE_1);
			}
			return HITBOX_SPINE_1;
		}
	}

	return HITBOX_HEAD;
}

/* Returns the target group of the current weapon */
EGroupType CAimbotHitscan::GetGroupType(CBaseCombatWeapon* pWeapon)
{
	if (pWeapon->GetWeaponID() == TF_WEAPON_MEDIGUN)
	{
		return EGroupType::PLAYERS_TEAMMATES;
	}

	const bool hasPissRifle = (G::CurItemDefIndex == Sniper_m_TheSydneySleeper);
	if (SandvichAimbot::bIsSandvich || hasPissRifle)
	{
		return EGroupType::PLAYERS_ALL;
	}

	return EGroupType::PLAYERS_ENEMIES;
}

/* Returns all valid targets */
std::vector<Target_t> CAimbotHitscan::GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	std::vector<Target_t> validTargets;
	const auto sortMethod = static_cast<ESortMethod>(Vars::Aimbot::Hitscan::SortMethod.Value);

	const Vec3 vLocalPos = pLocal->GetShootPos();
	const Vec3 vLocalAngles = I::EngineClient->GetViewAngles();

	const bool respectFOV = (sortMethod == ESortMethod::FOV) || (Vars::Aimbot::Hitscan::RespectFOV.Value);

	// Players
	if (Vars::Aimbot::Global::AimPlayers.Value)
	{
		int nHitbox = GetHitbox(pLocal, pWeapon);
		const bool bIsMedigun = pWeapon->GetWeaponID() == TF_WEAPON_MEDIGUN;
		const bool hasPissRifle = G::CurItemDefIndex == Sniper_m_TheSydneySleeper;
		const auto groupType = GetGroupType(pWeapon);

		for (const auto& pTarget : g_EntityCache.GetGroup(groupType))
		{
			// Is the target valid and alive?
			if (!pTarget->IsAlive() || pTarget->IsAGhost() || pTarget == pLocal)
			{
				continue;
			}

			// Can we extinguish a teammate using the piss rifle?
			if (hasPissRifle && (pTarget->GetTeamNum() == pLocal->GetTeamNum()))
			{
				if (!Vars::Aimbot::Hitscan::ExtinguishTeam.Value || !pTarget->IsOnFire())
				{
					continue;
				}
			}

			// Can the medigun reach the target?
			if (bIsMedigun && (pLocal->GetWorldSpaceCenter().DistTo(pTarget->GetWorldSpaceCenter()) > 472.f))
			{
				continue;
			}

			// Should we ignore the target?
			if (F::AimbotGlobal.ShouldIgnore(pTarget, bIsMedigun))
			{
				continue;
			}

			// Should we Baim if it would be lethal?
			if (Vars::Aimbot::Global::BAimLethal.Value)
			{
				if (pWeapon->GetChargeDamage() >= pTarget->GetHealth())
				{
					nHitbox = HITBOX_SPINE_1;
				}

				if (G::CurItemDefIndex == Spy_m_TheAmbassador || G::CurItemDefIndex ==
					Spy_m_FestiveAmbassador)
				{
					// This doesn't work
					/*if (pWeapon->GetWeaponData().m_nDamage >= Player->GetHealth())
					{
						nHitbox = HITBOX_PELVIS;
					}*/
					// Min damage is 18, max damage is 51 (non headshot)
					const float flDistTo = pTarget->GetAbsOrigin().DistTo(pLocal->GetAbsOrigin());
					const int nAmbassadorBodyshotDamage = Math::RemapValClamped(flDistTo, 90, 900, 51, 18);

					if (pTarget->GetHealth() < (nAmbassadorBodyshotDamage + 2)) // whatever
					{
						nHitbox = HITBOX_SPINE_1;
					}
				}
			}

			PriorityHitbox = nHitbox;

			Vec3 vPos = pTarget->GetHitboxPos(nHitbox);
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);

			// Should we respect the Aim FOV?
			if (respectFOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value)
			{
				continue;
			}

			// Get the target priority
			const auto& priority = F::AimbotGlobal.GetPriority(pTarget->GetIndex());

			// The target is valid! Add it to the target vector.
			const float flDistTo = vLocalPos.DistTo(vPos);
			validTargets.push_back({
				pTarget, ETargetType::PLAYER, vPos, vAngleTo, flFOVTo, flDistTo, nHitbox, false, priority
								   });
		}
	}

	// Buildings
	if (Vars::Aimbot::Global::AimBuildings.Value)
	{
		for (const auto& pBuilding : g_EntityCache.GetGroup(EGroupType::BUILDINGS_ENEMIES))
		{
			// Is the building valid and alive?
			if (!pBuilding || !pBuilding->IsAlive()) { continue; }

			Vec3 vPos = pBuilding->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);

			// Should we respect the Aim FOV?
			if (respectFOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value)
			{
				continue;
			}

			// The target is valid! Add it to the target vector.
			const float flDistTo = vLocalPos.DistTo(vPos);
			validTargets.push_back({ pBuilding, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	// Stickies
	if (Vars::Aimbot::Global::AimStickies.Value)
	{
		for (const auto& pProjectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
		{
			// Is the projectile a sticky?
			if (!(pProjectile->GetPipebombType() == TYPE_STICKY))
			{
				continue;
			}

			const auto& pOwner = I::ClientEntityList->GetClientEntityFromHandle(reinterpret_cast<int>(pProjectile->GetThrower()));
			if (!pOwner) { continue; }

			if ((!pProjectile->GetTouched()) || (pOwner->GetTeamNum() == pLocal->GetTeamNum()))
			{
				continue;
			}

			Vec3 vPos = pProjectile->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);

			// Should we respect the Aim FOV?
			if (respectFOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value)
			{
				continue;
			}

			// The target is valid! Add it to the target vector.
			const float flDistTo = vLocalPos.DistTo(vPos);
			validTargets.push_back({ pProjectile, ETargetType::STICKY, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	// NPCs
	if (Vars::Aimbot::Global::AimNPC.Value)
	{
		for (const auto& pNPC : g_EntityCache.GetGroup(EGroupType::WORLD_NPC))
		{
			Vec3 vPos = pNPC->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);

			// Should we respect the Aim FOV?
			if (respectFOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value)
			{
				continue;
			}

			// The target is valid! Add it to the target vector.
			const float flDistTo = vLocalPos.DistTo(vPos);
			validTargets.push_back({ pNPC, ETargetType::NPC, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	//Bombs
	if (Vars::Aimbot::Global::AimBombs.Value)
	{
		//TODO: Check if player is close enough to bomb to damage
		for (const auto& pBomb : g_EntityCache.GetGroup(EGroupType::WORLD_BOMBS))
		{
			Vec3 vPos = pBomb->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);

			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);
			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			// Should we respect the Aim FOV?
			if (respectFOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value)
			{
				continue;
			}

			// The target is valid! Add it to the target vector.
			validTargets.push_back({ pBomb, ETargetType::BOMBS, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	// Did we find at least one target?
	return validTargets;
}

bool CAimbotHitscan::ScanHitboxes(CBaseEntity* pLocal, Target_t& target)
{
	const Vec3 vLocalPos = pLocal->GetShootPos();

	if (target.m_TargetType == ETargetType::PLAYER)
	{
		if (Vars::Aimbot::Hitscan::ScanHitboxes.Value == 0) // even if they have multibox stuff on, if they are scanning nothing we return false, frick you.
		{
			return false;
		}

		matrix3x4 boneMatrix[128];
		if (const model_t* pModel = target.m_pEntity->GetModel())
		{
			if (const studiohdr_t* pHDR = I::ModelInfoClient->GetStudioModel(pModel))
			{
				if (target.m_pEntity->SetupBones(boneMatrix, 128, 0x100, I::GlobalVars->curtime))
				{
					if (const mstudiohitboxset_t* pSet = pHDR->GetHitboxSet(target.m_pEntity->GetHitboxSet()))
					{
						for (int nHitbox = -1; nHitbox < target.m_pEntity->GetNumOfHitboxes(); nHitbox++)
						{
							if (!IsHitboxValid(nHitbox, Vars::Aimbot::Hitscan::ScanHitboxes.Value, (target.m_pEntity->GetVelocity().Length() < 10.f))) { continue; }
							if (nHitbox == -1) { nHitbox = PriorityHitbox; }
							else if (nHitbox == PriorityHitbox) { continue; }


							Vec3 vHitbox = target.m_pEntity->GetHitboxPos(nHitbox);

							if (nHitbox == 0 ? Utils::VisPosHitboxId(pLocal, target.m_pEntity, vLocalPos, vHitbox, nHitbox) : Utils::VisPos(pLocal, target.m_pEntity, vLocalPos, vHitbox))
							// properly check if we hit the hitbox we were scanning and not just a hitbox. (only if the hitbox we are scanning is head)
							{
								target.m_vPos = vHitbox;
								target.m_nAimedHitbox = nHitbox;
								target.m_vAngleTo = Math::CalcAngle(vLocalPos, vHitbox);
								return true;
							}

							if (IsHitboxValid(nHitbox, Vars::Aimbot::Hitscan::MultiHitboxes.Value, (target.m_pEntity->GetVelocity().Length() < 10.f)))
							{
								if (const mstudiobbox_t* pBox = pSet->hitbox(nHitbox))
								{
									const float fScale = Vars::Aimbot::Hitscan::PointScale.Value;
									const Vec3 vMins = pBox->bbmin * fScale;
									const Vec3 vMaxs = pBox->bbmax * fScale;

									const std::vector<Vec3> vecPoints = {
										Vec3(vMaxs.x, vMaxs.y, vMins.z),
										Vec3(vMaxs.x, vMins.y, vMins.z),
										Vec3(vMins.x, vMaxs.y, vMins.z),
										Vec3(vMins.x, vMins.y, vMins.z),
										Vec3(vMins.x, vMins.y, vMaxs.z),
										Vec3(vMins.x, vMaxs.y, vMaxs.z),
										Vec3(vMaxs.x, vMaxs.y, vMaxs.z),
										Vec3(vMins.x, vMins.y, vMaxs.z),
									};

									for (const auto& point : vecPoints)
									{
										Vec3 vTransformed = {};
										Math::VectorTransform(point, boneMatrix[pBox->bone], vTransformed);

										if (nHitbox == 0
											? Utils::VisPosHitboxId(pLocal, target.m_pEntity, vLocalPos, vTransformed, nHitbox)
											: Utils::VisPos(pLocal, target.m_pEntity, vLocalPos, vHitbox))
									// there is no need to scan multiple times just because we hit the arm or whatever. Only count as failure if this hitbox was head.
										{
											target.m_vPos = vTransformed;
											target.m_nAimedHitbox = nHitbox;
											target.m_vAngleTo = Math::CalcAngle(vLocalPos, vTransformed);
											target.m_bHasMultiPointed = true;
											return true;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	else if (target.m_TargetType == ETargetType::BUILDING)
	{
		for (int nHitbox = 0; nHitbox < target.m_pEntity->GetNumOfHitboxes(); nHitbox++)
		{
			Vec3 vHitbox = target.m_pEntity->GetHitboxPos(nHitbox);

			if (Utils::VisPos(pLocal, target.m_pEntity, vLocalPos, vHitbox))
			{
				target.m_vPos = vHitbox;
				target.m_vAngleTo = Math::CalcAngle(vLocalPos, vHitbox);
				return true;
			}
		}
	}

	return false;
}

bool CAimbotHitscan::ScanBuildings(CBaseEntity* pLocal, Target_t& target)
{
	if (!Vars::Aimbot::Hitscan::ScanBuildings.Value)
	{
		return false;
	}

	const Vec3 vLocalPos = pLocal->GetShootPos();

	const float pointScale = Vars::Aimbot::Hitscan::PointScale.Value;

	const Vec3 vMins = target.m_pEntity->GetCollideableMins() * pointScale;
	const Vec3 vMaxs = target.m_pEntity->GetCollideableMaxs() * pointScale;

	const std::vector<Vec3> vecPoints = {
		Vec3(vMins.x * 0.9f, ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3(vMaxs.x * 0.9f, ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3(((vMins.x + vMaxs.x) * 0.5f), vMins.y * 0.9f, ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3(((vMins.x + vMaxs.x) * 0.5f), vMaxs.y * 0.9f, ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3(((vMins.x + vMaxs.x) * 0.5f), ((vMins.y + vMaxs.y) * 0.5f), vMins.z * 0.9f),
		Vec3(((vMins.x + vMaxs.x) * 0.5f), ((vMins.y + vMaxs.y) * 0.5f), vMaxs.z * 0.9f)
	};

	const matrix3x4& transform = target.m_pEntity->GetRgflCoordinateFrame();

	for (const auto& point : vecPoints)
	{
		Vec3 vTransformed = {};
		Math::VectorTransform(point, transform, vTransformed);

		if (Utils::VisPos(pLocal, target.m_pEntity, vLocalPos, vTransformed))
		{
			target.m_vPos = vTransformed;
			target.m_vAngleTo = Math::CalcAngle(vLocalPos, vTransformed);
			return true;
		}
	}

	return false;
}

bool CAimbotHitscan::VerifyTarget(CBaseEntity* pLocal, Target_t& target)
{
	switch (target.m_TargetType)
	{
		case ETargetType::PLAYER:
		{
			if (ScanHitboxes(pLocal, target) && !(!F::Backtrack.CanHitOriginal(target.m_pEntity) || G::ChokeMap[target.m_pEntity->GetIndex()] > Vars::Aimbot::Global::TickTolerance.Value || Vars::Aimbot::Hitscan::BackTrackMethod.Value == 4))
			{
				return true;
			}
			if (Vars::Backtrack::Enabled.Value)
			{
				for (int nHitbox = 0; nHitbox < target.m_pEntity->GetNumOfHitboxes(); nHitbox++)
				{
					if (!IsHitboxValid(nHitbox, Vars::Aimbot::Hitscan::ScanHitboxes.Value, (target.m_pEntity->GetVelocity().Length() < 10.f))) { continue; }
					if (std::optional<TickRecord> ValidRecord = F::Backtrack.Aimbot(target.m_pEntity, (BacktrackMode)Vars::Aimbot::Hitscan::BackTrackMethod.Value, nHitbox))
					{
						target.SimTime = ValidRecord->flSimTime;
						target.m_vAngleTo = Math::CalcAngle(pLocal->GetShootPos(), target.m_pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&ValidRecord->BoneMatrix.BoneMatrix)));
						target.ShouldBacktrack = true;
						target.m_nAimedHitbox = nHitbox;
						return true;
					}
				}
			}
			return false;
		}
		case ETargetType::BUILDING:
		{
			if (!Utils::VisPos(pLocal, target.m_pEntity, pLocal->GetShootPos(), target.m_vPos))
			{
				//Sentryguns have hitboxes, it's better to use ScanHitboxes for them
				if (target.m_pEntity->GetClassID() == ETFClassID::CObjectSentrygun ? !ScanHitboxes(pLocal, target) : !ScanBuildings(pLocal, target))
				{
					return false;
				}
			}

			break;
		}

		default:
		{
			if (!Utils::VisPos(pLocal, target.m_pEntity, pLocal->GetShootPos(), target.m_vPos))
			{
				return false;
			}

			break;
		}
	}

	return true;
}

/* Returns all valid and verified targets */
bool CAimbotHitscan::GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& outTarget)
{
	auto validTargets = GetTargets(pLocal, pWeapon);
	if (validTargets.empty()) { return false; }

	// Sort the targets
	const auto& sortMethod = static_cast<ESortMethod>(Vars::Aimbot::Hitscan::SortMethod.Value);
	F::AimbotGlobal.SortTargets(&validTargets, sortMethod);

	// Verify all targets
	for (auto& target : validTargets)
	{
		if (VerifyTarget(pLocal, target))
		{
			outTarget = target;
			return true;
		}
	}

	return false;
}

void CAimbotHitscan::Aim(CUserCmd* pCmd, Vec3& vAngle)
{
	vAngle -= G::PunchAngles;
	Math::ClampAngles(vAngle);

	const int nAimMethod = (Vars::Aimbot::Hitscan::SpectatedSmooth.Value && G::LocalSpectated)
		? 1
		: Vars::Aimbot::Hitscan::AimMethod.Value;

	switch (nAimMethod)
	{
		case 0: //Plain
		{
			pCmd->viewangles = vAngle;
			I::EngineClient->SetViewAngles(pCmd->viewangles);
			break;
		}

		case 1: //Smooth
		{
			if (Vars::Aimbot::Hitscan::SmoothingAmount.Value == 0)
			{
				// plain aim at 0 smoothing factor
				pCmd->viewangles = vAngle;
				I::EngineClient->SetViewAngles(pCmd->viewangles);
				break;
			}
			//Calculate delta of current viewangles and wanted angles
			Vec3 vecDelta = vAngle - I::EngineClient->GetViewAngles();

			//Clamp, keep the angle in possible bounds
			Math::ClampAngles(vecDelta);

			//Basic smooth by dividing the delta by wanted smooth amount
			pCmd->viewangles += vecDelta / Vars::Aimbot::Hitscan::SmoothingAmount.Value;

			//Set the viewangles from engine
			I::EngineClient->SetViewAngles(pCmd->viewangles);
			break;
		}

		case 2: //Silent
		{
			if (Vars::Aimbot::Global::FlickatEnemies.Value && !G::ShouldShift)
			{
				if (G::IsAttacking)
				{
					Utils::FixMovement(pCmd, vAngle);
					pCmd->viewangles = vAngle;
				}
			}
			else
			{
				Utils::FixMovement(pCmd, vAngle);
				pCmd->viewangles = vAngle;
			}
			break;
		}

		default: break;
	}
}

/* Returns whether AutoShoot should fire */
bool CAimbotHitscan::ShouldFire(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const CUserCmd* pCmd, const Target_t& target)
{
	if (!Vars::Aimbot::Global::AutoShoot.Value)
	{
		return false;
	}

	switch (G::CurItemDefIndex)
	{
		case Sniper_m_TheMachina:
		case Sniper_m_ShootingStar:
		{
			if (!pLocal->IsScoped())
			{
				return false;
			}

			break;
		}
		default: break;
	}

	switch (pLocal->GetClassNum())
	{
		case CLASS_SNIPER:
		{
			const bool bIsScoped = pLocal->IsScoped();

			if (Vars::Aimbot::Hitscan::WaitForHeadshot.Value)
			{
				if (G::CurItemDefIndex != Sniper_m_TheClassic
					&& G::CurItemDefIndex != Sniper_m_TheSydneySleeper
					&& !G::WeaponCanHeadShot && bIsScoped)
				{
					return false;
				}
			}

			if (Vars::Aimbot::Hitscan::WaitForCharge.Value && bIsScoped)
			{
				const int nHealth = target.m_pEntity->GetHealth();
				const bool bIsCritBoosted = pLocal->IsCritBoostedNoMini();

				if (target.m_nAimedHitbox == HITBOX_HEAD && G::CurItemDefIndex !=
					Sniper_m_TheSydneySleeper)
				{
					if (nHealth > 150)
					{
						const float flDamage = Math::RemapValClamped(pWeapon->GetChargeDamage(), 0.0f, 150.0f, 0.0f, 450.0f);
						const int nDamage = static_cast<int>(flDamage);

						if (nDamage < nHealth && nDamage != 450)
						{
							return false;
						}
					}

					else
					{
						if (!bIsCritBoosted && !G::WeaponCanHeadShot)
						{
							return false;
						}
					}
				}

				else
				{
					if (nHealth > (bIsCritBoosted ? 150 : 50))
					{
						float flMult = target.m_pEntity->IsInJarate() ? 1.36f : 1.0f;

						if (bIsCritBoosted)
						{
							flMult = 3.0f;
						}

						const float flMax = 150.0f * flMult;
						const int nDamage = static_cast<int>(pWeapon->GetChargeDamage() * flMult);

						if (nDamage < target.m_pEntity->GetHealth() && nDamage != static_cast<int>(flMax))
						{
							return false;
						}
					}
				}
			}

			break;
		}

		case CLASS_SPY:
		{
			if (Vars::Aimbot::Hitscan::WaitForHeadshot.Value && !pWeapon->AmbassadorCanHeadshot())
			{
				if (target.m_nAimedHitbox == HITBOX_HEAD)
				{
					return false;
				}
			}

			break;
		}

		default: break;
	}

	const int nAimMethod = (Vars::Aimbot::Hitscan::SpectatedSmooth.Value && G::LocalSpectated)
		? 1
		: Vars::Aimbot::Hitscan::AimMethod.Value;

	if (nAimMethod == 1)
	{
		Vec3 vForward = {};
		Math::AngleVectors(pCmd->viewangles, &vForward);
		const Vec3 vTraceStart = pLocal->GetShootPos();
		const Vec3 vTraceEnd = (vTraceStart + (vForward * 8192.0f));

		CGameTrace trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pLocal;

		Utils::Trace(vTraceStart, vTraceEnd, (MASK_SHOT /* | CONTENTS_GRATE | MASK_VISIBLE*/), &filter, &trace);

		if (trace.entity != target.m_pEntity)
		{
			return false;
		}

		if (target.m_nAimedHitbox == HITBOX_HEAD)
		{
			if (trace.hitbox != HITBOX_HEAD)
			{
				return false;
			}

			if (!target.m_bHasMultiPointed)
			{
				Vec3 vMins = {}, vMaxs = {}, vCenter = {};
				matrix3x4 matrix = {};

				if (!target.m_pEntity->GetHitboxMinsAndMaxsAndMatrix(HITBOX_HEAD, vMins, vMaxs, matrix, &vCenter))
				{
					return false;
				}

				vMins *= 0.5f;
				vMaxs *= 0.5f;

				if (!Math::RayToOBB(vTraceStart, vForward, vCenter, vMins, vMaxs, matrix))
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool CAimbotHitscan::IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon)
{
	return ((pCmd->buttons & IN_ATTACK) && G::WeaponCanAttack);
}

void BulletTracer(CBaseEntity* pLocal, const Target_t& target)
{
	const Vec3 vecPos = G::CurWeaponType == EWeaponType::PROJECTILE ? G::PredictedPos : target.m_vPos;
	//Color_t Color = (Utils::Rainbow());

	Vec3 shootPos = pLocal->GetShootPos();
	shootPos.z -= 5.0f;
	const Color_t tracerColor = Vars::Visuals::BulletTracerRainbow.Value ? Utils::Rainbow() : Colors::BulletTracer;
	I::DebugOverlay->AddLineOverlayAlpha(shootPos, vecPos, tracerColor.r, tracerColor.g, tracerColor.b, tracerColor.a, true, 5);
}

void CAimbotHitscan::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	static int nLastTracerTick = pCmd->tick_count;
	static int nextSafeTick = pCmd->tick_count;

	if (!Vars::Aimbot::Global::Active.Value)
	{
		return;
	}

	Target_t target = {};

	const int nWeaponID = pWeapon->GetWeaponID();
	const bool bShouldAim = (Vars::Aimbot::Global::AimKey.Value == VK_LBUTTON
							 ? (pCmd->buttons & IN_ATTACK)
							 : F::AimbotGlobal.IsKeyDown());

//Rev minigun if enabled and aimbot active
	if (bShouldAim)
	{
		if (Vars::Aimbot::Hitscan::AutoRev.Value && nWeaponID == TF_WEAPON_MINIGUN)
		{
			pCmd->buttons |= IN_ATTACK2;
		}
	}

	if (GetTarget(pLocal, pWeapon, target) && bShouldAim)
	{
		if (SandvichAimbot::bIsSandvich)
		{
			SandvichAimbot::RunSandvichAimbot(pLocal, pWeapon, pCmd, target.m_pEntity);
		}

		if (nWeaponID != TF_WEAPON_COMPOUND_BOW
			&& pLocal->GetClassNum() == CLASS_SNIPER
			&& pWeapon->GetSlot() == SLOT_PRIMARY)
		{
			const bool bScoped = pLocal->IsScoped();

			if (Vars::Aimbot::Hitscan::AutoScope.Value && !bScoped)
			{
				pCmd->buttons |= IN_ATTACK2;
				return;
			}

			if (Vars::Aimbot::Hitscan::ScopedOnly.Value && !bScoped)
			{
				return;
			}
		}

		G::CurrentTargetIdx = target.m_pEntity->GetIndex();
		G::HitscanRunning = true;
		G::HitscanSilentActive = Vars::Aimbot::Hitscan::AimMethod.Value == 2;

		// Smooth if spectated
		if (Vars::Aimbot::Hitscan::SpectatedSmooth.Value && G::LocalSpectated)
		{
			G::HitscanSilentActive = false;
		}

		if (G::HitscanSilentActive)
		{
			G::AimPos = target.m_vPos;
		}

		if (ShouldFire(pLocal, pWeapon, pCmd, target))
		{
			if (nWeaponID == TF_WEAPON_MINIGUN)
			{
				pCmd->buttons |= IN_ATTACK2;
			}

			if (G::CurItemDefIndex == Engi_s_TheWrangler || G::CurItemDefIndex ==
				Engi_s_FestiveWrangler)
			{
				pCmd->buttons |= IN_ATTACK2;
			}

			pCmd->buttons |= IN_ATTACK;


			/*
			// Get circular gaussian spread. Under some cases we fire a bullet right down the crosshair:
			//	- The first bullet of a spread weapon (except for rapid fire spread weapons like the minigun)
			//	- The first bullet of a non-spread weapon if it's been >1.25 second since firing
			bool bFirePerfect = false;
			if ( iBullet == 0 && pWpn )
			{
				float flTimeSinceLastShot = (gpGlobals->curtime - pWpn->m_flLastFireTime );
				if ( nBulletsPerShot > 1 && flTimeSinceLastShot > 0.25 )
				{
					bFirePerfect = true;
				}
				else if ( nBulletsPerShot == 1 && flTimeSinceLastShot > 1.25 )
				{
					bFirePerfect = true;
				}
			}
			*/

			if (Vars::Aimbot::Hitscan::TapFire.Value && nWeaponID == TF_WEAPON_MINIGUN && !pLocal->IsPrecisionRune())
			{
				const bool bDo = Vars::Aimbot::Hitscan::TapFire.Value == 1
					? pLocal->GetShootPos().DistTo(target.m_vPos) > Vars::Aimbot::Hitscan::TapFireDist.Value
					: true;

				if (bDo && pWeapon->GetWeaponSpread() != 0.f)
				{
					const float flTimeSinceLastShot = (pLocal->GetTickBase() * TICK_INTERVAL) - pWeapon->GetLastFireTime();

					if (pWeapon->GetWeaponData().m_nBulletsPerShot > 1)
					{
						if (flTimeSinceLastShot <= 0.25f)
						{
							pCmd->buttons &= ~IN_ATTACK;
						}
					}
					else if (flTimeSinceLastShot <= 1.25f)
					{
						pCmd->buttons &= ~IN_ATTACK;
					}
				}
			}
		}

		const bool bIsAttacking = IsAttacking(pCmd, pWeapon);

		if (bIsAttacking)
		{
			F::Resolver.Aimbot(target.m_pEntity, target.m_nAimedHitbox == 0);

			G::IsAttacking = true;
			nextSafeTick = pCmd->tick_count; // just in case.weew
			if (Vars::Visuals::BulletTracer.Value && abs(pCmd->tick_count - nLastTracerTick) > 1)
			{
				//bulletTracer(pLocal, Target);
				nLastTracerTick = pCmd->tick_count;
			}
		}

		// Set the players tickcount (Backtrack / Interpolation removal)
		const float flLerpTime = TICKS_TO_TIME(TIME_TO_TICKS(G::LerpTime));
		const float simTime = target.ShouldBacktrack ? target.SimTime : target.m_pEntity->GetSimulationTime();
		if ((Vars::Misc::DisableInterpolation.Value && target.m_TargetType == ETargetType::PLAYER && bIsAttacking) ||
			target.ShouldBacktrack)
		{
			pCmd->tick_count = TIME_TO_TICKS(flLerpTime + simTime);
		}

		Aim(pCmd, target.m_vAngleTo);
	}
}
