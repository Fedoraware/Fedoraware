#include "AimbotProjectile.h"
#include "../../Vars.h"
#include "../MovementSimulation/MovementSimulation.h"

Vec3 CAimbotProjectile::Predictor_t::Extrapolate(float time)
{
	G::LinearPredLine = m_vPosition;

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
	switch (G::CurItemDefIndex)
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
				Math::RemapValClamped(charge, 0.0f, 1.f, 0.483f, 0.085f)
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
	const INetChannel* pNetChannel = I::Engine->GetNetChannelInfo();
	static ConVar* cl_flipviewmodels = g_ConVars.cl_flipviewmodels;

	G::PredBeforeLines.clear(); G::PredFutureLines.clear();
	
	if (!G::WeaponCanAttack) {
		return true;	// we can't attack, so it shouldn't matter if we say we've solved it, also shouldn't f wit da chammies iykyk
	}

	if (!pNetChannel || !cl_flipviewmodels)
	{
		return false;
	}

	static bool oValue = cl_flipviewmodels->GetBool(); // assume false
	if (Vars::Debug::DebugInfo.Value) {
		cl_flipviewmodels->SetValue(oValue);
	}

	Ray_t traceRay = {};
	CGameTrace trace = {};
	static CTraceFilterWorldAndPropsOnly traceFilter = {};
	traceFilter.pSkip = predictor.m_pEntity;

	Vec3 vLocalPos = pLocal->GetEyePosition();
	const float maxTime = predictor.m_pEntity->IsPlayer() ? (projInfo.m_flMaxTime == 0.f ? Vars::Aimbot::Projectile::predTime.Value : projInfo.m_flMaxTime) : (projInfo.m_flMaxTime == 0.f ? 1024.f : projInfo.m_flMaxTime);
	const float fLatency = pNetChannel->GetLatency(MAX_FLOWS);

	/*
			This should now be able to predict anything that moves.
			Should also stop wasting time predicting static players.
	*/
	const bool useTPred = !predictor.m_pEntity->GetVecVelocity().IsZero() ? true : false;

	if (!useTPred) {
		for (int flipState = Vars::Debug::DebugInfo.Value ? 0 : 1; flipState <= 1; flipState++) {
			if (Vars::Debug::DebugInfo.Value) {
				cl_flipviewmodels->SetValue(flipState ? !oValue : oValue);
			}
			Vec3 staticPos = predictor.m_pEntity->IsPlayer() ? GetAimPos(pLocal, predictor.m_pEntity, predictor.m_vPosition) : GetAimPosBuilding(pLocal, predictor.m_pEntity);
			if (staticPos.IsZero()) {
				return false;
			}

			// get angle offsets for demoman weapons?weew
			switch (pWeapon->GetWeaponID())
			{
			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_PIPEBOMBLAUNCHER:
			case TF_WEAPON_STICKBOMB:
			case TF_WEAPON_STICKY_BALL_LAUNCHER:
			{
				Vec3 vDelta = (staticPos - vLocalPos);
				const float fRange = Math::VectorNormalize(vDelta);
				const float fElevationAngle = std::min(fRange * (G::CurItemDefIndex == Demoman_m_TheLochnLoad ? 0.0075f : 0.013f), 45.f); // if our angle is above 45 degree will we even hit them? shouldn't we just return???

				float s = 0.0f, c = 0.0f;
				Math::SinCos((fElevationAngle * PI / 180.0f), &s, &c);

				const float fElevation = (fRange * (s / c));
				staticPos.z += (c > 0.0f ? fElevation : 0.0f);
				break;
			}

			default: break;
			}

			// trace hull of projectile
			Utils::TraceHull(predictor.m_vPosition, staticPos, Vec3(-3.8f, -3.8f, -3.8f), Vec3(3.8f, 3.8f, 3.8f), MASK_SOLID_BRUSHONLY, &traceFilter, &trace);
			if (trace.DidHit())
			{
				staticPos.z = trace.vEndPos.z;
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

			if (!CalcProjAngle(vLocalPos, staticPos, projInfo, out))
			{
				return false;
			}

			if (out.m_flTime > maxTime) {
				return false;
			}

			if (WillProjectileHit(pLocal, pWeapon, pCmd, staticPos, out, projInfo, predictor)) {
				G::PredictedPos = staticPos;
				return true;
			}
		}
	}
	else {
		Vec3 vPredictedPos = {};
		CMoveData moveData = {};
		Vec3 absOrigin = {};

		for (int flipState = Vars::Debug::DebugInfo.Value ? 0 : 1; flipState <= 1; flipState++) {
			if (Vars::Debug::DebugInfo.Value) {
				cl_flipviewmodels->SetValue(flipState ? !oValue : oValue);
			}
			if (F::MoveSim.Initialize(predictor.m_pEntity))
			{
				for (int n = 0; n < TIME_TO_TICKS(maxTime); n++)
				{
					if (predictor.m_pEntity == nullptr)
					{
						break;
					}
					F::MoveSim.RunTick(moveData, absOrigin);
					vPredictedPos = absOrigin;

					const Vec3 aimPosition = GetAimPos(pLocal, predictor.m_pEntity, vPredictedPos);
					if (aimPosition.IsZero()) {
						break;
					} // don't remove.

					//const Vec3 vAimDelta = predictor.m_pEntity->GetAbsOrigin() - aimPosition;
					//vPredictedPos.x += abs(vAimDelta.x);
					//vPredictedPos.y += abs(vAimDelta.y);
					//vPredictedPos.z += abs(vAimDelta.z);
					vPredictedPos = aimPosition;


					// get angle offsets for demoman weapons?
					switch (pWeapon->GetWeaponID())
					{
					case TF_WEAPON_GRENADELAUNCHER:
					case TF_WEAPON_PIPEBOMBLAUNCHER:
					case TF_WEAPON_STICKBOMB:
					case TF_WEAPON_STICKY_BALL_LAUNCHER:
					{
						Vec3 vDelta = (vPredictedPos - vLocalPos);
						const float fRange = Math::VectorNormalize(vDelta);
						const float fElevationAngle = std::min(fRange * (G::CurItemDefIndex == Demoman_m_TheLochnLoad ? 0.0075f : 0.013f), 45.f); // if our angle is above 45 degree will we even hit them? shouldn't we just return???

						float s = 0.0f, c = 0.0f;
						Math::SinCos((fElevationAngle * PI / 180.0f), &s, &c);

						const float fElevation = (fRange * (s / c));
						vPredictedPos.z += (c > 0.0f ? fElevation : 0.0f);
						break;
					}
					default: break;
					}

					//Utils::TraceHull(predictor.m_vPosition, vPredictedPos, Vec3(-3.8f, -3.8f, -3.8f), Vec3(3.8f, 3.8f, 3.8f),
					//	MASK_SOLID_BRUSHONLY, &traceFilter, &trace);

					//if (trace.DidHit())
					//{
					//	vPredictedPos.z = trace.vEndPos.z;
						G::PredictedPos = vPredictedPos;
					//}

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
						if (WillProjectileHit(pLocal, pWeapon, pCmd, vPredictedPos, out, projInfo, predictor)) {

							G::PredictedPos = vPredictedPos;
							F::MoveSim.Restore();
							return true;
						}
					}
				}
				F::MoveSim.Restore();
			}
		}
	}
	return false;
}

