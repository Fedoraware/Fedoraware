#include "../Hooks.h"

MAKE_HOOK(CAchievementMgr_CheckAchievementsEnabled, S::CAchievementMgr_CheckAchievementsEnabled(), bool, __fastcall, void* ecx, void* edx)
{
	return true;
}
