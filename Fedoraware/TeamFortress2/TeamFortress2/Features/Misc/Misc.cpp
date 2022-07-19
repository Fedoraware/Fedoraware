#include "Misc.h"

#include "../Vars.h"
#include "../ChatInfo/ChatInfo.h"
#include "../../Utils/Timer/Timer.hpp"
#include "../Aimbot/AimbotGlobal/AimbotGlobal.h"

extern int attackStringW;
extern int attackStringH;

void CMisc::Run(CUserCmd* pCmd)
{
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		AccurateMovement(pCmd, pLocal);
		AutoJump(pCmd, pLocal);
		AutoStrafe(pCmd, pLocal);
		NoiseMakerSpam(pLocal);
		ExtendFreeze(pLocal);
		Freecam(pCmd, pLocal);
		RageRetry(pLocal);
		AntiBackstab(pLocal, pCmd);
		LegJitter(pCmd, pLocal);
		ViewmodelFlip(pCmd, pLocal);
		AutoPeek(pCmd, pLocal);
	}

	AntiAFK(pCmd);
	ChatSpam();
	CheatsBypass();
	Teleport(pCmd);
	PingReducer();
	ServerHitbox(); // super secret deathpole feature!!!!
	WeaponSway();
	DetectChoke();
}

void CMisc::RunLate(CUserCmd* pCmd)
{
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		FastStop(pCmd, pLocal);
		AutoRocketJump(pCmd, pLocal);
		AutoScoutJump(pCmd, pLocal);
		FastAccel(pCmd, pLocal);
	}
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
	if (cl_wpn_sway_interp)
	{
		if (Vars::Visuals::ViewmodelSway.Value)
		{
			cl_wpn_sway_interp->SetValue(0.05f);
		}
		else
		{
			if (cl_wpn_sway_interp->GetFloat())
			{
				cl_wpn_sway_interp->SetValue(0.0f);
			}
		}
	}
}

void CMisc::DetectChoke()
{
	for (const auto& player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
	{
		if (!player->IsAlive() || player->GetDormant())
		{
			G::ChokeMap[player->GetIndex()] = 0;
			continue;
		}

		if (player->GetSimulationTime() == player->GetOldSimulationTime())
		{
			G::ChokeMap[player->GetIndex()]++;
		}
		else
		{
			G::ChokeMap[player->GetIndex()] = 0;
		}
	}
}

//	dumb feature made out of spite for fourteen
void CMisc::LegJitter(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static bool pos = true;
	const float scale = pLocal->IsDucking() ? 14.f : 2.f;
	if (G::IsAttacking || G::ShouldShift) { return; }
	if (pCmd->forwardmove == 0.f && pCmd->sidemove == 0.f && pLocal->GetVecVelocity().Length2D() < 10.f && Vars::AntiHack::AntiAim::LegJitter.Value && I::GlobalVars->tickcount % 2)
	{
		pos ? pCmd->forwardmove = scale : pCmd->forwardmove = -scale;
		pos ? pCmd->sidemove = scale : pCmd->sidemove = -scale;
		pos = !pos;
	}
}

void CMisc::ServerHitbox()
{
	// draw our serverside hitbox on local servers, used to test fakelag & antiaim
	if (I::Input->CAM_IsThirdPerson() && Vars::Visuals::ThirdPersonServerHitbox.Value)
	{
		//	i have no idea what this is
		using GetServerAnimating_t = void* (*)(int);
		static auto GetServerAnimating = reinterpret_cast<GetServerAnimating_t>(g_Pattern.Find(L"server.dll", L"55 8B EC 8B 55 ? 85 D2 7E ? A1"));

		using DrawServerHitboxes_t = void(__thiscall*)(void*, float, bool);	// C_BaseAnimating, Duration, MonoColour
		static auto DrawServerHitboxes = reinterpret_cast<DrawServerHitboxes_t>(g_Pattern.Find(L"server.dll", L"55 8B EC 83 EC ? 57 8B F9 80 BF ? ? ? ? ? 0F 85 ? ? ? ? 83 BF ? ? ? ? ? 75 ? E8 ? ? ? ? 85 C0 74 ? 8B CF E8 ? ? ? ? 8B 97"));

		const auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());
		if (pLocal && pLocal->IsAlive())
		{
			void* server_animating = GetServerAnimating(pLocal->GetIndex());
			if (server_animating)
			{
				DrawServerHitboxes(server_animating, I::GlobalVars->interval_per_tick * 2.f, true);
			}
		}
	}
}

