#include "FireBullets.h"


void __fastcall FireBullets::Hook(void* ecx, void* edx, CBaseCombatWeapon* pWeapon, const FireBulletsInfo_t& info, bool bDoEffects, int nDamageType, int nCustomDamageType)
{
	static auto original = Func.Original<fn>();

	if (!Vars::Visuals::BulletTracer.m_Var || !pWeapon) {
		return original(ecx, edx, pWeapon, info, bDoEffects, nDamageType, nCustomDamageType);
	}

	Vec3 vStart = info.m_vecSrc;
	Vec3 vEnd = vStart + info.m_vecDirShooting * info.m_flDistance;

	CGameTrace Trace;
	Ray_t Ray;
	Ray.Init(vStart, vEnd);

	g_Interfaces.EngineTrace->TraceRay(Ray, MASK_SHOT, NULL, &Trace);

	Color_t Color = Vars::Visuals::BulletTracerRainbow.m_Var ? Utils::Rainbow() : Colors::BulletTracer;
	int iAttachment = pWeapon->LookupAttachment(_("muzzle"));
	pWeapon->GetAttachment(iAttachment, Trace.vStartPos);

	g_Interfaces.DebugOverlay->AddLineOverlayAlpha(Trace.vStartPos, Trace.vEndPos, Color.r, Color.g, Color.b, Colors::BulletTracer.a, true, 5);
}

void FireBullets::Init() {

}