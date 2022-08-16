#include "../Hooks.h"

MAKE_HOOK(CAchievementMgr_CheckAchievementsEnabled, g_Pattern.Find(L"client.dll", L"55 8B EC A1 ? ? ? ? 83 EC 08 56 8B F1 8B 48 04 85 C9 0F 84 ? ? ? ? 83 78 18 00 0F 84 ? ? ? ? 8B 01 8B 40 04 FF D0 84 C0 0F 84 ? ? ? ? E8 ? ? ? ? 84 C0 74 15 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 04 32 C0 5E 8B E5 5D C3 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0 84 C0 74 15 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 04"), bool, __fastcall,
	void* ecx, void* edx)
{
	return true;
}
