#include "AimbotProjectile.h"
#include "../../Vars.h"

#include "../MovementSimulation/MovementSimulation.h"

Vec3 CAimbotProjectile::Predictor_t::Extrapolate(float time)
{
	g_GlobalInfo.linearPredLine = m_vPosition;

	Vec3 vecOut = {};

	if (m_pEntity->IsOnGround())
		vecOut = (m_vPosition + (m_vVelocity * time));

	else vecOut = (m_vPosition + (m_vVelocity * time) - m_vAcceleration * 0.5f * time * time);

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
		out = { 1100.0f, 0.0f };
		break;
	}

	case Soldier_m_TheDirectHit:
	{
		out = { 1980.0f, 0.0f };
		break;
	}

	case Soldier_m_TheLibertyLauncher:
	{
		out = { 1540.0f, 0.0f };
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
		out = { 1216.f, 0.4f };
		break;
	}

	case Soldier_s_TheRighteousBison:
	case Engi_m_ThePomson6000:
	{
		out = { 1200.0f, 0.0f };
		break;
	}

	case Demoman_m_TheLooseCannon:
	{
		out = { 1453.9f, 0.4f };
		break;
	}

	case Demoman_m_TheLochnLoad:
	{
		out = { 1513.3f, 0.4f };
		break;
	}

	case Engi_m_TheRescueRanger:
	case Medic_m_FestiveCrusadersCrossbow:
	case Medic_m_CrusadersCrossbow:
	{
		out = { 2400.0f, 0.2f };
		break;
	}

	case Pyro_m_DragonsFury:
	{
		out = { 3000.0f, 0.0f, 0.1753f };
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
		out = { 1000.0f, 0.0f, 0.33f };
		m_bIsFlameThrower = true;
		break;
	}

	case Pyro_s_TheDetonator:
	case Pyro_s_TheFlareGun:
	case Pyro_s_FestiveFlareGun:
	case Pyro_s_TheScorchShot:
	{
		out = { 2000.0f, 0.3f };
		break;
	}

	case Pyro_s_TheManmelter:
	case Scout_s_TheFlyingGuillotine:
	case Scout_s_TheFlyingGuillotineG:
	{
		out = { 3000.0f, 0.2f };
		break;
	}

	case Medic_m_SyringeGun:
	case Medic_m_SyringeGunR:
	case Medic_m_TheBlutsauger:
	case Medic_m_TheOverdose:
	{
		out = { 1000.0f, 0.2f };
		break;
	}

	case Sniper_m_TheHuntsman:
	case Sniper_m_FestiveHuntsman:
	case Sniper_m_TheFortifiedCompound:
	{
		float charge = (g_Interfaces.GlobalVars->curtime - pWeapon->GetChargeBeginTime());
		out = {
			Math::RemapValClamped(charge, 0.0f, 1.f, 1800, 2600),
			Math::RemapValClamped(charge, 0.0f, 1.f, 0.5, 0.1)
		};
		break;
	}
	}

	return out.m_flVelocity;
}

bool CAimbotProjectile::CalcProjAngle(const Vec3& vLocalPos, const Vec3& vTargetPos, const ProjectileInfo_t& ProjInfo,
	Solution_t& out)
{
	const Vec3 v = vTargetPos - vLocalPos;
	const float dx = sqrt(v.x * v.x + v.y * v.y);
	const float dy = v.z;
	const float v0 = ProjInfo.m_flVelocity;

	//Ballistics
	if (const float g = g_ConVars.sv_gravity->GetFloat() * ProjInfo.m_flGravity)
	{
		const float root = v0 * v0 * v0 * v0 - g * (g * dx * dx + 2.0f * dy * v0 * v0);

		if (root < 0.0f)
			return false;

		out.m_flPitch = atan((v0 * v0 - sqrt(root)) / (g * dx));
		out.m_flYaw = atan2(v.y, v.x);
	}

	//Straight trajectory (the time stuff later doesn't make sense with this but hey it works!)
	else
	{
		Vec3 vecAngle = Math::CalcAngle(vLocalPos, vTargetPos);
		out.m_flPitch = -DEG2RAD(vecAngle.x);
		out.m_flYaw = DEG2RAD(vecAngle.y);
	}

	out.m_flTime = dx / (cos(out.m_flPitch) * v0);

	return true;
}


