#include "Misc.h"

#include "../Vars.h"
#include "../ChatInfo/ChatInfo.h"
#include "../../Utils/Timer/Timer.hpp"
#include "../PlayerResource/PlayerResource.h"

extern int attackStringW;
extern int attackStringH;

void CMisc::Run(CUserCmd* pCmd)
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		AutoJump(pCmd, pLocal);
		AutoStrafe(pCmd, pLocal);
		NoiseMakerSpam(pLocal);
		ExtendFreeze(pLocal);
		Freecam(pCmd, pLocal);
		AntiBackstab(pLocal, pCmd);
		LegJitter(pCmd, pLocal);
	}
	AutoJoin();
	ChatSpam();
	CheatsBypass();
	NoPush();
	PingReducer();
	ServerHitbox(); // super secret deathpole feature!!!!
	WeaponSway();
	DetectChoke();
}

void CMisc::RunLate(CUserCmd* pCmd)
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		AutoPeek(pCmd, pLocal);
		AutoRocketJump(pCmd, pLocal);
	}
}

void CMisc::WeaponSway()	//	pasted but looks cool
{
	static ConVar* cl_wpn_sway_interp = g_Interfaces.CVars->FindVar("cl_wpn_sway_interp");
	if (cl_wpn_sway_interp)
	{
		if (Vars::Visuals::ViewmodelSway.m_Var) {
			cl_wpn_sway_interp->SetValue(0.05f);
		}
		else
		{
			if (cl_wpn_sway_interp->GetFloat())
				cl_wpn_sway_interp->SetValue(0.0f);
		}
	}
}

void CMisc::DetectChoke()
{
	for (const auto& player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
	{
		if (!player->IsAlive())
		{
			g_GlobalInfo.chokeMap[player->GetIndex()].ChokedTicks = 0;
			continue;
		}

		if (player->GetSimulationTime() == g_GlobalInfo.chokeMap[player->GetIndex()].LastSimTime)
		{
			g_GlobalInfo.chokeMap[player->GetIndex()].ChokedTicks++;
			// TODO: This could be used to detect cheaters
		} else
		{
			g_GlobalInfo.chokeMap[player->GetIndex()].ChokedTicks = 0;
		}

		g_GlobalInfo.chokeMap[player->GetIndex()].LastSimTime = player->GetSimulationTime();
	}
}

//	dumb feature made out of spite for fourteen
void CMisc::LegJitter(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static bool pos = true;

	if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon) {
		if (g_GlobalInfo.m_bAttacking || g_GlobalInfo.m_bShouldShift) { return; }
		if (pCmd->forwardmove == 0.f && pCmd->sidemove == 0.f && pLocal->GetVecVelocity().Length2D() < 10.f && Vars::AntiHack::AntiAim::legjitter.m_Var) {
			pos ? pCmd->forwardmove = 2.f : pCmd->forwardmove = -2.f;
			pos ? pCmd->sidemove = 2.f : pCmd->sidemove = -2.f;
			pos = !pos;
		}
	}
}

void CMisc::ServerHitbox()
{
	// draw our serverside hitbox on local servers, used to test fakelag & antiaim
	if (g_Interfaces.Input->CAM_IsThirdPerson() && Vars::Visuals::ThirdPersonServerHitbox.m_Var)
	{
		using GetServerAnimating_t = void* (*)(int);
		static auto GetServerAnimating = reinterpret_cast<GetServerAnimating_t>(g_Pattern.Find(
			XorStr(L"server.dll").c_str(), XorStr(L"55 8B EC 8B 55 ? 85 D2 7E ? A1").c_str()));
		using DrawServerHitboxes_t = void(__thiscall*)(void*, float, bool);
		static auto DrawServerHitboxes = reinterpret_cast<DrawServerHitboxes_t>(g_Pattern.Find(
			XorStr(L"server.dll").c_str(),
			XorStr(
				L"55 8B EC 83 EC ? 57 8B F9 80 BF ? ? ? ? ? 0F 85 ? ? ? ? 83 BF ? ? ? ? ? 75 ? E8 ? ? ? ? 85 C0 74 ? 8B CF E8 ? ? ? ? 8B 97")
			.c_str()));
		auto pLocal = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer());
		if (pLocal && pLocal->IsAlive())
		{
			void* server_animating = GetServerAnimating(pLocal->GetIndex());
			if (server_animating)
			{
				DrawServerHitboxes(server_animating, g_Interfaces.GlobalVars->interval_per_tick * 2.f, false);
			}
		}
	}
}

