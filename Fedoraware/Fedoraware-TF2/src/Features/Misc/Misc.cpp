#include "Misc.h"

#include "../Vars.h"
#include "../ChatInfo/ChatInfo.h"
#include "../../Utils/Timer/Timer.hpp"
#include "../Aimbot/AimbotGlobal/AimbotGlobal.h"
#include "../Backtrack/Backtrack.h"
#include "../AntiHack/CheaterDetection/CheaterDetection.h"

extern int attackStringW;
extern int attackStringH;

void CMisc::RunPre(CUserCmd* pCmd, bool* pSendPacket)
{
	bMovementStopped = false; bMovementScuffed = false;
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		FastStop(pCmd, pLocal);
		StopMovement(pCmd, pSendPacket);
		FastDeltaMove(pCmd, pSendPacket, pLocal);
		//PrintProjAngles(pLocal);
		AccurateMovement(pCmd, pLocal);
		FastAccel(pCmd, pLocal);
		AutoJump(pCmd, pLocal);
		AutoStrafe(pCmd, pLocal);
		NoiseMakerSpam(pLocal);
		ExtendFreeze(pLocal);
		Freecam(pCmd, pLocal);
		RageRetry(pLocal);
		AntiBackstab(pLocal, pCmd);
		ViewmodelFlip(pCmd, pLocal);
		AutoPeek(pCmd, pLocal);
		StickySpam(pLocal, pCmd);
		Glutton(pLocal, pCmd);
	}

	AntiAFK(pCmd);
	ChatSpam();
	CheatsBypass();
	PingReducer();
	WeaponSway();
	DetectChoke();
}

void CMisc::RunMid(CUserCmd* pCmd, const int nOldGroundEnt)
{
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		EdgeJump(pLocal, pCmd, nOldGroundEnt);
		DuckJump(pLocal, pCmd);
	}
}

void CMisc::RunPost(CUserCmd* pCmd, bool* pSendPacket)
{
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		DoubletapPacket(pSendPacket);
		LegJitter(pCmd, pLocal);
		AutoRocketJump(pCmd, pLocal);
		AutoScoutJump(pCmd, pLocal);
	}
}

void CMisc::StopMovement(CUserCmd* pCmd, bool* pSendPacket)
{
	if (!G::ShouldStop || F::AimbotGlobal.IsAttacking()) { return; }

	if (!Utils::StopMovement(pCmd))
	{
		return;
	}

	G::UpdateView = false;
	bMovementStopped = true;
	bMovementScuffed = true;
}

void CMisc::FastDeltaMove(CUserCmd* pCmd, bool* pSendPacket, CBaseEntity* pLocal) {
	if (pLocal->IsInBumperKart() ? ~(Vars::Misc::AltMovement.Value & 1 << 0) : ~(Vars::Misc::AltMovement.Value & 1 << 3)) { return; }

	bool bChanged = false;

	static bool bFwd = pCmd->forwardmove > 0;
	static bool bSde = pCmd->sidemove > 0;
	const bool bCurFwd = pCmd->forwardmove > 0;
	const bool bCurSde = pCmd->sidemove > 0;

	if (fabsf(pCmd->sidemove) > 400) {
		if (bSde != bCurSde) {
			pCmd->viewangles.y += bSde ? -90.f : 90.f;	//	face left or right depending
			pCmd->viewangles.x = 90.f;	//	look down
			pCmd->sidemove = bSde ? -pCmd->forwardmove : pCmd->forwardmove;	//	set our forward move to our sidemove
			bChanged = true;
		}

		if (bChanged) {
			bMovementScuffed = true;
			G::UpdateView = false;
			*pSendPacket = false;
		}

		// "why is dis one in anoda place doe" because if you're moving forward and you stop pressing the button foe 1 tick it no work :D
		bSde = bCurSde;

		if (bChanged) { return; }
	}
	if (fabsf(pCmd->forwardmove) > 400) {
		if (bFwd != bCurFwd) {
			pCmd->viewangles.x = 90.f;	//	look down
			pCmd->viewangles.y += bFwd ? 0.f : 180.f;
			pCmd->sidemove *= bFwd ? 1 : -1;
			bChanged = true;
		}

		if (bChanged) {
			bMovementScuffed = true;
			G::UpdateView = false;
			*pSendPacket = false;
		}

		// "why dont u weset it outside of dis doe" because if the user stop press buton foe 1 tick it no work :D
		bFwd = bCurFwd;

		if (bChanged) { return; }
	}
}

void CMisc::DoubletapPacket(bool* pSendPacket)
{
	*pSendPacket = (G::ShouldShift || G::Teleporting) ? G::ShiftedTicks == 1 : *pSendPacket;
}

void CMisc::AntiAFK(CUserCmd* pCmd)
{
	if (Vars::Misc::AntiAFK.Value && g_ConVars.afkTimer->GetInt() != 0)
	{
		if (pCmd->command_number % 2)
		{
			pCmd->buttons |= 1 << 27;
		}
	}
}

//	pasted but looks cool
void CMisc::WeaponSway()
{
	static ConVar* cl_wpn_sway_interp = g_ConVars.FindVar("cl_wpn_sway_interp");
	static ConVar* cl_wpn_sway_scale = g_ConVars.FindVar("cl_wpn_sway_scale");
	if (cl_wpn_sway_interp || cl_wpn_sway_scale)
	{
	{
		if (Vars::Visuals::ViewmodelSway.Value)
		{
			cl_wpn_sway_interp->SetValue(Vars::Visuals::ViewmodelSwayInterp.Value);
			cl_wpn_sway_scale->SetValue(Vars::Visuals::ViewmodelSwayScale.Value);
		}
		else
		{
				cl_wpn_sway_interp->SetValue(0.0f);
				cl_wpn_sway_scale->SetValue(0.0f);
		}
		}
	}
}

