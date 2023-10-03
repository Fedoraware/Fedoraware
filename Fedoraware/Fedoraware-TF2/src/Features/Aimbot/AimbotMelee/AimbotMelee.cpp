#include "AimbotMelee.h"

void CAimbotMelee::Aim(CUserCmd* pCmd, Vec3& vAngle) {
	vAngle -= G::PunchAngles;
	Math::ClampAngles(vAngle);
	switch (Vars::Aimbot::Melee::AimMethod.Value)
	{
	case 1: {
		if (!Vars::Aimbot::Melee::SmoothingAmount.Value) { break; }
		Vec3 vDelta = vAngle - pCmd->viewangles;
		Math::ClampAngles(vDelta);
		pCmd->viewangles += vDelta / Vars::Aimbot::Melee::SmoothingAmount.Value;
		I::EngineClient->SetViewAngles(pCmd->viewangles);
		return;
	}
	case 2: {
		if (!G::IsAttacking) { return; }
		G::SilentTime = true;
		Utils::FixMovement(pCmd, vAngle);
		pCmd->viewangles = vAngle;
		return;
	}
	}

	pCmd->viewangles = vAngle;
	I::EngineClient->SetViewAngles(pCmd->viewangles);
	return;
}

bool CAimbotMelee::VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& target) {
	const bool bSapper = pWeapon->GetWeaponID() == TF_WEAPON_BUILDER;
	const Vec3 vShootPos = vCache + pLocal->GetViewOffset();
	//	not gonna movesim enemies this time cause we can just backtrack them.
	//	check our current tick first
	if (F::Backtrack.CanHitOriginal(target.m_pEntity)) {
		const Vec3 vTargetPoint = target.m_pEntity->GetHitboxPos(HITBOX_SPINE_1);
		target.m_vAngleTo = Math::CalcAngle(vShootPos, vTargetPoint);
		if (bSapper ? SapperTrace(pLocal, target.m_pEntity, pWeapon) : CanMeleeHit(pLocal, target.m_pEntity, pWeapon, target.m_vAngleTo)) {
			target.SimTime = target.m_pEntity->GetSimulationTime();	//	technically we will end up backtracking unless we are using doubletap to instant melee, but i see that as a good thing
			target.m_vPos = vTargetPoint;
			return true;
		}
	}
	//	check our backtrack ticks next.
	if (CanBacktrack(target.m_pEntity)) {
		if (std::deque<TickRecord>* pRecords = F::Backtrack.GetRecords(target.m_pEntity)) {
			const Vec3 vOriginalPos = target.m_pEntity->GetAbsOrigin();
			for (TickRecord& record : *pRecords) {
				if (!F::Backtrack.WithinRewind(record, flSwingTime)) { continue; }
				target.m_pEntity->SetAbsOrigin(record.vOrigin);
				const Vec3 vTargetPoint = target.m_pEntity->GetHitboxPosMatrix(HITBOX_SPINE_1, (matrix3x4*)(&record.BoneMatrix));
				target.m_vAngleTo = Math::CalcAngle(vShootPos, vTargetPoint);
				if (CanMeleeHit(pLocal, target.m_pEntity, pWeapon, target.m_vAngleTo)) {

					target.SimTime = record.flSimTime;
					target.ShouldBacktrack = true;
					target.m_vPos = vTargetPoint;
					target.m_pEntity->SetAbsOrigin(vOriginalPos);
					return true;
				}
			}
			target.m_pEntity->SetAbsOrigin(vOriginalPos);
		}
	}
	return false;
}

bool CAimbotMelee::FillCache(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon) {
	flSwingTime = std::max(pWeapon->GetWeaponData().m_flSmackDelay - ((F::Ticks.MeleeDoubletapCheck(pLocal) && (Vars::Misc::CL_Move::AntiWarp.Value && pLocal->OnSolid())) ? TICKS_TO_TIME(G::ShiftedTicks) : 0.f), 0.f);
	const int iTicks = TIME_TO_TICKS(flSwingTime);

	if (Vars::Aimbot::Melee::PredictSwing.Value && iTicks) {
		if (F::MoveSim.Initialize(pLocal)) {
			CMoveData moveData = {};
			Vec3 absOrigin = {};
			for (int i = 0; i < iTicks; i++) {
				F::MoveSim.RunTick(moveData, absOrigin);
			}
			vCache = absOrigin;
			F::MoveSim.Restore();
		}
		else { return false; }
	}
	else {
		vCache = pLocal->GetAbsOrigin();
	}
	return true;
}

