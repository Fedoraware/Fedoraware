#include "AimbotHitscan.h"
#include "../../Vars.h"
#include "../../Backtrack/Backtrack.h"
#include "../../PlayerResource/PlayerResource.h"

bool IsHitboxValid(int nHitbox, int index)
{
	switch (nHitbox)
	{
	case HITBOX_HEAD: return (index & (1 << 0));
	case HITBOX_PELVIS: return (index & (1 << 1));
	case HITBOX_SPINE_0:
	case HITBOX_SPINE_1:
	case HITBOX_SPINE_2:
	case HITBOX_SPINE_3: return (index & (1 << 2));
	case HITBOX_UPPERARM_L:
	case HITBOX_LOWERARM_L:
	case HITBOX_HAND_L:
	case HITBOX_UPPERARM_R:
	case HITBOX_LOWERARM_R:
	case HITBOX_HAND_R: return (index & (1 << 3));
	case HITBOX_HIP_L:
	case HITBOX_KNEE_L:
	case HITBOX_FOOT_L:
	case HITBOX_HIP_R:
	case HITBOX_KNEE_R:
	case HITBOX_FOOT_R: return (index & (1 << 4));
	}
	return false;
};

int CAimbotHitscan::GetHitbox(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	switch (Vars::Aimbot::Hitscan::AimHitbox.m_Var)
	{
	case 0: { return HITBOX_HEAD; }
	case 1: { return HITBOX_SPINE_1; }
	case 2:
		{
			const int nClassNum = pLocal->GetClassNum();

			if (nClassNum == CLASS_SNIPER)
			{
				if (g_GlobalInfo.m_nCurItemDefIndex != Sniper_m_TheSydneySleeper)
				{
					return (pLocal->IsScoped() ? HITBOX_HEAD : HITBOX_SPINE_1);
				}

				return HITBOX_SPINE_1;
			}
			if (nClassNum == CLASS_SPY)
			{
				const bool bIsAmbassador = (g_GlobalInfo.m_nCurItemDefIndex == Spy_m_TheAmbassador || g_GlobalInfo.
					m_nCurItemDefIndex == Spy_m_FestiveAmbassador);
				return (bIsAmbassador ? HITBOX_HEAD : HITBOX_SPINE_1);
			}
			return HITBOX_SPINE_1;
		}
	}

	return HITBOX_HEAD;
}

ESortMethod CAimbotHitscan::GetSortMethod()
{
	switch (Vars::Aimbot::Hitscan::SortMethod.m_Var)
	{
	case 0: return ESortMethod::FOV;
	case 1: return ESortMethod::DISTANCE;
	default: return ESortMethod::UNKNOWN;
	}
}

