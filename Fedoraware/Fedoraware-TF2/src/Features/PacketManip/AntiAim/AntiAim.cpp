#include "AntiAim.h"

inline void CAntiAim::FakeShotAngles(CUserCmd* pCmd) {
	if (!F::AimbotGlobal.IsAttacking() || G::CurWeaponType != EWeaponType::HITSCAN || !Vars::AntiHack::AntiAim::InvalidShootPitch.Value) { return; }

	G::UpdateView = false;
	pCmd->viewangles.x = CalculateCustomRealPitch(-pCmd->viewangles.x, false) + 180;
	pCmd->viewangles.y += 180;
}

inline void CAntiAim::Keybinds() {
	// AA toggle key
	static KeyHelper kAA{ &Vars::AntiHack::AntiAim::ToggleKey.Value };
	Vars::AntiHack::AntiAim::Active.Value = (kAA.Pressed() ? !Vars::AntiHack::AntiAim::Active.Value : Vars::AntiHack::AntiAim::Active.Value);

	// AA invert key
	static KeyHelper kInvert{ &Vars::AntiHack::AntiAim::InvertKey.Value };
	bInvert = (kInvert.Pressed() ? !bInvert : bInvert);
}

inline void CAntiAim::SpinAngles() {
	flFakeOffset -= Vars::AntiHack::AntiAim::SpinSpeed.Value;
	flRealOffset += Vars::AntiHack::AntiAim::SpinSpeed.Value;
	flFakeOffset = (int)flFakeOffset % 360;
	flRealOffset = (int)flRealOffset % 360;
}

float CAntiAim::EdgeDistance(float flEdgeRayYaw, CBaseEntity* pEntity) {
	// Main ray tracing area
	CGameTrace trace;
	Ray_t ray;
	Vector forward;
	const float sy = sinf(DEG2RAD(flEdgeRayYaw)); // yaw
	const float cy = cosf(DEG2RAD(flEdgeRayYaw));
	constexpr float sp = 0.f; // pitch: sinf(DEG2RAD(0))
	constexpr float cp = 1.f; // cosf(DEG2RAD(0))
	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
	forward = forward * 300.0f + pEntity->GetEyePosition();
	ray.Init(pEntity->GetEyePosition(), forward);
	// trace::g_pFilterNoPlayer to only focus on the enviroment
	CTraceFilterWorldAndPropsOnly Filter = {};
	I::EngineTrace->TraceRay(ray, 0x4200400B, &Filter, &trace);

	const float edgeDistance = (trace.vStartPos - trace.vEndPos).Length2D();
	return edgeDistance;
}

bool CAntiAim::GetEdge(const float flEdgeOrigYaw = I::EngineClient->GetViewAngles().y, CBaseEntity* pEntity = g_EntityCache.GetLocal()) {
	// distance two vectors and report their combined distances
	float flEdgeLeftDist = EdgeDistance(flEdgeOrigYaw - 21, pEntity) + EdgeDistance(flEdgeOrigYaw - 27, pEntity);
	float flEdgeRightDist = EdgeDistance(flEdgeOrigYaw + 21, pEntity) + EdgeDistance(flEdgeOrigYaw + 27, pEntity);

	// If the distance is too far, then set the distance to max so the angle
	// isnt used
	if (flEdgeLeftDist >= 260) { flEdgeLeftDist = 999999999.f; }
	if (flEdgeRightDist >= 260) { flEdgeRightDist = 999999999.f; }

	// Depending on the edge, choose a direction to face
	return flEdgeRightDist < flEdgeLeftDist;
}

inline bool CAntiAim::ShouldAntiAim(CBaseEntity* pLocal) {
	const bool bPlayerReady = pLocal->IsAlive() && !pLocal->IsTaunting() && !pLocal->IsInBumperKart() && !pLocal->IsAGhost() && !F::AimbotGlobal.IsAttacking();
	const bool bMovementReady = pLocal->GetMoveType() <= 5 && !pLocal->IsCharging() && !F::Misc.bMovementStopped && !F::Misc.bFastAccel;
	const bool bNotBusy = !G::AvoidingBackstab;
	const bool bEnabled = Vars::AntiHack::AntiAim::Active.Value;

	return bPlayerReady && bMovementReady && bNotBusy && bEnabled;
}

inline float CAntiAim::CalculateCustomRealPitch(float WishPitch, bool FakeDown) {
	return FakeDown ? 720 + WishPitch : -720 + WishPitch;
}