inline bool CAimbotMelee::SapperTrace(CBaseEntity* pLocal, CBaseEntity* pObject, CBaseCombatWeapon* pWeapon) {
	const float flRange = 110.f;
	Vec3 vClosestPoint{};
	pObject->GetCollision()->CalcNearestPoint(pLocal->GetShootPos(), &vClosestPoint);
	return vClosestPoint.DistTo(pLocal->GetShootPos()) <= flRange;
}

inline bool CAimbotMelee::CanMeleeHit(CBaseEntity* pLocal, CBaseEntity* pEntity, CBaseCombatWeapon* pWeapon, const Vec3& vAngles) {
	const Vec3 vBackupPos = pLocal->GetAbsOrigin();
	const Vec3 vBackupAng  = I::EngineClient->GetViewAngles();


	//	swing pred doesn't seem to work? i genuinely can't tell.
	pLocal->SetVecOrigin(vCache);
	pLocal->SetAbsOrigin(vCache);
	pLocal->SetEyeAngles(vAngles);
	I::RenderView->SetMainView(vCache + pLocal->m_vecViewOffset(), vAngles);

	CGameTrace trace;
	const bool bSwingTrace = pWeapon->DoSwingTraceInternal(trace);

	pLocal->SetVecOrigin(vBackupPos);
	pLocal->SetAbsOrigin(vBackupPos);
	pLocal->SetEyeAngles(vBackupAng);
	I::RenderView->SetMainView(vBackupPos + pLocal->m_vecViewOffset(), vBackupAng);

	return bSwingTrace ? trace.entity == pEntity : false;
}

inline bool CAimbotMelee::IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon) {
	return pWeapon->GetWeaponID() == TF_WEAPON_KNIFE ? ((pCmd->buttons & IN_ATTACK) && G::WeaponCanAttack) : fabsf(pWeapon->GetSmackTime() - I::GlobalVars->curtime) < pWeapon->GetWeaponData().m_flSmackDelay;
}

inline bool CAimbotMelee::CanBacktrack(CBaseEntity* pEntity) {	//	truly despicable
	return Vars::Backtrack::Enabled.Value && pEntity->IsPlayer();
}

inline float CAimbotMelee::GetSwingVec(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon) {
	return 18.f * (Utils::ATTRIB_HOOK_FLOAT(1.f, "melee_bounds_multiplier", pWeapon, 0, true) * std::min(1.0f, pLocal->m_flModelScale()));
}

inline float CAimbotMelee::GetSwingRange(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon) {
	return Vars::Aimbot::Melee::RangeCheck.Value ? Utils::ATTRIB_HOOK_FLOAT(pWeapon->GetSwingRange(pLocal) * std::min(1.0f, pLocal->m_flModelScale()), "melee_range_multiplier", pWeapon, 0, true) : 4096.f;
}

inline EGroupType CAimbotMelee::GetGroupType(CBaseCombatWeapon* pWeapon)
{
	return static_cast<EGroupType>(Vars::Aimbot::Melee::WhipTeam.Value ? pWeapon->GetItemDefIndex() != Soldier_t_TheDisciplinaryAction : 1);
}

inline bool CAimbotMelee::IsPlayerValid(CBaseEntity* pTarget)
{
	//	return true if they are trans cause #transpplareVALID
	return pTarget->IsAlive() && !pTarget->IsAGhost();
}

inline bool CAimbotMelee::TargetTeamBuilding(CBaseObject* pBuilding, CBaseCombatWeapon* pWeapon) {
	if (pBuilding->GetSapped()) {
		return true;
	}

	//	Pyro Weps can't go any further
	if (pWeapon->GetWeaponID() != TF_WEAPON_WRENCH) {
		return false;
	}

	//	Upgrade/Heal Check
	if (pBuilding->GetLevel() != 3 || pBuilding->GetHealth() < pBuilding->GetMaxHealth()) {
		return true;
	}

	//	Ammo Check
	int iShells{};
	int iMaxShells{};
	int iRockets{};
	int iMaxRockets{};

	pBuilding->GetAmmoCount(iShells, iMaxShells, iRockets, iMaxRockets);
	if (iShells < iMaxShells || iRockets < iMaxRockets) {
		return true;
	}

	return false;
}

inline bool CAimbotMelee::TargetTeamBuildings(CBaseCombatWeapon* pWeapon) {
	const bool bWrench = (pWeapon->GetWeaponID() == TF_WEAPON_WRENCH);
	const bool bDamageSappers = Utils::ATTRIB_HOOK_FLOAT(0, "set_dmg_apply_to_sapper", pWeapon, 0, 1);
	return bDamageSappers || bWrench;
}

