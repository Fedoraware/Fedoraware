#include "AntiAim.h"
#include "../Vars.h"
#include "../../Utils/Timer/Timer.hpp"
#include "../LuaEngine/Callbacks/LuaCallbacks.h"

void CAntiAim::FixMovement(CUserCmd* pCmd, const Vec3& vOldAngles, float fOldSideMove, float fOldForwardMove)
{
	//better movement fix roll and pitch above 90 and -90 l0l
	static auto cl_forwardspeed = g_ConVars.FindVar("cl_forwardspeed");
	static auto cl_sidespeed = g_ConVars.FindVar("cl_sidespeed");
	static auto cl_upspeed = g_ConVars.FindVar("cl_upspeed");
	if (!cl_upspeed || !cl_sidespeed || !cl_forwardspeed)
	{
		return;
	}

	const float flMaxForwardSpeed = cl_forwardspeed->GetFloat();
	const float flMaxSideSpeed = cl_sidespeed->GetFloat();
	const float flMaxUpSpeed = cl_upspeed->GetFloat();
	Vector vecForward = {}, vecRight = {}, vecUp = {};
	Math::AngleVectors(vOldAngles, &vecForward, &vecRight, &vecUp);
	vecForward.z = vecRight.z = vecUp.x = vecUp.y = 0.f;
	vecForward.NormalizeInPlace();
	vecRight.NormalizeInPlace();
	vecUp.NormalizeInPlace();
	Vector vecOldForward = {}, vecOldRight = {}, vecOldUp = {};
	Math::AngleVectors(pCmd->viewangles, &vecOldForward, &vecOldRight, &vecOldUp);
	vecOldForward.z = vecOldRight.z = vecOldUp.x = vecOldUp.y = 0.f; // these can all have 3 vectors can they not?
	vecOldForward.NormalizeInPlace();
	vecOldRight.NormalizeInPlace();
	vecOldUp.NormalizeInPlace();
	const float flPitchForward = vecForward.x * pCmd->forwardmove; //	chunky
	const float flYawForward = vecForward.y * pCmd->forwardmove; //	chunky
	const float flPitchSide = vecRight.x * pCmd->sidemove; //	chunky
	const float flYawSide = vecRight.y * pCmd->sidemove; //	chunky
	const float flRollUp = vecUp.z * pCmd->sidemove; //	chunky
	const float x = vecOldForward.x * flPitchSide + vecOldForward.y * flYawSide + vecOldForward.x * flPitchForward + vecOldForward.y * flYawForward + vecOldForward.z * flRollUp;
	const float y = vecOldRight.x * flPitchSide + vecOldRight.y * flYawSide + vecOldRight.x * flPitchForward + vecOldRight.y * flYawForward + vecOldRight.z * flRollUp;
	const float z = vecOldUp.x * flYawSide + vecOldUp.y * flPitchSide + vecOldUp.x * flYawForward + vecOldUp.y * flPitchForward + vecOldUp.z * flRollUp;
	pCmd->forwardmove = std::clamp(x, -flMaxForwardSpeed, flMaxForwardSpeed);
	pCmd->sidemove = std::clamp(y, -flMaxSideSpeed, flMaxSideSpeed);
	pCmd->upmove = std::clamp(z, -flMaxUpSpeed, flMaxUpSpeed); //	not a good idea
}

bool CAntiAim::ShouldAntiAim(CBaseEntity* pLocal){
	if (!pLocal->IsAlive() || pLocal->IsTaunting() || pLocal->IsInBumperKart() || pLocal->IsAGhost()) 
	{ return false; }

	if (pLocal->GetMoveType() == MOVETYPE_NOCLIP || pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetMoveType() == MOVETYPE_OBSERVER)
	{ return false; }

	if (pLocal->IsCharging())
	{ return false; }

	if (G::IsAttacking) { return false; }

	return true;
}

float CAntiAim::EdgeDistance(float edgeRayYaw)
{
	// Main ray tracing area
	CGameTrace trace;
	Ray_t ray;
	Vector forward;
	const float sy = sinf(DEG2RAD(edgeRayYaw)); // yaw
	const float cy = cosf(DEG2RAD(edgeRayYaw));
	constexpr float sp = 0.f; // pitch: sinf(DEG2RAD(0))
	constexpr float cp = 1.f; // cosf(DEG2RAD(0))
	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
	forward = forward * 300.0f + g_EntityCache.GetLocal()->GetEyePosition();
	ray.Init(g_EntityCache.GetLocal()->GetEyePosition(), forward);
	// trace::g_pFilterNoPlayer to only focus on the enviroment
	CTraceFilterWorldAndPropsOnly Filter = {};
	I::EngineTrace->TraceRay(ray, 0x4200400B, &Filter, &trace);

	const float edgeDistance = (trace.vStartPos - trace.vEndPos).Length2D();
	return edgeDistance;
}