void CMisc::PrintProjAngles(CBaseEntity* pLocal)
{
	if (!Vars::Debug::DebugInfo.Value) { return; }
	if (!pLocal->IsAlive() || pLocal->IsAGhost()) { return; }
	static float flNextPrint = 0.f; if (flNextPrint > I::GlobalVars->curtime) { return; }
	const Vec3 vLocalEyeAngles = pLocal->GetEyeAngles();
	const Vec3 vLocalEyePosition = pLocal->GetEyePosition();
	for (CBaseEntity* pEntity : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
	{
		if (I::ClientEntityList->GetClientEntityFromHandle(pEntity->GethOwner()) != pLocal) { continue; }
		const Vec3 vProjAngles = pEntity->GetAbsAngles();
		const Vec3 vProjPosition = pEntity->GetAbsOrigin();

		const Vec3 vDeltaAng = vLocalEyeAngles - vProjAngles;
		const Vec3 vDeltaPos = vLocalEyePosition - vProjPosition;
		Utils::ConLog("ProjDebug", std::format("dAngles [{:.1f}, {:.1f}, {:.1f}] : dPosition [{:.1f}, {:.1f}, {:.1f}]", vDeltaAng.x, vDeltaAng.y, vDeltaAng.z, vDeltaPos.x, vDeltaPos.y, vDeltaPos.z).c_str(), { 255, 180, 0, 255 }, Vars::Debug::Logging.Value);
		flNextPrint = I::GlobalVars->curtime + 1.f;
	}
}

void CMisc::DetectChoke()
{
	static int iOldTick = I::GlobalVars->tickcount;
	if (I::GlobalVars->tickcount == iOldTick) { return; }
	iOldTick = I::GlobalVars->tickcount;
	for (const auto& pEntity : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
	{
		if (!pEntity->IsAlive() || pEntity->GetDormant())
		{
			G::ChokeMap[pEntity->GetIndex()] = 0;
			continue;
		}

		if (pEntity->GetSimulationTime() == pEntity->GetOldSimulationTime())
		{
			G::ChokeMap[pEntity->GetIndex()]++;
		}
		else
		{
			F::BadActors.ReportTickCount({ pEntity, G::ChokeMap[pEntity->GetIndex()] });
			G::ChokeMap[pEntity->GetIndex()] = 0;
		}
	}
}

//	dumb feature made out of spite for fourteen
void CMisc::LegJitter(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!pLocal->OnSolid() || pLocal->IsInBumperKart() || pLocal->IsAGhost() || !pLocal->IsAlive()) { return; }
	static bool pos = true;
	const float scale = pLocal->IsDucking() ? 14.f : 1.0f;
	if (F::AimbotGlobal.IsAttacking() || G::ShouldShift) { return; }
	if (F::AntiAim.ShouldAntiAim(pLocal)) { if (!F::AntiAim.bSendingReal) { return; } }
	else if (!Vars::AntiHack::AntiAim::LegJitter.Value) { return; }
	if (pCmd->forwardmove == 0.f && pCmd->sidemove == 0.f && pLocal->GetVecVelocity().Length2D() < 10.f)	//	force leg jitter if we are sending our real.
	{
		pos ? pCmd->forwardmove = scale : pCmd->forwardmove = -scale;
		pos ? pCmd->sidemove = scale : pCmd->sidemove = -scale;
		pos = !pos;
	}
}

void CMisc::AntiBackstab(CBaseEntity* pLocal, CUserCmd* pCmd)
{
	G::AvoidingBackstab = false;
	Vec3 vTargetPos;

	if (!pLocal->IsAlive() || pLocal->IsStunned() || pLocal->IsInBumperKart() || pLocal->IsAGhost() || !Vars::AntiHack::AntiAim::AntiBackstab.Value) {
		return;
	}

	const Vec3 vLocalPos = pLocal->GetWorldSpaceCenter();
	CBaseEntity* target = nullptr;

	for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!pEnemy || !pEnemy->IsAlive() || pEnemy->GetClassNum() != CLASS_SPY || pEnemy->IsCloaked() || pEnemy->IsAGhost() || pEnemy->GetFeignDeathReady())
		{
			continue;
		}

		if (CBaseCombatWeapon* pWeapon = pEnemy->GetActiveWeapon())
		{
			if (pWeapon->GetWeaponID() != TF_WEAPON_KNIFE) { continue; }
		}

		PlayerInfo_t pInfo{};
		if (!I::EngineClient->GetPlayerInfo(pEnemy->GetIndex(), &pInfo))
		{
			if (G::IsIgnored(pInfo.friendsID)) { continue; }
		}

		const Vec3 vEnemyPos = pEnemy->GetShootPos();
		if (!Utils::VisPos(pLocal, pEnemy, vLocalPos, vEnemyPos)) { continue; }
		if (!target && vLocalPos.DistTo(vEnemyPos) < 150.f)
		{
			target = pEnemy;
			vTargetPos = target->GetShootPos();
		}
		else if (vLocalPos.DistTo(vEnemyPos) < vLocalPos.DistTo(vTargetPos) && vLocalPos.DistTo(vEnemyPos) < 150.f)
		{
			target = pEnemy;
			vTargetPos = target->GetShootPos();
		}
	}

	if (target)
	{
		vTargetPos = target->GetShootPos();
		const Vec3 vAngleToSpy = Math::CalcAngle(vLocalPos, vTargetPos);
		G::AvoidingBackstab = true;
		Utils::FixMovement(pCmd, vAngleToSpy);
		pCmd->viewangles = vAngleToSpy;
	}
}

/*
void CMisc::InstantRespawnMVM() {
	if (I::Engine->IsInGame() && I::Engine->GetLocalPlayer() && !g_EntityCache.GetLocal()->IsAlive() && Vars::Misc::MVMRes.m_Var) {
		auto kv = new KeyValues("MVM_Revive_Response");
		kv->SetInt("accepted", 1);
		I::Engine->ServerCmdKeyValues(kv);
	}
}*/

void CMisc::CheatsBypass()
{
	static bool cheatset = false;
	if (ConVar* sv_cheats = g_ConVars.FindVar("sv_cheats"))
	{
		if (Vars::Misc::CheatsBypass.Value && sv_cheats)
		{
			sv_cheats->SetValue(1);
			cheatset = true;
		}
		else
		{
			if (cheatset)
			{
				sv_cheats->SetValue(0);
				cheatset = false;
			}
		}
	}
}

void CMisc::PingReducer()
{
	const ConVar* cl_cmdrate = g_ConVars.FindVar("cl_cmdrate");
	CNetChannel* netChannel = I::EngineClient->GetNetChannelInfo();
	if (cl_cmdrate == nullptr || netChannel == nullptr) { return; }

	static Timer updateRateTimer{};
	if (updateRateTimer.Run(500))
	{
		if (Vars::Misc::PingReducer.Value)
		{
			if (!g_EntityCache.GetPR()) return;
			const int currentPing = g_EntityCache.GetPR()->GetPing(I::EngineClient->GetLocalPlayer());
			NET_SetConVar cmd("cl_cmdrate", (Vars::Misc::PingTarget.Value <= currentPing) ? "-1" : std::to_string(cl_cmdrate->GetInt()).c_str());
			netChannel->SendNetMsg(cmd);
		}
		else
		{
			NET_SetConVar cmd("cl_cmdrate", std::to_string(cl_cmdrate->GetInt()).c_str());
			netChannel->SendNetMsg(cmd);
		}
	}
}

