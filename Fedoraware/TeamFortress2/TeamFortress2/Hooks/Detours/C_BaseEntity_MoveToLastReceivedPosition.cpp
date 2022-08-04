#include "../Hooks.h"

MAKE_HOOK(C_BaseEntity_MoveToLastReceivedPosition, g_Pattern.Find(L"client.dll", L"55 8B EC 80 7D 08 00 56 8B F1 75 0A 80 7E 58 17 0F 84 ? ? ? ? F3 0F 10 86 ? ? ? ? 0F 2E 86 ? ? ? ? 9F F6 C4 44 7A 2A F3 0F 10 86 ? ? ? ? 0F 2E 86 ? ? ? ? 9F F6 C4 44 7A 15 F3"), void, __fastcall,
	void* ecx, void* edx, bool force)
{
	return Hook.Original<FN>()(ecx, edx, force);
}