//	Get Pitches.
//	iModes;
//	0 - None
//	1 - Up
//	2 - Down
//	3 - Zero
//	4 - Custom
//	3 and 4 not available for iFake
inline float CAntiAim::GetPitch(const int iFake, const int iReal, const float flCurPitch) {
	switch (iReal) {
	case 1: {
		return iFake ? CalculateCustomRealPitch(-89.f, iFake - 1) : -89.f;
	}
	case 2: {
		return iFake ? CalculateCustomRealPitch(89.f, iFake - 1) : 89.f;
	}
	case 3: {
		return iFake ? CalculateCustomRealPitch(0.f, iFake - 1) : 0.f;
	}
	case 4: {
		return iFake ? CalculateCustomRealPitch(Vars::AntiHack::AntiAim::CustomRealPitch.Value, iFake - 1) : Vars::AntiHack::AntiAim::CustomRealPitch.Value;
	}
	}

	return iFake ? -89.f + (89.f * (iFake - 1)) : flCurPitch;	//	just in case someone forgets to put in a real pitch.
}

//	Get Yaw
//	iIndex's;
//	0 - None
//	1 - Left
//	2 - Right
//	3 - Forward (none)
//	4 - Backwards
//	5 - Spin
//	6 - Edge
//	7 - Invert
inline float CAntiAim::GetYawOffset(const int iIndex, const bool bFake) {
	switch (iIndex) {
	case 1: {
		return 90.f;
	}
	case 2: {
		return -90.f;
	}
	case 4: {
		return 180.f;
	}
	case 5: {
		return (bFake ? flFakeOffset : flRealOffset);
	}
	case 6: {
		return (GetEdge() ? 1.f : -1.f) * (bFake ? -90.f : 90.f);
	}
	case 7: {
		return (bFake ? -90.f : 90.f) * (bInvert ? -1.f : 1.f);
	}
	}
	return 0.f;
}

float CAntiAim::GetBaseYaw(int iMode, CBaseEntity* pLocal, CUserCmd* pCmd) {
	//	0 offset, 1 at player, 2 at player + offset
	const float flBaseOffset = Vars::AntiHack::AntiAim::BaseYawOffset.Value;
	switch (iMode)
	{
	case 0: { return pCmd->viewangles.y + flBaseOffset; }
	case 1:
	case 2:
	{
		float flSmallestAngleTo = 0.f; float flSmallestFovTo = 360.f;
		for (CBaseEntity* pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
		{
			if (!pEnemy || !pEnemy->IsAlive() || pEnemy->GetDormant()) { continue; }	//	is enemy valid
			PlayerInfo_t pInfo{ };
			if (I::EngineClient->GetPlayerInfo(pEnemy->GetIndex(), &pInfo))
			{
				if (G::IsIgnored(pInfo.friendsID)) { continue; }
			}
			const Vec3 vAngleTo = Math::CalcAngle(pLocal->GetAbsOrigin(), pEnemy->GetAbsOrigin());
			const float flFOVTo = Math::CalcFov(I::EngineClient->GetViewAngles(), vAngleTo);

			if (flFOVTo < flSmallestFovTo) { flSmallestAngleTo = vAngleTo.y; flSmallestFovTo = flFOVTo; }
		}
		return (flSmallestFovTo == 360.f ? pCmd->viewangles.y + (iMode == 2 ? flBaseOffset : 0) : flSmallestAngleTo + (iMode == 2 ? flBaseOffset : 0));
	}
	}
	return pCmd->viewangles.y;
}

void CAntiAim::Run(CUserCmd* pCmd, bool* pSendPacket) {
	Keybinds();
	FakeShotAngles(pCmd);
	SpinAngles();
	G::AAActive = false;
	bSendingReal = !*pSendPacket;

	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!iNetChan || !pLocal) {
		return;
	}

	Vec2& vAngles = bSendingReal ? vRealAngles : vFakeAngles;

	vAngles = { pCmd->viewangles.x, pCmd->viewangles.y };	//	reset angles

	if (!ShouldAntiAim(pLocal)) {
		vRealAngles = { pCmd->viewangles.x, pCmd->viewangles.y };
		vFakeAngles = { pCmd->viewangles.x, pCmd->viewangles.y };
		return;
	}

	G::AAActive = true;
	G::UpdateView = false;

	flBaseYaw = !iNetChan->m_nChokedPackets ? GetBaseYaw(Vars::AntiHack::AntiAim::BaseYawMode.Value, pLocal, pCmd) : flBaseYaw;	//	get base yaw on the first choked tick.
	vAngles = {
		GetPitch(Vars::AntiHack::AntiAim::PitchFake.Value, Vars::AntiHack::AntiAim::PitchReal.Value, pCmd->viewangles.x), 
		flBaseYaw + GetYawOffset(bSendingReal ? Vars::AntiHack::AntiAim::YawReal.Value : Vars::AntiHack::AntiAim::YawFake.Value, !bSendingReal)
	};
	Utils::FixMovement(pCmd, vAngles);
	pCmd->viewangles.x = vAngles.x;
	pCmd->viewangles.y = vAngles.y;
}