bool CAimbotHitscan::GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	const ESortMethod sortMethod = GetSortMethod();

	if (sortMethod == ESortMethod::FOV)
	{
		g_GlobalInfo.m_flCurAimFOV = Vars::Aimbot::Global::AimFOV.m_Var;
	}

	g_AimbotGlobal.m_vecTargets.clear();

	const Vec3 vLocalPos = pLocal->GetShootPos();
	const Vec3 vLocalAngles = I::Engine->GetViewAngles();

	// Players
	if (Vars::Aimbot::Global::AimPlayers.m_Var)
	{
		int nHitbox = GetHitbox(pLocal, pWeapon);
		const bool bIsMedigun = pWeapon->GetWeaponID() == TF_WEAPON_MEDIGUN;

		for (const auto& pTarget : g_EntityCache.GetGroup(bIsMedigun ? EGroupType::PLAYERS_TEAMMATES : SandvichAimbot::bIsSandvich ? EGroupType::PLAYERS_ALL : EGroupType::PLAYERS_ENEMIES))
		{
			if (!pTarget->IsAlive() || pTarget->IsAGhost())
			{
				continue;
			}

			if (pTarget == pLocal)
			{
				continue;
			}

			if (bIsMedigun && (pLocal->GetWorldSpaceCenter().DistTo(pTarget->GetWorldSpaceCenter()) > 472.f))
			{
				continue;
			}

			if (g_AimbotGlobal.ShouldIgnore(pTarget, bIsMedigun)) { continue; }

			if (Vars::Aimbot::Global::BAimLethal.m_Var)
			{
				if (pWeapon->GetChargeDamage() >= pTarget->GetHealth())
				{
					nHitbox = HITBOX_SPINE_1;
				}

				if (g_GlobalInfo.m_nCurItemDefIndex == Spy_m_TheAmbassador || g_GlobalInfo.m_nCurItemDefIndex ==
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
			const float flFOVTo = sortMethod == ESortMethod::FOV ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.m_Var)
			{
				continue;
			}

			const uint32_t priorityID = g_PR->isValid(pTarget->GetIndex()) ? g_PR->GetAccountID(pTarget->GetIndex()) : 0;
			const auto& priority = g_GlobalInfo.PlayerPriority[priorityID];

			g_AimbotGlobal.m_vecTargets.push_back({
				pTarget, ETargetType::PLAYER, vPos, vAngleTo, flFOVTo, flDistTo, nHitbox, false, priority
			});
		}
	}

	// Buildings
	if (Vars::Aimbot::Global::AimBuildings.m_Var)
	{
		for (const auto& pBuilding : g_EntityCache.GetGroup(EGroupType::BUILDINGS_ENEMIES))
		{
			if (!pBuilding->IsAlive())
			{
				continue;
			}

			Vec3 vPos = pBuilding->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = sortMethod == ESortMethod::FOV ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.m_Var)
			{
				continue;
			}

			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			g_AimbotGlobal.m_vecTargets.push_back({pBuilding, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo});
		}
	}

	// Stickies
	if (Vars::Aimbot::Global::AimStickies.m_Var)
	{
		for (const auto& pProjectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
		{
			if (!(pProjectile->GetPipebombType() == TYPE_STICKY))
			{
				continue;
			}

			const auto owner = I::EntityList->GetClientEntityFromHandle(reinterpret_cast<int>(pProjectile->GetThrower()));

			if ((!pProjectile->GetTouched()) || (owner->GetTeamNum() == pLocal->GetTeamNum()))
			{
				continue;
			}

			Vec3 vPos = pProjectile->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = sortMethod == ESortMethod::FOV ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.m_Var)
			{
				continue;
			}

			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;
			g_AimbotGlobal.m_vecTargets.push_back({pProjectile, ETargetType::STICKY, vPos, vAngleTo, flFOVTo, flDistTo});
		}
	}

	return !g_AimbotGlobal.m_vecTargets.empty();
}

