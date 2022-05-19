#include "../Hooks.h"

MAKE_HOOK(BaseClientDLL_PostEntity, Utils::GetVFuncPtr(g_Interfaces.Client, 6), void, __fastcall,
		  void* ecx, void* edx)
{
	Hook.Original<FN>()(ecx, edx);
	g_Interfaces.Engine->ClientCmd_Unrestricted(_("r_maxdlights 69420"));
	g_Interfaces.Engine->ClientCmd_Unrestricted(_("r_dynamic 1"));
}