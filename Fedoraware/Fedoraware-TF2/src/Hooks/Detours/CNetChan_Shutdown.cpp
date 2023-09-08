#include "../Hooks.h"

MAKE_HOOK(CNetChan_Shutdown, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC 10 56 8B F1 83 BE ? ? ? ? ? 0F 8C ? ? ? ?"), void, __fastcall,
	CNetChannel* netChannel, void* edi, const char* reason)
{
	return Hook.Original<FN>()(netChannel, edi, reason);
}