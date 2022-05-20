#include "../Hooks.h"

#include "../../Features/Chams/Chams.h"
#include "../../Features/Glow/Glow.h"

MAKE_HOOK(ClientModeShared_DoPostScreenSpaceEffects, Utils::GetVFuncPtr(I::ClientMode, 39), bool, __fastcall,
		  void* ecx, void* edx, const CViewSetup* pSetup)
{
	g_Chams.Render();
	g_Glow.Render();

	return Hook.Original<FN>()(ecx, edx, pSetup);
}