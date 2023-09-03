#include "../Hooks.h"

MAKE_HOOK(COcclusionSystem_IsOccluded, S::COcclusionSystem_IsOccluded(), bool, __fastcall,
		  void* ecx, void* edx, const Vec3& vecAbsMins, const Vec3& vecAbsMaxs)
{
	if (!I::OcclusionSystem)
	{
		I::OcclusionSystem = static_cast<IOcclusionSystem*>(ecx);
	}

	return Hook.Original<FN>()(ecx, edx, vecAbsMins, vecAbsMaxs);
}
