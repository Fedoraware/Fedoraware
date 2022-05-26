#include "AimbotProjectile.h"
#include "../../Vars.h"
#include "../../PlayerResource/PlayerResource.h"

#include "../MovementSimulation/MovementSimulation.h"

Vec3 CAimbotProjectile::Predictor_t::Extrapolate(float time)
{
	g_GlobalInfo.linearPredLine = m_vPosition;

	Vec3 vecOut;
	if (m_pEntity->IsOnGround())
	{
		vecOut = (m_vPosition + (m_vVelocity * time));
	}
	else
	{
		vecOut = (m_vPosition + (m_vVelocity * time) - m_vAcceleration * 0.5f * time * time);
	}

	return vecOut;
}

bool CAimbotProjectile::GetProjectileInfo(CBaseCombatWeapon* pWeapon, ProjectileInfo_t& out)
{
	switch (g_GlobalInfo.m_nCurItemDefIndex)
	{
	case Soldier_m_RocketLauncher:
	case Soldier_m_RocketLauncherR:
	case Soldier_m_TheBlackBox:
	case Soldier_m_TheCowMangler5000:
	case Soldier_m_TheOriginal:
	case Soldier_m_FestiveRocketLauncher:
	case Soldier_m_TheBeggarsBazooka:
	case Soldier_m_SilverBotkillerRocketLauncherMkI:
	case Soldier_m_GoldBotkillerRocketLauncherMkI:
	case Soldier_m_RustBotkillerRocketLauncherMkI:
	case Soldier_m_BloodBotkillerRocketLauncherMkI:
	case Soldier_m_CarbonadoBotkillerRocketLauncherMkI:
	case Soldier_m_DiamondBotkillerRocketLauncherMkI:
	case Soldier_m_SilverBotkillerRocketLauncherMkII:
	case Soldier_m_GoldBotkillerRocketLauncherMkII:
	case Soldier_m_FestiveBlackBox:
	case Soldier_m_TheAirStrike:
	case Soldier_m_WoodlandWarrior:
	case Soldier_m_SandCannon:
	case Soldier_m_AmericanPastoral:
	case Soldier_m_SmalltownBringdown:
	case Soldier_m_ShellShocker:
	case Soldier_m_AquaMarine:
	case Soldier_m_Autumn:
	case Soldier_m_BlueMew:
	case Soldier_m_BrainCandy:
	case Soldier_m_CoffinNail:
	case Soldier_m_HighRollers:
	case Soldier_m_Warhawk:
		{
			out = {1100.0f, 0.0f};
			break;
		}

	case Soldier_m_TheDirectHit:
		{
			out = {1980.0f, 0.0f};
			break;
		}

	case Soldier_m_TheLibertyLauncher:
		{
			out = {1540.0f, 0.0f};
			break;
		}

	case Demoman_m_GrenadeLauncher:
	case Demoman_m_GrenadeLauncherR:
	case Demoman_m_FestiveGrenadeLauncher:
	case Demoman_m_TheIronBomber:
	case Demoman_m_Autumn:
	case Demoman_m_MacabreWeb:
	case Demoman_m_Rainbow:
	case Demoman_m_SweetDreams:
	case Demoman_m_CoffinNail:
	case Demoman_m_TopShelf:
	case Demoman_m_Warhawk:
	case Demoman_m_ButcherBird:
		{
			out = {1216.f, 0.4f};
			break;
		}

	case Soldier_s_TheRighteousBison:
	case Engi_m_ThePomson6000:
		{
			out = {1200.0f, 0.0f};
			break;
		}

	case Demoman_m_TheLooseCannon:
		{
			out = {1453.9f, 0.4f};
			break;
		}

	case Demoman_m_TheLochnLoad:
		{
			out = {1513.3f, 0.4f};
			break;
		}

	case Engi_m_TheRescueRanger:
	case Medic_m_FestiveCrusadersCrossbow:
	case Medic_m_CrusadersCrossbow:
		{
			out = {2400.0f, 0.2f};
			break;
		}

	case Pyro_m_DragonsFury:
		{
			out = {3000.0f, 0.0f, 0.1753f};
			m_bIsFlameThrower = true;
			break;
		}

	case Pyro_m_FlameThrower:
	case Pyro_m_FlameThrowerR:
	case Pyro_m_TheBackburner:
	case Pyro_m_TheDegreaser:
	case Pyro_m_ThePhlogistinator:
	case Pyro_m_FestiveFlameThrower:
	case Pyro_m_TheRainblower:
	case Pyro_m_SilverBotkillerFlameThrowerMkI:
	case Pyro_m_GoldBotkillerFlameThrowerMkI:
	case Pyro_m_RustBotkillerFlameThrowerMkI:
	case Pyro_m_BloodBotkillerFlameThrowerMkI:
	case Pyro_m_CarbonadoBotkillerFlameThrowerMkI:
	case Pyro_m_DiamondBotkillerFlameThrowerMkI:
	case Pyro_m_SilverBotkillerFlameThrowerMkII:
	case Pyro_m_GoldBotkillerFlameThrowerMkII:
	case Pyro_m_FestiveBackburner:
	case Pyro_m_ForestFire:
	case Pyro_m_BarnBurner:
	case Pyro_m_BovineBlazemaker:
	case Pyro_m_EarthSkyandFire:
	case Pyro_m_FlashFryer:
	case Pyro_m_TurbineTorcher:
	case Pyro_m_Autumn:
	case Pyro_m_PumpkinPatch:
	case Pyro_m_Nutcracker:
	case Pyro_m_Balloonicorn:
	case Pyro_m_Rainbow:
	case Pyro_m_CoffinNail:
	case Pyro_m_Warhawk:
	case Pyro_m_NostromoNapalmer:
		{
			out = {1000.0f, 0.0f, 0.33f};
			m_bIsFlameThrower = true;
			break;
		}

	case Pyro_s_TheDetonator:
	case Pyro_s_TheFlareGun:
	case Pyro_s_FestiveFlareGun:
	case Pyro_s_TheScorchShot:
		{
			out = {2000.0f, 0.3f};
			break;
		}

	case Pyro_s_TheManmelter:
	case Scout_s_TheFlyingGuillotine:
	case Scout_s_TheFlyingGuillotineG:
		{
			out = {3000.0f, 0.2f};
			break;
		}

	case Medic_m_SyringeGun:
	case Medic_m_SyringeGunR:
	case Medic_m_TheBlutsauger:
	case Medic_m_TheOverdose:
		{
			out = {1000.0f, 0.2f};
			break;
		}

	case Sniper_m_TheHuntsman:
	case Sniper_m_FestiveHuntsman:
	case Sniper_m_TheFortifiedCompound:
		{
			const float charge = (I::GlobalVars->curtime - pWeapon->GetChargeBeginTime());
			out = {
				Math::RemapValClamped(charge, 0.0f, 1.f, 1800, 2600),
				Math::RemapValClamped(charge, 0.0f, 1.f, 0.5, 0.1)
			};
			break;
		}
	}

	return out.m_flVelocity;
}