void CMisc::AntiBackstab(CBaseEntity* pLocal, CUserCmd* pCmd)
{
	g_GlobalInfo.m_bAvoidingBackstab = false;
	CBaseEntity* target{}; Vec3 vTargetPos;

	if (!pLocal->IsAlive() || pLocal->IsStunned() || pLocal->IsInBumperKart() || pLocal->IsAGhost() || !Vars::AntiHack::AntiAim::AntiBackstab.m_Var)
		return;

	if (g_GlobalInfo.m_bAttacking) { return; }	// not needed but whatever
	if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon) { if (Utils::IsAttacking(pCmd, pWeapon)) { return; } }

	Vec3 vLocalPos = pLocal->GetWorldSpaceCenter();
	
	target = nullptr;

	for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!pEnemy || !pEnemy->IsAlive() || pEnemy->GetClassNum() != CLASS_SPY || pEnemy->IsCloaked() || pEnemy->IsAGhost())
			continue;

		Vec3 vEnemyPos = pEnemy->GetWorldSpaceCenter();
		if (!target && vLocalPos.DistTo(vEnemyPos) < 150.f) {
			target = pEnemy;
			vTargetPos = target->GetWorldSpaceCenter();
		}
		else if (vLocalPos.DistTo(vEnemyPos) < vLocalPos.DistTo(vTargetPos) && vLocalPos.DistTo(vEnemyPos) < 150.f) {
			target = pEnemy;
			vTargetPos = target->GetWorldSpaceCenter();
		}
	}

	if (target) {
		vTargetPos = target->GetWorldSpaceCenter();
		Vec3 vAngleToSpy = Math::CalcAngle(vLocalPos, vTargetPos);
		g_GlobalInfo.m_bAvoidingBackstab = true;
		Utils::FixMovement(pCmd, vAngleToSpy);
		pCmd->viewangles = vAngleToSpy;
		return;
	}
}

/*
void CMisc::InstantRespawnMVM() {
	if (g_Interfaces.Engine->IsInGame() && g_Interfaces.Engine->GetLocalPlayer() && !g_EntityCache.m_pLocal->IsAlive() && Vars::Misc::MVMRes.m_Var) {
		auto kv = new KeyValues("MVM_Revive_Response");
		kv->SetInt("accepted", 1);
		g_Interfaces.Engine->ServerCmdKeyValues(kv);
	}
}*/