bool CAimbotHitscan::ScanHitboxes(CBaseEntity* pLocal, Target_t& target)
{
	const Vec3 vLocalPos = pLocal->GetShootPos();

	if (target.m_TargetType == ETargetType::PLAYER)
	{
		if (!Vars::Aimbot::Hitscan::ScanHitboxes.m_Var) // even if they have multibox stuff on, if they are scanning nothing we return false, frick you.
		{
			return false;
		}

		matrix3x4 boneMatrix[128];
		if (const model_t* pModel = target.m_pEntity->GetModel())
		{
			if (const studiohdr_t* pHDR = I::ModelInfo->GetStudioModel(pModel))
			{
				if (target.m_pEntity->SetupBones(boneMatrix, 128, 0x100, I::GlobalVars->curtime))
				{
					if (const mstudiohitboxset_t* pSet = pHDR->GetHitboxSet(target.m_pEntity->GetHitboxSet()))
					{
						for (int nHitbox = -1; nHitbox < target.m_pEntity->GetNumOfHitboxes(); nHitbox++)
						{
							if (nHitbox == -1) { nHitbox = PriorityHitbox; }
							else if (nHitbox == PriorityHitbox) { continue; }

							if (!IsHitboxValid(nHitbox, Vars::Aimbot::Hitscan::ScanHitboxes.m_Var)) { continue; }

							Vec3 vHitbox = target.m_pEntity->GetHitboxPos(nHitbox);

							if (nHitbox == 0 ? Utils::VisPosHitboxId(pLocal, target.m_pEntity, vLocalPos, vHitbox, nHitbox) : Utils::VisPos(pLocal, target.m_pEntity, vLocalPos, vHitbox))
							// properly check if we hit the hitbox we were scanning and not just a hitbox. (only if the hitbox we are scanning is head)
							{
								target.m_vPos = vHitbox;
								target.m_vAngleTo = Math::CalcAngle(vLocalPos, vHitbox);
								return true;
							}

							if (IsHitboxValid(nHitbox, Vars::Aimbot::Hitscan::MultiHitboxes.m_Var))
							{
								if (const mstudiobbox_t* pBox = pSet->hitbox(nHitbox))
								{
									const Vec3 vMins = pBox->bbmin;
									const Vec3 vMaxs = pBox->bbmax;

									constexpr float fScale = 0.8f;
									const std::vector<Vec3> vecPoints = {
										Vec3(((vMins.x + vMaxs.x) * 0.5f), (vMins.y * fScale), ((vMins.z + vMaxs.z) * 0.5f)),
										Vec3((vMins.x * fScale), ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
										Vec3((vMaxs.x * fScale), ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f))
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
	if (!Vars::Aimbot::Hitscan::ScanBuildings.m_Var)
	{
		return false;
	}

	const Vec3 vLocalPos = pLocal->GetShootPos();

	const Vec3 vMins = target.m_pEntity->GetCollideableMins();
	const Vec3 vMaxs = target.m_pEntity->GetCollideableMaxs();

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

bool CAimbotHitscan::VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& target)
{
	switch (target.m_TargetType)
	{
	case ETargetType::PLAYER:
		{
			if (Vars::Backtrack::Enabled.m_Var && Vars::Backtrack::Aim.m_Var && Vars::Aimbot::Hitscan::AimMethod.m_Var != 1)
			{
				Vec3 hitboxPos;
				if (!g_Backtrack.Record[target.m_pEntity->GetIndex()].empty())
				{
					auto& lastTick = g_Backtrack.Record[target.m_pEntity->GetIndex()].back();
					if (const auto& pHdr = lastTick.HDR)
					{
						if (const auto& pSet = pHdr->GetHitboxSet(lastTick.HitboxSet))
						{
							if (const auto& pBox = pSet->hitbox(target.m_nAimedHitbox))
							{
								const Vec3 vPos = (pBox->bbmin + pBox->bbmax) * 0.5f;
								Vec3 vOut;
								Math::VectorTransform(vPos, reinterpret_cast<matrix3x4*>(&lastTick.BoneMatrix)[pBox->bone], vOut);
								hitboxPos = vOut;
							}
						}
					}


					if (Utils::VisPos(pLocal, target.m_pEntity, pLocal->GetShootPos(), hitboxPos))
					{
						target.m_vAngleTo = Math::CalcAngle(pLocal->GetShootPos(), hitboxPos);
						return true;
					}
				}
			}
			if (!ScanHitboxes(pLocal, target))
			{
				return false;
			}

			break;
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

bool CAimbotHitscan::GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& outTarget)
{
	if (!GetTargets(pLocal, pWeapon))
	{
		return false;
	}

	g_AimbotGlobal.SortTargets(GetSortMethod());

	for (auto& target : g_AimbotGlobal.m_vecTargets)
	{
		if (!VerifyTarget(pLocal, pWeapon, target))
		{
			continue;
		}

		outTarget = target;
		return true;
	}

	return false;
}

void CAimbotHitscan::Aim(CUserCmd* pCmd, Vec3& vAngle)
{
	vAngle -= g_GlobalInfo.m_vPunchAngles;
	Math::ClampAngles(vAngle);

	const int nAimMethod = (Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var && g_GlobalInfo.m_bLocalSpectated)
		                       ? 1
		                       : Vars::Aimbot::Hitscan::AimMethod.m_Var;

	switch (nAimMethod)
	{
	case 0: //Plain
		{
			pCmd->viewangles = vAngle;
			I::Engine->SetViewAngles(pCmd->viewangles);
			break;
		}

	case 1: //Smooth
		{
			if (Vars::Aimbot::Hitscan::SmoothingAmount.m_Var == 0)
			{
				// plain aim at 0 smoothing factor
				pCmd->viewangles = vAngle;
				I::Engine->SetViewAngles(pCmd->viewangles);
				break;
			}
			//Calculate delta of current viewangles and wanted angles
			Vec3 vecDelta = vAngle - I::Engine->GetViewAngles();

			//Clamp, keep the angle in possible bounds
			Math::ClampAngles(vecDelta);

			//Basic smooth by dividing the delta by wanted smooth amount
			pCmd->viewangles += vecDelta / Vars::Aimbot::Hitscan::SmoothingAmount.m_Var;

			//Set the viewangles from engine
			I::Engine->SetViewAngles(pCmd->viewangles);
			break;
		}

	case 2: //Silent
		{
			if (Vars::AntiHack::AntiAim::invalidshootpitch.m_Var && Vars::AntiHack::AntiAim::Active.m_Var && ((Vars::AntiHack::AntiAim::YawReal.m_Var && Vars::AntiHack::AntiAim::YawFake.m_Var) ||
				Vars::AntiHack::AntiAim::Pitch.m_Var))
			{
				g_GlobalInfo.m_bFakeShotPitch = true;

				if (vAngle.x > 0.f)
				{
					vAngle.x = Math::RemapValClamped(vAngle.x, 0.0f, 89.0f, 180.0f, 91.0f);
				}
				else
				{
					vAngle.x = Math::RemapValClamped(vAngle.x, 0.0f, -89.0f, -180.0f, -91.0f);
				}

				vAngle.y = Math::RemapValClamped(vAngle.y, 0.0f, 180.0f, -180.0f, 0.0f);
			}
			if (g_GlobalInfo.m_bAttacking)
			{
				Utils::FixMovement(pCmd, vAngle);
				pCmd->viewangles = vAngle;
			}
			break;
		}

	default: break;
	}
}

bool CAimbotHitscan::ShouldFire(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, const Target_t& target)
{
	if (!Vars::Aimbot::Global::AutoShoot.m_Var)
	{
		return false;
	}

	switch (g_GlobalInfo.m_nCurItemDefIndex)
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

			if (Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var)
			{
				if (g_GlobalInfo.m_nCurItemDefIndex != Sniper_m_TheClassic
					&& g_GlobalInfo.m_nCurItemDefIndex != Sniper_m_TheSydneySleeper
					&& !g_GlobalInfo.m_bWeaponCanHeadShot && bIsScoped)
				{
					return false;
				}
			}

			if (Vars::Aimbot::Hitscan::WaitForCharge.m_Var && bIsScoped)
			{
				const int nHealth = target.m_pEntity->GetHealth();
				const bool bIsCritBoosted = pLocal->IsCritBoostedNoMini();

				if (target.m_nAimedHitbox == HITBOX_HEAD && g_GlobalInfo.m_nCurItemDefIndex !=
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
						if (!bIsCritBoosted && !g_GlobalInfo.m_bWeaponCanHeadShot)
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
			if (Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var && !pWeapon->AmbassadorCanHeadshot())
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

	const int nAimMethod = (Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var && g_GlobalInfo.m_bLocalSpectated)
		                       ? 1
		                       : Vars::Aimbot::Hitscan::AimMethod.m_Var;

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
	return ((pCmd->buttons & IN_ATTACK) && g_GlobalInfo.m_bWeaponCanAttack);
}

void BulletTracer(CBaseEntity* pLocal, const Target_t& target)
{
	const Vec3 vecPos = g_GlobalInfo.m_WeaponType == EWeaponType::PROJECTILE ? g_GlobalInfo.m_vPredictedPos : target.m_vPos;
	//Color_t Color = (Utils::Rainbow());

	Vec3 shootPos = pLocal->GetShootPos();
	shootPos.z -= 5.0f;
	const Color_t tracerColor = Vars::Visuals::BulletTracerRainbow.m_Var ? Utils::Rainbow() : Colors::BulletTracer;
	I::DebugOverlay->AddLineOverlayAlpha(shootPos, vecPos, tracerColor.r, tracerColor.g, tracerColor.b, tracerColor.a, true, 5);
}


void CAimbotHitscan::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	static int nLastTracerTick = pCmd->tick_count;

	if (!Vars::Aimbot::Global::Active.m_Var)
	{
		return;
	}

	Target_t target = {};

	const int nWeaponID = pWeapon->GetWeaponID();
	const bool bShouldAim = (Vars::Aimbot::Global::AimKey.m_Var == VK_LBUTTON
		                         ? (pCmd->buttons & IN_ATTACK)
		                         : g_AimbotGlobal.IsKeyDown());

	//Rev minigun if enabled and aimbot active
	if (bShouldAim)
	{
		if (Vars::Aimbot::Hitscan::AutoRev.m_Var && nWeaponID == TF_WEAPON_MINIGUN)
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

			if (Vars::Aimbot::Hitscan::AutoScope.m_Var && !bScoped)
			{
				pCmd->buttons |= IN_ATTACK2;
				return;
			}

			if (Vars::Aimbot::Hitscan::ScopedOnly.m_Var && !bScoped)
			{
				return;
			}
		}

		//if (Vars::Misc::CL_Move::WaitForDT.m_Var)
		//{
		//	if (g_GlobalInfo.m_nWaitForShift && g_GlobalInfo.m_nShifted &&
		//		GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var))
		//		//if dt not ready and "ticks" = 0 and key is held, dont aimbot
		//		return;
		//}

		g_GlobalInfo.m_nCurrentTargetIdx = target.m_pEntity->GetIndex();
		g_GlobalInfo.m_bHitscanRunning = true;
		g_GlobalInfo.m_bHitscanSilentActive = Vars::Aimbot::Hitscan::AimMethod.m_Var == 2;

		if (Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var && g_GlobalInfo.m_bLocalSpectated)
		{
			g_GlobalInfo.m_bHitscanSilentActive = false;
		}

		if (g_GlobalInfo.m_bHitscanSilentActive)
		{
			g_GlobalInfo.m_vAimPos = target.m_vPos;
		}

		if (ShouldFire(pLocal, pWeapon, pCmd, target))
		{
			if (nWeaponID == TF_WEAPON_MINIGUN)
			{
				pCmd->buttons |= IN_ATTACK2;
			}

			if (g_GlobalInfo.m_nCurItemDefIndex == Engi_s_TheWrangler || g_GlobalInfo.m_nCurItemDefIndex ==
				Engi_s_FestiveWrangler)
			{
				pCmd->buttons |= IN_ATTACK2;
			}

			pCmd->buttons |= IN_ATTACK;

			if (Vars::Misc::CL_Move::Enabled.m_Var && Vars::Misc::CL_Move::Doubletap.m_Var && (pCmd->buttons & IN_ATTACK) && g_GlobalInfo.m_nShifted && !g_GlobalInfo.m_nWaitForShift)
			{
				if (
					(Vars::Misc::CL_Move::DTMode.m_Var == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) ||
					(Vars::Misc::CL_Move::DTMode.m_Var == 1) ||
					(Vars::Misc::CL_Move::DTMode.m_Var == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)))
				{
					if ((Vars::Misc::CL_Move::NotInAir.m_Var && !pLocal->IsOnGround() && g_GlobalInfo.m_nShifted))
					{
						g_GlobalInfo.m_bShouldShift = false;
					}
					else
					{
						g_GlobalInfo.m_bShouldShift = true;
					}
				}
			}

			if (Vars::Aimbot::Hitscan::TapFire.m_Var && nWeaponID == TF_WEAPON_MINIGUN)
			{
				const bool bDo = Vars::Aimbot::Hitscan::TapFire.m_Var == 1
					                 ? pLocal->GetAbsOrigin().DistTo(target.m_pEntity->GetAbsOrigin()) > 1000.0f
					                 : true;

				if (bDo && pWeapon->GetWeaponSpread())
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
			g_GlobalInfo.m_bAttacking = true;
			if (Vars::Visuals::BulletTracer.m_Var && abs(pCmd->tick_count - nLastTracerTick) > 1)
			{
				//bulletTracer(pLocal, Target);
				nLastTracerTick = pCmd->tick_count;
			}
		}

		if (Vars::Misc::DisableInterpolation.m_Var && target.m_TargetType == ETargetType::PLAYER && bIsAttacking)
		{
			pCmd->tick_count = TIME_TO_TICKS(target.m_pEntity->GetSimulationTime() +
				std::max(g_ConVars.cl_interp->GetFloat(), g_ConVars.cl_interp_ratio->GetFloat() / g_ConVars.
					cl_updaterate->GetFloat()));
		}

		Aim(pCmd, target.m_vAngleTo);
	}
}
