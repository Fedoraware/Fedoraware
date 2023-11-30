#include "../Hooks.h"

//	xref C_BaseEntity_MoveToLastReceivedPosition
MAKE_HOOK(C_BaseEntity_BaseInterpolatePart1, S::C_BaseEntity_BaseInterpolatePart1(), int, __fastcall,
	void* ecx, void* edx, float &currentTime, Vec3 &oldOrigin, Vec3 &oldAngles, Vec3 &oldVel, int &bNoMoreChanges)
{
	if (Vars::Misc::DisableInterpolation.Value) {
		CBaseEntity* pLocal = g_EntityCache.GetLocal();
		CBaseEntity* pEntity = reinterpret_cast<CBaseEntity*>(ecx);
		if ((!pEntity || !pLocal) ||
			(pEntity == pLocal && !G::Recharging) ||
			(pEntity->GetClassID() != ETFClassID::CTFPlayer && pEntity->GetClassID() != ETFClassID::CBaseDoor)){
			return Hook.Original<FN>()(ecx, edx, currentTime, oldOrigin, oldAngles, oldVel, bNoMoreChanges);
		}
	}
	bNoMoreChanges = 1;
	return 0;
}
