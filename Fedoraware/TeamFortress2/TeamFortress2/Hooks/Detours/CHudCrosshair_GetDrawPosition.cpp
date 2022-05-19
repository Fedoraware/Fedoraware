#include "../Hooks.h"

MAKE_HOOK(CHudCrosshair_GetDrawPosition, g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 53 57 E8 ? ? ? ? 8B 08 89 4D C8 8B 48 04 8B 40 08"), void, __cdecl,
		  float* pX, float* pY, bool* pbBehindCamera, Vec3 angleCrosshairOffset)
{
	if (Vars::Visuals::CrosshairAimPos.m_Var && !g_GlobalInfo.m_vAimPos.IsZero())
	{
		Vec3 vScreen;
		if (Utils::W2S(g_GlobalInfo.m_vAimPos, vScreen))
		{
			if (pX) { *pX = vScreen.x; }
			if (pY) { *pY = vScreen.y; }
			if (pbBehindCamera) { *pbBehindCamera = false; }
		}
	}
	else if (
		const auto& pLocal = g_EntityCache.m_pLocal;
		Vars::Visuals::ThirdpersonOffset.m_Var &&
		Vars::Visuals::ThirdpersonCrosshair.m_Var &&
		g_Interfaces.Input->CAM_IsThirdPerson() &&
		(Vars::Visuals::ThirdpersonRight.m_Var > 1.f ||
		Vars::Visuals::ThirdpersonRight.m_Var < -1.f) &&
		(Vars::Visuals::ThirdpersonUp.m_Var > 1.f ||
		Vars::Visuals::ThirdpersonUp.m_Var < -1.f)
		)
	{
		Vec3 viewangles = g_Interfaces.Engine->GetViewAngles();
		Vec3 vForward{}, vRight{}, vUp{};
		Math::AngleVectors(viewangles, &vForward, &vRight, &vUp);
		Vec3 vStartPos = pLocal->GetShootPos();
		Vec3 vEndPos = (vStartPos + vForward * 8192);
		CGameTrace trace = { };
		CTraceFilterHitscan filter = { };
		filter.pSkip = pLocal;
		Utils::Trace(vStartPos, vEndPos, MASK_SHOT, &filter, &trace);
		Vec3 vScreen;
		if (Utils::W2S(trace.vEndPos, vScreen))
		{
			if (pX) { *pX = vScreen.x; }
			if (pY) { *pY = vScreen.y; }
		}
		if (pbBehindCamera) { *pbBehindCamera = false; }
	}
	else
	{
		Hook.Original<FN>()(pX, pY, pbBehindCamera, angleCrosshairOffset);
	}
}