void CMisc::ExtendFreeze(CBaseEntity* pLocal)
{
	if (Vars::Misc::ExtendFreeze.Value && I::EngineClient->IsInGame() && !pLocal->IsAlive())
	{
		static Timer cmdTimer{};
		if (cmdTimer.Run(2000))
		{
			I::EngineClient->ClientCmd_Unrestricted("extendfreeze");
		}
	}
}

void CMisc::Freecam(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static KeyHelper fcKey{ &Vars::Visuals::FreecamKey.Value };
	if (fcKey.Down())
	{
		if (G::FreecamActive == false)
		{
			G::FreecamPos = pLocal->GetVecOrigin();
			G::FreecamActive = true;
		}

		const Vec3 viewAngles = I::EngineClient->GetViewAngles();
		const float zMove = sinf(DEG2RAD(viewAngles.x));
		Vec3 vForward, vRight, vUp;
		Math::AngleVectors(viewAngles, &vForward, &vRight, &vUp);
		Vec3 moveVector;

		if (pCmd->buttons & IN_FORWARD)
		{
			moveVector += vForward;
			moveVector.z -= zMove;
		}

		if (pCmd->buttons & IN_BACK)
		{
			moveVector -= vForward;
			moveVector.z += zMove;
		}

		if (pCmd->buttons & IN_MOVELEFT)
		{
			moveVector -= vRight;
		}

		if (pCmd->buttons & IN_MOVERIGHT)
		{
			moveVector += vRight;
		}

		Math::VectorNormalize(moveVector);
		moveVector *= Vars::Visuals::FreecamSpeed.Value;
		G::FreecamPos += moveVector;

		pCmd->buttons = 0;
		pCmd->forwardmove = 0.f;
		pCmd->sidemove = 0.f;
		pCmd->upmove = 0.f;
	}
	else
	{
		G::FreecamActive = false;
	}
}

void CMisc::StickySpam(CBaseEntity* pLocal, CUserCmd* pCmd) {
	static KeyHelper kSpam{ &Vars::Misc::StickySpamKey.Value };
	if (!G::WeaponCanAttack || !kSpam.Down() || !pLocal->IsAlive()) { return; }

	CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon();
	const int iAmmo = pWeapon->GetClip1();

	if (iAmmo == 0) { return; }
	if (pWeapon->GetWeaponID() != TF_WEAPON_PIPEBOMBLAUNCHER) { return; }
	
	const float flChargingTime = (I::GlobalVars->curtime - pWeapon->GetChargeBeginTime());
	const float flCharge = Math::RemapValClamped(flChargingTime, 0.f, 4.f, 0, 100);

	//Utils::ConLog("CMisc::StickySpam", tfm::format("flCharge			:	%.0f", flCharge).c_str(), { 132, 255, 201, 255 });
	//Utils::ConLog("CMisc::StickySpam", tfm::format("flChargeT		:	%.1f", flChargingTime).c_str(), { 132, 255, 201, 255 });

	static bool bHasStarted = false;
	static bool bFlip = false;

	if (flCharge >= Vars::Misc::StickyChargePercent.Value && bHasStarted) { bFlip = false; }
	else { bFlip = true; }

	if (bFlip) { pCmd->buttons |= IN_ATTACK; bHasStarted = true; }
	else { pCmd->buttons &= ~IN_ATTACK; bHasStarted = false; }

	//Utils::ConLog("CMisc::StickySpam", tfm::format("bFlip				:	%d", bFlip).c_str(), { 132, 255, 201, 255 });
	//Utils::ConLog("CMisc::StickySpam", tfm::format("bHasStarted		:	%d\n", bFlip).c_str(), { 132, 255, 201, 255 });
}

void CMisc::Glutton(CBaseEntity* pLocal, CUserCmd* pCmd) {
	static KeyHelper kGlutton{ &Vars::Misc::InfiniteEatKey.Value };
	if (!pLocal->IsAlive() || !kGlutton.Down()) { return; }

	CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon();
	const int iWeaponID = pWeapon->GetWeaponID();
	if (iWeaponID != TF_WEAPON_LUNCHBOX) { return; }

	pCmd->buttons |= IN_ATTACK;

	static float flLastSendTime = I::GlobalVars->curtime;		//	dont get disconnected
	if (fabsf(I::GlobalVars->curtime - flLastSendTime) > .5f) {
		I::EngineClient->ClientCmd_Unrestricted("taunt");
		flLastSendTime = I::GlobalVars->curtime;
	}
}

void CMisc::RageRetry(CBaseEntity* pLocal)
{
	if (Vars::Misc::RageRetry.Value)
	{
		if (pLocal->IsAlive() && pLocal->GetHealth() <= (pLocal->GetMaxHealth() * (Vars::Misc::RageRetryHealth.Value * 0.01f)))
		{
			I::EngineClient->ClientCmd_Unrestricted("retry");
		}
	}
}

void CMisc::EdgeJump(CBaseEntity* pLocal, CUserCmd* pCmd, const int nOldGroundEnt)
{
	if (nOldGroundEnt < 0 || !Vars::Misc::EdgeJump.Value) { return; }
	static KeyHelper kEdge{ &Vars::Misc::EdgeJumpKey.Value };
	if (Vars::Misc::EdgeJumpKey.Value && !kEdge.Down()) { return; }
	if (!pLocal->IsAlive() || pLocal->OnSolid() || pLocal->IsSwimming()) { return; }
	pCmd->buttons |= IN_JUMP;
}

void CMisc::DuckJump(CBaseEntity* pLocal, CUserCmd* pCmd) {
	if (!Vars::Misc::DuckJump.Value && !Vars::Misc::Followbot::Enabled.Value) { return; }
	if (!pLocal->IsAlive() || pLocal->OnSolid() || pLocal->IsSwimming() || pLocal->IsStunned()) { return; }
	
	pCmd->buttons |= IN_DUCK;
}