bool CAimbotProjectile::CalcProjAngle(const Vec3& vLocalPos, const Vec3& vTargetPos, const ProjectileInfo_t& projInfo, Solution_t& out)
{
	const Vec3 v = vTargetPos - vLocalPos;
	const float dx = sqrt(v.x * v.x + v.y * v.y);
	const float dy = v.z;
	const float v0 = projInfo.m_flVelocity;

	//Ballistics
	if (const float g = g_ConVars.sv_gravity->GetFloat() * projInfo.m_flGravity)
	{
		const float root = v0 * v0 * v0 * v0 - g * (g * dx * dx + 2.0f * dy * v0 * v0);

		if (root < 0.0f)
		{
			return false;
		}

		out.m_flPitch = atan((v0 * v0 - sqrt(root)) / (g * dx));
		out.m_flYaw = atan2(v.y, v.x);
	}

	//Straight trajectory (the time stuff later doesn't make sense with this but hey it works!)
	else
	{
		const Vec3 vecAngle = Math::CalcAngle(vLocalPos, vTargetPos);
		out.m_flPitch = -DEG2RAD(vecAngle.x);
		out.m_flYaw = DEG2RAD(vecAngle.y);
	}

	out.m_flTime = dx / (cos(out.m_flPitch) * v0);

	return true;
}