bool CAntiAim::FindEdge(float edgeOrigYaw)
{
	// distance two vectors and report their combined distances
	float edgeLeftDist = EdgeDistance(edgeOrigYaw - 21);
	edgeLeftDist = edgeLeftDist + EdgeDistance(edgeOrigYaw - 27);
	float edgeRightDist = EdgeDistance(edgeOrigYaw + 21);
	edgeRightDist = edgeRightDist + EdgeDistance(edgeOrigYaw + 27);

	// If the distance is too far, then set the distance to max so the angle
	// isnt used
	if (edgeLeftDist >= 260) { edgeLeftDist = 999999999.f; }
	if (edgeRightDist >= 260) { edgeRightDist = 999999999.f; }

	// If none of the vectors found a wall, then dont edge
	if (Utils::CompareFloat(edgeLeftDist, edgeRightDist)) { return false; }

	// Depending on the edge, choose a direction to face
	bEdge = edgeRightDist < edgeLeftDist;
	if (G::RealViewAngles.x == 89.f) // Check for real up
	{
		bEdge = !bEdge;
	}

	return true;
}

bool CAntiAim::IsOverlapping(float epsilon = 45.f)
{
	if (!Vars::AntiHack::AntiAim::AntiOverlap.Value) { return false; }
	return std::abs(YawIndex(Vars::AntiHack::AntiAim::YawReal.Value) - YawIndex(Vars::AntiHack::AntiAim::YawFake.Value)) < epsilon;
}

float CAntiAim::CalculateCustomRealPitch(float WishPitch, bool FakeDown)
{
	return FakeDown ? 720 + WishPitch : -720 + WishPitch;
}

float CAntiAim::YawIndex(int iIndex){
	float &flLastYaw = bSendState ? flLastRealOffset : flLastFakeOffset;
	switch (iIndex){
	case 1: { return 0.f; }
	case 2: { return 90.f; }
	case 3: { return -90.f; }
	case 4: { return 180.f; }
	case 5: { flLastYaw = tAATimer.Run(Vars::AntiHack::AntiAim::RandInterval.Value * 10) ? Utils::RandFloatRange(-180.0f, 180.0f) : flLastYaw; return flLastYaw; }
	case 6: { return flLastYaw += (bSendState ? 1.f : -1.f) * Vars::AntiHack::AntiAim::SpinSpeed.Value; }
	case 7: { return (bEdge ? 1.f : -1.f) * (bSendState ? 90.f : -90.f); }
	case 8: { return flLastYaw = bWasHit ? Utils::RandFloatRange(-180.0f, 180.0f) : flLastYaw; }
	case 9: { return bSendState ? Vars::AntiHack::AntiAim::CustomRealYaw.Value : Vars::AntiHack::AntiAim::CustomFakeYaw.Value; }
	default: { return 0.f; }
	}
}

void CAntiAim::SetupPitch(int iMode, CUserCmd* pCmd){
	switch (iMode){
	case 1: { pCmd->viewangles.x = 0.f; G::FakeViewAngles.x = 0.f; return; }
	case 2: { pCmd->viewangles.x = -89.f; G::FakeViewAngles.x = -89.f; return; }
	case 3: { pCmd->viewangles.x = 89.f; G::FakeViewAngles.x = 89.f; return; }
	case 4: { pCmd->viewangles.x = -271.f; G::FakeViewAngles.x = -89.f; return; }
	case 5: { pCmd->viewangles.x = 271.f; G::FakeViewAngles.x = 89.f; return; }
	case 6: { flLastPitch = tAATimer.Run(Vars::AntiHack::AntiAim::RandInterval.Value * 10) ? Utils::RandFloatRange(-89.0f, 89.0f) : flLastPitch; pCmd->viewangles.x = flLastPitch; G::FakeViewAngles.x = flLastPitch; return; }	//	problem?
	case 7: { pCmd->viewangles.x = -45.f; G::FakeViewAngles.x = -45.f; return; }
	case 8: { pCmd->viewangles.x = bPitchFlip ? 89.f : -89.f; G::FakeViewAngles.x = bPitchFlip ? 89.f : -89.f; bPitchFlip = !bPitchFlip; return; }
	case 9:
	case 10: { pCmd->viewangles.x = CalculateCustomRealPitch(Vars::AntiHack::AntiAim::CustomRealPitch.Value, iMode == 10); G::FakeViewAngles.x = iMode == 10 ? 89.f : -89.f; return; }
	}
}