void CMisc::FastAccel(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	bFastAccel = false;

	if ((G::AAActive || bMovementScuffed || bMovementStopped))
	{
		return;
	}

	const bool bShouldAccel = !G::ShouldShift && pLocal->IsInBumperKart() ? (Vars::Misc::AltMovement.Value & 1 << 0) : pLocal->IsDucking() ? (Vars::Misc::AltMovement.Value & 1 << 1) : (Vars::Misc::AltMovement.Value & 1 << 2);
	if (!bShouldAccel)
	{
		return;
	}

	if (G::Recharging || G::RechargeQueued || G::Frozen)
	{
		return;
	}

	if (!pLocal->IsAlive() || pLocal->IsSwimming() || pLocal->IsAGhost() || !pLocal->OnSolid() || F::AimbotGlobal.IsAttacking())
	{
		return;
	}

	if (pLocal->IsCharging())
	{	//	demoman charge
		return;
	}

	if (pLocal->GetMoveType() == MOVETYPE_NOCLIP
		|| pLocal->GetMoveType() == MOVETYPE_LADDER
		|| pLocal->GetMoveType() == MOVETYPE_OBSERVER)
	{
		return;
	}

	const int maxSpeed = std::min(pLocal->GetMaxSpeed() * (pCmd->forwardmove < 0 ? .9f : 1.f) - 1, 510.f); //	get our max speed, then if we are going backwards, reduce it.
	const float curSpeed = pLocal->GetVecVelocity().Length2D();

	if (curSpeed > maxSpeed)
	{
		return;	//	no need to accelerate if we are moving at our max speed
	}

	if (pLocal->GetClassNum() == ETFClass::CLASS_HEAVY && pCmd->buttons & IN_ATTACK2 && pLocal->IsDucking())
	{
		return;
	}

	if (pCmd->buttons & (IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK))
	{
		const Vec3 vecMove(pCmd->forwardmove, pCmd->sidemove, 0.0f);
		const float flLength = vecMove.Length();
		Vec3 angMoveReverse;
		Math::VectorAngles(vecMove * -1.f, angMoveReverse);
		pCmd->forwardmove = -flLength;
		pCmd->sidemove = 0.0f;
		pCmd->viewangles.y = fmodf(pCmd->viewangles.y - angMoveReverse.y, 360.0f);	//	this doesn't have to be clamped inbetween 180 and -180 because the engine automatically fixes it.
		pCmd->viewangles.z = 270.f;
		G::UpdateView = false; bFastAccel = true;
	}
}

void CMisc::AccurateMovement(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!Vars::Misc::AccurateMovement.Value || (bMovementScuffed || bMovementStopped))
	{
		return;
	}

	if (!pLocal->IsAlive()
		|| pLocal->IsSwimming()
		|| (pLocal->IsInBumperKart() && !(Vars::Misc::AltMovement.Value & 1 << 0))
		|| pLocal->IsAGhost()
		|| pLocal->IsCharging()
		|| !pLocal->OnSolid())
	{
		return;
	}

	if (pLocal->GetMoveType() == MOVETYPE_NOCLIP
		|| pLocal->GetMoveType() == MOVETYPE_LADDER
		|| pLocal->GetMoveType() == MOVETYPE_OBSERVER)
	{
		return;
	}

	if (pCmd->buttons & (IN_JUMP | IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK))
	{
		return;
	}

	const float speed = pLocal->GetVecVelocity().Length2D();
	constexpr float speedLimit = 10.f;

	const int iStopMode = pLocal->IsInBumperKart() ? 2 : (Vars::Misc::AccurateMovement.Value == 3) ? (G::ShiftedTicks ? 1 : 2) : Vars::Misc::AccurateMovement.Value;

	if (speed > speedLimit)
	{
		switch (iStopMode)
		{
			case 1:
			{
				Vec3 direction = pLocal->GetVecVelocity().toAngle();
				direction.y = pCmd->viewangles.y - direction.y;
				const Vec3 negatedDirection = direction.fromAngle() * -speed;
				pCmd->forwardmove = negatedDirection.x;
				pCmd->sidemove = negatedDirection.y;
				break;
			}
			case 2:
			{
				G::ShouldStop = true;
				break;
			}
		}
	}
	else
	{
		pCmd->forwardmove = 0.0f;
		pCmd->sidemove = 0.0f;
	}
}

void CMisc::AutoJump(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!Vars::Misc::AutoJump.Value
		|| !pLocal->IsAlive()
		|| pLocal->IsSwimming()
		|| pLocal->IsInBumperKart()
		|| pLocal->IsAGhost())
	{
		return;
	}

	if (pLocal->GetMoveType() == MOVETYPE_NOCLIP
		|| pLocal->GetMoveType() == MOVETYPE_LADDER
		|| pLocal->GetMoveType() == MOVETYPE_OBSERVER)
	{
		return;
	}

	const bool bJumpHeld = pCmd->buttons & IN_JUMP;
	const bool bCurHop = bJumpHeld && pLocal->OnSolid();
	static bool bHopping = bCurHop;
	static bool bTried = false;

	if (bCurHop && !bTried)
	{	//	this is our initial jump
		bTried = true;
		bHopping = true; return;
	}
	else if (bCurHop && bTried) {
		//	we tried and failed to bunnyhop, let go of the key and try again the next tick
		bTried = false;
		pCmd->buttons &= ~IN_JUMP; return;
	}
	else if (bHopping && bJumpHeld && (!pLocal->OnSolid() || pLocal->IsDucking()))
	{	//	 we are not on the ground and the key is in the same hold cycle
		bTried = false;
		pCmd->buttons &= ~IN_JUMP; return;
	}
	else if (bHopping && !bJumpHeld)
	{	//	we are no longer in the jump key cycle
		bTried = false;
		bHopping = false; return;
	}
	else if (!bHopping && bJumpHeld)
	{	//	we exited the cycle but now we want back in, don't mess with keys for doublejump, enter us back into the cycle for next tick
		bTried = false;
		bHopping = true; return;
	}

	return;
}