bool CAimbotProjectile::SolveProjectile(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Predictor_t& predictor, const ProjectileInfo_t& projInfo, Solution_t& out)
{
	auto pNetChannel = I::Engine->GetNetChannelInfo();

	g_GlobalInfo.predBeforeLines.clear();
	g_GlobalInfo.predFutureLines.clear(); // clear here to stop them from drawing on non move-simmed entities

	if (!pNetChannel)
	{
		return false;
	}

	Ray_t traceRay = {};
	CGameTrace trace = {};
	CTraceFilterWorldAndPropsOnly traceFilter = {};

	traceFilter.pSkip = predictor.m_pEntity;

	Vec3 vLocalPos = pLocal->GetEyePosition();
	float maxTime = projInfo.m_flMaxTime;
	float fLatency = pNetChannel->GetLatency(FLOW_OUTGOING) + pNetChannel->GetLatency(FLOW_INCOMING);
	if (predictor.m_pEntity->GetClassID() == ETFClassID::CObjectSentrygun || predictor.m_pEntity->GetClassID() ==
		ETFClassID::CObjectDispenser ||
		predictor.m_pEntity->GetClassID() == ETFClassID::CObjectTeleporter || predictor.m_pEntity->GetClassID() !=
		ETFClassID::CTFPlayer ||
		!Vars::Aimbot::Projectile::MovementSimulation.m_Var)
	{
		float TIME_STEP = 0.015f;

		for (float fPredTime = 0.0f; fPredTime < maxTime; fPredTime += TIME_STEP)
		{
			float fCorrectTime = (fPredTime + fLatency);
			Vec3 vPredictedPos = predictor.Extrapolate(fCorrectTime);

			switch (pWeapon->GetWeaponID())
			{
			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_PIPEBOMBLAUNCHER:
			case TF_WEAPON_STICKBOMB:
			case TF_WEAPON_STICKY_BALL_LAUNCHER:
				{
					Vec3 vDelta = (vPredictedPos - vLocalPos);
					float fRange = Math::VectorNormalize(vDelta);

					float fElevationAngle = (fRange * (g_GlobalInfo.m_nCurItemDefIndex == Demoman_m_TheLochnLoad
						                                   ? 0.0075f
						                                   : 0.013f));

					if (fElevationAngle > 45.0f)
					{
						fElevationAngle = 45.0f;
					}

					float s = 0.0f, c = 0.0f;
					Math::SinCos((fElevationAngle * PI / 180.0f), &s, &c);

					float fElevation = (fRange * (s / c));
					vPredictedPos.z += (c > 0.0f ? fElevation : 0.0f);
					break;
				}

			default: break;
			}

			Utils::TraceHull(predictor.m_vPosition, vPredictedPos, Vec3(-2, -2, -2), Vec3(2, 2, 2),
			                 MASK_SOLID_BRUSHONLY, &traceFilter, &trace);

			if (trace.DidHit())
			{
				vPredictedPos.z = trace.vEndPos.z;
			}

			switch (pWeapon->GetWeaponID())
			{
			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_PIPEBOMBLAUNCHER:
			case TF_WEAPON_STICKBOMB:
			case TF_WEAPON_STICKY_BALL_LAUNCHER:
				{
					Vec3 vecOffset(16.0f, 8.0f, -6.0f);
					Utils::GetProjectileFireSetup(pLocal, pCmd->viewangles, vecOffset, &vLocalPos);
					break;
				}

			default: break;
			}

			if (!CalcProjAngle(vLocalPos, vPredictedPos, projInfo, out))
			{
				return false;
			}

			if (out.m_flTime < fCorrectTime)
			{
				Vec3 vVisCheck = vLocalPos;

				switch (pWeapon->GetWeaponID())
				{
				case TF_WEAPON_ROCKETLAUNCHER:
				//case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
				case TF_WEAPON_DIRECTHIT:
				// dragons furry
				case 109:
				case TF_WEAPON_FLAREGUN:
				//case TF_WEAPON_FLAREGUN_REVENGE:
				case TF_WEAPON_RAYGUN_REVENGE:
				case TF_WEAPON_COMPOUND_BOW:
				case TF_WEAPON_SYRINGEGUN_MEDIC:
					{
						if (g_GlobalInfo.m_nCurItemDefIndex != Soldier_m_TheOriginal)
						{
							Vec3 vecOffset(23.5f, 12.0f, -3.0f);

							if (pLocal->IsDucking())
							{
								vecOffset.z = 8.0f;
							}

							Utils::GetProjectileFireSetup(pLocal, pCmd->viewangles, vecOffset, &vVisCheck);
						}

						break;
					}

				case TF_WEAPON_GRENADELAUNCHER:
				case TF_WEAPON_PIPEBOMBLAUNCHER:
				case TF_WEAPON_STICKBOMB:
				case TF_WEAPON_STICKY_BALL_LAUNCHER:
					{
						auto vecAngle = Vec3(), vecForward = Vec3(), vecRight = Vec3(), vecUp = Vec3();
						Math::AngleVectors({-RAD2DEG(out.m_flPitch), RAD2DEG(out.m_flYaw), 0.0f}, &vecForward,
						                   &vecRight, &vecUp);
						Vec3 vecVelocity = ((vecForward * projInfo.m_flVelocity) - (vecUp * 200.0f));
						Math::VectorAngles(vecVelocity, vecAngle);
						out.m_flPitch = -DEG2RAD(vecAngle.x);

						break;
					}

				default: break;
				}

				Utils::TraceHull(vVisCheck, vPredictedPos, Vec3(-2, -2, -2), Vec3(2, 2, 2), MASK_SOLID_BRUSHONLY,
				                 &traceFilter, &trace);

				if (trace.DidHit())
				{
					return false;
				}

				g_GlobalInfo.m_vPredictedPos = vPredictedPos;
				return true;
			}
		}
	}
	else
	{
		Vec3 vPredictedPos = {};
		CMoveData moveData = {};
		Vec3 worldSpaceCenter = {};

		if (g_MoveSim.Initialize(predictor.m_pEntity))
		{
			int n = 0;
			for (; n < TIME_TO_TICKS(maxTime); n++)
			{
				if (predictor.m_pEntity == nullptr)
				{
					break;
				}
				g_MoveSim.RunTick(moveData, worldSpaceCenter);
				vPredictedPos = worldSpaceCenter;

				Vec3 vAimDelta = predictor.m_pEntity->GetAbsOrigin() - GetAimPos(pLocal, predictor.m_pEntity);
				vPredictedPos.x += abs(vAimDelta.x);
				vPredictedPos.y += abs(vAimDelta.y);
				vPredictedPos.z += abs(vAimDelta.z);


				//Weapon offsets
				switch (pWeapon->GetWeaponID())
				{
				// This is from tf_bot_behaviour :/
				case TF_WEAPON_GRENADELAUNCHER:
				case TF_WEAPON_PIPEBOMBLAUNCHER:
				case TF_WEAPON_STICKBOMB:
				case TF_WEAPON_STICKY_BALL_LAUNCHER:
					{
						Vec3 toThreat = (vPredictedPos - vLocalPos);
						float fRange = Math::VectorNormalize(toThreat);

						float fElevationAngle = (fRange * (g_GlobalInfo.m_nCurItemDefIndex == Demoman_m_TheLochnLoad ? 0.0075f : 0.013f));

						if (fElevationAngle > 45.0f)
						{
							fElevationAngle = 45.0f;
						}

						float s = 0.0f, c = 0.0f;
						Math::SinCos((fElevationAngle * PI / 180.0f), &s, &c);

						if (c > 0.0f)
						{
							float elevation = fRange * s / c;
							vPredictedPos.z += elevation;
						}
						break;
					}

				default: break;
				}

				Utils::TraceHull(predictor.m_vPosition, vPredictedPos, Vec3(-3.8f, -3.8f, -3.8f), Vec3(3.8f, 3.8f, 3.8f),
				                 MASK_SOLID_BRUSHONLY, &traceFilter, &trace);

				if (trace.DidHit())
				{
					vPredictedPos.z = trace.vEndPos.z;
					g_GlobalInfo.m_vPredictedPos = vPredictedPos;
				}

				switch (pWeapon->GetWeaponID())
				{
				case TF_WEAPON_GRENADELAUNCHER:
				case TF_WEAPON_PIPEBOMBLAUNCHER:
				case TF_WEAPON_STICKBOMB:
				case TF_WEAPON_STICKY_BALL_LAUNCHER:
					{
						Vec3 vecOffset(16.0f, 8.0f, -6.0f);
						Utils::GetProjectileFireSetup(pLocal, pCmd->viewangles, vecOffset, &vLocalPos);
						break;
					}

				default: break;
				}

				if (!CalcProjAngle(vLocalPos, vPredictedPos, projInfo, out))
				{
					break;
				}

				out.m_flTime += fLatency;

				if (out.m_flTime < TICKS_TO_TIME(n))
				{
					if (!WillProjectileHit(pLocal, pWeapon, pCmd, vPredictedPos, out, projInfo, predictor)) { break; }

					g_GlobalInfo.m_vPredictedPos = vPredictedPos;
					g_MoveSim.Restore();
					return true;
				}
			}
			g_MoveSim.Restore();
		}
	}
	return false;
}