std::vector<Target_t> CAimbotMelee::GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	std::vector<Target_t> vTargets{};
	const ESortMethod sortMethod = static_cast<ESortMethod>(Vars::Aimbot::Melee::SortMethod.Value);
	const Vec3& vLocalPos = pLocal->GetShootPos();
	const Vec3& vLocalAngles = I::EngineClient->GetViewAngles();
	const bool bRespectFOV = (sortMethod == ESortMethod::FOV || Vars::Aimbot::Melee::RespectFOV.Value);
	const bool bSapper = pWeapon->GetWeaponID() == TF_WEAPON_BUILDER;

	//	Players
	if (Vars::Aimbot::Global::AimPlayers.Value && !bSapper) {
		for (CBaseEntity* pTarget : g_EntityCache.GetGroup(GetGroupType(pWeapon))) {
			if (!IsPlayerValid(pTarget)) { continue; }
			if (F::AimbotGlobal.ShouldIgnore(pTarget, true)) { continue; }

			const Vec3 vPos = pTarget->GetHitboxPos(HITBOX_PELVIS);
			const Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);

			if (bRespectFOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value) {
				continue;
			}

			const Priority& priority = F::AimbotGlobal.GetPriority(pTarget->GetIndex());
			const float flDistTo = vLocalPos.DistTo(vPos);
			vTargets.push_back({ pTarget, ETargetType::PLAYER, vPos, vAngleTo, flFOVTo, flDistTo, -1, false, priority });
		}
	}
	if (Vars::Aimbot::Global::AimBuildings.Value) {
		for (CBaseEntity* pObject : g_EntityCache.GetGroup(TargetTeamBuildings(pWeapon) ? EGroupType::BUILDINGS_ALL : EGroupType::BUILDINGS_ENEMIES))
		{
			CBaseObject* pBuilding = reinterpret_cast<CBaseObject*>(pObject);

			if (!pObject || !pBuilding || !pObject->IsAlive()) {
				continue;
			}

			if (pBuilding->GetTeamNum() == pLocal->GetTeamNum()) {
				if (!TargetTeamBuilding(pBuilding, pWeapon)) {
					continue;
				}
			}

			if (bSapper && pBuilding->GetSapped()) {	//	dont sap a sapped building u sapaholic
				continue;
			}

			const Vec3 vPos = pObject->GetWorldSpaceCenter();
			const Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);

			if (bRespectFOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value) {
				continue;
			}

			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;
			vTargets.push_back({ pObject, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}
	if (Vars::Aimbot::Global::AimNPC.Value && !bSapper)
	{
		for (const auto& pNPC : g_EntityCache.GetGroup(EGroupType::WORLD_NPC))
		{
			const Vec3 vPos = pNPC->GetWorldSpaceCenter();
			const Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);

			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);
			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			if (bRespectFOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value) {
				continue;
			}

			vTargets.push_back({ pNPC, ETargetType::NPC, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	return vTargets;
}

bool CAimbotMelee::GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon) {
	std::vector<Target_t> vTargets = GetTargets(pLocal, pWeapon);
	if (vTargets.empty()) { return false; }

	const ESortMethod sortMethod = static_cast<ESortMethod>(Vars::Aimbot::Melee::SortMethod.Value);
	F::AimbotGlobal.SortTargets(&vTargets, sortMethod);

	for (Target_t& target : vTargets) {
		if (VerifyTarget(pLocal, pWeapon, target)) {
			tTarget = target;
			return true;
		}
	}

	return false;
}

inline bool CAimbotMelee::ShouldAttack() {
	return Vars::Aimbot::Global::AutoShoot.Value;
}

void CAimbotMelee::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd) {
	G::CurrentTargetIdx = 0;

	const bool bShouldAim = (Vars::Aimbot::Melee::RequireBind.Value ? F::AimbotGlobal.IsKeyDown() : true);
	if (!Vars::Aimbot::Global::Active.Value || G::AutoBackstabRunning || pWeapon->GetWeaponID() == TF_WEAPON_KNIFE || !bShouldAim) {
		return;
	}
	FillCache(pLocal, pWeapon);

	if (GetTarget(pLocal, pWeapon)) {
		if (ShouldAttack()) {
			pCmd->buttons |= IN_ATTACK;
		}

		G::CurrentTargetIdx = tTarget.m_pEntity->GetIndex();

		if (Vars::Aimbot::Melee::AimMethod.Value == 2) {
			G::AimPos = tTarget.m_vPos;
		}

		G::IsAttacking = G::IsAttacking || IsAttacking(pCmd, pWeapon);

		if (G::IsAttacking && tTarget.m_TargetType == ETargetType::PLAYER) {
			pCmd->tick_count = TIME_TO_TICKS(tTarget.SimTime + G::LerpTime);
		}

		Aim(pCmd, tTarget.m_vAngleTo);
	}
}