void CMisc::AutoStrafe(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!Vars::Misc::AutoStrafe.Value)
	{
		return;
	}

	if (!pLocal->IsAlive()
		|| pLocal->IsSwimming()
		|| pLocal->IsInBumperKart()
		|| pLocal->IsAGhost()
		|| pLocal->OnSolid())
	{
		return;
	}

	if (pLocal->GetMoveType() == MOVETYPE_NOCLIP
		|| pLocal->GetMoveType() == MOVETYPE_LADDER
		|| pLocal->GetMoveType() == MOVETYPE_OBSERVER)
	{
		return;
	}

	static auto cl_sidespeed = g_ConVars.FindVar("cl_sidespeed");
	if (!cl_sidespeed || !cl_sidespeed->GetFloat())
	{
		return;
	}

	static bool wasJumping = false;
	const bool isJumping = pCmd->buttons & IN_JUMP;

	switch (Vars::Misc::AutoStrafe.Value)
	{
		default:
		{
			break;
		}
		case 1:
		{
			if (pCmd->mousedx && (!isJumping || wasJumping))
			{
				pCmd->sidemove = pCmd->mousedx > 0 ? cl_sidespeed->GetFloat() : -cl_sidespeed->GetFloat();
			}
			wasJumping = isJumping;
			break;
		}
		case 2:
		{
			if (Vars::Misc::DirectionalOnlyOnMove.Value)
			{
				if (!(pCmd->buttons & (IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK)))
				{
					break;
				}
			}

			const float speed = pLocal->GetVelocity().Length2D();

			if (speed < 2.0f)
			{
				break;
			}

			const auto vel = pLocal->GetVelocity();

			constexpr auto perfectDelta = [](float speed, CBaseEntity* pLocal) noexcept
			{
				const auto speedVar = pLocal->TeamFortress_CalculateMaxSpeed();
				static auto airVar = g_ConVars.FindVar("sv_airaccelerate");
				static auto wishSpeed = 90.0f;

				const auto term = wishSpeed / airVar->GetFloat() / speedVar * 100.f / speed;

				if (term < 1.0f && term > -1.0f)
				{
					return acosf(term);
				}
				return 0.0f;
			};

			const float pDelta = perfectDelta(speed, pLocal);
			if ((!isJumping || wasJumping) && pDelta)
			{
				const float yaw = DEG2RAD(pCmd->viewangles.y);
				const float velDir = atan2f(vel.y, vel.x) - yaw;
				const float wishAng = atan2f(-pCmd->sidemove, pCmd->forwardmove);
				const float delta = Math::AngleDiffRad(velDir, wishAng);

				const float moveDir = delta < 0.0f ? velDir + pDelta : velDir - pDelta;

				pCmd->forwardmove = cosf(moveDir) * cl_sidespeed->GetFloat();
				pCmd->sidemove = -sinf(moveDir) * cl_sidespeed->GetFloat();
			}
			wasJumping = isJumping;
			break;
		}
	}
}

void CMisc::NoiseMakerSpam(CBaseEntity* pLocal)
{
	if (!Vars::Misc::NoisemakerSpam.Value || pLocal->GetUsingActionSlot()) { return; }

	if (pLocal->GetNextNoiseMakerTime() < I::GlobalVars->curtime)
	{
		I::EngineClient->ServerCmdKeyValues(new KeyValues("use_action_slot_item_server"));
	}
}

const std::string SPAM_FED[] = {
	"Fedoraware - github.com/Fedoraware",
	"Fedoraware - Best free and open-source cheat!",
	"Fedoraware - One tip ahead of the game!",
	"Fedoraware - Now available @ https://github.com/Fedoraware!",
	"Fedoraware - Based on SEOwned public source!"
};

const std::string SPAM_LBOX[] = {
	"GET GOOD, GET LMAOBOX!",
	"LMAOBOX - WAY TO THE TOP",
	"WWW.LMAOBOX.NET - BEST FREE TF2 HACK!"
};

const std::string SPAM_CH[] = {
	"Cathook - more fun than a ball of yarn!",
	"GNU/Linux is the best OS!",
	"Visit https://cathook.club for more information!",
	"Cathook - Free and Open-Source tf2 cheat!",
	"Cathook - ca(n)t stop me meow!"
};

/* Chat & Voicechat Spam */
void CMisc::ChatSpam()
{
	const float flCurTime = I::EngineClient->Time();
	static float flNextSend = 0.0f;

	if (flCurTime > flNextSend)
	{
		// Chat Spam
		if (Vars::Misc::ChatSpam.Value != 0)
		{
			std::string spamMsg;

			switch (Vars::Misc::ChatSpam.Value)
			{
				case 2: spamMsg = SPAM_LBOX[Utils::RandIntSimple(0, ARRAYSIZE(SPAM_LBOX) - 1)];
					break;
				case 3: spamMsg = SPAM_CH[Utils::RandIntSimple(0, ARRAYSIZE(SPAM_CH) - 1)];
					break;
				default: spamMsg = SPAM_FED[Utils::RandIntSimple(0, ARRAYSIZE(SPAM_FED) - 1)];
					break;
			}

			Utils::ReplaceSpecials(spamMsg);

			spamMsg.insert(0, "say ");
			I::EngineClient->ClientCmd_Unrestricted(spamMsg.c_str());
		}

		// Voicechat Spam
		if (Vars::Misc::VoicechatSpam.Value != 0)
		{
			std::string voiceCommand;
			switch (Vars::Misc::VoicechatSpam.Value)
			{
				case 1: voiceCommand = "0 0"; break;
				case 2: voiceCommand = "2 0"; break;
				case 3: voiceCommand = "2 6"; break;
				default: voiceCommand = std::format("{:d} {:d}", Utils::RandIntSimple(0, 2), Utils::RandIntSimple(0, 8)); break;
			}

			voiceCommand.insert(0, "voicemenu ");
			I::EngineClient->ClientCmd_Unrestricted(voiceCommand.c_str());
		}

		flNextSend = (flCurTime + Vars::Misc::SpamInterval.Value);
	}
}