//	Tries to find the best position to aim at on our target.
Vec3 CAimbotProjectile::GetAimPos(CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	Vec3 retVec = pEntity->GetAbsOrigin();

	const Vec3 vLocalPos = pLocal->GetShootPos();

	const bool bIsDucking = pEntity->IsDucking();

	const Vec3 vMins = I::GameMovement->GetPlayerMins(bIsDucking);
	const Vec3 vMaxs = I::GameMovement->GetPlayerMaxs(bIsDucking);

	const std::vector vecPoints = {
		Vec3(vMins.x, ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3(vMaxs.x, ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3(((vMins.x + vMaxs.x) * 0.5f), vMins.y, ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3(((vMins.x + vMaxs.x) * 0.5f), vMaxs.y, ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3(((vMins.x + vMaxs.x) * 0.5f), ((vMins.y + vMaxs.y) * 0.5f), vMins.z),
		Vec3(((vMins.x + vMaxs.x) * 0.5f), ((vMins.y + vMaxs.y) * 0.5f), vMaxs.z)
	};

	std::vector<Vec3> visiblePoints{};
	const matrix3x4& transform = pEntity->GetRgflCoordinateFrame();

	for (const auto& point : vecPoints)
	{
		Vec3 vTransformed = {};
		Math::VectorTransform(point, transform, vTransformed);

		if (Utils::VisPos(pLocal, pEntity, vLocalPos, vTransformed))
		{
			visiblePoints.push_back(vTransformed);
		}
	}

	int aimMode = Vars::Aimbot::Projectile::AimPosition.m_Var;
	if (aimMode == 3)
	{
		// auto
		switch (pLocal->GetClassNum())
		{
		case CLASS_SOLDIER:
		case CLASS_DEMOMAN:
			{
				if (Vars::Aimbot::Projectile::FeetAimIfOnGround.m_Var && pEntity->IsOnGround())
				{
					aimMode = 2;
					break;
				}
				aimMode = 1;
				break;
			}
		case CLASS_SNIPER:
			{
				aimMode = 0;
				break;
			}
		default:
			{
				aimMode = 1;
				break;
			}
		}
	}

	switch (aimMode)
	{
	case 0: //head
		{
			const Vec3 hitboxPosition = pEntity->GetHitboxPos(HITBOX_HEAD);

			for (const auto& aimPoint : visiblePoints)
			{
				if ((hitboxPosition.z - aimPoint.z) < (hitboxPosition.z - retVec.z)) { retVec = aimPoint; }
			}
			break;
		}
	case 1: //body
		{
			const Vec3 hitboxPosition = pEntity->GetHitboxPos(HITBOX_PELVIS);

			for (const auto& aimPoint : visiblePoints)
			{
				if ((hitboxPosition.z - aimPoint.z) < (hitboxPosition.z - retVec.z)) { retVec = Vec3(aimPoint.x, aimPoint.y, hitboxPosition.z); }
			}
			break;
		}
	case 2: //feet
		{
			const auto hitboxPosition = Vec3(pEntity->GetHitboxPos(HITBOX_PELVIS).x, pEntity->GetHitboxPos(HITBOX_PELVIS).y, pEntity->GetHitboxPos(HITBOX_FOOT_L).z);

			for (const auto& aimPoint : visiblePoints)
			{
				if ((hitboxPosition.z - aimPoint.z) > (hitboxPosition.z - retVec.z)) { retVec = aimPoint; }
			}
			break;
		}
	}

	return retVec;
}

bool CAimbotProjectile::WillProjectileHit(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Vec3 vPredictedPos, Solution_t& out, const ProjectileInfo_t& projInfo,
                                          const Predictor_t& predictor)
{
	Vec3 vVisCheck = pLocal->GetEyePosition();
	const Vec3 predictedViewAngles = {-RAD2DEG(out.m_flPitch), RAD2DEG(out.m_flYaw), 0.0f};
	CGameTrace trace = {};
	CTraceFilterWorldAndPropsOnly traceFilter = {};
	traceFilter.pSkip = predictor.m_pEntity;
	switch (pWeapon->GetWeaponID())
	{
	case TF_WEAPON_RAYGUN_REVENGE:
	case TF_WEAPON_ROCKETLAUNCHER:
	case TF_WEAPON_DIRECTHIT:
		{
			Vec3 vecOffset(23.5f, 12.0f, -3.0f); //tf_weaponbase_gun.cpp @L529 & @L760
			if (pLocal->IsDucking())
			{
				vecOffset.z = 8.0f;
			}
			Utils::GetProjectileFireSetup(pLocal, predictedViewAngles, vecOffset, &vVisCheck);
			break;
		}
	case TF_WEAPON_SYRINGEGUN_MEDIC:
		{
			const Vec3 vecOffset(16.f, 6.f, -8.f); //tf_weaponbase_gun.cpp @L628
			Utils::GetProjectileFireSetup(pLocal, predictedViewAngles, vecOffset, &vVisCheck);
			break;
		}
	case TF_WEAPON_COMPOUND_BOW:
		{
			const Vec3 vecOffset(23.5f, 12.0f, -3.0f); //tf_weapon_grapplinghook.cpp @L355 ??
			Utils::GetProjectileFireSetup(pLocal, predictedViewAngles, vecOffset, &vVisCheck);
			break;
		}
	case TF_WEAPON_RAYGUN:
	case TF_WEAPON_PARTICLE_CANNON:
	case TF_WEAPON_DRG_POMSON:
		{
			Vec3 vecOffset(23.5f, -8.0f, -3.0f); //tf_weaponbase_gun.cpp @L568
			if (pLocal->IsDucking())
			{
				vecOffset.z = 8.0f;
			}
			Utils::GetProjectileFireSetup(pLocal, predictedViewAngles, vecOffset, &vVisCheck);
			break;
		}
	case TF_WEAPON_GRENADELAUNCHER:
	case TF_WEAPON_PIPEBOMBLAUNCHER:
	case TF_WEAPON_STICKBOMB:
	case TF_WEAPON_STICKY_BALL_LAUNCHER:
		{
			auto vecAngle = Vec3(), vecForward = Vec3(), vecRight = Vec3(), vecUp = Vec3();
			Math::AngleVectors({-RAD2DEG(out.m_flPitch), RAD2DEG(out.m_flYaw), 0.0f}, &vecForward,
			                   &vecRight, &vecUp);
			const Vec3 vecVelocity = ((vecForward * projInfo.m_flVelocity) - (vecUp * 200.0f));
			Math::VectorAngles(vecVelocity, vecAngle);
			out.m_flPitch = -DEG2RAD(vecAngle.x);

			break;
		}
	default: break;
	}

	//	TODO: find the actual hull size of projectiles
	//	maybe - https://www.unknowncheats.me/forum/team-fortress-2-a/475502-weapons-projectile-min-max-collideables.html
	//	UTIL_SetSize( this, -Vector( 1.0f, 1.0f, 1.0f ), Vector( 1.0f, 1.0f, 1.0f ) ); @tf_projectile_base.cpp L117
	Utils::TraceHull(vVisCheck, vPredictedPos, Vec3(-3.8f, -3.8f, -3.8f), Vec3(3.8f, 3.8f, 3.8f), MASK_SOLID_BRUSHONLY, &traceFilter, &trace);

	if (trace.DidHit())
	{
		return false;
	}
	return true;
}

ESortMethod CAimbotProjectile::GetSortMethod()
{
	switch (Vars::Aimbot::Projectile::SortMethod.m_Var)
	{
	case 0: return ESortMethod::FOV;
	case 1: return ESortMethod::DISTANCE;
	default: return ESortMethod::UNKNOWN;
	}
}

void ProjectileTracer(CBaseEntity* pLocal, const Target_t& target)
{
	if (pLocal->IsTaunting())
	{
		return;
	}
	const Vec3 vecPos = g_GlobalInfo.m_WeaponType == EWeaponType::PROJECTILE ? g_GlobalInfo.m_vPredictedPos : target.m_vPos;
	const Color_t tracerColor = Vars::Visuals::BulletTracerRainbow.m_Var ? Utils::Rainbow() : Colors::BulletTracer;
	Vec3 shootPos;
	const int iAttachment = pLocal->GetActiveWeapon()->LookupAttachment(_("muzzle"));
	pLocal->GetActiveWeapon()->GetAttachment(iAttachment, shootPos);
	I::DebugOverlay->AddLineOverlayAlpha(shootPos, vecPos, tracerColor.r, tracerColor.g, tracerColor.b, tracerColor.a, true, 5);
}

bool CAimbotProjectile::GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
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
		const int nWeaponID = pWeapon->GetWeaponID();
		const bool bIsCrossbow = nWeaponID == TF_WEAPON_CROSSBOW;

		for (const auto& pTarget : g_EntityCache.GetGroup(
			     bIsCrossbow ? EGroupType::PLAYERS_ALL : EGroupType::PLAYERS_ENEMIES))
		{
			if (!pTarget->IsAlive() || pTarget->IsAGhost() || pTarget == pLocal || (bIsCrossbow && (pTarget->GetHealth() >=
				pTarget->GetMaxHealth()) && (pTarget->GetTeamNum() == pLocal->GetTeamNum())))
			{
				continue;
			}

			if (pTarget->GetTeamNum() != pLocal->GetTeamNum())
			{
				if (g_AimbotGlobal.ShouldIgnore(pTarget)) { continue; }
			}

			Vec3 vPos = pTarget->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = (sortMethod == ESortMethod::FOV) ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			const float flDistTo = (sortMethod == ESortMethod::DISTANCE) ? vLocalPos.DistTo(vPos) : 0.0f;

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.m_Var)
			{
				continue;
			}

			const uint32_t priorityID = g_PR->isValid(pTarget->GetIndex()) ? g_PR->GetAccountID(pTarget->GetIndex()) : 0;
			const auto& priority = g_GlobalInfo.PlayerPriority[priorityID];

			g_AimbotGlobal.m_vecTargets.push_back({ pTarget, ETargetType::PLAYER, vPos, vAngleTo, flFOVTo, flDistTo, -1, false, priority });
		}
	}

	// Buildings
	if (Vars::Aimbot::Global::AimBuildings.m_Var)
	{
		const bool bIsRescueRanger = pWeapon->GetWeaponID() == TF_WEAPON_SHOTGUN_BUILDING_RESCUE;

		for (const auto& pBuilding : g_EntityCache.GetGroup(
			     bIsRescueRanger ? EGroupType::BUILDINGS_ALL : EGroupType::BUILDINGS_ENEMIES))
		{
			if (!pBuilding->IsAlive())
			{
				continue;
			}

			Vec3 vPos = pBuilding->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = sortMethod == ESortMethod::FOV ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.m_Var)
			{
				continue;
			}

			g_AimbotGlobal.m_vecTargets.push_back({pBuilding, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo});
		}
	}

	return !g_AimbotGlobal.m_vecTargets.empty();
}

