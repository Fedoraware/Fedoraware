#include "FakeLag.h"
#include "../../Visuals/FakeAngleManager/FakeAng.h"

bool CFakeLag::IsAllowed(CBaseEntity* pLocal) {

	const int doubleTapAllowed = 22 - G::ShiftedTicks;
	const bool retainFakelagTest = Vars::Misc::CL_Move::RetainFakelag.Value ? G::ShiftedTicks != 1 : !G::ShiftedTicks;

	// Failsafe, in case we're trying to choke too many ticks
	if (ChokeCounter > 21) {
		return false;
	}

	if (ChokeCounter >= ChosenAmount && Vars::Misc::CL_Move::FakelagMode.Value < 3) {
		return false;
	}

	// Are we attacking? TODO: Add more logic here
	if (G::IsAttacking) {
		return false;
	}

	// Is a fakelag key set and pressed?
	static KeyHelper fakelagKey{ &Vars::Misc::CL_Move::FakelagKey.Value };
	if (!fakelagKey.Down() && Vars::Misc::CL_Move::FakelagOnKey.Value && Vars::Misc::CL_Move::FakelagMode.Value == 0) {
		return false;
	}

	// Are we recharging
	if (ChokeCounter >= doubleTapAllowed || G::Recharging || G::RechargeQueued || !retainFakelagTest) {
		return false;
	}

	// Do we have enough velocity for velocity mode?
	if (Vars::Misc::CL_Move::FakelagMode.Value == FL_Velocity && pLocal->GetVecVelocity().Length2D() < 20.f)
	{
		return false;
	}

	if (Vars::Misc::CL_Move::FakelagMode.Value == FL_Adaptive){
		const Vec3 vDelta = vLastPosition - pLocal->GetAbsOrigin();
		if (vDelta.Length2DSqr() > pow(64, 2)){
			return false;
		}
	}

	switch (Vars::Misc::CL_Move::FakelagMode.Value){
	case FL_Velocity:{
		return pLocal->GetVecVelocity().Length2D() > 20.f;
	}
	case FL_Adaptive:{
		const Vec3 vDelta = vLastPosition - pLocal->GetAbsOrigin();
		return vDelta.Length2DSqr() < pow(64, 2);
	}
	case FL_SmartAdaptive:{
		const Vec3 vDelta = vLastPosition - pLocal->GetAbsOrigin();
		const float flVelocity = pLocal->GetVecVelocity().Length2D();
		return vDelta.Length2DSqr() < pow(64, 2) && flVelocity > 10.f;
	}
	default: {
		return true;
	}
	}
}

void CFakeLag::OnTick(CUserCmd* pCmd, bool* pSendPacket) {
	G::IsChoking = false;	//	do this first
	if (!Vars::Misc::CL_Move::Fakelag.Value) { return; }
	if (G::ShouldShift) { return; }

	// Set the selected choke amount (if not random)
	if (Vars::Misc::CL_Move::FakelagMode.Value != FL_Random) {
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
	if (!IsAllowed(pLocal)) {
		vLastPosition = pLocal->GetAbsOrigin();
		*pSendPacket = true;
		// Set a new random amount (if desired)
		if (Vars::Misc::CL_Move::FakelagMode.Value == FL_Random) { ChosenAmount = Utils::RandIntSimple(Vars::Misc::CL_Move::FakelagMin.Value, Vars::Misc::CL_Move::FakelagMax.Value); }
		ChokeCounter = 0;
		return;
	}

	G::IsChoking = true;
	*pSendPacket = false;
	ChokeCounter++;
}