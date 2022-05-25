#include "FakeLagHandler.h"
#include "../../Visuals/FakeAngleManager/FakeAng.h"


bool FakeLag::flgLogic(int logic, CBaseEntity* pLocal) {
	switch (logic) {
	case 0:		// 
	case 1: {	// these two don't have logic lol (maybe add some?)
		return true; 
	}
	case 2: {	// velocity
		return (pLocal->GetVecVelocity().Length2D() > 20.f);
	}
	}
}

bool FakeLag::isflgAllowed(KeyHelper fakelagKey) {
	if (chokeCounter > 21) {
		return false;
	}

	if (g_GlobalInfo.m_bAttacking) {
		return false;
	}

	if (!fakelagKey.Down() && Vars::Misc::CL_Move::FakelagOnKey.m_Var && Vars::Misc::CL_Move::FakelagMode.m_Var == 0) {
		return false;
	}
	if (g_GlobalInfo.m_nShifted || g_GlobalInfo.m_bRechargeQueued) {
		return false;
	}
	return true;
}

void FakeLag::onTick(CUserCmd* pCmd, CBaseEntity* pLocal, bool* pSendPacket) {
	static KeyHelper fakelagKey{ &Vars::Misc::CL_Move::FakelagKey.m_Var };

	if (pLocal && pLocal->IsAlive() && Vars::Misc::CL_Move::Fakelag.m_Var) {
		if (!isflgAllowed(fakelagKey)) {
			*pSendPacket = true;
			g_FakeAng.Run(pCmd);
			chokeCounter = 0;
			return;
		}

		if (Vars::Misc::CL_Move::FakelagMode.m_Var != 1) {
			chosenAmount = Vars::Misc::CL_Move::FakelagValue.m_Var;
		}

		if (chosenAmount > chokeCounter) {
			*pSendPacket = false;
			chokeCounter++;
		}
		else {
			*pSendPacket = true;
			chosenAmount = (rand() % (Vars::Misc::CL_Move::FakelagMax.m_Var - Vars::Misc::CL_Move::FakelagMin.m_Var)) + Vars::Misc::CL_Move::FakelagMin.m_Var;
			chokeCounter = 0;
			g_FakeAng.Run(pCmd);
			g_FakeAng.DrawChams = true;
		}

	}
	else if (chokeCounter > 0)	// failsafe
	{
		*pSendPacket = true;
		chokeCounter = 0;
		g_GlobalInfo.m_bChoking = false;
	}
	else
	{
		g_GlobalInfo.m_bChoking = false;
		g_FakeAng.DrawChams = false;
	}
}