void CMisc::AutoRocketJump(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!Vars::Misc::AutoRocketJump.Value || !G::WeaponCanAttack || !GetAsyncKeyState(VK_RBUTTON))
	{
		return;
	}

	if (I::EngineVGui->IsGameUIVisible() || I::VGuiSurface->IsCursorVisible())
	{
		return;
	}

	if (pLocal->GetClassNum() != CLASS_SOLDIER || !pLocal->OnSolid() || pLocal->IsDucking() || (pLocal->GetHealth() < 60 && Vars::Misc::NonLethalRocketJump.Value)) // health check is meh, you could check the damage of the launcher, and find the damage at distance from explosion, but that's a lot of work, and it will just be ~40 anyway.
	{
		return;
	}

	if (const auto& pWeapon = g_EntityCache.GetWeapon())
	{
		if (pWeapon->IsInReload())
		{
			pCmd->buttons |= IN_ATTACK;
			return;
		}
		if (pCmd->buttons & IN_ATTACK)
		{
			pCmd->buttons &= ~IN_ATTACK;
		}

		if (G::CurItemDefIndex == Soldier_m_TheBeggarsBazooka
			|| G::CurItemDefIndex == Soldier_m_TheCowMangler5000
			|| pWeapon->GetSlot() != SLOT_PRIMARY)
		{
			return;
		}

		if (pLocal->GetViewOffset().z < 60.05f)
		{
			pCmd->buttons |= IN_ATTACK | IN_JUMP;

			const Vec3 vVelocity = pLocal->GetVelocity();
			Vec3 vAngles = { vVelocity.IsZero() ? 89.0f : 45.0f, Math::VelocityToAngles(vVelocity).y - 180.0f, 0.0f };

			if (G::CurItemDefIndex != Soldier_m_TheOriginal && !vVelocity.IsZero())
			{
				Vec3 vForward = {}, vRight = {}, vUp = {};
				Math::AngleVectors(vAngles, &vForward, &vRight, &vUp);
				Math::VectorAngles((vForward * 23.5f) + (vRight * -5.6f) + (vUp * -3.0f), vAngles);
			}

			Math::ClampAngles(vAngles);
			pCmd->viewangles = vAngles;
			G::SilentTime = true;
		}

		else
		{
			pCmd->buttons |= IN_DUCK;
		}
	}
}

bool IsForceANature()
{
	return (G::CurItemDefIndex == Scout_m_ForceANature || G::CurItemDefIndex == Scout_m_FestiveForceANature);
}

void CMisc::AutoScoutJump(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static int iJumpKey = VK_RBUTTON;
	static KeyHelper jumpKey{ &iJumpKey };
	if (!Vars::Misc::AutoScoutJump.Value || !G::WeaponCanAttack || !jumpKey.Pressed())
	{
		return;
	}

	if (I::EngineVGui->IsGameUIVisible() || I::VGuiSurface->IsCursorVisible())
	{
		return;
	}

	if (pLocal->GetClassNum() != CLASS_SCOUT || !IsForceANature() || pLocal->IsDucking())
	{
		return;
	}

	pCmd->buttons |= IN_ATTACK | IN_JUMP | IN_FORWARD;
	pCmd->forwardmove = 10.f;
	pCmd->viewangles.x = 25.f;
	G::SilentTime = true;
}

bool CMisc::TauntControl(CUserCmd* pCmd)
{
	bool bReturn = true;
	// Handle Taunt Slide
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		if (pLocal->IsTaunting())
		{
			static KeyHelper spinKey{ &Vars::Misc::TauntSpinKey.Value };
			if (Vars::Misc::TauntSpin.Value)
			{
				if (spinKey.Down())
				{
					if (m_flSpinYaw > 180)
					{
						m_flSpinYaw = -180;
					}
					m_flSpinYaw += Vars::Misc::TauntSpinSpeed.Value;

					pCmd->viewangles.y += m_flSpinYaw;
					pCmd->viewangles.x = 90.f;

					bReturn = false;
				}
			}

			if (Vars::Misc::TauntSlide.Value)
			{
				if (Vars::Misc::TauntControl.Value)
				{
					if (pCmd->buttons & IN_BACK)
					{
						pCmd->viewangles.x = 91.0f;
					}
					else if (pCmd->buttons & IN_FORWARD)
					{
						pCmd->viewangles.x = 0.0f;
					}
					else
					{
						pCmd->viewangles.x = 90.0f;
					}
					bReturn = false;
				}

				if (Vars::Misc::TauntFollowsCamera.Value && !spinKey.Down())
				{
					const Vec3 vAngle = I::EngineClient->GetViewAngles();
					pCmd->viewangles.y = vAngle.y;

					bReturn = false;
				}

				bReturn = false;
			}
		}
	}

	return bReturn;
}

void CMisc::ViewmodelFlip(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!Vars::Misc::ViewmodelFlip.Value || G::CurWeaponType != EWeaponType::PROJECTILE) { return; }

	static auto cl_flipviewmodels = g_ConVars.FindVar("cl_flipviewmodels");
	static bool defaultValue = cl_flipviewmodels->GetBool();

	const auto aimTarget = I::ClientEntityList->GetClientEntity(G::CurrentTargetIdx);
	if (G::CurrentTargetIdx <= 0 || !aimTarget || Utils::VisPosFraction(pLocal, pLocal->GetEyePosition(), aimTarget->GetWorldSpaceCenter()))
	{
		cl_flipviewmodels->SetValue(defaultValue);
		return;
	}

	const auto localAngles = I::EngineClient->GetViewAngles();
	const auto aimAngles = Math::CalcAngle(pLocal->GetEyePosition(), aimTarget->GetWorldSpaceCenter());

	auto mod = [](float a, float n)
	{
		return a - std::floor(a / n) * n;
	};

	const auto angleDelta = mod((aimAngles.y - localAngles.y) + 180.f, 360.f) - 180.f;
	if (angleDelta < -5.f)
	{
		cl_flipviewmodels->SetValue(true);
	}
	else if (angleDelta > 5.f)
	{
		cl_flipviewmodels->SetValue(false);
	}
}

