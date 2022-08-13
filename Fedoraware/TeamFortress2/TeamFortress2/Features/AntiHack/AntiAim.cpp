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
	if (edgeRightDist < edgeLeftDist)
	{
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

float CAntiAim::CalculateCustomRealPitch(float WishPitch, bool FakeDown)
{
	return FakeDown ? 720 + WishPitch : -720 + WishPitch;
}

float CAntiAim::GetAngle(int nIndex)
{
	float& lastAngleRef = bPacketFlip ? lastRealAngle : lastFakeAngle;
	float retnAngle = 0.f;
	switch (nIndex)
	{
	case 1:
		{
			retnAngle = 0.0f;
			break;
		}
	case 2:
		{
			retnAngle = 90.0f;
			break;
		}
	case 3:
		{
			retnAngle = -90.0f;
			break;
		}
	case 4:
		{
			retnAngle = 180.0f;
			break;
		}
	case 5:
		{
			static Timer updateTimer{};
			if (updateTimer.Run(Vars::AntiHack::AntiAim::RandInterval.Value * 10))
			{
				lastAngleRef = Utils::RandFloatRange(-180.0f, 180.0f);
			}
			retnAngle = lastAngleRef;
			break;
		}
	case 6: //	spin
		{
			lastAngleRef += (bPacketFlip ? +1 : -1) * Vars::AntiHack::AntiAim::SpinSpeed.Value;
			if (lastAngleRef > 180.f) { lastAngleRef -= 360.f; }
			if (lastAngleRef < -180.f) { lastAngleRef += 360.f; }
			retnAngle = lastAngleRef;
			break;
		}
	case 7:
		{
			if (edgeToEdgeOn == 1) { retnAngle = bPacketFlip ? 90.f : -90.f; }
			else if (edgeToEdgeOn == 2) { retnAngle = bPacketFlip ? -90.0f : 90.f; }
			break;
		}
	case 8:
		{
			if (wasHit)
			{
				lastAngleRef = Utils::RandFloatRange(-180.0f, 180.0f);
				wasHit = false;
			}
			retnAngle = lastAngleRef;
			break;
		}
	case 9:
		{
			retnAngle = bPacketFlip ? Vars::AntiHack::AntiAim::CustomRealYaw.Value : Vars::AntiHack::AntiAim::CustomFakeYaw.Value;
			break;
		}
	}
	return retnAngle;
}

std::pair<float, float> CAntiAim::GetAnglePairPitch(int nIndex)
{
	std::pair retnAngles = { 0.f, 0.f };
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		const float speed = pLocal->GetVelocity().Length2D();
		const auto& pWeapon = g_EntityCache.GetWeapon();
		
		switch (nIndex)
		{
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
		case 4: //	fakeup
			{
				retnAngles.first = -271.0f;
				retnAngles.second = 89.0f;
				G::FakeViewAngles.x = -89.f;
				break;
			}
		case 5: //	fakedown
			{
				retnAngles.first = 271.0f;
				retnAngles.second = -89.0f;
				G::FakeViewAngles.x = 89.f;
				break;
			}
		case 6:
			{
				static float currentAngle = Utils::RandFloatRange(-89.0f, 89.0f);
				static Timer updateTimer{};
				if (updateTimer.Run(Vars::AntiHack::AntiAim::RandInterval.Value * 10))
				{
					currentAngle = Utils::RandFloatRange(-89.0f, 89.0f);
				}
				retnAngles.first = currentAngle;
				retnAngles.second = currentAngle;
				G::FakeViewAngles.x = currentAngle;
				break;
			}
		case 7: //Half Up
			{
				retnAngles.first = -45.0f;
				retnAngles.second = -45.0f;
				G::FakeViewAngles.x = -45.0f;
				break;
			}
		case 8:
			{
				//	jitter
				static bool flip = false;
				retnAngles.first = flip ? 89.f : -89.f;
				retnAngles.second = retnAngles.first;
				G::FakeViewAngles.x = retnAngles.first;
				if (bPacketFlip)
				{
					//	dumb hack
					flip = !flip;
				}
				break;
			}
		case 9:
		case 10:
			{
				//	fake up/down, real custom
				const bool FakeDown = nIndex == 10;
				retnAngles.first = CalculateCustomRealPitch(Vars::AntiHack::AntiAim::CustomRealPitch.Value, FakeDown);
				retnAngles.second = retnAngles.first;
				G::FakeViewAngles.x = FakeDown ? 89.f : -89.f;
				break;
			}
		case 11:
			{
				const int nClassNum = pLocal->GetClassNum();
				const int nWeaponSlot = pWeapon->GetSlot();
				retnAngles.first = 89.f;
				retnAngles.second = 89.f;
				switch (nClassNum)
				{
				case CLASS_HEAVY:
				case CLASS_DEMOMAN:
				case CLASS_SOLDIER:
				case CLASS_ENGINEER:
				case CLASS_MEDIC:
				case CLASS_SNIPER:
					{
						if (nWeaponSlot == SLOT_PRIMARY)
						{
							retnAngles.first = -89.f;
							retnAngles.second = -89.f;
						}
						break;
					}
				default: break;
				}

				if (nClassNum == CLASS_MEDIC && speed < 2.0f)
				{
					retnAngles.first = -89.f;
					retnAngles.second = -89.f;
				}
				break;
			}
		}
	}

	return retnAngles;
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

	if (!Vars::AntiHack::AntiAim::Active.Value || G::ForceSendPacket || G::AvoidingBackstab) { return; }

	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		if (!pLocal->IsAlive()
			|| pLocal->IsTaunting()
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

		if (pLocal->IsCharging())
		{
			return;
		}

		if (G::IsAttacking) { return; }

		const bool bPitchSet = Vars::AntiHack::AntiAim::Pitch.Value;
		const bool bYawSet = bPacketFlip ? Vars::AntiHack::AntiAim::YawReal.Value : Vars::AntiHack::AntiAim::YawFake.Value;

		const Vec3 vOldAngles = pCmd->viewangles;
		const float fOldSideMove = pCmd->sidemove;
		const float fOldForwardMove = pCmd->forwardmove;

		// Pitch
		if (Vars::AntiHack::AntiAim::Pitch.Value)
		{
			const std::pair<float, float> angPair = GetAnglePairPitch(Vars::AntiHack::AntiAim::Pitch.Value);
			pCmd->viewangles.x = angPair.first;
			G::RealViewAngles.x = angPair.second;
		}

		if (Vars::AntiHack::AntiAim::YawReal.Value == 7 || Vars::AntiHack::AntiAim::YawFake.Value == 7)
		{
			FindEdge(pCmd->viewangles.y);
		}

		if (bPacketFlip)
		{
			//	real
			const float angOffset = GetAngle(Vars::AntiHack::AntiAim::YawReal.Value);
			pCmd->viewangles.y += angOffset;

			// Lua callback
			F::LuaCallbacks.OnAntiAim(pCmd, pSendPacket, bPacketFlip);

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
			G::AntiAim.first = true;
		}
		else
		{
			//	fake
			const float angOffset = GetAngle(Vars::AntiHack::AntiAim::YawFake.Value);
			pCmd->viewangles.y += angOffset;

			// Lua callback
			F::LuaCallbacks.OnAntiAim(pCmd, pSendPacket, bPacketFlip);

			G::FakeViewAngles = pCmd->viewangles;
			G::AntiAim.second = true;
		}

		if (bYawSet) { *pSendPacket = bPacketFlip = !bPacketFlip; }
		if (Vars::AntiHack::AntiAim::YawFake.Value == 0)
		{
			*pSendPacket = bPacketFlip = true;
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

			wasHit = true;
		}
	}
}
