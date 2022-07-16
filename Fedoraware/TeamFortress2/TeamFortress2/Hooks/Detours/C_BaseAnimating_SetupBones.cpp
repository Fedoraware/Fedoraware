#include "../Hooks.h"

//	"C_BaseAnimating::SetupBones"
MAKE_HOOK(C_BaseAnimating_SetupBones, g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 33 C9 33 D2 89 4D EC 89 55 F0 8B 46 08 85 C0 74 3B"), bool, __fastcall,
	void* eax, void* edi, matrix3x4* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	return Hook.Original<FN>()(eax, edi, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
}
