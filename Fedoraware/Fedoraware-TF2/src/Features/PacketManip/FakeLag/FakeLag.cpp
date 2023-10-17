#include "FakeLag.h"


inline bool CFakelag::IsAllowed(CBaseEntity* pLocal) {
	const int iMaxSend = 22 - G::ShiftedTicks;
	const bool bVar = Vars::Misc::CL_Move::Fakelag.Value || bPreservingBlast;
	const bool bChargePrio = (iMaxSend > 0 && Vars::Misc::CL_Move::RetainFakelag.Value && iChokeAmount < iMaxSend) || !G::ShiftedTicks;
	const bool bNeedAirUpdate = iAirTicks >= 14 && !pLocal->OnSolid();
	const bool bAttacking = F::AimbotGlobal.IsAttacking();

	if (bNeedAirUpdate || !bChargePrio || bAttacking || !bVar) {
		return false;
	}

	if (bPreservingBlast) {
		return true;
	}

	const bool bMoving = !Vars::Misc::CL_Move::WhileMoving.Value || pLocal->GetVecVelocity().Length2D() < 10.f;
	if (!bMoving) {
		return false;
	}

	switch (Vars::Misc::CL_Move::FakelagMode.Value) {
	case FL_Plain:
	case FL_Random: { 
		return iChokeAmount < iChokeGoal; 
	}
	case FL_Adaptive: {
		const Vec3 vDelta = vLastPosition - pLocal->m_vecOrigin();
		return vDelta.Length2DSqr() < 4096.f;
	}
	default: { return false; }
	}
}

inline void CFakelag::Prediction(const int nOldGroundInt, const int nOldFlags) {
	PreserveBlastJump(nOldGroundInt, nOldFlags);
}

inline void CFakelag::PreserveBlastJump(const int nOldGroundInt, const int nOldFlags) {
	bPreservingBlast = false;
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	const bool bVar = Vars::Misc::CL_Move::RetainBlastJump.Value;
	const bool bPlayerReady = pLocal->IsAlive() && pLocal->IsPlayer() && pLocal->OnSolid() && nOldGroundInt > 0 && nOldFlags & FL_ONGROUND;
	const bool bCanPreserve = pLocal->GetClassNum() == ETFClass::CLASS_SOLDIER && pLocal->GetCondEx2() & TFCondEx2_BlastJumping;
	bPreservingBlast = bVar && bPlayerReady && bCanPreserve;
}

void CFakelag::Run(CUserCmd* pCmd, bool* pSendPacket, const int nOldGround, const int nOldFlags) {
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!iNetChan || !pLocal) {
		return;
	}

	iChokeAmount = iNetChan->m_nChokedPackets;
	iChokeGoal = Vars::Misc::CL_Move::FakelagValue.Value;
	Prediction(nOldGround, nOldFlags);

	if (!IsAllowed(pLocal)) {
		//	you may say this is a terrible idea but i think it's actually very good.
		vLastPosition = pLocal->m_vecOrigin();
		if (Vars::Misc::CL_Move::FakelagMode.Value == FL_Random) { iChokeGoal = Utils::RandIntSimple(Vars::Misc::CL_Move::FakelagMin.Value, Vars::Misc::CL_Move::FakelagMax.Value); }
		iAirTicks = 0;
		return;
	}

	*pSendPacket = false;

	if (!pLocal->OnSolid()) {
		iAirTicks++;
	}
}