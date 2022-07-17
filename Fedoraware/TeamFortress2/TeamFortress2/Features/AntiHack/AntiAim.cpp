#include "AntiAim.h"
#include "../Vars.h"
#include "../../Utils/Timer/Timer.hpp"

int edgeToEdgeOn = 0;
float lastRealAngle = -90.f;
float lastFakeAngle = 90.f;
bool wasHit = false;

void CAntiAim::FixMovement(CUserCmd* pCmd, const Vec3& vOldAngles, float fOldSideMove, float fOldForwardMove) {
	//better movement fix roll and pitch above 90 and -90 l0l
		static auto cl_forwardspeed = g_ConVars.FindVar("cl_forwardspeed");
		static auto cl_sidespeed = g_ConVars.FindVar("cl_sidespeed");
		static auto cl_upspeed = g_ConVars.FindVar("cl_upspeed");
		if (!cl_upspeed || !cl_sidespeed || !cl_forwardspeed) {
			return;
		}

		const float flMaxForwardSpeed = cl_forwardspeed->GetFloat();
		const float flMaxSideSpeed = cl_sidespeed->GetFloat();
		const float flMaxUpSpeed = cl_upspeed->GetFloat();
		Vector vecForward = { }, vecRight = { }, vecUp = { };
		Math::AngleVectors(vOldAngles, &vecForward, &vecRight, &vecUp);
		vecForward.z = vecRight.z = vecUp.x = vecUp.y = 0.f;
		vecForward.NormalizeInPlace();
		vecRight.NormalizeInPlace();
		vecUp.NormalizeInPlace();
		Vector vecOldForward = { }, vecOldRight = { }, vecOldUp = { };
		Math::AngleVectors(pCmd->viewangles, &vecOldForward, &vecOldRight, &vecOldUp);
		vecOldForward.z = vecOldRight.z = vecOldUp.x = vecOldUp.y = 0.f;	// these can all have 3 vectors can they not?
		vecOldForward.NormalizeInPlace();
		vecOldRight.NormalizeInPlace();
		vecOldUp.NormalizeInPlace();
		const float flPitchForward = vecForward.x * pCmd->forwardmove;	//	chunky
		const float flYawForward = vecForward.y * pCmd->forwardmove;	//	chunky
		const float flPitchSide = vecRight.x * pCmd->sidemove;			//	chunky
		const float flYawSide = vecRight.y * pCmd->sidemove;			//	chunky
		const float flRollUp = vecUp.z * pCmd->sidemove;				//	chunky
		const float x = vecOldForward.x * flPitchSide + vecOldForward.y * flYawSide + vecOldForward.x * flPitchForward + vecOldForward.y * flYawForward + vecOldForward.z * flRollUp;
		const float y = vecOldRight.x * flPitchSide + vecOldRight.y * flYawSide + vecOldRight.x * flPitchForward + vecOldRight.y * flYawForward + vecOldRight.z * flRollUp;
		const float z = vecOldUp.x * flYawSide + vecOldUp.y * flPitchSide + vecOldUp.x * flYawForward + vecOldUp.y * flPitchForward + vecOldUp.z * flRollUp;
		pCmd->forwardmove = std::clamp(x, -flMaxForwardSpeed, flMaxForwardSpeed);
		pCmd->sidemove = std::clamp(y, -flMaxSideSpeed, flMaxSideSpeed);
		pCmd->upmove = std::clamp(z, -flMaxUpSpeed, flMaxUpSpeed);	//	not a good idea
}

