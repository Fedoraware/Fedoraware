#include "../Hooks.h"

MAKE_HOOK(CViewRender_DrawUnderwaterOverlay, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 2C 53 8B 99 ? ? ? ? 89 5D E0 85 DB 0F 84 ? ? ? ? 8B 0D ? ? ? ? 57 33 FF 89"), void, __fastcall,
	void* eax, void* edi)
{
	if (Vars::Visuals::RemoveScreenOverlays.Value) {
		return;
	}
	return Hook.Original<FN>()(eax, edi);
}
