#include "../Hooks.h"
#include "../../Features/Backtrack/Backtrack.h"

MAKE_HOOK(FX_FireBullets, g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 53 8B 5D ? 56 53"), void, __cdecl,
	void *pWpn, int iPlayer, const Vec3 &vecOrigin, const Vec3 &vecAngles, int iWeapon, int iMode, int iSeed, float flSpread, float flDamage, bool bCritical)
{
	F::BacktrackNew.ReportShot(iPlayer, pWpn);

	static auto C_TFWeaponBaseGun__FireBullet_Call = g_Pattern.Find(L"client.dll", L"83 C4 ? 5F 5E 5B 8B E5 5D C2 ? ? CC CC CC CC 53");
	if (reinterpret_cast<DWORD>(_ReturnAddress()) != C_TFWeaponBaseGun__FireBullet_Call)
	{
		return;
	}

	return Hook.Original<FN>()(pWpn, iPlayer, vecOrigin, vecAngles, iWeapon, iMode, iSeed, flSpread, flDamage, bCritical);
}
