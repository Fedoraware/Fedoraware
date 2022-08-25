#include "../Hooks.h"

MAKE_HOOK(C_BaseAnimating_PushAllowBoneAccess, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 20 8D 45 E0 53 56 57 6A 01 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 50 E8 ? ? ? ? 8B 15 ? ? ? ? 83 C4 18 8B 0D"), void, __cdecl,
	bool bAllowForNormalModels, bool bAllowForViewModels, char const *tagPush)
{
	return Hook.Original<FN>()(bAllowForNormalModels, bAllowForViewModels, tagPush);
}
