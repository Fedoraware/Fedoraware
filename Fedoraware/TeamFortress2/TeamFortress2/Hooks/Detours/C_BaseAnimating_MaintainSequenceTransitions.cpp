#include "../Hooks.h"

MAKE_HOOK(C_BaseAnimating_MaintainSequenceTransitions, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC ? 56 8B 75 ? 57 8B F9 8B CE E8 ? ? ? ? 85 C0"), void, __fastcall,
		  void* ecx, void* edx, void* boneSetup, float flCycle, Vec3 pos[], Vector4D q[])
{
	return;
}
