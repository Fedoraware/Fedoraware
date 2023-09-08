#include "../Hooks.h"

MAKE_HOOK(C_BaseAnimating_MaintainSequenceTransitions, S::CBaseAnimating_MaintainSequenceTransitions(), void, __fastcall,
		  void* ecx, void* edx, void* boneSetup, float flCycle, Vec3 pos[], Vector4D q[])
{
	return;
}
