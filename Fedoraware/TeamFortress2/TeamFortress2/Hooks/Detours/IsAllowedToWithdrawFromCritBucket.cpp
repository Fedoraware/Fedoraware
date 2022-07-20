#include "../Hooks.h"

MAKE_HOOK(IsAllowedToWithdrawFromCritBucket, g_Pattern.Find(L"client.dll", L"55 8B EC 56 8B F1 0F B7 86 ? ? ? ? FF 86 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 80 B8 ? ? ? ? ? 74 0A F3 0F 10 15"), bool, __fastcall,
	void* eax, float flAmount)
{
	return Hook.Original<FN>()(eax, flAmount);
}