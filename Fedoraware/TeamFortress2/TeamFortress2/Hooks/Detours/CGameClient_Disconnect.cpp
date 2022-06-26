#include "../Hooks.h"

MAKE_HOOK(CGameClient_Disconnect, g_Pattern.Find(L"engine.dll", L"55 8B EC 81 EC ? ? ? ? 57 8B 7D 08 83 BF ? ? ? ? ? 0F 84 ? ? ? ? 56 8D 77 FC 56 E8 ? ? ? ?"), void, __fastcall,
		  void* ecx, void* edx, const char* fmt, ...)
{
	Hook.Original<FN>()(ecx, edx, G::DisconnectReason.empty() ? fmt : G::DisconnectReason.c_str());
}