bool CAimbotProjectile::VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Target_t& target)
{
	ProjectileInfo_t projInfo = {};
	if (!GetProjectileInfo(pWeapon, projInfo))
	{
		return false;
	}

	auto vVelocity = Vec3();
	auto vAcceleration = Vec3();

	switch (target.m_TargetType)
	{
	case ETargetType::PLAYER:
		{
			vVelocity = target.m_pEntity->GetVelocity();
			vAcceleration = Vec3(0.0f, 0.0f, g_ConVars.sv_gravity->GetFloat() * ((target.m_pEntity->GetCondEx2() & TFCondEx2_Parachute) ? 0.224f : 1.0f));
			break;
		}

	default: break;
	}

	Predictor_t predictor = {
		target.m_pEntity,
		target.m_vPos,
		vVelocity,
		vAcceleration
	};

	Solution_t solution = {};

	if (!SolveProjectile(pLocal, pWeapon, pCmd, predictor, projInfo, solution))
	{
		return false;
	}

	target.m_vAngleTo = {-RAD2DEG(solution.m_flPitch), RAD2DEG(solution.m_flYaw), 0.0f};

	return true;
}

// Returns the best target
bool CAimbotProjectile::GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Target_t& outTarget)
{
	if (!GetTargets(pLocal, pWeapon))
	{
		return false;
	}

	if (Vars::Aimbot::Projectile::PerformanceMode.m_Var)
	{
		Target_t target = g_AimbotGlobal.GetBestTarget(GetSortMethod());

		if (!VerifyTarget(pLocal, pWeapon, pCmd, target))
		{
			return false;
		}

		outTarget = target;
		return true;
	}
	g_AimbotGlobal.SortTargets(GetSortMethod());

	//instead of this just limit to like 4-6 targets, should save perf without any noticeable changes in functionality
	for (auto& target : g_AimbotGlobal.m_vecTargets)
	{
		if (!VerifyTarget(pLocal, pWeapon, pCmd, target))
		{
			continue;
		}

		outTarget = target;
		return true;
	}

	return false;
}

