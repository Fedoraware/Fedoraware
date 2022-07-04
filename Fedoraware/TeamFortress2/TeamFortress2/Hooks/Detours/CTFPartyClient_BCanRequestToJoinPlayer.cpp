#include "../Hooks.h"

MAKE_HOOK(CTFPartyClient_BCanRequestToJoinPlayer, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 08 8B 55 08 56 8B 75 0C 57 8B F9 89 55 F8 89 75 FC 8B 4F 18 85 C9 74 1E"), bool, __fastcall,
		  CTFPartyClient* ecx, void* edx, int64 targetPlayer)
{
	return true;
}