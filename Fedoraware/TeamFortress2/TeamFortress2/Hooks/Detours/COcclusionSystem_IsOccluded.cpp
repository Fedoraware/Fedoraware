#include "../Hooks.h"

MAKE_HOOK(COcclusionSystem_IsOccluded, g_Pattern.Find(L"engine.dll", L"55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 57"), bool, __fastcall,
		  void* ecx, void* edx, const Vec3& vecAbsMins, const Vec3& vecAbsMaxs)
{
	if (!I::OcclusionSystem)
	{
		I::OcclusionSystem = static_cast<IOcclusionSystem*>(ecx);
	}

	return Hook.Original<FN>()(ecx, edx, vecAbsMins, vecAbsMaxs);
}