bool IsPointAllowed(int nHitbox) {
	switch (nHitbox) {
	case 0: return Vars::Aimbot::Projectile::AllowedHitboxes.Value & (1 << 0);
	case 1: return Vars::Aimbot::Projectile::AllowedHitboxes.Value & (1 << 1);
	case 2: return Vars::Aimbot::Projectile::AllowedHitboxes.Value & (1 << 2);
	case 3:
	case 4:
	case 5:
	case 6: return Vars::Aimbot::Projectile::AllowedHitboxes.Value & (1 << 0);
	case 7:
	case 8:
	case 9:
	case 10: return Vars::Aimbot::Projectile::AllowedHitboxes.Value & (1 << 1);
	case 11:
	case 12:
	case 13:
	case 14: return Vars::Aimbot::Projectile::AllowedHitboxes.Value & (1 << 2);
	}
	return true; // never
}

Vec3 getHeadOffset(CBaseEntity* pEntity) {
	const Vec3 headPos = pEntity->GetHitboxPos(HITBOX_HEAD);
	const Vec3 entPos = pEntity->GetAbsOrigin();
	const Vec3 delta = entPos - headPos;
	return delta * -1.f;
}

//	Tries to find the best position to aim at on our target.
Vec3 CAimbotProjectile::GetAimPos(CBaseEntity* pLocal, CBaseEntity* pEntity, const Vec3& targetPredPos)
{
	Vec3 retVec = pLocal->GetAbsOrigin();
	Vec3 localPos = pLocal->GetAbsOrigin();

	const Vec3 vLocalPos = pLocal->GetShootPos();

	const bool bIsDucking = (pEntity->m_bDucked() || pEntity->m_fFlags() & FL_DUCKING);

	const float bboxScale = Vars::Aimbot::Projectile::ScanScale.Value; // stop shoot flor (:D)

	// this way overshoots players that are crouching and I don't know why.
	const Vec3 vMaxs = I::GameMovement->GetPlayerMaxs(bIsDucking) * bboxScale;
	const Vec3 vMins = Vec3(-vMaxs.x, -vMaxs.y, vMaxs.z - vMaxs.z * bboxScale);

	const Vec3 headDelta = getHeadOffset(pEntity);

	const std::vector vecPoints = {	// oh you don't like 15 points because it fucks your fps??? TOO BAD!//
		Vec3(headDelta.x, headDelta.y, vMaxs.z),				//	head bone probably
		Vec3(0, 0, (vMins.z + vMaxs.z) / 2),					//	middles (scan first bc they are more accurate)
		Vec3(0, 0, vMins.z),									//	-
		Vec3(vMins.x, vMins.y, vMaxs.z),						//	top four corners
		Vec3(vMins.x, vMaxs.y, vMaxs.z),						//	-
		Vec3(vMaxs.x, vMaxs.y, vMaxs.z),						//	-
		Vec3(vMaxs.x, vMins.y, vMaxs.z),						//	-
		Vec3(vMins.x, vMins.y, (vMins.z + vMaxs.z) / 2),		//	middle four corners
		Vec3(vMins.x, vMaxs.y, (vMins.z + vMaxs.z) / 2),		//	-
		Vec3(vMaxs.x, vMaxs.y, (vMins.z + vMaxs.z) / 2),		//	-
		Vec3(vMaxs.x, vMins.y, (vMins.z + vMaxs.z) / 2),		//	-
		Vec3(vMins.x, vMins.y, vMins.z),						//	bottom four corners
		Vec3(vMins.x, vMaxs.y, vMins.z),						//	-
		Vec3(vMaxs.x, vMaxs.y, vMins.z),						//	-
		Vec3(vMaxs.x, vMins.y, vMins.z)							//	-
	};

	std::vector<Vec3> visiblePoints{};
	const matrix3x4 transform = {
		{1.f, 0, 0, targetPredPos.x},
		{0, 1.f, 0, targetPredPos.y},
		{0, 0, 1.f, pEntity->GetVecVelocity().IsZero() ? pEntity->GetAbsOrigin().z : targetPredPos.z}
	};

	int aimMethod = Vars::Aimbot::Projectile::AimPosition.Value;
	int curPoint = 0, testPoints = 0; //maybe better way to do this
	for (const auto& point : vecPoints)
	{
		if (testPoints > Vars::Aimbot::Projectile::VisTestPoints.Value) { break; }
		if (visiblePoints.size() >= Vars::Aimbot::Projectile::ScanPoints.Value) { break; }
		if (!IsPointAllowed(curPoint)) { curPoint++; continue; }

		Vec3 vTransformed = {};
		Math::VectorTransform(point, transform, vTransformed);

		if (Utils::VisPos(pLocal, pEntity, vLocalPos, vTransformed))
		{
			if (curPoint == aimMethod && aimMethod < 3) { return vTransformed; }	// return this value now if it is going to get returned anyway, avoid useless scanning.
			visiblePoints.push_back(vTransformed);
		}
		curPoint++;
		testPoints++;	// Only increment this if we actually tested.
	}
	if (visiblePoints.empty()) { 
		return Vec3(0, 0, 0); 
	}

	Vec3 HeadPoint, TorsoPoint, FeetPoint;

	const int classNum = pLocal->GetClassNum();

	switch (classNum) {
	case CLASS_SOLDIER:
	case CLASS_DEMOMAN:
	{
		if (Vars::Aimbot::Projectile::FeetAimIfOnGround.Value && pEntity->IsOnGround()) {
			aimMethod = 2;
		}
		break;
	}
	}

	if (aimMethod == 3 && classNum) { // auto
		switch (classNum) {
		case CLASS_SOLDIER:
		case CLASS_DEMOMAN:
		{
			aimMethod = 1;
			break;
		}
		case CLASS_SNIPER:
		{
			aimMethod = 0;
			break;
		}
		default:
		{
			aimMethod = 1;
			break;
		}
		}
	}

	switch (aimMethod) {
	case 0: {	//head
		Math::VectorTransform(vecPoints.at(0), transform, HeadPoint);			//	get transformed location of our "best point" for our selected prio hitbox
		for (const auto& aimPoint : visiblePoints) {							//	iterate through visible points
			if (aimPoint.DistTo(HeadPoint) < retVec.DistTo(HeadPoint)) {		//	if the distance to our best point is lower than the previous selected point,
				retVec = aimPoint;												//	set the new point to our currently selected point
			}
		}
		break;
	}
	case 1: {	//torso
		Math::VectorTransform(vecPoints.at(1), transform, TorsoPoint);
		for (const auto& aimPoint : visiblePoints) {
			if (aimPoint.DistTo(TorsoPoint) < retVec.DistTo(TorsoPoint)) {
				retVec = aimPoint;
			}
		}
		break;
	}
	case 2: {	//feet
		Math::VectorTransform(vecPoints.at(2), transform, FeetPoint);
		for (const auto& aimPoint : visiblePoints) {
			if (aimPoint.DistTo(FeetPoint) < retVec.DistTo(FeetPoint)) {
				retVec = aimPoint;
			}
		}
		break;
	}
	}
	return retVec;
}

