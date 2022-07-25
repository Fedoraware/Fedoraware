#include "../Hooks.h"

MAKE_HOOK(CL_ProcessPacketEntities, g_Pattern.Find(L"engine.dll", L"55 8B EC B8 ? ? ? ? E8 ? ? ? ? 56 57 B9 ? ? ? ? E8 ? ? ? ? FF 35 ? ? ? ? 8B F8 8B CF 89 7D FC E8 ? ? ? ? 8B 0D ? ? ? ? 33 F6 39 71 30 74 2A 68"), bool, __fastcall,
	void* eax, void* edi, CNetMessage* msg)
{
	I::Cvar->ConsoleColorPrintf({ 255, 0, 0, 255 }, "%s\n", msg->ToString());
	return Hook.Original<FN>()(eax, edi, msg);
}
