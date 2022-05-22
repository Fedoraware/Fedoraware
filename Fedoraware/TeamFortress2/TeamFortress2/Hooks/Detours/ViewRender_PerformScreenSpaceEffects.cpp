#include "../Hooks.h"

MAKE_HOOK(ViewRender_PerformScreenSpaceEffects, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC ? 8B 0D ? ? ? ? 53 56 57 33 F6 33 FF 89 75 ? 89 7D ? 8B 01 85 C0 74 ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 57 57 57 57 8D 4D ? 51 50 8B 40 ? FF D0 8B 7D ? 83 C4 ? 8B 75 ? 8B 0D ? ? ? ? 8B 19 8B 0D"),
		  void, __fastcall, void* ecx, void* edx, int x, int y, int w, int h)
{
	// Do nothing.
}