//	Accelerate ( wishdir, wishspeed, sv_accelerate.GetFloat() );
//	accelspeed = accel * gpGlobals->frametime * wishspeed * player->m_surfaceFriction;
//	wishspeed = side/forwardmove from pCmd
//	accel = sv_accelerate value
//	10 * .015 * 450 * surfaceFriction	=	acceleration
//	67.5(surfaceFriction)				=	acceleration
//	acceleration = 60
//	surfaceFriction = 1.125	// this doesn't account for ice, etc. (it is also possible that the reason our accel is lower is because we are locked below 450 with our actual acceleration)
//	if our forward velocity is 400, to get it to 0, we would need to spend ~7 ticks of time decelerating.
void CMisc::FastStop(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	// 1<<17 = TFCond_Charging

	if (pLocal && pLocal->IsAlive() && !pLocal->IsCharging() && !pLocal->IsTaunting() && !pLocal->IsStunned() && pLocal->GetVelocity().Length2D() > 5.f)
	{
		const int stopType = (
			G::ShouldShift && G::ShiftedTicks && Vars::Misc::CL_Move::AntiWarp.Value ?
			pLocal->OnSolid() ? 1 : 2 : 0
			); // 0.none, 1.ground, 2.not ground
		static Vec3 predEndPoint = {};
		static Vec3 currentPos{};
		static int nShiftTickG = 0;
		static int nShiftTickA = 0;

		switch (stopType)
		{
			case 0:
			{
				nShiftTickG = 0;
				nShiftTickA = 0;
				return;
			}
			case 1:
			{
				switch (nShiftTickG)
				{
					case 0:
					{
						G::ShouldStop = true;
						predEndPoint = pLocal->GetVecOrigin() + pLocal->GetVecVelocity();
						nShiftTickG++;
						break;
					}

					default:
					{
						nShiftTickG++;
						break;
					}
				}//

				currentPos = pLocal->GetVecOrigin();
				Utils::WalkTo(pCmd, pLocal, predEndPoint, currentPos, (1.f / currentPos.Dist2D(predEndPoint)));
				//	the "slight stop" that u can see when we do this is due to (i believe) the player reaching the desired point, and then constantly accelerating backwards, meaning their velocity-
				//	when they finish shifting ticks, is lower than when they started.
				//	alot of things worked better than (1/dist) as the scale, but caused issues on different classes, for now this is the best I can get it to.
				return;
			}
			case 2:
			{
				switch (nShiftTickA)
				{
					case 0:
					{
						predEndPoint = pLocal->GetVecOrigin();
						nShiftTickA++;
						break;
					}
					default:
					{
						nShiftTickA++;
						break;
					}
				}

				currentPos = pLocal->GetVecOrigin();
				Utils::WalkTo(pCmd, pLocal, predEndPoint, currentPos, 500);
				return;
			}
			default:
			{
				return;
			}
		}
	}
}