// Aims at the given angles
void CAimbotProjectile::Aim(CUserCmd* pCmd, CBaseCombatWeapon* pWeapon, Vec3& vAngle)
{
	vAngle -= g_GlobalInfo.m_vPunchAngles;
	Math::ClampAngles(vAngle);

	switch (Vars::Aimbot::Projectile::AimMethod.m_Var)
	{
	case 0:
		{
			// Plain
			pCmd->viewangles = vAngle;
			I::Engine->SetViewAngles(pCmd->viewangles);
			break;
		}

	case 1:
		{
			// Silent
			Utils::FixMovement(pCmd, vAngle);
			pCmd->viewangles = vAngle;
			break;
		}

	default: break;
	}
}

bool CAimbotProjectile::ShouldFire(CUserCmd* pCmd)
{
	return (Vars::Aimbot::Global::AutoShoot.m_Var && g_GlobalInfo.m_bWeaponCanAttack);
}

bool CAimbotProjectile::IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon)
{
	if (g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheBeggarsBazooka)
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
		if (pWeapon->GetWeaponID() == TF_WEAPON_COMPOUND_BOW)
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
			if ((pCmd->buttons & IN_ATTACK) && g_GlobalInfo.m_bWeaponCanAttack)
			{
				return true;
			}
		}
	}

	return false;
}

