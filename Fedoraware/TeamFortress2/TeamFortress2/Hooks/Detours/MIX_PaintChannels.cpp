#include "../Hooks.h"

MAKE_HOOK(MIX_PaintChannels, g_Pattern.Find(L"engine.dll", L"55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 33 DB 89 5D D0 89 5D D4 8B 01 89 5D D8 89 5D DC 85 C0 74 3C 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68"), void, __fastcall,
	void* ecx, void* edx, int endtime, bool bIsUnderwater)
{
	Hook.Original<FN>()(ecx, edx, endtime, bIsUnderwater);
}