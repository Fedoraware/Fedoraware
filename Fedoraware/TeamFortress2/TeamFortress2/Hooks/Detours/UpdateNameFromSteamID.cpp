#include "../Hooks.h"

MAKE_HOOK(UpdateNameFromSteamID, g_Pattern.E8(L"engine.dll", L"E8 ? ? ? ? 83 C4 08 8B 45 FC"), void, __cdecl, IConVar* pConvar, CSteamID* pSteamID) {}