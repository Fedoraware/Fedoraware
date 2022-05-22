#include "../Hooks.h"

DWORD UpdateNameFromSteamIDAddress()
{
	DWORD dwUpdateNameFromSteamID = g_Pattern.Find(L"engine.dll", L"E8 ? ? ? ? 83 C4 08 8B 45 FC") + 0x1;
	return *(PDWORD)dwUpdateNameFromSteamID + dwUpdateNameFromSteamID + 4;
}

MAKE_HOOK(UpdateNameFromSteamID, UpdateNameFromSteamIDAddress(), void, __cdecl,
		  IConVar* pConvar, CSteamID* pSteamID)
{
	// Do nothing...
}