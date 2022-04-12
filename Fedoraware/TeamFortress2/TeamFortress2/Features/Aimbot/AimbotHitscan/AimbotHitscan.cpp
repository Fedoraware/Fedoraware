#include "AimbotHitscan.h"
#include "../../Vars.h"
#include "../../Backtrack/Backtrack.h"

int CAimbotHitscan::GetHitbox(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	switch (Vars::Aimbot::Hitscan::AimHitbox.m_Var)
	{
	case 0: { return HITBOX_HEAD; }
	case 1: { return HITBOX_SPINE_1; }
	case 2: { return HITBOX_PELVIS; }
	case 3:
	{
		int nClassNum = pLocal->GetClassNum();

		if (nClassNum == CLASS_SNIPER)
		{
			if (g_GlobalInfo.m_nCurItemDefIndex != Sniper_m_TheSydneySleeper)
				return (pLocal->IsScoped() ? HITBOX_HEAD : HITBOX_PELVIS);

			return HITBOX_PELVIS;
		}
		if (nClassNum == CLASS_SPY)
		{
			bool bIsAmbassador = (g_GlobalInfo.m_nCurItemDefIndex == Spy_m_TheAmbassador || g_GlobalInfo.
				m_nCurItemDefIndex == Spy_m_FestiveAmbassador);
			return (bIsAmbassador ? HITBOX_HEAD : HITBOX_PELVIS);
		}
		return HITBOX_PELVIS;
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
	ESortMethod SortMethod = GetSortMethod();

	if (SortMethod == ESortMethod::FOV)
		g_GlobalInfo.m_flCurAimFOV = Vars::Aimbot::Global::AimFOV.m_Var;

	g_AimbotGlobal.m_vecTargets.clear();

	Vec3 vLocalPos = pLocal->GetShootPos();
	Vec3 vLocalAngles = g_Interfaces.Engine->GetViewAngles();

	PlayerInfo_t info{};

	if (Vars::Aimbot::Global::AimPlayers.m_Var)
	{
		int nHitbox = GetHitbox(pLocal, pWeapon);
		bool bIsMedigun = pWeapon->GetWeaponID() == TF_WEAPON_MEDIGUN;

		for (const auto& Player : g_EntityCache.GetGroup(bIsMedigun ? EGroupType::PLAYERS_TEAMMATES : SandvichAimbot::bIsSandvich ? EGroupType::PLAYERS_ALL : EGroupType::PLAYERS_ENEMIES))
		{
			if (!Player->IsAlive() || Player->IsAGhost())
				continue;

			if (bIsMedigun && pLocal->GetWorldSpaceCenter().DistTo(Player->GetWorldSpaceCenter()) > 472.f)
				continue;

			if (!g_Interfaces.Engine->GetPlayerInfo(Player->GetIndex(), &info))
				continue;

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

			if (Vars::Aimbot::Global::IgnoreTaunting.m_Var && Player->IsTaunting() && (!bIsMedigun))
				continue;

			if (Vars::Aimbot::Global::IgnoreFriends.m_Var && g_EntityCache.Friends[Player->GetIndex()] && (!bIsMedigun))
				continue;

			if (g_GlobalInfo.ignoredPlayers.find(info.friendsID) != g_GlobalInfo.ignoredPlayers.end() && (!bIsMedigun))
				continue;

			if (Vars::Aimbot::Global::BAimLethal.m_Var)
			{
				if (pWeapon->GetChargeDamage() >= Player->GetHealth())
				{
					nHitbox = HITBOX_PELVIS;
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
					float flDistTo = Player->GetAbsOrigin().DistTo(pLocal->GetAbsOrigin());
					int nAmbassadorBodyshotDamage = Math::RemapValClamped(flDistTo, 90, 900, 51, 18);

					if (Player->GetHealth() < (nAmbassadorBodyshotDamage + 2)) // whatever
					{
						nHitbox = HITBOX_PELVIS;
					}
				}
			}

			Vec3 vPos = Player->GetHitboxPos(nHitbox);
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			float flFOVTo = SortMethod == ESortMethod::FOV ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			float flDistTo = SortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			if (SortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.m_Var)
				continue;

			g_AimbotGlobal.m_vecTargets.push_back({
				Player, ETargetType::PLAYER, vPos, vAngleTo, flFOVTo, flDistTo, nHitbox
				});
		}
	}

	if (Vars::Aimbot::Global::AimBuildings.m_Var)
	{
		for (const auto& Building : g_EntityCache.GetGroup(EGroupType::BUILDINGS_ENEMIES))
		{
			if (!Building->IsAlive())
				continue;

			Vec3 vPos = Building->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			float flFOVTo = SortMethod == ESortMethod::FOV ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;

			if (SortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.m_Var)
				continue;

			float flDistTo = SortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			g_AimbotGlobal.m_vecTargets.push_back({ Building, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	return !g_AimbotGlobal.m_vecTargets.empty();
}

bool CAimbotHitscan::ScanHitboxes(CBaseEntity* pLocal, Target_t& Target)
{
	if (Target.m_TargetType == ETargetType::PLAYER)
	{
		if (!Vars::Aimbot::Hitscan::ScanHitboxes.m_Var)
			return false;
	}

	else if (Target.m_TargetType == ETargetType::BUILDING)
	{
		if (!Vars::Aimbot::Hitscan::ScanBuildings.m_Var)
			return false;
	}

	Vec3 vLocalPos = pLocal->GetShootPos();

	for (int nHitbox = Target.m_TargetType == ETargetType::PLAYER ? 1 : 0; nHitbox < Target.m_pEntity->
		GetNumOfHitboxes(); nHitbox++)
	{
		if (Target.m_TargetType == ETargetType::PLAYER && nHitbox == HITBOX_PELVIS)
			continue;

		Vec3 vHitbox = Target.m_pEntity->GetHitboxPos(nHitbox);

		if (Utils::VisPos(pLocal, Target.m_pEntity, vLocalPos, vHitbox))
		{
			Target.m_vPos = vHitbox;
			Target.m_vAngleTo = Math::CalcAngle(vLocalPos, vHitbox);
			return true;
		}
	}

	return false;
}

bool CAimbotHitscan::ScanHead(CBaseEntity* pLocal, Target_t& Target)
{
	if (!Vars::Aimbot::Hitscan::ScanHead.m_Var)
		return false;

	model_t* pModel = Target.m_pEntity->GetModel();
	if (!pModel)
		return false;

	auto pHDR = g_Interfaces.ModelInfo->GetStudioModel(pModel);
	if (!pHDR)
		return false;

	matrix3x4 BoneMatrix[128];
	if (!Target.m_pEntity->SetupBones(BoneMatrix, 128, 0x100, g_Interfaces.GlobalVars->curtime))
		return false;

	mstudiohitboxset_t* pSet = pHDR->GetHitboxSet(Target.m_pEntity->GetHitboxSet());
	if (!pSet)
		return false;

	mstudiobbox_t* pBox = pSet->hitbox(HITBOX_HEAD);
	if (!pBox)
		return false;

	Vec3 vLocalPos = pLocal->GetShootPos();
	Vec3 vMins = pBox->bbmin;
	Vec3 vMaxs = pBox->bbmax;

	const float fScale = 0.8f;
	const std::vector<Vec3> vecPoints = {
		Vec3(((vMins.x + vMaxs.x) * 0.5f), (vMins.y * fScale), ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3((vMins.x * fScale), ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3((vMaxs.x * fScale), ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f))
	};

	for (const auto& Point : vecPoints)
	{
		Vec3 vTransformed = {};
		Math::VectorTransform(Point, BoneMatrix[pBox->bone], vTransformed);

		if (Utils::VisPosHitboxId(pLocal, Target.m_pEntity, vLocalPos, vTransformed, HITBOX_HEAD))
		{
			Target.m_vPos = vTransformed;
			Target.m_vAngleTo = Math::CalcAngle(vLocalPos, vTransformed);
			Target.m_bHasMultiPointed = true;
			return true;
		}
	}

	return false;
}

bool CAimbotHitscan::ScanBuildings(CBaseEntity* pLocal, Target_t& Target)
{
	if (!Vars::Aimbot::Hitscan::ScanBuildings.m_Var)
		return false;

	Vec3 vLocalPos = pLocal->GetShootPos();

	Vec3 vMins = Target.m_pEntity->GetCollideableMins();
	Vec3 vMaxs = Target.m_pEntity->GetCollideableMaxs();

	const std::vector<Vec3> vecPoints = {
		Vec3(vMins.x * 0.9f, ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3(vMaxs.x * 0.9f, ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3(((vMins.x + vMaxs.x) * 0.5f), vMins.y * 0.9f, ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3(((vMins.x + vMaxs.x) * 0.5f), vMaxs.y * 0.9f, ((vMins.z + vMaxs.z) * 0.5f)),
		Vec3(((vMins.x + vMaxs.x) * 0.5f), ((vMins.y + vMaxs.y) * 0.5f), vMins.z * 0.9f),
		Vec3(((vMins.x + vMaxs.x) * 0.5f), ((vMins.y + vMaxs.y) * 0.5f), vMaxs.z * 0.9f)
	};

	const matrix3x4& Transform = Target.m_pEntity->GetRgflCoordinateFrame();

	for (const auto& Point : vecPoints)
	{
		Vec3 vTransformed = {};
		Math::VectorTransform(Point, Transform, vTransformed);

		if (Utils::VisPos(pLocal, Target.m_pEntity, vLocalPos, vTransformed))
		{
			Target.m_vPos = vTransformed;
			Target.m_vAngleTo = Math::CalcAngle(vLocalPos, vTransformed);
			return true;
		}
	}

	return false;
}

bool CAimbotHitscan::VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& Target)
{
	switch (Target.m_TargetType)
	{
	case ETargetType::PLAYER:
	{
		if (Target.m_nAimedHitbox == HITBOX_HEAD)
		{
			int nHit = -1;

			if (Vars::Backtrack::Enabled.m_Var && Vars::Backtrack::Aim.m_Var && Vars::Aimbot::Hitscan::AimMethod.m_Var != 1) {
				if (!g_Backtrack.Record[Target.m_pEntity->GetIndex()].empty()) {
					if (Utils::VisPos(pLocal, Target.m_pEntity, pLocal->GetShootPos(),
						g_Backtrack.Record[Target.m_pEntity->GetIndex()].back().HeadPosition)) {
						Target.m_vAngleTo = Math::CalcAngle(pLocal->GetShootPos(), g_Backtrack.Record[Target.m_pEntity->GetIndex()].back().HeadPosition);
						return true;
					}
				}
			}
			if (!Utils::VisPosHitboxIdOut(pLocal, Target.m_pEntity, pLocal->GetShootPos(), Target.m_vPos, nHit))
				return false;

			if (nHit != HITBOX_HEAD && !ScanHead(pLocal, Target))
				return false;
		}

		//else if (Target.m_nAimedHitbox == HITBOX_PELVIS)
		//{
		//	if (Vars::Backtrack::Enabled.m_Var && Vars::Backtrack::Aim.m_Var && Vars::Aimbot::Hitscan::AimMethod.m_Var != 1) {
		//		
		//		Vec3 pelvisPos;
		//		if (!g_Backtrack.Record[Target.m_pEntity->GetIndex()].empty()) {
		//			auto lastTick = g_Backtrack.Record[Target.m_pEntity->GetIndex()].back();
		//			if (const auto& pHdr = lastTick.HDR) {
		//				if (const auto& pSet = pHdr->GetHitboxSet(lastTick.HitboxSet)) {
		//					if (const auto& pBox = pSet->hitbox(HITBOX_PELVIS)) {
		//						Vec3 vPos = (pBox->bbmin + pBox->bbmax) * 0.5f, vOut;
		//						Math::VectorTransform(vPos, reinterpret_cast<matrix3x4*>(&lastTick.BoneMatrix)[pBox->bone], vOut);
		//						pelvisPos = vOut;
		//					}
		//				}
		//			}


		//			if (Utils::VisPos(pLocal, Target.m_pEntity, pLocal->GetShootPos(), pelvisPos)) {
		//				Target.m_vAngleTo = Math::CalcAngle(pLocal->GetShootPos(), pelvisPos);
		//				return true;
		//			}


		//		}
		//	}
		//	if (!Utils::VisPos(pLocal, Target.m_pEntity, pLocal->GetShootPos(), Target.m_vPos) && !ScanHitboxes(
		//		pLocal, Target))
		//		return false;
		//}

		else {
			if (Vars::Backtrack::Enabled.m_Var && Vars::Backtrack::Aim.m_Var && Vars::Aimbot::Hitscan::AimMethod.m_Var != 1) {

				Vec3 hitboxPos;
				if (!g_Backtrack.Record[Target.m_pEntity->GetIndex()].empty()) {
					auto lastTick = g_Backtrack.Record[Target.m_pEntity->GetIndex()].back();
					if (const auto& pHdr = lastTick.HDR) {
						if (const auto& pSet = pHdr->GetHitboxSet(lastTick.HitboxSet)) {
							if (const auto& pBox = pSet->hitbox(Target.m_nAimedHitbox)) {
								Vec3 vPos = (pBox->bbmin + pBox->bbmax) * 0.5f, vOut;
								Math::VectorTransform(vPos, reinterpret_cast<matrix3x4*>(&lastTick.BoneMatrix)[pBox->bone], vOut);
								hitboxPos = vOut;
							}
						}
					}


					if (Utils::VisPos(pLocal, Target.m_pEntity, pLocal->GetShootPos(), hitboxPos)) {
						Target.m_vAngleTo = Math::CalcAngle(pLocal->GetShootPos(), hitboxPos);
						return true;
					}


				}
			}
			if (!Utils::VisPos(pLocal, Target.m_pEntity, pLocal->GetShootPos(), Target.m_vPos) && !ScanHitboxes(
				pLocal, Target))
				return false;
		}

		break;
	}

	case ETargetType::BUILDING:
	{
		if (!Utils::VisPos(pLocal, Target.m_pEntity, pLocal->GetShootPos(), Target.m_vPos))
		{
			//Sentryguns have hitboxes, it's better to use ScanHitboxes for them
			if (Target.m_pEntity->GetClassID() == ETFClassID::CObjectSentrygun
				? !ScanHitboxes(pLocal, Target)
				: !ScanBuildings(pLocal, Target))
				return false;
		}

		break;
	}

	default:
	{
		if (!Utils::VisPos(pLocal, Target.m_pEntity, pLocal->GetShootPos(), Target.m_vPos))
			return false;

		break;
	}
	}

	return true;
}

bool CAimbotHitscan::GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& Out)
{
	if (!GetTargets(pLocal, pWeapon))
		return false;

	g_AimbotGlobal.SortTargets(GetSortMethod());

	for (auto& Target : g_AimbotGlobal.m_vecTargets)
	{
		if (!VerifyTarget(pLocal, pWeapon, Target))
			continue;

		Out = Target;
		return true;
	}

	return false;
}

void CAimbotHitscan::Aim(CUserCmd* pCmd, Vec3& vAngle)
{
	vAngle -= g_GlobalInfo.m_vPunchAngles;
	Math::ClampAngles(vAngle);

	int nAimMethod = (Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var && g_GlobalInfo.m_bLocalSpectated)
		? 1
		: Vars::Aimbot::Hitscan::AimMethod.m_Var;

	switch (nAimMethod)
	{
	case 0: //Plain
	{
		pCmd->viewangles = vAngle;
		g_Interfaces.Engine->SetViewAngles(pCmd->viewangles);
		break;
	}

	case 1: //Smooth
	{
		if (Vars::Aimbot::Hitscan::SmoothingAmount.m_Var == 0) { // plain aim at 0 smoothing factor
			pCmd->viewangles = vAngle;
			g_Interfaces.Engine->SetViewAngles(pCmd->viewangles);
			break;
		}
		//Calculate delta of current viewangles and wanted angles
		Vec3 vecDelta = vAngle - g_Interfaces.Engine->GetViewAngles();

		//Clamp, keep the angle in possible bounds
		Math::ClampAngles(vecDelta);

		//Basic smooth by dividing the delta by wanted smooth amount
		pCmd->viewangles += vecDelta / Vars::Aimbot::Hitscan::SmoothingAmount.m_Var;

		//Set the viewangles from engine
		g_Interfaces.Engine->SetViewAngles(pCmd->viewangles);
		break;
	}

	case 2: //Silent
	{
		if (Vars::AntiHack::AntiAim::invalidshootpitch.m_Var && Vars::AntiHack::AntiAim::Active.m_Var && ((Vars::AntiHack::AntiAim::YawReal.m_Var && Vars::AntiHack::AntiAim::YawFake.m_Var) || Vars::AntiHack::AntiAim::Pitch.m_Var)) {
			g_GlobalInfo.m_bFakeShotPitch = true;

			if (vAngle.x > 0.f)
				vAngle.x = Math::RemapValClamped(vAngle.x, 0.0f, 89.0f, 180.0f, 91.0f);
			else
				vAngle.x = Math::RemapValClamped(vAngle.x, 0.0f, -89.0f, -180.0f, -91.0f);

			vAngle.y = Math::RemapValClamped(vAngle.y, 0.0f, 180.0f, -180.0f, 0.0f);
		}
		Utils::FixMovement(pCmd, vAngle);
		pCmd->viewangles = vAngle;
		break;
	}

	default: break;
	}
}

bool CAimbotHitscan::ShouldFire(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, const Target_t& Target)
{
	if (!Vars::Aimbot::Global::AutoShoot.m_Var)
		return false;

	switch (g_GlobalInfo.m_nCurItemDefIndex)
	{
	case Sniper_m_TheMachina:
	case Sniper_m_ShootingStar:
	{
		if (!pLocal->IsScoped())
			return false;

		break;
	}
	default: break;
	}

	switch (pLocal->GetClassNum())
	{
	case CLASS_SNIPER:
	{
		bool bIsScoped = pLocal->IsScoped();

		if (Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var)
		{
			if (g_GlobalInfo.m_nCurItemDefIndex != Sniper_m_TheClassic
				&& g_GlobalInfo.m_nCurItemDefIndex != Sniper_m_TheSydneySleeper
				&& !g_GlobalInfo.m_bWeaponCanHeadShot && bIsScoped)
				return false;
		}

		if (Vars::Aimbot::Hitscan::WaitForCharge.m_Var && bIsScoped)
		{
			int nHealth = Target.m_pEntity->GetHealth();
			bool bIsCritBoosted = pLocal->IsCritBoostedNoMini();

			if (Target.m_nAimedHitbox == HITBOX_HEAD && g_GlobalInfo.m_nCurItemDefIndex !=
				Sniper_m_TheSydneySleeper)
			{
				if (nHealth > 150)
				{
					float flDamage = Math::RemapValClamped(pWeapon->GetChargeDamage(), 0.0f, 150.0f, 0.0f, 450.0f);
					int nDamage = static_cast<int>(flDamage);

					if (nDamage < nHealth && nDamage != 450)
						return false;
				}

				else
				{
					if (!bIsCritBoosted && !g_GlobalInfo.m_bWeaponCanHeadShot)
						return false;
				}
			}

			else
			{
				if (nHealth > (bIsCritBoosted ? 150 : 50))
				{
					float flMult = Target.m_pEntity->IsInJarate() ? 1.36f : 1.0f;

					if (bIsCritBoosted)
						flMult = 3.0f;

					float flMax = 150.0f * flMult;
					int nDamage = static_cast<int>(pWeapon->GetChargeDamage() * flMult);

					if (nDamage < Target.m_pEntity->GetHealth() && nDamage != static_cast<int>(flMax))
						return false;
				}
			}
		}

		break;
	}

	case CLASS_SPY:
	{
		if (Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var && !pWeapon->AmbassadorCanHeadshot())
		{
			if (Target.m_nAimedHitbox == HITBOX_HEAD)
			{
				return false;
			}
		}

		break;
	}

	default: break;
	}

	int nAimMethod = (Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var && g_GlobalInfo.m_bLocalSpectated)
		? 1
		: Vars::Aimbot::Hitscan::AimMethod.m_Var;

	if (nAimMethod == 1)
	{
		Vec3 vForward = {};
		Math::AngleVectors(pCmd->viewangles, &vForward);
		Vec3 vTraceStart = pLocal->GetShootPos();
		Vec3 vTraceEnd = (vTraceStart + (vForward * 8192.0f));

		CGameTrace trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pLocal;

		Utils::Trace(vTraceStart, vTraceEnd, (MASK_SHOT /* | CONTENTS_GRATE | MASK_VISIBLE*/), &filter, &trace);

		if (trace.entity != Target.m_pEntity)
			return false;

		if (Target.m_nAimedHitbox == HITBOX_HEAD)
		{
			if (trace.hitbox != HITBOX_HEAD)
				return false;

			if (!Target.m_bHasMultiPointed)
			{
				Vec3 vMins = {}, vMaxs = {}, vCenter = {};
				matrix3x4 Matrix = {};

				if (!Target.m_pEntity->GetHitboxMinsAndMaxsAndMatrix(HITBOX_HEAD, vMins, vMaxs, Matrix, &vCenter))
					return false;

				vMins *= 0.5f;
				vMaxs *= 0.5f;

				if (!Math::RayToOBB(vTraceStart, vForward, vCenter, vMins, vMaxs, Matrix))
					return false;
			}
		}
	}

	return true;
}

bool CAimbotHitscan::IsAttacking(CUserCmd* pCmd, CBaseCombatWeapon* pWeapon)
{
	return ((pCmd->buttons & IN_ATTACK) && g_GlobalInfo.m_bWeaponCanAttack);
}

void bulletTracer(CBaseEntity* pLocal, Target_t Target)
{
	Vec3 vecPos = g_GlobalInfo.m_WeaponType == EWeaponType::PROJECTILE ? g_GlobalInfo.m_vPredictedPos : Target.m_vPos;
	//Color_t Color = (Utils::Rainbow());

	Vec3 shootPos = pLocal->GetShootPos();
	shootPos.z -= 5.0f;
	Color_t Color = Vars::Visuals::BulletTracerRainbow.m_Var ? Utils::Rainbow() : Colors::BulletTracer;
	g_Interfaces.DebugOverlay->AddLineOverlayAlpha(shootPos, vecPos, Color.r, Color.g, Color.b, Color.a, true, 5);
}




void CAimbotHitscan::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	static int nLastTracerTick = pCmd->tick_count;

	if (!Vars::Aimbot::Global::Active.m_Var)
		return;

	Target_t Target = {};

	const int nWeaponID = pWeapon->GetWeaponID();
	const bool bShouldAim = (Vars::Aimbot::Global::AimKey.m_Var == VK_LBUTTON
		? (pCmd->buttons & IN_ATTACK)
		: g_AimbotGlobal.IsKeyDown());

	//Rev minigun if enabled and aimbot active
	if (bShouldAim)
	{
		if (Vars::Aimbot::Hitscan::AutoRev.m_Var && nWeaponID == TF_WEAPON_MINIGUN)
			pCmd->buttons |= IN_ATTACK2;
	}

	if (GetTarget(pLocal, pWeapon, Target) && bShouldAim)
	{

		if (SandvichAimbot::bIsSandvich) {
			SandvichAimbot::RunSandvichAimbot(pLocal, pWeapon, pCmd, Target.m_pEntity);
		}

		if (nWeaponID != TF_WEAPON_COMPOUND_BOW
			&& pLocal->GetClassNum() == CLASS_SNIPER
			&& pWeapon->GetSlot() == SLOT_PRIMARY)
		{
			bool bScoped = pLocal->IsScoped();

			if (Vars::Aimbot::Hitscan::AutoScope.m_Var && !bScoped)
			{
				pCmd->buttons |= IN_ATTACK2;
				return;
			}

			if (Vars::Aimbot::Hitscan::ScopedOnly.m_Var && !bScoped)
				return;
		}

		if (Vars::Misc::CL_Move::WaitForDT.m_Var)
		{
			if (g_GlobalInfo.m_nWaitForShift && g_GlobalInfo.m_nShifted &&
				GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var))
				//if dt not ready and "ticks" = 0 and key is held, dont aimbot
				return;
		}

		g_GlobalInfo.m_nCurrentTargetIdx = Target.m_pEntity->GetIndex();
		g_GlobalInfo.m_bHitscanRunning = true;
		g_GlobalInfo.m_bHitscanSilentActive = Vars::Aimbot::Hitscan::AimMethod.m_Var == 2;

		if (Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var && g_GlobalInfo.m_bLocalSpectated)
			g_GlobalInfo.m_bHitscanSilentActive = false;

		if (g_GlobalInfo.m_bHitscanSilentActive)
			g_GlobalInfo.m_vAimPos = Target.m_vPos;

		Aim(pCmd, Target.m_vAngleTo);

		if (ShouldFire(pLocal, pWeapon, pCmd, Target))
		{
			if (nWeaponID == TF_WEAPON_MINIGUN)
				pCmd->buttons |= IN_ATTACK2;

			if (g_GlobalInfo.m_nCurItemDefIndex == Engi_s_TheWrangler || g_GlobalInfo.m_nCurItemDefIndex ==
				Engi_s_FestiveWrangler)
				pCmd->buttons |= IN_ATTACK2;

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

			if (Vars::Aimbot::Hitscan::TapFire.m_Var && nWeaponID == TF_WEAPON_MINIGUN)
			{
				bool bDo = Vars::Aimbot::Hitscan::TapFire.m_Var == 1
					? pLocal->GetAbsOrigin().DistTo(Target.m_pEntity->GetAbsOrigin()) > 1000.0f
					: true;

				if (bDo && pWeapon->GetWeaponSpread())
				{
					float flTimeSinceLastShot = (pLocal->GetTickBase() * TICK_INTERVAL) - pWeapon->GetLastFireTime();

					if (pWeapon->GetWeaponData().m_nBulletsPerShot > 1)
					{
						if (flTimeSinceLastShot <= 0.25f)
							pCmd->buttons &= ~IN_ATTACK;
					}

					else if (flTimeSinceLastShot <= 1.25f)
						pCmd->buttons &= ~IN_ATTACK;
				}
			}
		}

		bool bIsAttacking = IsAttacking(pCmd, pWeapon);

		if (bIsAttacking)
		{
			g_GlobalInfo.m_bAttacking = true;
			if (Vars::Visuals::BulletTracer.m_Var && abs(pCmd->tick_count - nLastTracerTick) > 1)
			{
				//bulletTracer(pLocal, Target);
				nLastTracerTick = pCmd->tick_count;
			}
		}

		if (Vars::Misc::DisableInterpolation.m_Var && Target.m_TargetType == ETargetType::PLAYER && bIsAttacking)
		{
			pCmd->tick_count = TIME_TO_TICKS(Target.m_pEntity->GetSimulationTime() +
				std::max(g_ConVars.cl_interp->GetFloat(), g_ConVars.cl_interp_ratio->GetFloat() / g_ConVars.
					cl_updaterate->GetFloat()));
		}
	}
}
