#include "../Hooks.h"

MAKE_HOOK( UpdateNameFromSteamID, g_Pattern.Find( L"engine.dll", L"55 8B EC 56 8B 75 08 85 F6 74 2E 57" ), void, __cdecl,
	IConVar* pConvar, CSteamID* pSteamID ) {
	// Do nothing...
}