float CAntiAim::EdgeDistance(float edgeRayYaw) {
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

bool CAntiAim::FindEdge(float edgeOrigYaw) {
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
	if (edgeRightDist < edgeLeftDist) {
		edgeToEdgeOn = 1;
		if (Vars::AntiHack::AntiAim::Pitch.Value == 2 ||
			Vars::AntiHack::AntiAim::Pitch.Value == 4 ||
			G::RealViewAngles.x < 10.f) // Check for real up
		{
			edgeToEdgeOn = 2;
		}
		return true;
	}

	edgeToEdgeOn = 2;
	if (Vars::AntiHack::AntiAim::Pitch.Value == 2 ||
		Vars::AntiHack::AntiAim::Pitch.Value == 4 ||
		G::RealViewAngles.x < 10.f) // Check for real up
	{
		edgeToEdgeOn = 1;
	}

	return true;
}

bool CAntiAim::IsOverlapping(float a, float b, float epsilon = 45.f)
{
	if (!Vars::AntiHack::AntiAim::AntiOverlap.Value) { return false; }
	return std::abs(a - b) < epsilon;
}

std::pair<float, float> CAntiAim::GetAnglePairPitch(int nIndex) {
	std::pair<float, float> retnAngles = { 0.f, 0.f };
	switch (Vars::AntiHack::AntiAim::Pitch.Value) {
	case 2:
	{
		retnAngles.first = -89.0f;
		retnAngles.second = -89.0f;
		break;
	}
	case 3:
	{
		retnAngles.first = 89.0f;
		retnAngles.second = 89.0f;
		break;
	}
	case 4:
	{
		retnAngles.first = -271.0f;
		retnAngles.second = 89.0f;
		break;
	}
	case 5:
	{
		retnAngles.first = 271.0f;
		retnAngles.second = -89.0f;
		break;
	}
	case 6:
	{
		static float currentAngle = Utils::RandFloatRange(-89.0f, 89.0f);
		static Timer updateTimer{ };
		if (updateTimer.Run(Vars::AntiHack::AntiAim::RandInterval.Value * 10))
		{
			currentAngle = Utils::RandFloatRange(-89.0f, 89.0f);
		}
		retnAngles.first = currentAngle;
		retnAngles.second = currentAngle;
		break;
	}
	case 7://Half Up
	{
		retnAngles.first = -45.0f;
		retnAngles.second = -45.0f;
		break;
	}
	}
	return retnAngles;
}

void CAntiAim::Run(CUserCmd* pCmd, bool* pSendPacket) {
	G::AAActive = false;
	G::RealViewAngles = G::ViewAngles;
	G::FakeViewAngles = G::ViewAngles;

	// AA toggle key
	static KeyHelper aaKey{ &Vars::AntiHack::AntiAim::ToggleKey.Value };
	if (aaKey.Pressed())
	{
		Vars::AntiHack::AntiAim::Active.Value = !Vars::AntiHack::AntiAim::Active.Value;
	}

	if (!Vars::AntiHack::AntiAim::Active.Value || G::ForceSendPacket || G::AvoidingBackstab) { return; }

	if (const auto& pLocal = g_EntityCache.GetLocal()) {
		if (!pLocal->IsAlive()
			|| pLocal->IsTaunting()
			|| pLocal->IsInBumperKart()
			|| pLocal->IsAGhost()) {
			return;
		}

		if (pLocal->GetMoveType() == MOVETYPE_NOCLIP
			|| pLocal->GetMoveType() == MOVETYPE_LADDER
			|| pLocal->GetMoveType() == MOVETYPE_OBSERVER)
		{
			return;
		}

		if (G::IsAttacking) { return; }

		static bool bSendReal = true;
		bool bPitchSet = false;
		bool bYawSet = true;

		const Vec3 vOldAngles = pCmd->viewangles;
		const float fOldSideMove = pCmd->sidemove;
		const float fOldForwardMove = pCmd->forwardmove;

		// Pitch
		if (Vars::AntiHack::AntiAim::Pitch.Value) {
			const std::pair<float, float> angPair = GetAnglePairPitch(Vars::AntiHack::AntiAim::Pitch.Value);
			pCmd->viewangles.x = angPair.first;
			G::RealViewAngles.x = angPair.second;
			bPitchSet = true;
		}

		if (Vars::AntiHack::AntiAim::YawReal.Value == 7 || Vars::AntiHack::AntiAim::YawFake.Value == 7) {
			FindEdge(pCmd->viewangles.y);
		}

		// Yaw (Real)
		if (bSendReal) {
			switch (Vars::AntiHack::AntiAim::YawReal.Value) {
			case 1:
			{
				pCmd->viewangles.y += 0.0f;
				break;
			}
			case 2:
			{
				pCmd->viewangles.y += 90.0f;
				break;
			}
			case 3:
			{
				pCmd->viewangles.y -= 90.0f;
				break;
			}
			case 4:
			{
				pCmd->viewangles.y += 180.0f;
				break;
			}
			case 5:
			{
				static Timer updateTimer{ };
				if (updateTimer.Run(Vars::AntiHack::AntiAim::RandInterval.Value * 10))
				{
					lastRealAngle = Utils::RandFloatRange(-180.0f, 180.0f);
				}
				pCmd->viewangles.y = lastRealAngle;
				break;
			}
			case 6:
			{
				lastRealAngle += Vars::AntiHack::AntiAim::SpinSpeed.Value;
				if (lastRealAngle > 180.f) { lastRealAngle -= 360.f; }
				if (lastRealAngle < -180.f) { lastRealAngle += 360.f; }
				pCmd->viewangles.y = lastRealAngle;
				break;
			}
			case 7:
			{
				if (edgeToEdgeOn == 1) { pCmd->viewangles.y += 90; }
				else if (edgeToEdgeOn == 2) { pCmd->viewangles.y -= 90.0f; }
				break;
			}
			case 8:
			{
				if (wasHit)
				{
					lastRealAngle = Utils::RandFloatRange(-180.0f, 180.0f);
					wasHit = false;
				}
				pCmd->viewangles.y = lastRealAngle;
				break;
			}
			default:
			{
				bYawSet = false;
				break;
			}
			}

			// Check if our real angle is overlapping with the fake angle
			if (Vars::AntiHack::AntiAim::YawFake.Value != 0 && IsOverlapping(pCmd->viewangles.y, G::FakeViewAngles.y))
			{
				if (Vars::AntiHack::AntiAim::SpinSpeed.Value > 0)
				{
					pCmd->viewangles.y += 50.f;
					lastRealAngle += 50.f;
				}
				else
				{
					pCmd->viewangles.y -= 50.f;
					lastRealAngle -= 50.f;
				}
			}

			G::RealViewAngles.y = pCmd->viewangles.y;
		}

		// Yaw ( Fake)
		else {
			switch (Vars::AntiHack::AntiAim::YawFake.Value) {
			case 1: //fake forward for legit aa
			{
				pCmd->viewangles.y += 0.0f;
				break;
			}
			case 2:
			{
				pCmd->viewangles.y += 90.0f;
				break;
			}
			case 3:
			{
				pCmd->viewangles.y -= 90.0f;
				break;
			}
			case 4:
			{
				pCmd->viewangles.y += 180.0f;
				break;
			}
			case 5:
			{
				static Timer updateTimer{ };
				if (updateTimer.Run(Vars::AntiHack::AntiAim::RandInterval.Value * 10))
				{
					lastFakeAngle = Utils::RandFloatRange(-180.0f, 180.0f);
				}
				pCmd->viewangles.y = lastFakeAngle;
				break;
			}
			case 6:
			{
				lastFakeAngle += Vars::AntiHack::AntiAim::SpinSpeed.Value;
				if (lastFakeAngle > 180.f) { lastFakeAngle -= 360.f; }
				if (lastFakeAngle < -180.f) { lastFakeAngle += 360.f; }
				pCmd->viewangles.y = lastFakeAngle;
				break;
			}
			case 7:
			{
				if (edgeToEdgeOn == 1) { pCmd->viewangles.y -= 90; }
				else if (edgeToEdgeOn == 2) { pCmd->viewangles.y += 90.0f; }
				break;
			}
			case 8:
			{
				if (wasHit)
				{
					lastFakeAngle = Utils::RandFloatRange(-180.0f, 180.0f);
					wasHit = false;
				}
				pCmd->viewangles.y = lastFakeAngle;
				break;
			}
			default:
			{
				bYawSet = false;
				break;
			}
			}

			G::FakeViewAngles = pCmd->viewangles;
		}

		if (bYawSet) { *pSendPacket = bSendReal = !bSendReal; }
		if (Vars::AntiHack::AntiAim::YawFake.Value == 0)
		{
			*pSendPacket = bSendReal = true;
		}
		G::AAActive = bPitchSet || bYawSet;

		FixMovement(pCmd, vOldAngles, fOldSideMove, fOldForwardMove);
	}
}

void CAntiAim::Event(CGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (uNameHash == FNV1A::HashConst("player_hurt"))
	{
		if (const auto pEntity = I::EntityList->GetClientEntity(
			I::Engine->GetPlayerForUserID(pEvent->GetInt("userid"))))
		{
			const auto nAttacker = pEvent->GetInt("attacker");
			const auto& pLocal = g_EntityCache.GetLocal();
			if (!pLocal) { return; }
			if (pEntity != pLocal) { return; }

			PlayerInfo_t pi{};
			I::Engine->GetPlayerInfo(I::Engine->GetLocalPlayer(), &pi);
			if (nAttacker == pi.userID) { return; }

			wasHit = true;
		}
	}
}
