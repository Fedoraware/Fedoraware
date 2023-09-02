#include "../Hooks.h"

MAKE_HOOK(CVoiceBanMgr_GetPlayerBan, g_Pattern.Find(L"client.dll", L"55 8B EC FF 75 ? E8 ? ? ? ? 85 C0 0F 95 C0 5D C2 ? ? CC CC CC CC CC CC CC CC CC CC CC CC 55 8B EC 83 EC"), bool, __fastcall,
	void* ecx, void* edx, char const playerID[SIGNED_GUID_LEN])
{
	const bool bReturn = Hook.Original<FN>()(ecx, edx, playerID);
	//Utils::ConLog("CVoiceBanMgr_GetPlayerBan", tfm::format("%s is%s voice-banned.", playerID, (bReturn ? "" : " not")).c_str(), {118, 116, 107, 255});
	return bReturn;
}