Vec3 CAimbotProjectile::GetAimPosBuilding(CBaseEntity* pLocal, CBaseEntity* pEntity) {
	Vec3 retVec = pLocal->GetAbsOrigin();
	Vec3 localPos = pLocal->GetAbsOrigin();

	const Vec3 vLocalPos = pLocal->GetShootPos();

	const float bboxScale = std::max(Vars::Aimbot::Projectile::ScanScale.Value - 0.05f, 0.5f);	// set the maximum scale for buildings at .95f

	const Vec3 vMins = pEntity->GetCollideableMins() * bboxScale;
	const Vec3 vMaxs = pEntity->GetCollideableMaxs() * bboxScale;

	const std::vector vecPoints = {
		Vec3(vMaxs.x / 2, vMaxs.y / 2, vMaxs.z / 2),								//	middle
		Vec3(vMins.x, vMins.y, vMaxs.z),											//	top four corners
		Vec3(vMins.x, vMaxs.y, vMaxs.z),											//	-
		Vec3(vMaxs.x, vMaxs.y, vMaxs.z),											//	-
		Vec3(vMaxs.x, vMins.y, vMaxs.z),											//	-
		Vec3(vMins.x, vMins.y, vMins.z),											//	bottom four corners
		Vec3(vMins.x, vMaxs.y, vMins.z),											//	-
		Vec3(vMaxs.x, vMaxs.y, vMins.z),											//	-
		Vec3(vMaxs.x, vMins.y, vMins.z)												//	-
	};

	const matrix3x4& transform = pEntity->GetRgflCoordinateFrame();

	for (const auto& point : vecPoints)
	{
		Vec3 vTransformed = {};
		Math::VectorTransform(point, transform, vTransformed);

		if (Utils::VisPos(pLocal, pEntity, vLocalPos, vTransformed))
		{
			return vTransformed; // just return the first point we see
		}
	}
	return Vec3(0, 0, 0);
}

