#include "FakeLag.h"

#include "../../Aimbot/AimbotGlobal/AimbotGlobal.h"
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

bool CFakeLag::IsAllowed(CBaseEntity* pLocal)
{
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	const int doubleTapAllowed = 21 - G::ShiftedTicks;
	const bool retainFakelagTest = Vars::Misc::CL_Move::RetainFakelag.Value ? G::ShiftedTicks != 1 : !G::ShiftedTicks;
	if (!iNetChan) { return false; }	//	no netchannel no fakelag

	// Failsafe, in case we're trying to choke too many ticks
	if (std::max(ChokeCounter, iNetChan->m_nChokedPackets) >= 21)
	{
		return false;
	}

	// Should fix an issue with getting teleported back to the ground for now, pretty ghetto imo
	if (!pLocal->OnSolid() && pInAirTicks.first && pInAirTicks.second > 13)
	{
		return false;
	}

	if (bAttacked && !G::IsChoking) { 
		bAttacked = false; 
		return false; 
	}

	// Are we attacking? TODO: Add more logic here
	if (F::AimbotGlobal.IsAttacking())
	{
		if (Vars::AntiHack::AntiAim::RehideAntiAimPostShot.Value) {
			bAttacked = true;
		}
		if (Vars::Misc::CL_Move::UnchokeOnAttack.Value) {
			return false;
		}
	}

	// Special Cases
	if (bPreservingBlast || (bUnducking && pLocal->OnSolid())) {
		return true;
	}

	// Are we recharging
	if ((ChokeCounter >= doubleTapAllowed || G::Recharging || G::RechargeQueued || !retainFakelagTest) && Vars::Misc::CL_Move::Enabled.Value)
	{
		return false;
	}

	if (Vars::Misc::CL_Move::WhileInAir.Value && !pLocal->OnSolid())
	{
		return true;
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

void CFakeLag::PreserveBlastJump(const int nOldGround, const int nOldFlags) {
	//if (G::IsAttacking) { return; }
	bPreservingBlast = false;
	if (!Vars::Misc::CL_Move::RetainBlastJump.Value) { return; }

	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive() || !pLocal->IsPlayer()) { return; }
	if (!pLocal->OnSolid() && nOldGround < 0 && !(nOldFlags & FL_ONGROUND)) { return; }
	if (pLocal->GetClassNum() != ETFClass::CLASS_SOLDIER) { return; }
	if (pLocal->InCond(TF_COND_BLASTJUMPING)) {
		bPreservingBlast = true;
	}
}

void CFakeLag::Unduck(const int nOldFlags) {
	if (!Vars::Misc::CL_Move::WhileUnducking.Value) { return; }

	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive() || pLocal->IsDucking() || !pLocal->IsPlayer() || !pLocal->OnSolid()) { return; }
	if (!(nOldFlags & FL_DUCKING)) { return; }
	// we were ducking before & are not ducking next
	bUnducking = true;
}

void CFakeLag::Prediction(const int nOldGroundInt, const int nOldFlags) {
	//	do blast jump preservation here.
	PreserveBlastJump(nOldGroundInt, nOldFlags);
	Unduck(nOldFlags);
}

void CFakeLag::OnTick(CUserCmd* pCmd, bool* pSendPacket, const int nOldGroundInt, const int nOldFlags)
{
	Prediction(nOldGroundInt, nOldFlags);
	G::IsChoking = !*pSendPacket;	//	do this first
	if (G::ShouldShift) { return; }
	if (!Vars::Misc::CL_Move::Fakelag.Value && !bPreservingBlast) { ChokeCounter = 0; return; }

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
		pInAirTicks = { pLocal->OnSolid(), 0 };
		bUnducking = false;
		return;
	}

	G::IsChoking = true;
	*pSendPacket = false;
	ChokeCounter++;

	if (!pLocal->OnSolid()) {
		pInAirTicks.second++;
	}
}