float CAntiAim::GetBaseYaw(int iMode, CBaseEntity* pLocal, CUserCmd* pCmd){
	//	0 offset, 1 at player, 2 at player + offset
	const float flBaseOffset = Vars::AntiHack::AntiAim::BaseYawOffset.Value;
	switch (iMode){
	case 0: { return pCmd->viewangles.y + flBaseOffset; }
	case 1: { 
		float flSmallestAngleTo = 0.f; float flSmallestFovTo = 360.f;
		for (CBaseEntity* pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES)){
			if (!pEnemy || !pEnemy->IsAlive() || pEnemy->GetDormant()) { continue; }
			const Vec3 vAngleTo = Math::CalcAngle(pLocal->GetAbsOrigin(), pEnemy->GetAbsOrigin());
			const float flFOVTo = Math::CalcFov(I::EngineClient->GetViewAngles(), vAngleTo);
			
			if (flFOVTo < flSmallestFovTo) { flSmallestAngleTo = vAngleTo.y; flSmallestFovTo = flFOVTo; }
		}
		return flSmallestAngleTo;
	}
	case 2: {
		float flSmallestAngleTo = 0.f; float flSmallestFovTo = 360.f;
		for (CBaseEntity* pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES)){
			if (!pEnemy || !pEnemy->IsAlive() || pEnemy->GetDormant()) { continue; }
			const Vec3 vAngleTo = Math::CalcAngle(pLocal->GetAbsOrigin(), pEnemy->GetAbsOrigin());
			const float flFOVTo = Math::CalcFov(I::EngineClient->GetViewAngles(), vAngleTo);
			
			if (flFOVTo < flSmallestFovTo) { flSmallestAngleTo = vAngleTo.y; flSmallestFovTo = flFOVTo; }
		}
		return flSmallestAngleTo + flBaseOffset;
	}
	}
}

void CAntiAim::Run(CUserCmd* pCmd, bool* pSendPacket)
{
	G::AAActive = false;
	G::RealViewAngles = G::ViewAngles;
	G::FakeViewAngles = G::ViewAngles;
	G::AntiAim = {false, false};

	// AA toggle key
	static KeyHelper aaKey{&Vars::AntiHack::AntiAim::ToggleKey.Value};
	if (aaKey.Pressed())
	{
		Vars::AntiHack::AntiAim::Active.Value = !Vars::AntiHack::AntiAim::Active.Value;
	}

	if (!Vars::AntiHack::AntiAim::Active.Value || G::ForceSendPacket || G::AvoidingBackstab || G::ShouldShift) { return; }

	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		if (!ShouldAntiAim(pLocal)) { return; }

		const bool bPitchSet = Vars::AntiHack::AntiAim::Pitch.Value;
		const bool bYawSet = bSendState ? Vars::AntiHack::AntiAim::YawReal.Value : Vars::AntiHack::AntiAim::YawFake.Value;

		const Vec3 vOldAngles = pCmd->viewangles;
		const float fOldSideMove = pCmd->sidemove;
		const float fOldForwardMove = pCmd->forwardmove;

		// Get Base Yaw

		// Pitch
		SetupPitch(Vars::AntiHack::AntiAim::Pitch.Value, pCmd);

		//	get edges if needed
		if (Vars::AntiHack::AntiAim::YawReal.Value == 7 || Vars::AntiHack::AntiAim::YawFake.Value == 7) { FindEdge(flBaseYaw); }

		// Yaw
		if (bSendState)
		{
			const float flRealYaw = flBaseYaw + YawIndex(Vars::AntiHack::AntiAim::YawReal.Value);
			pCmd->viewangles.y = flRealYaw;

			// Lua callback
			F::LuaCallbacks.OnAntiAim(pCmd, pSendPacket, true);

			// Check if our real angle is overlapping with the fake angle
			if (Vars::AntiHack::AntiAim::YawFake.Value && IsOverlapping())
			{
				const float flFixOffset = Vars::AntiHack::AntiAim::SpinSpeed.Value ? 45.f : -45.f;
				pCmd->viewangles.y += flFixOffset;
				flLastRealOffset += flFixOffset;
			}

			G::RealViewAngles.y = pCmd->viewangles.y;
			G::AntiAim.first = true;
		}
		else
		{
			const float flFakeYaw = flBaseYaw + YawIndex(Vars::AntiHack::AntiAim::YawReal.Value);
			pCmd->viewangles.y = flFakeYaw;

			// Lua callback
			F::LuaCallbacks.OnAntiAim(pCmd, pSendPacket, false);

			G::FakeViewAngles = pCmd->viewangles;
			G::AntiAim.second = true;
		}

		if (bYawSet) { *pSendPacket = bSendState = !bSendState; }
		if (Vars::AntiHack::AntiAim::YawFake.Value == 0)
		{
			*pSendPacket = bSendState = true;
		}
		G::AAActive = bPitchSet || bYawSet;

		FixMovement(pCmd, vOldAngles, fOldSideMove, fOldForwardMove);
	}
}

void CAntiAim::Event(CGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (uNameHash == FNV1A::HashConst("player_hurt"))
	{
		if (const auto pEntity = I::ClientEntityList->GetClientEntity(
			I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"))))
		{
			const auto nAttacker = pEvent->GetInt("attacker");
			const auto& pLocal = g_EntityCache.GetLocal();
			if (!pLocal) { return; }
			if (pEntity != pLocal) { return; }

			PlayerInfo_t pi{};
			I::EngineClient->GetPlayerInfo(I::EngineClient->GetLocalPlayer(), &pi);
			if (nAttacker == pi.userID) { return; }

			bWasHit = true;
		}
	}
}