bool CAimbotProjectile::WillProjectileHit(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, const Vec3& vPredictedPos, Solution_t& out, const ProjectileInfo_t& projInfo,
                                          const Predictor_t& predictor)
{
	Vec3 hullSize = { 3.8f, 3.8f, 3.8f };
	Vec3 vVisCheck = pLocal->GetEyePosition();
	const Vec3 predictedViewAngles = {-RAD2DEG(out.m_flPitch), RAD2DEG(out.m_flYaw), 0.0f};
	CGameTrace trace = {};
	static CTraceFilterWorldAndPropsOnly traceFilter = {};
	traceFilter.pSkip = predictor.m_pEntity;

	// this shit's messy
	{
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
			hullSize = { 1.f, 1.f, 1.f };

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

			hullSize = { 8.f, 8.f, 8.f };

			auto vecAngle = Vec3(), vecForward = Vec3(), vecRight = Vec3(), vecUp = Vec3();
			Math::AngleVectors({ -RAD2DEG(out.m_flPitch), RAD2DEG(out.m_flYaw), 0.0f }, &vecForward, &vecRight, &vecUp);
			const Vec3 vecVelocity = ((vecForward * projInfo.m_flVelocity) - (vecUp * 200.0f));
			Math::VectorAngles(vecVelocity, vecAngle);
			out.m_flPitch = -DEG2RAD(vecAngle.x);

			// see relevant code @tf_weaponbase_gun.cpp L684
			float fRight = g_ConVars.cl_flipviewmodels->GetInt() ? -8.f : 8.f;
			vVisCheck += vecForward * 16.0f + vecRight * fRight + vecUp * -6.0f;

			Utils::TraceHull(pLocal->GetEyePosition(), vVisCheck, hullSize, hullSize * -1.f, MASK_SHOT_HULL, &traceFilter, &trace);
			if (trace.DidHit()) {
				return false;
			}

			break;
		}
		default: break;
		}
	}

	//	TODO: find the actual hull size of projectiles
	//	maybe - https://www.unknowncheats.me/forum/team-fortress-2-a/475502-weapons-projectile-min-max-collideables.html
	//	UTIL_SetSize( this, -Vector( 1.0f, 1.0f, 1.0f ), Vector( 1.0f, 1.0f, 1.0f ) ); @tf_projectile_base.cpp L117
	//	UTIL_TraceHull( vecEye, vecSrc, -Vector(8,8,8), Vector(8,8,8), MASK_SOLID_BRUSHONLY, &traceFilter, &trace ); @tf_weaponbase_gun.cpp L696 pills
	Utils::TraceHull(vVisCheck, vPredictedPos, hullSize, hullSize * -1.f, MASK_SHOT_HULL, &traceFilter, &trace);

	return !trace.DidHit();
}

