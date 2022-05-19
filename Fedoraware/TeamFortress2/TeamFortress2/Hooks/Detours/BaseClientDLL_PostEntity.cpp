#include "../Hooks.h"

MAKE_HOOK(BaseClientDLL_PostEntity, Utils::GetVFuncPtr(I::Client, 6), void, __fastcall,
		  void* ecx, void* edx)
{
	Hook.Original<FN>()(ecx, edx);
	I::Engine->ClientCmd_Unrestricted(_("r_maxdlights 69420"));
	I::Engine->ClientCmd_Unrestricted(_("r_dynamic 1"));
}