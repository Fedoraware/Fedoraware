#include "../Hooks.h"

MAKE_HOOK(CTFPartyClient_BCanRequestToJoinPlayer, S::CTFPartyClient_BCanRequestToJoinPlayer(), bool, __fastcall,
		  CTFPartyClient* ecx, void* edx, int64 targetPlayer)
{
	return true;
}