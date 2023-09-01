#include "../Hooks.h"

//	xref C_BaseEntity_MoveToLastReceivedPosition
MAKE_HOOK(C_BaseEntity_BaseInterpolatePart1, g_Pattern.Find(L"client.dll", L"55 8B EC 53 8B 5D 18 56 8B F1 C7 03"), int, __fastcall,
	void* ecx, void* edx, float &currentTime, Vec3 &oldOrigin, Vec3 &oldAngles, Vec3 &oldVel, int &bNoMoreChanges)
{
	// 1;INTERPOLATE_CONTINUE
	// 0;INTERPOLATE_STOP
	return Hook.Original<FN>()(ecx, edx, currentTime, oldOrigin, oldAngles, oldVel, bNoMoreChanges);
}