ESortMethod CAimbotProjectile::GetSortMethod()
{
	switch (Vars::Aimbot::Projectile::SortMethod.Value)
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
	const Vec3 vecPos = G::CurWeaponType == EWeaponType::PROJECTILE ? G::PredictedPos : target.m_vPos;
	const Color_t tracerColor = Vars::Visuals::BulletTracerRainbow.Value ? Utils::Rainbow() : Colors::BulletTracer;
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
		G::CurAimFOV = Vars::Aimbot::Global::AimFOV.Value;
	}

	F::AimbotGlobal.m_vecTargets.clear();

	const Vec3 vLocalPos = pLocal->GetShootPos();
	const Vec3 vLocalAngles = I::Engine->GetViewAngles();

	// Players
	if (Vars::Aimbot::Global::AimPlayers.Value)
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
				if (F::AimbotGlobal.ShouldIgnore(pTarget)) { continue; }
			}

			Vec3 vPos = pTarget->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = (sortMethod == ESortMethod::FOV) ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			const float flDistTo = (sortMethod == ESortMethod::DISTANCE) ? vLocalPos.DistTo(vPos) : 0.0f;

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value)
			{
				continue;
			}

			const uint32_t priorityID = g_EntityCache.GetPR()->GetValid(pTarget->GetIndex()) ? g_EntityCache.GetPR()->GetAccountID(pTarget->GetIndex()) : 0;
			const auto& priority = G::PlayerPriority[priorityID];

			F::AimbotGlobal.m_vecTargets.push_back({ pTarget, ETargetType::PLAYER, vPos, vAngleTo, flFOVTo, flDistTo, -1, false, priority });
		}
	}

	// Buildings
	if (Vars::Aimbot::Global::AimBuildings.Value)
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

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value)
			{
				continue;
			}

			F::AimbotGlobal.m_vecTargets.push_back({pBuilding, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo});
		}
	}

	return !F::AimbotGlobal.m_vecTargets.empty();
}