void CMisc::CheatsBypass()
{
	static bool cheatset = false;
	ConVar* sv_cheats = g_Interfaces.CVars->FindVar("sv_cheats");
	if (Vars::Misc::CheatsBypass.m_Var && sv_cheats)
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

void CMisc::PingReducer() {
	const ConVar* cl_cmdrate = g_Interfaces.CVars->FindVar("cl_cmdrate");
	CNetChannel* netChannel = g_Interfaces.Engine->GetNetChannelInfo();
	if (cl_cmdrate == nullptr || netChannel == nullptr) { return; }

	static Timer updateRateTimer{ };
	if (updateRateTimer.Run(500)) {
		if (Vars::Misc::PingReducer.m_Var) {
			const int currentPing = g_PR->GetPing(g_Interfaces.Engine->GetLocalPlayer());
			NET_SetConVar cmd("cl_cmdrate", (Vars::Misc::PingTarget.m_Var <= currentPing) ? "-1" : std::to_string(cl_cmdrate->GetInt()).c_str());
			netChannel->SendNetMsg(cmd);
		}
		else {
			NET_SetConVar cmd("cl_cmdrate", std::to_string(cl_cmdrate->GetInt()).c_str());
			netChannel->SendNetMsg(cmd);
		}
	}
}

void CMisc::ExtendFreeze(CBaseEntity* pLocal)
{
	if (Vars::Misc::ExtendFreeze.m_Var && g_Interfaces.Engine->IsInGame() && !pLocal->IsAlive()) {
		static Timer cmdTimer{ };
		if (cmdTimer.Run(2000)) {
			g_Interfaces.Engine->ClientCmd_Unrestricted("extendfreeze");
		}
	}
}

void CMisc::Freecam(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (Vars::Visuals::FreecamKey.m_Var && GetAsyncKeyState(Vars::Visuals::FreecamKey.m_Var) & 0x8000) {
		if (g_GlobalInfo.m_bFreecamActive == false) {
			g_GlobalInfo.m_vFreecamPos = pLocal->GetVecOrigin();
			g_GlobalInfo.m_bFreecamActive = true;
		}

		const Vec3 viewAngles = g_Interfaces.Engine->GetViewAngles();
		const float zMove = sinf(DEG2RAD(viewAngles.x));
		Vec3 vForward, vRight, vUp;
		Math::AngleVectors(viewAngles, &vForward, &vRight, &vUp);
		Vec3 moveVector;

		if (pCmd->buttons & IN_FORWARD) {
			moveVector += vForward;
			moveVector.z -= zMove;
		}

		if (pCmd->buttons & IN_BACK) {
			moveVector -= vForward;
			moveVector.z += zMove;
		}

		if (pCmd->buttons & IN_MOVELEFT) {
			moveVector -= vRight;
		}

		if (pCmd->buttons & IN_MOVERIGHT) {
			moveVector += vRight;
		}

		Math::VectorNormalize(moveVector);
		moveVector *= Vars::Visuals::FreecamSpeed.m_Var;
		g_GlobalInfo.m_vFreecamPos += moveVector;

		pCmd->buttons = 0;
		pCmd->forwardmove = 0.f;
		pCmd->sidemove = 0.f;
		pCmd->upmove = 0.f;
	}
	else {
		g_GlobalInfo.m_bFreecamActive = false;
	}
}

const std::string classNames[] = {"scout", "soldier", "pyro", "demoman", "heavyweapons", "engineer", "medic", "sniper", "spy"};
void CMisc::AutoJoin()
{
	if (Vars::Misc::AutoJoin.m_Var > 0) {
		static Timer cmdTimer{ };
		if (cmdTimer.Run(250)) {
			bool inTeam = false;
			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				inTeam = pLocal->GetTeamNum() != TEAM_NONE && pLocal->IsInValidTeam();
				if (g_Interfaces.Engine->IsInGame() && !pLocal->IsClass(Vars::Misc::AutoJoin.m_Var - 1)) {
					const std::string classCmd = "join_class " + classNames[Vars::Misc::AutoJoin.m_Var - 1];
					g_Interfaces.Engine->ClientCmd_Unrestricted(classCmd.c_str());
				}
			}

			if (!inTeam && g_Interfaces.Engine->IsConnected()) {
				g_Interfaces.Engine->ClientCmd_Unrestricted("autoteam");
			}
		}
	}
}

void CMisc::EdgeJump(CUserCmd* pCmd, const int nOldFlags)
{
	if ((nOldFlags & FL_ONGROUND) && Vars::Misc::EdgeJump.m_Var)
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		if (!Vars::Misc::EdgeJumpKey.m_Var || GetAsyncKeyState(Vars::Misc::EdgeJumpKey.m_Var))
		{
			if (pLocal->IsAlive() && !pLocal->IsOnGround() && !pLocal->IsSwimming())
				pCmd->buttons |= IN_JUMP;
		}
	}
}

