#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"

MAKE_HOOK(ClientModeShared_OverrrideView, Utils::GetVFuncPtr(g_Interfaces.ClientMode, 16), void, __fastcall,
		  void* ecx, void* edx, CViewSetup* pView)
{
	Hook.Original<FN>()(ecx, edx, pView);
	g_Visuals.FOV(pView);
	g_Visuals.ThirdPerson(pView);
}