bool CAimbotProjectile::VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, Target_t& target)
{
	ProjectileInfo_t projInfo = {};
	if (!GetProjectileInfo(pWeapon, projInfo))
	{
		return false;
	}

	if (pLocal->IsPrecisionRune() && !m_bIsFlameThrower) {
		projInfo.m_flVelocity *= 2.5f;

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

	F::AimbotGlobal.SortTargets(GetSortMethod());

	//instead of this just limit to like 4-6 targets, should save perf without any noticeable changes in functionality
	for (auto& target : F::AimbotGlobal.m_vecTargets)
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
	vAngle -= G::PunchAngles;
	Math::ClampAngles(vAngle);

	switch (Vars::Aimbot::Projectile::AimMethod.Value)
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
	return (Vars::Aimbot::Global::AutoShoot.Value && G::WeaponCanAttack);
}

bool CAimbotProjectile::IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon)
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
			if ((pCmd->buttons & IN_ATTACK) && G::WeaponCanAttack)
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
	if (!Vars::Aimbot::Projectile::SplashPrediction.Value) { return false; }

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
	const auto& vLocalOrigin = pLocal->GetAbsOrigin();

	for (const auto& pTarget : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!pTarget || !pTarget->IsAlive() || !pTarget->IsOnGround()) { continue; }

		const auto& vTargetCenter = pTarget->GetWorldSpaceCenter();
		const auto& vTargetOrigin = pTarget->GetAbsOrigin();

		if (vLocalOrigin.DistTo(vTargetOrigin) > 800.f) { continue; } // Don't shoot too far
		if (vLocalOrigin.z < vTargetOrigin.z - 45.f) { continue; } // Don't shoot from below

		// Don't predict enemies that are visible
		if (Utils::VisPos(pLocal, pTarget, pLocal->GetShootPos(), vTargetCenter)) { continue; }

		// Scan every 45 degree angle
		for (int i = 0; i < 315; i += 45)
		{
			Vec3 scanPos = Utils::GetRotatedPosition(vTargetCenter, static_cast<float>(i), *splashRadius);

			CGameTrace trace = {};
			CTraceFilterWorldAndPropsOnly traceFilter = {};

			// Check FOV if enabled
			const Vec3 vAngleTo = Math::CalcAngle(vLocalShootPos, scanPos);
			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);
			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value) { continue; }

			// Can the target receive splash damage? (Don't predict through walls)
			Utils::Trace(scanPos, pTarget->GetWorldSpaceCenter(), MASK_SOLID, &traceFilter, &trace);
			if (trace.flFraction < 0.99f && trace.entity != pTarget) { continue; }

			// Is the predicted position even visible?
			if (!Utils::WillProjectileHit(pLocal, pWeapon, scanPos)) { continue; }

			// Get the closest point to the target
			float currentRadius = *splashRadius;
			while (currentRadius > 10.f && Utils::WillProjectileHit(pLocal, pWeapon, scanPos))
			{
				scanPos = Utils::GetRotatedPosition(vTargetCenter, static_cast<float>(i), currentRadius - 10.f);
				currentRadius -= 10.f;
			}

			/*
			 *	We found the closest point!
			 *	Now we need to get the shoot pos/angles relative to vTargetOrigin instead of vTargetCenter
			 */
			currentRadius = std::clamp(currentRadius + 10.f, 0.f, *splashRadius);
			scanPos = Utils::GetRotatedPosition(vTargetOrigin, static_cast<float>(i), currentRadius);

			if (Vars::Debug::DebugInfo.Value)
			{
				I::DebugOverlay->AddLineOverlay(vTargetOrigin, scanPos, 255, 0, 0, false, MAXIMUM_TICK_INTERVAL);
				I::DebugOverlay->AddTextOverlay(scanPos, MAXIMUM_TICK_INTERVAL, "X");
			}

			const Vec3 vAngleToSplash = Math::CalcAngle(pLocal->GetEyePosition(), scanPos);
			outTarget = { pTarget, ETargetType::PLAYER, vTargetOrigin, vAngleToSplash };
			return true;
		}
	}

	return false;
}