void CMisc::NoPush()
{
	ConVar* noPush = g_Interfaces.CVars->FindVar("tf_avoidteammates_pushaway");
	noPush->SetValue(Vars::Misc::NoPush.m_Var ? 0 : 1);
}

void CMisc::AutoJump(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!Vars::Misc::AutoJump.m_Var
		|| !pLocal->IsAlive()
		|| pLocal->IsSwimming()
		|| pLocal->IsInBumperKart()
		|| pLocal->IsAGhost())
		return;

	if (pLocal->GetMoveType() == MoveType_t::MOVETYPE_NOCLIP
		|| pLocal->GetMoveType() == MoveType_t::MOVETYPE_LADDER
		|| pLocal->GetMoveType() == MoveType_t::MOVETYPE_OBSERVER)
		return;

	static bool s_bState = false;
	static float height = pLocal->GetVecVelocity().z;

	if (pCmd->buttons & IN_JUMP)
	{
		if (!s_bState && !pLocal->IsOnGround())
		{
			pCmd->buttons &= ~IN_JUMP;

			if (Vars::Misc::DuckJump.m_Var)
			{
				if (pLocal->GetVecVelocity().z > height)
					pCmd->buttons |= IN_DUCK;
				else
					pCmd->buttons &= ~IN_DUCK;
			}
		}
		else if (s_bState)
			s_bState = false;
	}
	else if (!s_bState)
		s_bState = true;
}

void CMisc::AutoStrafe(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!Vars::Misc::AutoStrafe.m_Var)
		return;

	if (!pLocal->IsAlive()
		|| pLocal->IsSwimming()
		|| pLocal->IsInBumperKart()
		|| pLocal->IsAGhost()
		|| pLocal->IsOnGround())
		return;

	if (pLocal->GetMoveType() == MOVETYPE_NOCLIP
		|| pLocal->GetMoveType() == MOVETYPE_LADDER
		|| pLocal->GetMoveType() == MOVETYPE_OBSERVER)
		return;

	ConVar* cl_sidespeed = g_Interfaces.CVars->FindVar(_("cl_sidespeed"));

	if (!cl_sidespeed || !cl_sidespeed->GetFloat())
		return;

	static bool was_jumping = false;
	bool is_jumping = pCmd->buttons & IN_JUMP;

	switch (Vars::Misc::AutoStrafe.m_Var)
	{
	default:
		break;
	case 1:
	{
		if (pCmd->mousedx && (!is_jumping || was_jumping))
			pCmd->sidemove = pCmd->mousedx > 1 ? cl_sidespeed->GetFloat() : -cl_sidespeed->GetFloat();
		was_jumping = is_jumping;
		break;
	}
	case 2:
	{
		const float speed = pLocal->GetVelocity().Length2D();

		if (speed < 2.0f)
			break;

		auto vel = pLocal->GetVelocity();

		constexpr auto perfectDelta = [](float speed) noexcept
		{
			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				static auto speedVar = pLocal->TeamFortress_CalculateMaxSpeed();
				static auto airVar = g_Interfaces.CVars->FindVar(_("sv_airaccelerate"));
				static auto wishSpeed = 30.0f;

				const auto term = wishSpeed / airVar->GetFloat() / speedVar * 100.f / speed;

				if (term < 1.0f && term > -1.0f)
					return acosf(term);
			}
			return 0.0f;
		};

		const float pDelta = perfectDelta(speed);
		if ((!is_jumping || was_jumping) && pDelta)
		{
			const float yaw = DEG2RAD(pCmd->viewangles.y);
			const float velDir = atan2f(vel.y, vel.x) - yaw;
			const float wishAng = atan2f(-pCmd->sidemove, pCmd->forwardmove);
			const float delta = angleDiffRad(velDir, wishAng);

			float moveDir = delta < 0.0f ? velDir + pDelta : velDir - pDelta;

			pCmd->forwardmove = cosf(moveDir) * cl_sidespeed->GetFloat();
			pCmd->sidemove = -sinf(moveDir) * cl_sidespeed->GetFloat();
		}
		was_jumping = is_jumping;
		break;
	}
	}
}