void CMisc::AntiBackstab(CBaseEntity* pLocal, CUserCmd* pCmd)
{
	G::AvoidingBackstab = false;
	Vec3 vTargetPos;

	if (!pLocal->IsAlive() || pLocal->IsStunned() || pLocal->IsInBumperKart() || pLocal->IsAGhost() || !Vars::AntiHack::AntiAim::AntiBackstab.Value)
	{
		return;
	}

	if (G::IsAttacking) { return; }

	const Vec3 vLocalPos = pLocal->GetWorldSpaceCenter();
	CBaseEntity* target = nullptr;

	for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!pEnemy || !pEnemy->IsAlive() || pEnemy->GetClassNum() != CLASS_SPY || pEnemy->IsCloaked() || pEnemy->IsAGhost())
		{
			continue;
		}

		Vec3 vEnemyPos = pEnemy->GetWorldSpaceCenter();
		if (!Utils::VisPos(pLocal, pEnemy, vLocalPos, vEnemyPos)) { continue; }
		if (!target && vLocalPos.DistTo(vEnemyPos) < 150.f)
		{
			target = pEnemy;
			vTargetPos = target->GetWorldSpaceCenter();
		}
		else if (vLocalPos.DistTo(vEnemyPos) < vLocalPos.DistTo(vTargetPos) && vLocalPos.DistTo(vEnemyPos) < 150.f)
		{
			target = pEnemy;
			vTargetPos = target->GetWorldSpaceCenter();
		}
	}

	if (target)
	{
		vTargetPos = target->GetWorldSpaceCenter();
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
	ConVar* sv_cheats = g_ConVars.FindVar("sv_cheats");
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

void CMisc::Teleport(const CUserCmd* pCmd)
{
	// Stupid
	static KeyHelper tpKey{ &Vars::Misc::CL_Move::TeleportKey.Value };
	if (tpKey.Down())
	{
		if (Vars::Misc::CL_Move::TeleportMode.Value == 0 && G::TickShiftQueue == 0 && G::ShiftedTicks > 0)
		{
			// Plain teleport
			G::TickShiftQueue = G::ShiftedTicks;
		} // Why do it like this wtf?
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

void CMisc::EdgeJump(CUserCmd* pCmd, const int nOldGroundEnt)
{
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		// Edge Jump
		if ((nOldGroundEnt >= 0) && Vars::Misc::EdgeJump.Value)
		{
			static KeyHelper edgeKey{ &Vars::Misc::EdgeJumpKey.Value };
			if (!Vars::Misc::EdgeJumpKey.Value || edgeKey.Down())
			{
				if (pLocal->IsAlive() && !pLocal->OnSolid() && !pLocal->IsSwimming())
				{
					pCmd->buttons |= IN_JUMP;
				}
			}
		}

		// Duck Jump
		if ((Vars::Misc::DuckJump.Value || Vars::Misc::Followbot::Enabled.Value))
		{
			if (pLocal->IsAlive() && !pLocal->OnSolid() && !pLocal->IsSwimming() && !pLocal->IsStunned())
			{
				pCmd->buttons |= IN_DUCK;
			}
		}
	}
}

void CMisc::FastAccel(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static bool FlipVar = false;
	FlipVar = !FlipVar;
	
	if (!FlipVar) {
		return;
	}

	const bool ShouldAccel = pLocal->IsDucking() ? Vars::Misc::CrouchSpeed.Value : (Vars::Misc::FastAccel.Value || (G::ShouldShift && Vars::Misc::CL_Move::AntiWarp.Value));	//	G::ShouldShift means we can use it in antiwarp independantly of what our user wants.
	if (!ShouldAccel) {
		return;
	}

	if (G::Recharging || G::RechargeQueued || G::Frozen) {
		return;
	}

	if (!pLocal->IsAlive() || pLocal->IsSwimming() || pLocal->IsInBumperKart() || pLocal->IsAGhost() || !pLocal->OnSolid() || G::IsAttacking)
	{
		return;
	}

	if (pCmd->buttons & IN_BULLRUSH) {	//	demoman charge
		return;
	}

	if (pLocal->GetMoveType() == MOVETYPE_NOCLIP
		|| pLocal->GetMoveType() == MOVETYPE_LADDER
		|| pLocal->GetMoveType() == MOVETYPE_OBSERVER)
	{
		return;
	}

	const int maxSpeed = std::min(pLocal->GetMaxSpeed() * (pCmd->forwardmove < 0 ? .85f : .95f) - 1, 510.f); //	get our max speed, then if we are going backwards, reduce it.
	const float curSpeed = pLocal->GetVecVelocity().Length2D();

	if (curSpeed > maxSpeed) {	
		return;	//	no need to accelerate if we are moving at our max speed
	}

	if (pLocal->GetClassNum() == ETFClass::CLASS_HEAVY && pCmd->buttons & IN_ATTACK2 && pLocal->IsDucking()) {
		return;
	}

	if (pCmd->buttons & (IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK))
	{
		Vec3 vecMove(pCmd->forwardmove, pCmd->sidemove, 0.0f);
		float flLength = vecMove.Length();
		if (flLength > 0.0f)
		{
			Vec3 angMoveReverse;
			Math::VectorAngles(vecMove * -1.f, angMoveReverse);
			pCmd->forwardmove = -flLength;
			pCmd->sidemove = 0.0f;
			pCmd->viewangles.y = fmodf(pCmd->viewangles.y - angMoveReverse.y, 360.0f);	//	this doesn't have to be clamped inbetween 180 and -180 because the engine automatically fixes it.
			pCmd->viewangles.z = 270.f;
			G::RollExploiting = true;
			if (Vars::Misc::FakeAccelAngle.Value && !G::AAActive) {
				G::ForceChokePacket = true;
			}
		}
	}
}

void CMisc::AccurateMovement(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!Vars::Misc::AccurateMovement.Value)
	{
		return;
	}

	if (!pLocal->IsAlive()
		|| pLocal->IsSwimming()
		|| pLocal->IsInBumperKart()
		|| pLocal->IsAGhost()
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

	const float Speed = pLocal->GetVecVelocity().Length2D();
	const float SpeedLimit = Vars::Debug::DebugBool.Value ? 2.f : 10.f;	//	does some fucky stuff


	if (Speed > SpeedLimit)
	{
		switch (Vars::Misc::AccurateMovement.Value) {
		case 1: {
			Vec3 direction = pLocal->GetVecVelocity().toAngle();
			direction.y = pCmd->viewangles.y - direction.y;
			const Vec3 negatedDirection = direction.fromAngle() * -Speed;
			pCmd->forwardmove = negatedDirection.x;
			pCmd->sidemove = negatedDirection.y;
			break;
		}
		case 2: {
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

	static bool s_bState = false;

	if (pCmd->buttons & IN_JUMP)
	{
		if (!s_bState && !pLocal->OnSolid())
		{
			pCmd->buttons &= ~IN_JUMP;
		}
		else if (s_bState)
		{
			s_bState = false;
		}
	}
	else if (!s_bState)
	{
		s_bState = true;
	}
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

	ConVar* cl_sidespeed = g_ConVars.FindVar(_("cl_sidespeed"));
	if (!cl_sidespeed || !cl_sidespeed->GetFloat())
	{
		return;
	}

	static bool wasJumping = false;
	const bool isJumping = pCmd->buttons & IN_JUMP;

	switch (Vars::Misc::AutoStrafe.Value)
	{
	default:
		break;
	case 1:
		{
			if (pCmd->mousedx && (!isJumping || wasJumping))
			{
				pCmd->sidemove = pCmd->mousedx > 1 ? cl_sidespeed->GetFloat() : -cl_sidespeed->GetFloat();
			}
			wasJumping = isJumping;
			break;
		}
	case 2:
		{
			const float speed = pLocal->GetVelocity().Length2D();

			if (speed < 2.0f)
			{
				break;
			}

			const auto vel = pLocal->GetVelocity();

			constexpr auto perfectDelta = [](float speed) noexcept {
				if (const auto& pLocal = g_EntityCache.GetLocal())
				{
					static auto speedVar = pLocal->TeamFortress_CalculateMaxSpeed();
					static auto airVar = g_ConVars.FindVar(_("sv_airaccelerate"));
					static auto wishSpeed = 30.0f;

					const auto term = wishSpeed / airVar->GetFloat() / speedVar * 100.f / speed;

					if (term < 1.0f && term > -1.0f)
					{
						return acosf(term);
					}
				}
				return 0.0f;
			};

			const float pDelta = perfectDelta(speed);
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
	_("Fedoraware - github.com/Fedoraware"),
	_("Fedoraware - Best free and open-source cheat!"),
	_("Fedoraware - One tip ahead of the game!"),
	_("Fedoraware - Now available @ https://github.com/Fedoraware!"),
	_("Fedoraware - Based on SEOwned public source!")
};

const std::string SPAM_LBOX[] = {
	_("GET GOOD, GET LMAOBOX!"),
	_("LMAOBOX - WAY TO THE TOP"),
	_("WWW.LMAOBOX.NET - BEST FREE TF2 HACK!")
};

const std::string SPAM_CH[] = {
	_("Cathook - more fun than a ball of yarn!"),
	_("GNU/Linux is the best OS!"),
	_("Visit https://cathook.club for more information!"),
	_("Cathook - Free and Open-Source tf2 cheat!"),
	_("Cathook - ca(n)t stop me meow!")
};

void CMisc::ChatSpam()
{
	if (Vars::Misc::ChatSpam.Value == 0) { return; }

	auto getSpam = []() {
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
		return "say " + spamMsg;
	};

	const float flCurTime = I::EngineClient->Time();
	static float flNextSend = 0.0f;

	if (flCurTime > flNextSend)
	{
		I::EngineClient->ClientCmd_Unrestricted(getSpam().c_str());
		flNextSend = (flCurTime + 4.0f);
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

	if (pLocal->GetClassNum() != CLASS_SOLDIER || !pLocal->IsOnGround() || pLocal->IsDucking() || (pLocal->GetHealth() < 60 && Vars::Misc::NonLethalRocketJump.Value)) // health check is meh, you could check the damage of the launcher, and find the damage at distance from explosion, but that's a lot of work, and it will just be ~40 anyway.
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
			Vec3 vAngles = {vVelocity.IsZero() ? 89.0f : 45.0f, Math::VelocityToAngles(vVelocity).y - 180.0f, 0.0f};

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

	if (pLocal->GetClassNum() != CLASS_SCOUT || G::CurItemDefIndex != Scout_m_ForceANature || pLocal->IsDucking())
	{
		return;
	}

	pCmd->buttons |= IN_ATTACK | IN_JUMP | IN_FORWARD;
	pCmd->forwardmove = 10.f;
	pCmd->viewangles.x = 25.f;
	G::SilentTime = true;
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

	auto mod = [](float a, float n) {
		return a - std::floor(a / n) * n;
	};

	const auto angleDelta = mod((aimAngles.y - localAngles.y) + 180.f, 360.f) - 180.f;
	if (angleDelta < -5.f)
	{
		cl_flipviewmodels->SetValue(true);
	} else if (angleDelta > 5.f)
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
	if (pLocal && pLocal->IsAlive() && !pLocal->IsTaunting() && !pLocal->IsStunned() && pLocal->GetVelocity().Length2D() > 10.f) {
		const int stopType = (
			G::ShouldShift && G::ShiftedTicks && Vars::Misc::CL_Move::AntiWarp.Value ?
			pLocal->OnSolid() ? 1 : 2 : 0
			); // 0.none, 1.ground, 2.not ground
		static Vec3 predEndPoint = {};
		static Vec3 currentPos{};
		static int nShiftTick = 0;

		switch (stopType) {
		case 0: {
			nShiftTick = 0;
			return;
		}
		case 1: {
			switch (nShiftTick) {
			case 0: {
				predEndPoint = pLocal->GetVecOrigin() + pLocal->GetVecVelocity();
				nShiftTick++;
				break;
			}
			case 1: {
				G::ShouldStop = true;
				nShiftTick++;
				break;
			}
			default: {
				nShiftTick++;
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
		case 2: {
			switch (nShiftTick) {
			case 0: {
				predEndPoint = pLocal->GetVecOrigin();
				nShiftTick++;
				break;
			}
			default: {
				nShiftTick++;
				break;
			}
			}

			currentPos = pLocal->GetVecOrigin();
			Utils::WalkTo(pCmd, pLocal, predEndPoint, currentPos, 500);
			return;
		}
		default: {
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

			if (Utils::VisPos(pLocal, target, pPos, target->GetHitboxPos(HITBOX_HEAD)))
			{
				return true;
			}
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
			if (pLocal->IsOnGround())
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
		if (!Vars::Misc::CL_Move::AutoPeekFree.Value && !hasDirection && pLocal->IsOnGround())
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
		if (G::LastUserCmd->buttons & IN_ATTACK || G::IsAttacking)
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
			g_SteamInterfaces.Friends015->SetRichPresence("steam_display", "");
			//this will only make it say "Team Fortress 2" until the player leaves/joins some server. its bad but its better than making 1000 checks to recreate the original
			SteamCleared = true;
		}
		return;
	}

	SteamCleared = false;
	g_SteamInterfaces.Friends015->SetRichPresence("steam_display", "#TF_RichPresence_Display");

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
		g_SteamInterfaces.Friends015->SetRichPresence("state", "MainMenu");
	}
	else
	{
		g_SteamInterfaces.Friends015->SetRichPresence("state", "PlayingMatchGroup");

		switch (Vars::Misc::Steam::MatchGroup.Value)
		{
		case 0:
			g_SteamInterfaces.Friends015->SetRichPresence("matchgrouploc", "SpecialEvent");
			break;
		case 1:
			g_SteamInterfaces.Friends015->SetRichPresence("matchgrouploc", "MannUp");
			break;
		case 2:
			g_SteamInterfaces.Friends015->SetRichPresence("matchgrouploc", "Competitive6v6");
			break;
		case 3:
			g_SteamInterfaces.Friends015->SetRichPresence("matchgrouploc", "Casual");
			break;
		case 4:
			g_SteamInterfaces.Friends015->SetRichPresence("matchgrouploc", "BootCamp");
			break;
		default:
			g_SteamInterfaces.Friends015->SetRichPresence("matchgrouploc", "SpecialEvent");
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
			g_SteamInterfaces.Friends015->SetRichPresence("currentmap", "Fedoraware");
		}
		else
		{
			g_SteamInterfaces.Friends015->SetRichPresence("currentmap", Vars::Misc::Steam::CustomText.Value.c_str());
		}
		break;
	case 1:
		g_SteamInterfaces.Friends015->SetRichPresence("currentmap", "Fedoraware");
		break;
	case 2:
		g_SteamInterfaces.Friends015->SetRichPresence("currentmap", "Figoraware");
		break;
	case 3:
		g_SteamInterfaces.Friends015->SetRichPresence("currentmap", "Meowhook.club");
		break;
	case 4:
		g_SteamInterfaces.Friends015->SetRichPresence("currentmap", "Rathook.cc");
		break;
	case 5:
		g_SteamInterfaces.Friends015->SetRichPresence("currentmap", "Nitro.tf");
		break;
	default:
		g_SteamInterfaces.Friends015->SetRichPresence("currentmap", "Fedoraware");
		break;
	}

	g_SteamInterfaces.Friends015->SetRichPresence("steam_player_group_size",
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

// Myzarfin added this
void CNotifications::Think()
{
	constexpr int x{1};
	int y{1};
	constexpr int size{20};

	if (NotificationTexts.size() > (MAX_NOTIFY_SIZE + 1))
	{
		NotificationTexts.erase(NotificationTexts.begin());
	}

	for (size_t i{}; i < NotificationTexts.size(); ++i)
	{
		const auto notify = NotificationTexts[i];

		notify->Time -= I::GlobalVars->absoluteframetime;

		if (notify->Time <= 0.f)
		{
			NotificationTexts.erase(NotificationTexts.begin() + i);
		}
	}

	if (NotificationTexts.empty())
	{
		return;
	}

	for (size_t i{}; i < NotificationTexts.size(); ++i)
	{
		const auto notify = NotificationTexts[i];

		const float left = notify->Time;
		Color_t color = notify->Color;

		if (left < .5f)
		{
			float f = left;
			Math::Clamp(f, 0.f, .5f);

			f /= .5f;

			color.a = static_cast<int>(f * 255.f);

			if (i == 0 && f < 0.2f)
			{
				y -= size * (1.f - f / 0.2f);
			}
		}

		else
		{
			color.a = 255;
		}

		const size_t cSize = strlen(notify->Text.c_str()) + 1;
		const auto wc = new wchar_t[cSize];
		mbstowcs(wc, notify->Text.c_str(), cSize);

		int w, h;

		I::VGuiSurface->GetTextSize(FONT_INDICATORS, wc, w, h);

		delete[] wc; // Memory leak

		g_Draw.Line(x, y, x, y + 19, {Colors::NotifOutline.r, Colors::NotifOutline.g, Colors::NotifOutline.b, color.a});
		g_Draw.GradientRectA(x + 1, y, w / 3 + 9, y + 19,
		                     {Colors::NotifBG.r, Colors::NotifBG.g, Colors::NotifBG.b, color.a},
		                     {
			                     Colors::NotifBG.r, Colors::NotifBG.g, Colors::NotifBG.b, 1
		                     }, true);
		g_Draw.String(FONT_INDICATORS, x + 6, y + 2,
		              {Colors::NotifText.r, Colors::NotifText.g, Colors::NotifText.b, color.a},
		              ALIGN_DEFAULT, notify->Text.c_str());

		y += size;
	}
}