// Returns the best target for splash damage
bool CAimbotProjectile::GetSplashTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Target_t& outTarget)
{
	if (!Vars::Aimbot::Projectile::SplashPrediction.m_Var) { return false; }

	// TODO: I have no clue if these values are accurate
	std::optional<float> splashRadius;
	switch (pWeapon->GetClassID())
	{
	case ETFClassID::CTFRocketLauncher:
	case ETFClassID::CTFRocketLauncher_AirStrike:
	case ETFClassID::CTFRocketLauncher_Mortar:
	case ETFClassID::CTFStickBomb:
		splashRadius = 130.f; break;
	}

	// Don't do it with the direct hit or if the splash radius is unknown
	if (pWeapon->GetClassID() == ETFClassID::CTFRocketLauncher_DirectHit || !splashRadius) { return false; }

	const auto sortMethod = GetSortMethod();
	const auto& vLocalAngles = I::Engine->GetViewAngles();
	const auto& vLocalShootPos = pLocal->GetShootPos();

	for (const auto& pTarget : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!pTarget || !pTarget->IsAlive()) { continue; }
		if (!pTarget->IsOnGround()) { continue; }
		if (pLocal->GetAbsOrigin().DistTo(pTarget->GetAbsOrigin()) > 900.f) { continue; }

		const auto& vTargetOrigin = pTarget->GetAbsOrigin();

		// Scan every 45 degree angle
		for (int i = 0; i < 315; i += 45)
		{
			Vec3 scanPos = Utils::GetRotatedPosition(vTargetOrigin, static_cast<float>(i), *splashRadius);

			CGameTrace trace = {};
			CTraceFilterWorldAndPropsOnly traceFilter = {};

			// Check FOV if enabled
			const Vec3 vAngleTo = Math::CalcAngle(vLocalShootPos, scanPos);
			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);
			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.m_Var) { continue; }

			// Don't predict through walls
			Utils::Trace(scanPos, pTarget->GetWorldSpaceCenter(), MASK_SOLID, &traceFilter, &trace);
			if (trace.flFraction < 0.99f && trace.entity != pTarget) { continue; }

			// Don't predict enemies that are visible
			if (Utils::VisPos(pLocal, pTarget, pLocal->GetShootPos(), vTargetOrigin)) { continue; }

			if (Utils::VisPos(pLocal, pTarget, vLocalShootPos, scanPos))
			{
				// We found a target point! Get the closest point possible...
				float currentRadius = *splashRadius;
				while (currentRadius > 10.f && Utils::VisPos(pLocal, pTarget, vLocalShootPos, scanPos))
				{
					scanPos = Utils::GetRotatedPosition(vTargetOrigin, static_cast<float>(i), currentRadius - 10.f);
					currentRadius -= 10.f;
				}

				// Closest point found!
				currentRadius = std::clamp(currentRadius + 10.f, 0.f, *splashRadius);
				scanPos = Utils::GetRotatedPosition(vTargetOrigin, static_cast<float>(i), currentRadius);

				if (Vars::Debug::DebugInfo.m_Var)
				{
					I::DebugOverlay->AddLineOverlay(vTargetOrigin, scanPos, 255, 0, 0, false, MAXIMUM_TICK_INTERVAL);
					I::DebugOverlay->AddTextOverlay(scanPos, MAXIMUM_TICK_INTERVAL, "X");
				}

				const Vec3 vAngleToSplash = Math::CalcAngle(pLocal->GetEyePosition(), scanPos);
				outTarget = { pTarget, ETargetType::PLAYER, vTargetOrigin, vAngleToSplash };
				return true;
			}
		}
	}

	return false;
}