void CMisc::InitSpamKV(void* pKV)
{
	char chCommand[30] = "use_action_slot_item_server";
	using HashFunc_t = int(__cdecl*)(const char*, bool);

	static DWORD dwHashFunctionLocation = g_Pattern.Find(_(L"client.dll"), _(L"FF 15 ? ? ? ? 83 C4 08 89 06 8B C6"));
	static auto SymbForString = (HashFunc_t)**(PDWORD*)(dwHashFunctionLocation + 0x2);

	int nAddr = 0;
	while (nAddr < 29)
	{
		switch (nAddr)
		{
		case 0:
			*(PDWORD)((DWORD)pKV + nAddr) = SymbForString(chCommand, true);
			break;
		case 16:
			*(PDWORD)((DWORD)pKV + nAddr) = 0x10000;
			break;
		default:
			*(PDWORD)((DWORD)pKV + nAddr) = 0;
			break;
		}

		nAddr += 4;
	}
}

void CMisc::NoiseMakerSpam(CBaseEntity* pLocal)
{
	if (!Vars::Misc::NoisemakerSpam.m_Var)
		return;

	if (pLocal->GetUsingActionSlot())
		return;

	if (pLocal->GetNextNoiseMakerTime() < g_Interfaces.GlobalVars->curtime)
	{
		if (const auto pKV = Utils::InitKeyValue())
		{
			InitSpamKV(pKV);
			g_Interfaces.Engine->ServerCmdKeyValues(pKV);
		}
	}
}

void CMisc::BypassPure()
{
	if (Vars::Misc::BypassPure.m_Var)
	{
		static DWORD dwAddress = 0x0;

		while (!dwAddress)
			dwAddress = g_Pattern.Find(_(L"engine.dll"), _(L"A1 ? ? ? ? 56 33 F6 85 C0"));

		static DWORD* pPure = nullptr;

		while (!pPure)
		{
			if (reinterpret_cast<DWORD**>(dwAddress + 0x01))
				pPure = *reinterpret_cast<DWORD**>(dwAddress + 0x01);
		}

		if (*pPure)
			*pPure = 0x0;
	}
}

const std::string spam_Fed[] = {
	_("Fedoraware - github.com/M-FeD/Fedoraware"),
	_("Fedoraware - Best free and open-source cheat!"),
	_("Fedoraware - One tip ahead of the game!"),
	_("Fedoraware - Now available @ https://github.com/tf2cheater2013!"),
	_("Fedoraware - Based on SEOwned public source!")
};

const std::string spam_Lbox[] = {
	_("GET GOOD, GET LMAOBOX!"),
	_("LMAOBOX - WAY TO THE TOP"),
	_("WWW.LMAOBOX.NET - BEST FREE TF2 HACK!")
};

const std::string spam_CH[] = {
	_("Cathook - more fun than a ball of yarn!"),
	_("GNU/Linux is the best OS!"),
	_("Visit https://cathook.club for more information!"),
	_("Cathook - Free and Open-Source tf2 cheat!"),
	_("Cathook - ca(n)t stop me meow!")
};

std::string GetSpam(const int nMode)
{
	std::string str;

	switch (nMode)
	{
	case 2: str = spam_Lbox[Utils::RandIntSimple(0, ARRAYSIZE(spam_Lbox) - 1)];
		break;
	case 3: str = spam_CH[Utils::RandIntSimple(0, ARRAYSIZE(spam_CH) - 1)];
		break;
	default: str = spam_Fed[Utils::RandIntSimple(0, ARRAYSIZE(spam_Fed) - 1)];
		break;
	}

	Utils::ReplaceSpecials(str);
	return "say " + str;
}

