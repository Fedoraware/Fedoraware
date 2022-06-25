#include "../Hooks.h"

//	"C_BaseAnimating::SetupBones"
MAKE_HOOK(CBaseAnimating_SetupBones, g_Pattern.E8(L"client.dll", L"E8 ? ? ? ? 80 BF ? ? ? ? ? 8A D8"), bool, __fastcall,
	void* eax, void* edi, matrix3x4* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	return Hook.Original<FN>()(eax, edi, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
}