void CAimbotProjectile::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	static int nLastTracerTick = pCmd->tick_count;

	m_bIsFlameThrower = false;

	if (!Vars::Aimbot::Global::Active.m_Var)
	{
		return;
	}

	const bool bShouldAim = (Vars::Aimbot::Global::AimKey.m_Var == VK_LBUTTON
		                         ? (pCmd->buttons & IN_ATTACK)
		                         : g_AimbotGlobal.IsKeyDown());
	if (!bShouldAim) { return; }

	Target_t target{ };
	if (GetTarget(pLocal, pWeapon, pCmd, target) || GetSplashTarget(pLocal, pWeapon, pCmd, target))
	{
		// Aim at the current target or splashtarget
		g_GlobalInfo.m_nCurrentTargetIdx = target.m_pEntity->GetIndex();

		if (Vars::Aimbot::Projectile::AimMethod.m_Var == 1)
		{
			g_GlobalInfo.m_vAimPos = g_GlobalInfo.m_vPredictedPos;
		}

		if (ShouldFire(pCmd))
		{
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

			if (g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheBeggarsBazooka)
			{
				if (pWeapon->GetClip1() > 0)
				{
					pCmd->buttons &= ~IN_ATTACK;
				}
			}

			else
			{
				if (pWeapon->GetWeaponID() == TF_WEAPON_COMPOUND_BOW && pWeapon->GetChargeBeginTime() > 0.0f)
				{
					pCmd->buttons &= ~IN_ATTACK;
				}
			}
		}

		const bool bIsAttacking = IsAttacking(pCmd, pWeapon);

		if (bIsAttacking)
		{
			g_GlobalInfo.m_bAttacking = true;
			if (Vars::Visuals::BulletTracer.m_Var && abs(pCmd->tick_count - nLastTracerTick) > 1)
			{
				ProjectileTracer(pLocal, target);
				nLastTracerTick = pCmd->tick_count;
			}

			//I::DebugOverlay->AddLineOverlayAlpha(Target.m_vPos, g_GlobalInfo.m_vPredictedPos, 0, 255, 0, 255, true, 2); // Predicted aim pos
		}

		if (Vars::Aimbot::Projectile::AimMethod.m_Var == 1)
		{
			if (m_bIsFlameThrower)
			{
				g_GlobalInfo.m_bProjectileSilentActive = true;
				Aim(pCmd, pWeapon, target.m_vAngleTo);
			}

			else
			{
				if (bIsAttacking)
				{
					Aim(pCmd, pWeapon, target.m_vAngleTo);
					g_GlobalInfo.m_bSilentTime = true;
				}
			}
		}
		else
		{
			Aim(pCmd, pWeapon, target.m_vAngleTo);
		}
	}
}