void CMisc::ChatSpam()
{
	if (Vars::Misc::ChatSpam.m_Var == 0)
		return;

	float flCurTime = g_Interfaces.Engine->Time();
	static float flNextSend = 0.0f;

	if (flCurTime > flNextSend)
	{
		g_Interfaces.Engine->ClientCmd_Unrestricted(GetSpam(Vars::Misc::ChatSpam.m_Var).c_str());
		flNextSend = (flCurTime + 4.0f);
	}
}

void CMisc::AutoRocketJump(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	if (!Vars::Misc::AutoRocketJump.m_Var || !g_GlobalInfo.m_bWeaponCanAttack || !GetAsyncKeyState(VK_RBUTTON))
		return;

	if (g_Interfaces.EngineVGui->IsGameUIVisible() || g_Interfaces.Surface->IsCursorVisible())
		return;
	
	if (pLocal->GetClassNum() != CLASS_SOLDIER || !pLocal->IsOnGround() || pLocal->IsDucking())
		return;

	if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
	{
		if (pWeapon->IsInReload())
		{
			pCmd->buttons |= IN_ATTACK;
			return;
		}
		if (pCmd->buttons & IN_ATTACK)
			pCmd->buttons &= ~IN_ATTACK;

		if (g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheBeggarsBazooka
			|| g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheCowMangler5000
			|| pWeapon->GetSlot() != SLOT_PRIMARY)
			return;

		if (pLocal->GetViewOffset().z < 60.05f)
		{
			pCmd->buttons |= IN_ATTACK | IN_JUMP;

			Vec3 vVelocity = pLocal->GetVelocity();
			Vec3 vAngles = { vVelocity.IsZero() ? 89.0f : 45.0f, Math::VelocityToAngles(vVelocity).y - 180.0f, 0.0f };

			if (g_GlobalInfo.m_nCurItemDefIndex != Soldier_m_TheOriginal && !vVelocity.IsZero())
			{
				Vec3 vForward = {}, vRight = {}, vUp = {};
				Math::AngleVectors(vAngles, &vForward, &vRight, &vUp);
				Math::VectorAngles((vForward * 23.5f) + (vRight * -5.6f) + (vUp * -3.0f), vAngles);
			}

			Math::ClampAngles(vAngles);
			pCmd->viewangles = vAngles;
			g_GlobalInfo.m_bSilentTime = true;
		}

		else pCmd->buttons |= IN_DUCK;
	}
}