void CAimbotProjectile::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	static int nLastTracerTick = pCmd->tick_count;

	m_bIsFlameThrower = false;

	if (!Vars::Aimbot::Global::Active.Value)
	{
		return;
	}

	const bool bShouldAim = (Vars::Aimbot::Global::AimKey.Value == VK_LBUTTON
		                         ? (pCmd->buttons & IN_ATTACK)
		                         : F::AimbotGlobal.IsKeyDown());
	if (!bShouldAim) { return; }

	Target_t target{ };
	if (GetTarget(pLocal, pWeapon, pCmd, target) || GetSplashTarget(pLocal, pWeapon, pCmd, target))
	{
		// Aim at the current target or splashtarget
		G::CurrentTargetIdx = target.m_pEntity->GetIndex();

		if (Vars::Aimbot::Projectile::AimMethod.Value == 1)
		{
			G::AimPos = G::PredictedPos;
		}

		if (ShouldFire(pCmd))
		{
			pCmd->buttons |= IN_ATTACK;

			if (Vars::Misc::CL_Move::Enabled.Value && Vars::Misc::CL_Move::Doubletap.Value && (pCmd->buttons & IN_ATTACK) && G::ShiftedTicks && !G::WaitForShift)
			{
				if (
					(Vars::Misc::CL_Move::DTMode.Value == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.Value)) ||
					(Vars::Misc::CL_Move::DTMode.Value == 1) ||
					(Vars::Misc::CL_Move::DTMode.Value == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.Value)))
				{
					if ((Vars::Misc::CL_Move::NotInAir.Value && !pLocal->IsOnGround() && G::ShiftedTicks))
					{
						G::ShouldShift = false;
					}
					else
					{
						G::ShouldShift = true;
					}
				}
			}

			if (G::CurItemDefIndex == Soldier_m_TheBeggarsBazooka)
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
			G::IsAttacking = true;
			if (Vars::Visuals::BulletTracer.Value && abs(pCmd->tick_count - nLastTracerTick) > 1)
			{
				ProjectileTracer(pLocal, target);
				nLastTracerTick = pCmd->tick_count;
			}

			//I::DebugOverlay->AddLineOverlayAlpha(Target.m_vPos, G::m_vPredictedPos, 0, 255, 0, 255, true, 2); // Predicted aim pos
		}

		if (Vars::Aimbot::Projectile::AimMethod.Value == 1)
		{
			if (m_bIsFlameThrower)
			{
				G::ProjectileSilentActive = true;
				Aim(pCmd, pWeapon, target.m_vAngleTo);
			}

			else
			{
				if (bIsAttacking)
				{
					Aim(pCmd, pWeapon, target.m_vAngleTo);
					G::SilentTime = true;
				}
			}
		}
		else
		{
			Aim(pCmd, pWeapon, target.m_vAngleTo);
		}
	}
}