bool CAimbotProjectile::SolveProjectile(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd,
	Predictor_t& Predictor, const ProjectileInfo_t& ProjInfo, Solution_t& out)
{
	auto pNetChannel = g_Interfaces.Engine->GetNetChannelInfo();

	if (!pNetChannel)
		return false;

	Ray_t Ray = {};
	CTraceFilterWorldAndPropsOnly TraceFilter = {};
	CGameTrace Trace = {};

	TraceFilter.pSkip = Predictor.m_pEntity;

	Vec3 vLocalPos = pLocal->GetEyePosition();
	float MAX_TIME = ProjInfo.m_flMaxTime;
	float fLatency = pNetChannel->GetLatency(FLOW_OUTGOING) + pNetChannel->GetLatency(FLOW_INCOMING);
	if (Predictor.m_pEntity->GetClassID() == ETFClassID::CObjectSentrygun || Predictor.m_pEntity->GetClassID() ==
		ETFClassID::CObjectDispenser ||
		Predictor.m_pEntity->GetClassID() == ETFClassID::CObjectTeleporter || Predictor.m_pEntity->GetClassID() !=
		ETFClassID::CTFPlayer ||
		!Vars::Aimbot::Projectile::MovementSimulation.m_Var)
	{
		float TIME_STEP = (MAX_TIME / 128.0f);

		for (float fPredTime = 0.0f; fPredTime < MAX_TIME; fPredTime += TIME_STEP)
		{
			float fCorrectTime = (fPredTime + fLatency);
			Vec3 vPredictedPos = Predictor.Extrapolate(fCorrectTime);

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
					fElevationAngle = 45.0f;

				float s = 0.0f, c = 0.0f;
				Math::SinCos((fElevationAngle * PI / 180.0f), &s, &c);

				float fElevation = (fRange * (s / c));
				vPredictedPos.z += (c > 0.0f ? fElevation : 0.0f);
				break;
			}

			default: break;
			}

			Utils::TraceHull(Predictor.m_vPosition, vPredictedPos, Vec3(-2, -2, -2), Vec3(2, 2, 2),
				MASK_SOLID_BRUSHONLY, &TraceFilter, &Trace);

			if (Trace.DidHit())
			{
				vPredictedPos.z = Trace.vEndPos.z;
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

			if (!CalcProjAngle(vLocalPos, vPredictedPos, ProjInfo, out))
				return false;

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
							vecOffset.z = 8.0f;

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
					Math::AngleVectors({ -RAD2DEG(out.m_flPitch), RAD2DEG(out.m_flYaw), 0.0f }, &vecForward,
						&vecRight, &vecUp);
					Vec3 vecVelocity = ((vecForward * ProjInfo.m_flVelocity) - (vecUp * 200.0f));
					Math::VectorAngles(vecVelocity, vecAngle);
					out.m_flPitch = -DEG2RAD(vecAngle.x);

					break;
				}

				default: break;
				}

				Utils::TraceHull(vVisCheck, vPredictedPos, Vec3(-2, -2, -2), Vec3(2, 2, 2), MASK_SOLID_BRUSHONLY,
					&TraceFilter, &Trace);

				if (Trace.DidHit())
					return false;

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
		bool returnValue = false;
		g_GlobalInfo.predBeforeLines.clear();
		g_GlobalInfo.predFutureLines.clear();
		if (g_MoveSim.Initialize(Predictor.m_pEntity))
		{
			int n = 0;
			for (; n < TIME_TO_TICKS(MAX_TIME); n++)
			{
				if (Predictor.m_pEntity == nullptr)
					break;
				g_MoveSim.RunTick(moveData, worldSpaceCenter);
				vPredictedPos = worldSpaceCenter;
				//Class offsets

				switch (pLocal->GetClassNum())
				{
				case CLASS_SOLDIER:
				case CLASS_DEMOMAN:
				{

					if (pLocal->GetClassNum() == CLASS_DEMOMAN && Vars::Aimbot::Projectile::FeetAimIfOnGround.m_Var && Predictor.m_pEntity->IsOnGround()) {
						vPredictedPos -= Vec3(0.0f, 0.0f, 27.0f);
						break;
					}
					if (Vars::Aimbot::Projectile::AimPosition.m_Var == 0) {
						break;
					}
					vPredictedPos -= Vec3(0.0f, 0.0f, 27.0f);


					break;
				}
				case CLASS_SNIPER:
				{
					if (Vars::Aimbot::Projectile::AimPosition.m_Var == 1) {
						vPredictedPos.z -= 27.0f;
						break;
					}
					if (Vars::Aimbot::Projectile::AimPosition.m_Var > 0) {
						// thank you spook953
						Vec3 vHeadDelta = Predictor.m_pEntity->GetHitboxPos(HITBOX_HEAD) - Predictor.m_pEntity->
							m_vecOrigin();
						vPredictedPos.x += vHeadDelta.x;
						vPredictedPos.y += vHeadDelta.y;

						Vec3 vEntPos = Predictor.m_pEntity->GetWorldSpaceCenter();
						Vec3 vHeadPos = Predictor.m_pEntity->GetHitboxPos(HITBOX_HEAD);
						float zOffset = vHeadPos.z - vEntPos.z;
						vPredictedPos.z += zOffset;
					}

					Vec3 vEntForward = {};
					Math::AngleVectors(Predictor.m_pEntity->GetEyeAngles(), &vEntForward);
					Vec3 vToEnt = Predictor.m_vPosition - pLocal->GetAbsOrigin();

					if (vToEnt.Dot(vEntForward) > 0.1071f)
					{
						vPredictedPos.z += 5.0f;
					}

					if (Vars::Aimbot::Projectile::ManualZAdjust.m_Var) {
						vPredictedPos.z += Vars::Aimbot::Projectile::ZAdjustAmount.m_Var;
					}


					break;
				}
				default:
				{
					if (Vars::Aimbot::Projectile::AimPosition.m_Var == 1) {
						vPredictedPos -= Vec3(0.0f, 0.0f, 27.0f);
					}
					break;
				}
				}
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
						fElevationAngle = 45.0f;

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

				Utils::TraceHull(Predictor.m_vPosition, vPredictedPos, Vec3(-2, -2, -2), Vec3(2, 2, 2),
					MASK_SOLID_BRUSHONLY, &TraceFilter, &Trace);

				if (Trace.DidHit())
				{
					vPredictedPos.z = Trace.vEndPos.z;
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

				if (!CalcProjAngle(vLocalPos, vPredictedPos, ProjInfo, out))
					break;

				out.m_flTime += fLatency;

				if (out.m_flTime < TICKS_TO_TIME(n))
				{
					Vec3 vVisCheck = vLocalPos;

					switch (pWeapon->GetWeaponID())
					{
					case TF_WEAPON_ROCKETLAUNCHER:
						//case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
					case 65: //TF_WEAPON_DIRECTHIT:
					// dragons furry
					case 109:
					case TF_WEAPON_FLAREGUN:
						//case TF_WEAPON_FLAREGUN_REVENGE:
					case 84: //TF_WEAPON_RAYGUN_REVENGE:
					case TF_WEAPON_COMPOUND_BOW:
					case TF_WEAPON_SYRINGEGUN_MEDIC:
					{
						Vec3 vecOffset(23.5f, 12.0f, -3.0f);
						if (pLocal->IsDucking())
							vecOffset.z = 8.0f;
						if (g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheOriginal) { vecOffset.z = 0.f; }

						Utils::GetProjectileFireSetup(pLocal, pCmd->viewangles, vecOffset, &vVisCheck);

						break;
					}

					case TF_WEAPON_GRENADELAUNCHER:
					case TF_WEAPON_PIPEBOMBLAUNCHER:
					case TF_WEAPON_STICKBOMB:
					case TF_WEAPON_STICKY_BALL_LAUNCHER:
					{
						auto vecAngle = Vec3(), vecForward = Vec3(), vecRight = Vec3(), vecUp = Vec3();
						Math::AngleVectors({ -RAD2DEG(out.m_flPitch), RAD2DEG(out.m_flYaw), 0.0f }, &vecForward,
							&vecRight, &vecUp);
						Vec3 vecVelocity = ((vecForward * ProjInfo.m_flVelocity) - (vecUp * 200.0f));
						Math::VectorAngles(vecVelocity, vecAngle);
						out.m_flPitch = -DEG2RAD(vecAngle.x);

						break;
					}
					default: break;
					}

					Utils::TraceHull(vVisCheck, vPredictedPos, Vec3(-18.6f, -3.8f, -3.8f), Vec3(18.6f, 3.8f, 3.8f), MASK_SOLID_BRUSHONLY,
						&TraceFilter, &Trace);

					if (Trace.DidHit())
					{
						break;
					}

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

Vec3 CAimbotProjectile::GetAimPos(CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	switch (Vars::Aimbot::Projectile::AimPosition.m_Var)
	{
	case 0: return pEntity->GetWorldSpaceCenter(); //body
	case 1: return pEntity->GetWorldSpaceCenter() - Vec3(0.0f, 0.0f, 27.0f); //feet
	case 2: //auto
	{
		switch (pLocal->GetClassNum())
		{
			//case CLASS_SOLDIER: 
		case CLASS_SOLDIER: //soldier just aims at legs
		{
			return pEntity->GetWorldSpaceCenter() - Vec3(0.0f, 0.0f, 27.0f);
		}
		case CLASS_DEMOMAN:
		{
			if (Vars::Aimbot::Projectile::FeetAimIfOnGround.m_Var && pEntity->GetFlags() & FL_ONGROUND) return
				pEntity->GetWorldSpaceCenter() - Vec3(0.0f, 0.0f, 27.0f);
			return pEntity->GetWorldSpaceCenter();
		}
		case CLASS_SNIPER:
		{
			CBaseCombatWeapon* wep = pLocal->GetActiveWeapon();
			Vec3 vPos = pEntity->GetHitboxPos(HITBOX_HEAD);
			if (wep != nullptr)
			{
				if (wep->GetChargeDamage() >= pEntity->GetHealth())
				{
					vPos = pEntity->GetHitboxPos(HITBOX_PELVIS);
				}
			}


			Vec3 vEntForward = {};
			Math::AngleVectors(pEntity->GetEyeAngles(), &vEntForward);
			Vec3 vToEnt = pEntity->GetAbsOrigin() - pLocal->GetAbsOrigin();
			vToEnt.NormalizeInPlace();

			if (Vars::Aimbot::Projectile::ManualZAdjust.m_Var)
			{
				vPos.z += Vars::Aimbot::Projectile::ZAdjustAmount.m_Var;

				return vPos;
			}
			if (vToEnt.Dot(vEntForward) > 0.1071f)
			{
				vPos.z += 5.0f;
			}

			return vPos;
		}

		default: return pEntity->GetWorldSpaceCenter();
		}
	}
	default: return Vec3();
	}
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

void projectileTracer(CBaseEntity* pLocal, Target_t Target)
{
	if (pLocal->IsTaunting())
	{
		return;
	}
	Vec3 vecPos = g_GlobalInfo.m_WeaponType == EWeaponType::PROJECTILE ? g_GlobalInfo.m_vPredictedPos : Target.m_vPos;
	Color_t Color = Vars::Visuals::BulletTracerRainbow.m_Var ? Utils::Rainbow() : Colors::BulletTracer;
	Vec3 shootPos;
	int iAttachment = pLocal->GetActiveWeapon()->LookupAttachment(_("muzzle"));
	pLocal->GetActiveWeapon()->GetAttachment(iAttachment, shootPos);
	g_Interfaces.DebugOverlay->AddLineOverlayAlpha(shootPos, vecPos, Color.r, Color.g, Color.b, Color.a, true, 5);
}

bool CAimbotProjectile::GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	ESortMethod SortMethod = GetSortMethod();

	if (SortMethod == ESortMethod::FOV)
		g_GlobalInfo.m_flCurAimFOV = Vars::Aimbot::Global::AimFOV.m_Var;

	g_AimbotGlobal.m_vecTargets.clear();

	Vec3 vLocalPos = pLocal->GetShootPos();
	Vec3 vLocalAngles = g_Interfaces.Engine->GetViewAngles();

	PlayerInfo_t info{};

	if (Vars::Aimbot::Global::AimPlayers.m_Var)
	{
		int nWeaponID = pWeapon->GetWeaponID();
		bool bIsCrossbow = nWeaponID == TF_WEAPON_CROSSBOW;

		for (const auto& Player : g_EntityCache.GetGroup(
			bIsCrossbow ? EGroupType::PLAYERS_ALL : EGroupType::PLAYERS_ENEMIES))
		{
			if (!Player->IsAlive() || Player->IsAGhost() || Player == pLocal || (bIsCrossbow && (Player->GetHealth() >=
				Player->GetMaxHealth()) && (Player->GetTeamNum() == pLocal->GetTeamNum())))
				continue;

			if (!g_Interfaces.Engine->GetPlayerInfo(Player->GetIndex(), &info))
				continue;

			if (Player->GetTeamNum() != pLocal->GetTeamNum())
			{
				if (Vars::Aimbot::Global::IgnoreInvlunerable.m_Var && !Player->IsVulnerable())
					continue;

				if (Vars::Aimbot::Global::IgnoreCloaked.m_Var && Player->IsCloaked())
				{
					int nCond = Player->GetCond();
					if (nCond & TFCond_Milked || nCond & TFCond_Jarated)
					{
						//pass
					}
					else
					{
						continue;
					}
				}

				if (Vars::Aimbot::Global::IgnoreTaunting.m_Var && Player->IsTaunting())
					continue;

				if (Vars::Aimbot::Global::IgnoreFriends.m_Var && g_EntityCache.Friends[Player->GetIndex()] && Player->
					GetTeamNum() != g_EntityCache.m_pLocal->GetTeamNum())
					continue;

				if (g_GlobalInfo.ignoredPlayers.find(info.friendsID) != g_GlobalInfo.ignoredPlayers.end())
					continue;
			}

			Vec3 vPos = GetAimPos(pLocal, Player);
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			float flFOVTo = (SortMethod == ESortMethod::FOV) ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			float flDistTo = (SortMethod == ESortMethod::DISTANCE) ? vLocalPos.DistTo(vPos) : 0.0f;

			if (SortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.m_Var)
				continue;

			g_AimbotGlobal.m_vecTargets.push_back({ Player, ETargetType::PLAYER, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	if (Vars::Aimbot::Global::AimBuildings.m_Var)
	{
		bool bIsRescueRanger = pWeapon->GetWeaponID() == TF_WEAPON_SHOTGUN_BUILDING_RESCUE;

		for (const auto& Building : g_EntityCache.GetGroup(
			bIsRescueRanger ? EGroupType::BUILDINGS_ALL : EGroupType::BUILDINGS_ENEMIES))
		{
			if (!Building->IsAlive())
				continue;

			Vec3 vPos = Building->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			float flFOVTo = SortMethod == ESortMethod::FOV ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			float flDistTo = SortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			if (SortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.m_Var)
				continue;

			g_AimbotGlobal.m_vecTargets.push_back({ Building, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	return !g_AimbotGlobal.m_vecTargets.empty();
}

bool CAimbotProjectile::VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Target_t& Target)
{
	ProjectileInfo_t ProjInfo = {};

	if (!GetProjectileInfo(pWeapon, ProjInfo))
		return false;

	auto vVelocity = Vec3();
	auto vAcceleration = Vec3();

	switch (Target.m_TargetType)
	{
	case ETargetType::PLAYER:
	{
		vVelocity = Target.m_pEntity->GetVelocity();
		vAcceleration = Vec3(
			0.0f, 0.0f, g_ConVars.sv_gravity->GetFloat() * ((Target.m_pEntity->GetCondEx2() & TFCondEx2_Parachute)
				? 0.224f
				: 1.0f));
		break;
	}

	default: break;
	}

	Predictor_t Predictor = {
		Target.m_pEntity,
		Target.m_vPos,
		vVelocity,
		vAcceleration
	};

	Solution_t Solution = {};

	if (!SolveProjectile(pLocal, pWeapon, pCmd, Predictor, ProjInfo, Solution))
		return false;

	Target.m_vAngleTo = { -RAD2DEG(Solution.m_flPitch), RAD2DEG(Solution.m_flYaw), 0.0f };

	return true;
}

bool CAimbotProjectile::GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Target_t& Out)
{
	if (!GetTargets(pLocal, pWeapon))
		return false;

	if (Vars::Aimbot::Projectile::PerformanceMode.m_Var)
	{
		Target_t Target = g_AimbotGlobal.GetBestTarget(GetSortMethod());

		if (!VerifyTarget(pLocal, pWeapon, pCmd, Target))
			return false;

		Out = Target;
		return true;
	}
	g_AimbotGlobal.SortTargets(GetSortMethod());

	//instead of this just limit to like 4-6 targets, should save perf without any noticeable changes in functionality
	for (auto& Target : g_AimbotGlobal.m_vecTargets)
	{
		if (!VerifyTarget(pLocal, pWeapon, pCmd, Target))
			continue;

		Out = Target;
		return true;
	}

	return false;
}

void CAimbotProjectile::Aim(CUserCmd* pCmd, CBaseCombatWeapon* pWeapon, Vec3& vAngle)
{
	vAngle -= g_GlobalInfo.m_vPunchAngles;
	Math::ClampAngles(vAngle);

	switch (Vars::Aimbot::Projectile::AimMethod.m_Var)
	{
	case 0:
	{
		pCmd->viewangles = vAngle;
		g_Interfaces.Engine->SetViewAngles(pCmd->viewangles);
		break;
	}

	case 1:
	{
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

bool CAimbotProjectile::IsAttacking(CUserCmd* pCmd, CBaseCombatWeapon* pWeapon)
{
	if (g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheBeggarsBazooka)
	{
		static bool bLoading = false;

		if (pWeapon->GetClip1() > 0)
			bLoading = true;

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
				bCharging = true;

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
				return true;
		}
	}

	return false;
}

void CAimbotProjectile::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	static int nLastTracerTick = pCmd->tick_count;

	m_bIsFlameThrower = false;

	if (!Vars::Aimbot::Global::Active.m_Var)
		return;

	Target_t Target = {};

	bool bShouldAim = (Vars::Aimbot::Global::AimKey.m_Var == VK_LBUTTON
		? (pCmd->buttons & IN_ATTACK)
		: g_AimbotGlobal.IsKeyDown());

	if (GetTarget(pLocal, pWeapon, pCmd, Target) && bShouldAim)
	{
		g_GlobalInfo.m_nCurrentTargetIdx = Target.m_pEntity->GetIndex();

		if (Vars::Aimbot::Projectile::AimMethod.m_Var == 1)
			g_GlobalInfo.m_vAimPos = g_GlobalInfo.m_vPredictedPos;

		if (ShouldFire(pCmd))
		{
			pCmd->buttons |= IN_ATTACK;

			if (Vars::Misc::CL_Move::Enabled.m_Var && Vars::Misc::CL_Move::Doubletap.m_Var && (pCmd->buttons & IN_ATTACK) && g_GlobalInfo.m_nShifted && !g_GlobalInfo.m_nWaitForShift) {
				if (
					(Vars::Misc::CL_Move::DTMode.m_Var == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) ||
					(Vars::Misc::CL_Move::DTMode.m_Var == 1) ||
					(Vars::Misc::CL_Move::DTMode.m_Var == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)))
				{
					if ((Vars::Misc::CL_Move::NotInAir.m_Var && !pLocal->IsOnGround() && g_GlobalInfo.m_nShifted))
					{
						g_GlobalInfo.m_bShouldShift = false;
					}
					else {
						g_GlobalInfo.m_bShouldShift = true;
					}
				}
			}

			if (g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheBeggarsBazooka)
			{
				if (pWeapon->GetClip1() > 0)
					pCmd->buttons &= ~IN_ATTACK;
			}

			else
			{
				if (pWeapon->GetWeaponID() == TF_WEAPON_COMPOUND_BOW && pWeapon->GetChargeBeginTime() > 0.0f)
					pCmd->buttons &= ~IN_ATTACK;
			}
		}

		bool bIsAttacking = IsAttacking(pCmd, pWeapon);

		if (bIsAttacking)
		{
			g_GlobalInfo.m_bAttacking = true;
			if (Vars::Visuals::BulletTracer.m_Var && abs(pCmd->tick_count - nLastTracerTick) > 1)
			{
				projectileTracer(pLocal, Target);
				nLastTracerTick = pCmd->tick_count;
			}

			//g_Interfaces.DebugOverlay->AddLineOverlayAlpha(Target.m_vPos, g_GlobalInfo.m_vPredictedPos, 0, 255, 0, 255, true, 2); // Predicted aim pos
		}

		if (Vars::Aimbot::Projectile::AimMethod.m_Var == 1)
		{
			if (m_bIsFlameThrower)
			{
				g_GlobalInfo.m_bProjectileSilentActive = true;
				Aim(pCmd, pWeapon, Target.m_vAngleTo);
			}

			else
			{
				if (bIsAttacking)
				{
					Aim(pCmd, pWeapon, Target.m_vAngleTo);
					g_GlobalInfo.m_bSilentTime = true;
				}
			}
		}

		else Aim(pCmd, pWeapon, Target.m_vAngleTo);
	}
}