bool CanAttack(CBaseEntity* pLocal, const Vec3& pPos)
{
	// TODO: This needs some improvement

	if (const auto pWeapon = pLocal->GetActiveWeapon()) {
		if (!g_GlobalInfo.m_bWeaponCanHeadShot && pLocal->IsScoped()) { return false; }
		if (!pWeapon->CanShoot(pLocal)) { return false; }

		PlayerInfo_t info{};
		for (const auto& target : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
		{
			if (!target->IsAlive()) { continue; }

			if (Vars::Aimbot::Global::IgnoreInvlunerable.m_Var && !target->IsVulnerable()) { continue; }

			if (!g_Interfaces.Engine->GetPlayerInfo(target->GetIndex(), &info)) { continue; }

			if (Vars::Aimbot::Global::IgnoreFriends.m_Var && g_EntityCache.Friends[target->GetIndex()]) { continue; }

			if (g_GlobalInfo.ignoredPlayers.find(info.friendsID) != g_GlobalInfo.ignoredPlayers.end()) { continue; }

			if (Utils::VisPos(pLocal, target, pPos, target->GetHitboxPos(HITBOX_HEAD))) {
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

	if (pLocal->IsAlive() && Vars::Misc::CL_Move::AutoPeekKey.m_Var && GetAsyncKeyState(Vars::Misc::CL_Move::AutoPeekKey.m_Var)) {
		const Vec3 localPos = pLocal->GetAbsOrigin();

		// We just started peeking. Save the return position!
		if (!posPlaced) {
			if (pLocal->IsOnGround()) {
				PeekReturnPos = localPos;
				posPlaced = true;
			}
		} else {
			static Timer particleTimer{ };
			if (particleTimer.Run(700)) {
				Particles::DispatchParticleEffect("ping_circle", PeekReturnPos, { });
			}
		}

		// We need a peek direction (A / D)
		if (!Vars::Misc::CL_Move::AutoPeekFree.m_Var && !hasDirection && pLocal->IsOnGround()) {
			const Vec3 viewAngles = g_Interfaces.Engine->GetViewAngles();
			Vec3 vForward, vRight, vUp, vDirection;
			Math::AngleVectors(viewAngles, &vForward, &vRight, &vUp);

			if (GetAsyncKeyState(VK_A) & 0x8000 || GetAsyncKeyState(VK_W) & 0x8000 || GetAsyncKeyState(VK_D) & 0x8000 || GetAsyncKeyState(VK_S) & 0x8000) {
				CGameTrace trace;
				CTraceFilterWorldAndPropsOnly traceFilter;
				Ray_t traceRay;

				if (GetAsyncKeyState(VK_A) & 0x8000 || GetAsyncKeyState(VK_W) & 0x8000) {
					vDirection = pLocal->GetEyePosition() - vRight * Vars::Misc::CL_Move::AutoPeekDistance.m_Var; // Left
				}
				else if (GetAsyncKeyState(VK_D) & 0x8000 || GetAsyncKeyState(VK_S) & 0x8000) {
					vDirection = pLocal->GetEyePosition() + vRight * Vars::Misc::CL_Move::AutoPeekDistance.m_Var; // Right
				}

				traceRay.Init(pLocal->GetEyePosition(), vDirection);
				g_Interfaces.EngineTrace->TraceRay(traceRay, MASK_SOLID, &traceFilter, &trace);
				peekStart = trace.vStartPos;
				peekVector = trace.vEndPos - trace.vStartPos;
				hasDirection = true;
			}
		}

		// Should we peek?
		if (!Vars::Misc::CL_Move::AutoPeekFree.m_Var && hasDirection) {
			bool targetFound = false;
			for (int i = 10; i < 100; i += 10) {
				const float step = i / 100.f;
				Vec3 currentPos = peekStart + (peekVector * step);
				if (CanAttack(pLocal, currentPos)) {
					Utils::WalkTo(pCmd, pLocal, currentPos);
					targetFound = true;
				}

				if (targetFound) {
					g_Interfaces.DebugOverlay->AddLineOverlayAlpha(PeekReturnPos, currentPos, 68, 189, 50, 100, false, 0.04f);
					break;
				}

				g_Interfaces.DebugOverlay->AddLineOverlayAlpha(PeekReturnPos, currentPos, 235, 59, 90, 100, false, 0.04f);
			}

			if (!targetFound) { isReturning = true; }
		}

		// We've just attacked. Let's return!
		if (g_GlobalInfo.lateUserCmd->buttons & IN_ATTACK || g_GlobalInfo.m_bAttacking) {
			isReturning = true;
		}

		if (isReturning) {
			if (localPos.DistTo(PeekReturnPos) < 7.f) {
				// We reached our destination. Recharge DT if wanted
				if (Vars::Misc::CL_Move::AutoRecharge.m_Var && isReturning && !g_GlobalInfo.m_bShouldShift && !g_GlobalInfo.m_nShifted) {
					g_GlobalInfo.m_bRechargeQueued = true;
				}
				isReturning = false;
				return;
			}

			Utils::WalkTo(pCmd, pLocal, PeekReturnPos);
		}
	} else {
		posPlaced = isReturning = hasDirection = false;
		PeekReturnPos = Vec3();
	}
}

void CMisc::SteamRPC()
{
	if (!Vars::Misc::Steam::EnableRPC.m_Var)
	{
		if (steamCleared == false) //stupid way to return back to normal rpc
		{
			g_SteamInterfaces.Friends015->SetRichPresence("steam_display", "");
			//this will only make it say "Team Fortress 2" until the player leaves/joins some server. its bad but its better than making 1000 checks to recreate the original
			steamCleared = true;
		}
		return;
	}

	steamCleared = false;
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
	if (!g_Interfaces.Engine->IsInGame() && Vars::Misc::Steam::OverrideMenu.m_Var)
		g_SteamInterfaces.Friends015->SetRichPresence("state", "MainMenu");
	else
	{
		g_SteamInterfaces.Friends015->SetRichPresence("state", "PlayingMatchGroup");

		switch (Vars::Misc::Steam::MatchGroup.m_Var)
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
	switch (Vars::Misc::Steam::MapText.m_Var)
	{
	case 0:
		if (Vars::Misc::Steam::CustomText.m_Var.empty())
			g_SteamInterfaces.Friends015->SetRichPresence("currentmap", "Fedoraware");
		else
			g_SteamInterfaces.Friends015->SetRichPresence("currentmap", Vars::Misc::Steam::CustomText.m_Var.c_str());
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
	                                              std::to_string(Vars::Misc::Steam::GroupSize.m_Var).c_str());
}

// Myzarfin added this
void CNotifications::Think()
{
	int x{1}, y{1}, size{20};
	Color_t color;
	float left;

	if (m_vNotificationTexts.size() > (MAX_NOTIFY_SIZE + 1))
		m_vNotificationTexts.erase(m_vNotificationTexts.begin());

	for (size_t i{}; i < m_vNotificationTexts.size(); ++i)
	{
		auto notify = m_vNotificationTexts[i];

		notify->m_time -= g_Interfaces.GlobalVars->absoluteframetime;

		if (notify->m_time <= 0.f)
		{
			m_vNotificationTexts.erase(m_vNotificationTexts.begin() + i);
		}
	}

	if (m_vNotificationTexts.empty())
		return;

	for (size_t i{}; i < m_vNotificationTexts.size(); ++i)
	{
		auto notify = m_vNotificationTexts[i];

		left = notify->m_time;
		color = notify->m_color;

		if (left < .5f)
		{
			float f = left;
			Math::Clamp(f, 0.f, .5f);

			f /= .5f;

			color.a = static_cast<int>(f * 255.f);

			if (i == 0 && f < 0.2f)
				y -= size * (1.f - f / 0.2f);
		}

		else
			color.a = 255;

		const size_t cSize = strlen(notify->m_text.c_str()) + 1;
		auto wc = new wchar_t[cSize];
		mbstowcs(wc, notify->m_text.c_str(), cSize);

		int w, h;

		g_Interfaces.Surface->GetTextSize(FONT_INDICATORS, wc, w, h);

		delete[] wc; // Memory leak

		g_Draw.Line(x, y, x, y + 19, { Colors::NotifOutline.r, Colors::NotifOutline.g, Colors::NotifOutline.b, color.a });
		g_Draw.GradientRectA(x + 1, y, w / 3 + 9, y + 19, 
							{ Colors::NotifBG.r, Colors::NotifBG.g, Colors::NotifBG.b, color.a }, 
							{ Colors::NotifBG.r, Colors::NotifBG.g,              Colors::NotifBG.b, 1
		                     }, true);
		g_Draw.String(FONT_INDICATORS, x + 6, y + 2,
		              {Colors::NotifText.r, Colors::NotifText.g, Colors::NotifText.b, color.a},
		              ALIGN_DEFAULT, notify->m_text.c_str());

		y += size;
	}
}
