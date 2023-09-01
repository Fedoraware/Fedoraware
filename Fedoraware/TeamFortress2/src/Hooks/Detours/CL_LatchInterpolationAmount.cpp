#include "../Hooks.h"

MAKE_HOOK(CL_LatchInterpolationAmount, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC 0C 83 3D ? ? ? ? ? 0F 8C ? ? ? ? 8B 0D ? ? ? ? 8B 01 8B 40 54 FF D0 D9 5D F8 B9 ? ? ? ? E8 ? ? ? ? D9 5D FC F3 0F 10 45"), void, __fastcall,
	void* ecx, void* edx)
{
	return Hook.Original<FN>()(ecx, edx);
}
