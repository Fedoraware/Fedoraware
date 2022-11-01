#include "FakeLag.h"
#include "../../Visuals/FakeAngleManager/FakeAng.h"

bool CFakeLag::IsVisible(CBaseEntity* pLocal)
{
	const Vec3 vVisCheckPoint = pLocal->GetEyePosition();
	const Vec3 vPredictedCheckPoint = pLocal->GetEyePosition() + (pLocal->m_vecVelocity() * (I::GlobalVars->interval_per_tick * 6));	//	6 ticks in da future
	for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!pEnemy || !pEnemy->IsAlive() || pEnemy->IsCloaked() || pEnemy->IsAGhost() || pEnemy->GetFeignDeathReady() || pEnemy->IsBonked()) { continue; }

		PlayerInfo_t pInfo{};	//	ignored players shouldn't trigger this
		if (!I::EngineClient->GetPlayerInfo(pEnemy->GetIndex(), &pInfo))
		{
			if (G::IsIgnored(pInfo.friendsID)) { continue; }
		}

		const Vec3 vEnemyPos = pEnemy->GetEyePosition();
		if (!Utils::VisPos(pLocal, pEnemy, vVisCheckPoint, vEnemyPos) && (Vars::Misc::CL_Move::PredictVisibility.Value ? !Utils::VisPos(pLocal, pEnemy, vPredictedCheckPoint, vEnemyPos) : true)) { continue; }
		return true;
	}
	return false;
}

bool CFakeLag::DuckLogic(CBaseEntity* pLocal)
{	//	0, do nothing, 1 choke, 2 
	//	logically we can wait until ducktime is close to 800, as that is when our actual animation transitions from ducked to unducked
	const bool bOldDuck = pLocal->IsDucking();
	if (Vars::Misc::CL_Move::WhileUnducking.Value)
	{
		if (bOldDuck && !(G::LastUserCmd->buttons & IN_DUCK)) { return false; }
	}
	return true;
}

bool CFakeLag::IsAllowed(CBaseEntity* pLocal)
{
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	const int doubleTapAllowed = 22 - G::ShiftedTicks;
	const bool retainFakelagTest = Vars::Misc::CL_Move::RetainFakelag.Value ? G::ShiftedTicks != 1 : !G::ShiftedTicks;
	if (!iNetChan) { return false; }	//	no netchannel no fakelag

	// Failsafe, in case we're trying to choke too many ticks
	if (std::max(ChokeCounter, iNetChan->m_nChokedPackets) > 21)
	{
		return false;
	}

	// Should fix an issue with getting teleported back to the ground for now, pretty ghetto imo
	if (!pLocal->OnSolid() && pInAirTicks.first && pInAirTicks.second > 13)
	{
		return false;
	}

	// Are we attacking? TODO: Add more logic here
	if (G::IsAttacking)
	{
		return false;
	}

	// Are we recharging
	if ((ChokeCounter >= doubleTapAllowed || G::Recharging || G::RechargeQueued || !retainFakelagTest) && Vars::Misc::CL_Move::Enabled.Value)
	{
		return false;
	}

	if (!DuckLogic(pLocal) || (Vars::Misc::CL_Move::WhileInAir.Value && pLocal->OnSolid()))
	{
		return false;
	}	//	no other checks, we want this

	// Is a fakelag key set and pressed?
	static KeyHelper fakelagKey{ &Vars::Misc::CL_Move::FakelagKey.Value };
	if (!fakelagKey.Down() && Vars::Misc::CL_Move::FakelagOnKey.Value)
	{
		return false;
	}

	// Do we have enough velocity for velocity mode?
	if (Vars::Misc::CL_Move::WhileMoving.Value && pLocal->GetVecVelocity().Length2D() < 10.f)
	{
		return false;
	}

	// Are we visible to any valid enemies?
	if (Vars::Misc::CL_Move::WhileVisible.Value && !IsVisible(pLocal))
	{
		return false;
	}

	switch (Vars::Misc::CL_Move::FakelagMode.Value)
	{
		case FL_Plain:
		case FL_Random: { return ChokeCounter < ChosenAmount; }
		case FL_Adaptive:
		{
			const Vec3 vDelta = vLastPosition - pLocal->m_vecOrigin();
			return vDelta.Length2DSqr() < 4096.f;
		}
		default: { return false; }
	}
}

void CFakeLag::OnTick(CUserCmd* pCmd, bool* pSendPacket)
{
	G::IsChoking = false;	//	do this first
	if (!Vars::Misc::CL_Move::Fakelag.Value) { return; }
	if (G::ShouldShift) { return; }

	// Set the selected choke amount (if not random)
	if (Vars::Misc::CL_Move::FakelagMode.Value != FL_Random)
	{
		ChosenAmount = Vars::Misc::CL_Move::FakelagValue.Value;
	}

	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive())
	{

		*pSendPacket = true;
		ChokeCounter = 0;

		return;
	}

	// Are we even allowed to choke?
	if (!IsAllowed(pLocal))
	{
		vLastPosition = pLocal->m_vecOrigin();
		*pSendPacket = true;
		// Set a new random amount (if desired)
		if (Vars::Misc::CL_Move::FakelagMode.Value == FL_Random) { ChosenAmount = Utils::RandIntSimple(Vars::Misc::CL_Move::FakelagMin.Value, Vars::Misc::CL_Move::FakelagMax.Value); }
		ChokeCounter = 0;
		pInAirTicks = {pLocal->OnSolid(), 0};
		return;
	}

	G::IsChoking = true;
	*pSendPacket = false;
	ChokeCounter++;

	if (!pLocal->OnSolid()){
		pInAirTicks.second++;
	}
}