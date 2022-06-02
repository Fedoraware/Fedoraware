#include "../Hooks.h"

DWORD CanJoinPartyAdress()
{
	const DWORD dwAddress = g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? 84 C0 74 42 E8 ? ? ? ? 8B C8") + 0x1;
	return *reinterpret_cast<PDWORD>(dwAddress) + dwAddress + 4;
}

MAKE_HOOK(CanJoinParty, CanJoinPartyAdress(), bool, __fastcall, void* ecx, void* edx, int targetPlayer, int a3)
{
	//return Hook.Original<FN>()(ecx, edx, targetPlayer, a3);
	return true;
}