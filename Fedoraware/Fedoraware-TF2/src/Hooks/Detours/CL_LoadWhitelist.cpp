#include "../Hooks.h"

MAKE_HOOK(CL_LoadWhitelist, S::CL_LoadWhitelist(), void*, __cdecl, void* table, const char* name)
{
	if (Vars::Misc::BypassPure.Value)
	{
		return nullptr;
	}

	return Hook.Original<FN>()(table, name);
}