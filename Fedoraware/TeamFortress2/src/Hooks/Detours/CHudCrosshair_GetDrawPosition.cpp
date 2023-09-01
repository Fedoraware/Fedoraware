#include "../Hooks.h"

MAKE_HOOK(CHudCrosshair_GetDrawPosition, g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 53 57 E8 ? ? ? ? 8B 08 89 4D C8 8B 48 04 8B 40 08"), void, __cdecl,
		  float* pX, float* pY, bool* pbBehindCamera, Vec3 angleCrosshairOffset)
{
	if (I::EngineClient->IsTakingScreenshot() && Vars::Visuals::CleanScreenshots.Value) { return Hook.Original<FN>()(pX, pY, pbBehindCamera, angleCrosshairOffset); }

	if (Vars::Visuals::CrosshairAimPos.Value && !G::AimPos.IsZero())
	{
		Vec3 vScreen;
		if (Utils::W2S(G::AimPos, vScreen))
		{
			if (pX) { *pX = vScreen.x; }
			if (pY) { *pY = vScreen.y; }
			if (pbBehindCamera) { *pbBehindCamera = false; }
		}
	}
	else if (
		const auto& pLocal = g_EntityCache.GetLocal();
		//Vars::Visuals::ThirdpersonOffset.Value &&
		Vars::Visuals::ThirdpersonCrosshair.Value &&
		I::Input->CAM_IsThirdPerson()/* &&
		(Vars::Visuals::ThirdpersonRight.Value > 1.f ||
		Vars::Visuals::ThirdpersonRight.Value < -1.f) &&
		(Vars::Visuals::ThirdpersonUp.Value > 1.f ||
		Vars::Visuals::ThirdpersonUp.Value < -1.f)*/
		)
	{
		const Vec3 viewangles = I::EngineClient->GetViewAngles();
		Vec3 vForward{}, vRight{}, vUp{};
		Math::AngleVectors(viewangles, &vForward, &vRight, &vUp);

		const Vec3 vStartPos = pLocal->GetShootPos();
		const Vec3 vEndPos = (vStartPos + vForward * 8192);

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