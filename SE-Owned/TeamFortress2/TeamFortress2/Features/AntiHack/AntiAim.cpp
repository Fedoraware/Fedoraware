#include "AntiAim.h"
#include "../Vars.h"

void CAntiAim::FixMovement(CUserCmd* pCmd, Vec3 vOldAngles, float fOldSideMove, float fOldForwardMove)
{
	Vec3 curAngs = pCmd->viewangles;

	float fDelta = pCmd->viewangles.y - vOldAngles.y;
	float f1, f2;

	if (vOldAngles.y < 0.0f)
		f1 = 360.0f + vOldAngles.y;

	else f1 = vOldAngles.y;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;

	else f2 = pCmd->viewangles.y;

	if (f2 < f1)
		fDelta = abs(f2 - f1);

	else fDelta = 360.0f - abs(f1 - f2);

	fDelta = 360.0f - fDelta;

	pCmd->forwardmove = cos(DEG2RAD(fDelta)) * fOldForwardMove + cos(DEG2RAD(fDelta + 90.0f)) * fOldSideMove;
	pCmd->sidemove = sin(DEG2RAD(fDelta)) * fOldForwardMove + sin(DEG2RAD(fDelta + 90.0f)) * fOldSideMove;
}

void CAntiAim::Run(CUserCmd* pCmd, bool* pSendPacket)
{
	g_GlobalInfo.m_bAAActive = false;
	g_GlobalInfo.m_vRealViewAngles = g_GlobalInfo.m_vViewAngles;
	g_GlobalInfo.m_vFakeViewAngles = g_GlobalInfo.m_vViewAngles;

	if (!Vars::AntiHack::AntiAim::Active.m_Var)
		return;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!pLocal->IsAlive()
			|| pLocal->IsTaunting()
			|| pLocal->IsInBumperKart()
			|| pLocal->IsAGhost())
			return;

		if (g_GlobalInfo.m_bAttacking)
			return;

		else
		{
			if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon) {
				if (Utils::IsAttacking(pCmd, pWeapon))
					return;
			}
		}

		bool bPitchSet = true;
		bool bYawSet = true;

		Vec3 vOldAngles = pCmd->viewangles;
		float fOldSideMove = pCmd->sidemove;
		float fOldForwardMove = pCmd->forwardmove;

		Vec3 vAngles = pCmd->viewangles;

		switch (Vars::AntiHack::AntiAim::Pitch.m_Var) {
		case 1: { pCmd->viewangles.x = -89.0f; g_GlobalInfo.m_vRealViewAngles.x = -89.0f; break; }
		case 2: { pCmd->viewangles.x = 89.0f; g_GlobalInfo.m_vRealViewAngles.x = 89.0f; break; }
		case 3: { pCmd->viewangles.x = -271.0f; g_GlobalInfo.m_vRealViewAngles.x = 89.0f; break; }
		case 4: { pCmd->viewangles.x = 271.0f; g_GlobalInfo.m_vRealViewAngles.x = -89.0f; break; }
		default: { bPitchSet = false; break; }
		}

		static bool b = false;

		if (b)
		{
			switch (Vars::AntiHack::AntiAim::YawReal.m_Var) {
			case 1: { pCmd->viewangles.y += 90.0f;  break; }
			case 2: { pCmd->viewangles.y -= 90.0f; break; }
			case 3: { pCmd->viewangles.y += 180.0f; break; }
			default: { bYawSet = false; break; }
			}

			g_GlobalInfo.m_vRealViewAngles.y = pCmd->viewangles.y;
		}

		else
		{
			switch (Vars::AntiHack::AntiAim::YawFake.m_Var) {
			case 1: { pCmd->viewangles.y += 90.0f; break; }
			case 2: { pCmd->viewangles.y -= 90.0f; break; }
			case 3: { pCmd->viewangles.y += 180.0f; break; }
			default: { bYawSet = false; break; }
			}

			g_GlobalInfo.m_vFakeViewAngles.y = pCmd->viewangles.y;
		}

		*pSendPacket = b = !b;
		g_GlobalInfo.m_bAAActive = bPitchSet || bYawSet;
		FixMovement(pCmd, vOldAngles, fOldSideMove, fOldForwardMove);
	}
}