#include "../Hooks.h"

MAKE_HOOK(C_TFPlayer_AvoidPlayers, g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 57 8B F9 89 7D D4 83 78 30 00"), void, __fastcall,
		  void* ecx, void* edx, CUserCmd* pCmd)
{
	if (!Vars::Misc::NoPush.Value)
	{
		Hook.Original<FN>()(ecx, edx, pCmd);
	}
}