bool CanAttack(CBaseEntity* pLocal, const Vec3& pPos)
{
	if (const auto pWeapon = pLocal->GetActiveWeapon())
	{
		if (!G::WeaponCanHeadShot && pLocal->IsScoped()) { return false; }
		if (!pWeapon->CanShoot(pLocal)) { return false; }

		for (const auto& target : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
		{
			if (!target->IsAlive()) { continue; }
			if (F::AimbotGlobal.ShouldIgnore(target)) { continue; }

			const bool bForceOnShot = Vars::Backtrack::Enabled.Value && (BacktrackMode)Vars::Aimbot::Hitscan::BackTrackMethod.Value == BacktrackMode::FORCEONSHOT && Utils::GetWeaponType(target->GetActiveWeapon()) == EWeaponType::HITSCAN;

			// Get the hitbox position (Backtrack if possible)
			Vec3 targetPos = target->GetHitboxPos(HITBOX_HEAD);
			
			const auto& btRecord = Vars::Backtrack::Enabled.Value ? F::Backtrack.Aimbot(target, (BacktrackMode)Vars::Aimbot::Hitscan::BackTrackMethod.Value, HITBOX_HEAD) : std::nullopt;	//	if backtrack is enabled, see if any records will be targetted by us
			return Vars::Backtrack::Enabled.Value && bForceOnShot ? btRecord.has_value() : (Utils::VisPos(pLocal, target, pPos, targetPos) || btRecord.has_value());
		}
	}

	return false;
}

void CMisc::AutoPeek(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static bool posPlaced = false;
	static bool isReturning = false;
	static bool hasDirection = false;
	static Vec3 peekStart;
	static Vec3 peekVector;

	static KeyHelper peekKey{ &Vars::Misc::CL_Move::AutoPeekKey.Value };
	if (pLocal->IsAlive() && Vars::Misc::CL_Move::AutoPeekKey.Value && peekKey.Down())
	{
		const Vec3 localPos = pLocal->GetAbsOrigin();

		// We just started peeking. Save the return position!
		if (!posPlaced)
		{
			if (pLocal->OnSolid())
			{
				PeekReturnPos = localPos;
				posPlaced = true;
			}
		}
		else
		{
			static Timer particleTimer{};
			if (particleTimer.Run(700))
			{
				Particles::DispatchParticleEffect("ping_circle", PeekReturnPos, {});
			}
		}

		// We need a peek direction (A / D)
		if (!Vars::Misc::CL_Move::AutoPeekFree.Value && !hasDirection && pLocal->OnSolid())
		{
			const Vec3 viewAngles = I::EngineClient->GetViewAngles();
			Vec3 vForward, vRight, vUp, vDirection;
			Math::AngleVectors(viewAngles, &vForward, &vRight, &vUp);

			if (GetAsyncKeyState(VK_A) & 0x8000 || GetAsyncKeyState(VK_W) & 0x8000 || GetAsyncKeyState(VK_D) & 0x8000 || GetAsyncKeyState(VK_S) & 0x8000)
			{
				CGameTrace trace;
				CTraceFilterWorldAndPropsOnly traceFilter;
				Ray_t traceRay;

				if (GetAsyncKeyState(VK_A) & 0x8000 || GetAsyncKeyState(VK_W) & 0x8000)
				{
					vDirection = pLocal->GetEyePosition() - vRight * Vars::Misc::CL_Move::AutoPeekDistance.Value; // Left
				}
				else if (GetAsyncKeyState(VK_D) & 0x8000 || GetAsyncKeyState(VK_S) & 0x8000)
				{
					vDirection = pLocal->GetEyePosition() + vRight * Vars::Misc::CL_Move::AutoPeekDistance.Value; // Right
				}

				traceRay.Init(pLocal->GetEyePosition(), vDirection);
				I::EngineTrace->TraceRay(traceRay, MASK_SOLID, &traceFilter, &trace);
				peekStart = trace.vStartPos;
				peekVector = trace.vEndPos - trace.vStartPos;
				hasDirection = true;
			}
		}

		// Should we peek?
		if (!Vars::Misc::CL_Move::AutoPeekFree.Value && hasDirection)
		{
			bool targetFound = false;
			for (int i = 10; i < 100; i += 10)
			{
				const float step = i / 100.f;
				Vec3 currentPos = peekStart + (peekVector * step);
				if (CanAttack(pLocal, currentPos))
				{
					Utils::WalkTo(pCmd, pLocal, currentPos);
					targetFound = true;
				}

				if (targetFound)
				{
					I::DebugOverlay->AddLineOverlayAlpha(PeekReturnPos, currentPos, 68, 189, 50, 100, false, 0.04f);
					break;
				}

				I::DebugOverlay->AddLineOverlayAlpha(PeekReturnPos, currentPos, 235, 59, 90, 100, false, 0.04f);
			}

			if (!targetFound) { isReturning = true; }
		}

		// We've just attacked. Let's return!
		if (G::LastUserCmd->buttons & IN_ATTACK || F::AimbotGlobal.IsAttacking())
		{
			isReturning = true;
		}

		if (isReturning)
		{
			if (localPos.DistTo(PeekReturnPos) < 7.f)
			{
				// We reached our destination. Recharge DT if wanted
				if (Vars::Misc::CL_Move::AutoRecharge.Value && isReturning && !G::ShouldShift && !G::ShiftedTicks)
				{
					G::RechargeQueued = true;
				}
				isReturning = false;
				return;
			}

			Utils::WalkTo(pCmd, pLocal, PeekReturnPos);
		}
	}
	else
	{
		posPlaced = isReturning = hasDirection = false;
		PeekReturnPos = Vec3();
	}
}

void CMisc::SteamRPC()
{
	if (!Vars::Misc::Steam::EnableRPC.Value)
	{
		if (SteamCleared == false) //stupid way to return back to normal rpc
		{
			g_SteamInterfaces.Friends->SetRichPresence("steam_display", "");
			//this will only make it say "Team Fortress 2" until the player leaves/joins some server. its bad but its better than making 1000 checks to recreate the original
			SteamCleared = true;
		}
		return;
	}

	SteamCleared = false;
	g_SteamInterfaces.Friends->SetRichPresence("steam_display", "#TF_RichPresence_Display");

	/*
	"TF_RichPresence_State_MainMenu"              "Main Menu"
	"TF_RichPresence_State_SearchingGeneric"      "Searching for a Match"
	"TF_RichPresence_State_SearchingMatchGroup"   "Searching - %matchgrouploc_token%"
	"TF_RichPresence_State_PlayingGeneric"        "In Match - %currentmap%"
	"TF_RichPresence_State_LoadingGeneric"        "Joining Match"
	"TF_RichPresence_State_PlayingMatchGroup"     "%matchgrouploc_token% - %currentmap%" <--!!!! used
	"TF_RichPresence_State_LoadingMatchGroup"     "Joining %matchgrouploc_token%"
	"TF_RichPresence_State_PlayingCommunity"      "Community - %currentmap%"
	"TF_RichPresence_State_LoadingCommunity"      "Joining Community Server"
	*/
	if (!I::EngineClient->IsInGame() && Vars::Misc::Steam::OverrideMenu.Value)
	{
		g_SteamInterfaces.Friends->SetRichPresence("state", "MainMenu");
	}
	else
	{
		g_SteamInterfaces.Friends->SetRichPresence("state", "PlayingMatchGroup");

		switch (Vars::Misc::Steam::MatchGroup.Value)
		{
			case 0:
				g_SteamInterfaces.Friends->SetRichPresence("matchgrouploc", "SpecialEvent");
				break;
			case 1:
				g_SteamInterfaces.Friends->SetRichPresence("matchgrouploc", "MannUp");
				break;
			case 2:
				g_SteamInterfaces.Friends->SetRichPresence("matchgrouploc", "Competitive6v6");
				break;
			case 3:
				g_SteamInterfaces.Friends->SetRichPresence("matchgrouploc", "Casual");
				break;
			case 4:
				g_SteamInterfaces.Friends->SetRichPresence("matchgrouploc", "BootCamp");
				break;
			default:
				g_SteamInterfaces.Friends->SetRichPresence("matchgrouploc", "SpecialEvent");
				break;
		}
	}

	/*
	"TF_RichPresence_MatchGroup_Competitive6v6"   "Competitive"
	"TF_RichPresence_MatchGroup_Casual"           "Casual"
	"TF_RichPresence_MatchGroup_SpecialEvent"     "Special Event"
	"TF_RichPresence_MatchGroup_MannUp"           "MvM Mann Up"
	"TF_RichPresence_MatchGroup_BootCamp"         "MvM Boot Camp"
	*/
	switch (Vars::Misc::Steam::MapText.Value)
	{
		case 0:
			if (Vars::Misc::Steam::CustomText.Value.empty())
			{
				g_SteamInterfaces.Friends->SetRichPresence("currentmap", "Fedoraware");
			}
			else
			{
				g_SteamInterfaces.Friends->SetRichPresence("currentmap", Vars::Misc::Steam::CustomText.Value.c_str());
			}
			break;
		case 1:
			g_SteamInterfaces.Friends->SetRichPresence("currentmap", "Fedoraware");
			break;
		case 2:
			g_SteamInterfaces.Friends->SetRichPresence("currentmap", "Figoraware");
			break;
		case 3:
			g_SteamInterfaces.Friends->SetRichPresence("currentmap", "Meowhook.club");
			break;
		case 4:
			g_SteamInterfaces.Friends->SetRichPresence("currentmap", "Rathook.cc");
			break;
		case 5:
			g_SteamInterfaces.Friends->SetRichPresence("currentmap", "Nitro.tf");
			break;
		default:
			g_SteamInterfaces.Friends->SetRichPresence("currentmap", "Fedoraware");
			break;
	}

	g_SteamInterfaces.Friends->SetRichPresence("steam_player_group_size",
												  std::to_string(Vars::Misc::Steam::GroupSize.Value).c_str());
}

void CMisc::UnlockAchievements()
{
	using FN = IAchievementMgr * (*)(void);
	const auto achievementmgr = GetVFunc<FN>(I::EngineClient, 114)();
	if (achievementmgr)
	{
		g_SteamInterfaces.UserStats->RequestCurrentStats();
		for (int i = 0; i < achievementmgr->GetAchievementCount(); i++)
		{
			achievementmgr->AwardAchievement(achievementmgr->GetAchievementByIndex(i)->GetAchievementID());
		}
		g_SteamInterfaces.UserStats->StoreStats();
		g_SteamInterfaces.UserStats->RequestCurrentStats();
	}
}

void CMisc::LockAchievements()
{
	using FN = IAchievementMgr * (*)(void);
	const auto achievementmgr = GetVFunc<FN>(I::EngineClient, 114)();
	if (achievementmgr)
	{
		g_SteamInterfaces.UserStats->RequestCurrentStats();
		for (int i = 0; i < achievementmgr->GetAchievementCount(); i++)
		{
			g_SteamInterfaces.UserStats->ClearAchievement(achievementmgr->GetAchievementByIndex(i)->GetName());
		}
		g_SteamInterfaces.UserStats->StoreStats();
		g_SteamInterfaces.UserStats->RequestCurrentStats();
	}
}
