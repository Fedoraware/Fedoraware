#include "../Hooks.h"

//	xref C_BaseEntity_MoveToLastReceivedPosition
MAKE_HOOK(C_BaseEntity_BaseInterpolatePart1, g_Pattern.Find(L"client.dll", L"55 8B EC 53 8B 5D 18 56 8B F1 C7 03 ? ? ? ? F6 46 7C 01 74 3C 80 BE ? ? ? ? ? 75 33 8B 96 ? ? ? ? 83 FA FF 74 28 A1 ? ? ? ? 8B CA 81 E1 ? ? ? ? 83 C0 04 C1 E1 04 03 C1 74 11 C1 EA 0C 39 50 04 75 09 83 38 00"), int, __fastcall,
	void* ecx, void* edx, float &currentTime, Vec3 &oldOrigin, Vec3 &oldAngles, Vec3 &oldVel, int &bNoMoreChanges)
{
	// 1;INTERPOLATE_CONTINUE
	// 0;INTERPOLATE_STOP
	return Hook.Original<FN>()(ecx, edx, currentTime, oldOrigin, oldAngles, oldVel, bNoMoreChanges);
}
