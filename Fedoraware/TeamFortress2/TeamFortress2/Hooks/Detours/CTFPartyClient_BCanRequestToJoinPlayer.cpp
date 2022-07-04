#include "../Hooks.h"

MAKE_HOOK(CTFPartyClient_BCanRequestToJoinPlayer, g_Pattern.E8(L"client.dll", L"E8 ? ? ? ? 84 C0 74 42 E8 ? ? ? ? 8B C8"), bool, __fastcall,
		  void* ecx, void* edx, int targetPlayer, int a3)
{
	return true;
}