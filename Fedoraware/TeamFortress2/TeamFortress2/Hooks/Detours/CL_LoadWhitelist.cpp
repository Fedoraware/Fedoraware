#include "../Hooks.h"

MAKE_HOOK(CL_LoadWhitelist, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC ? 56 8B 75 ? 8B CE FF 75"), void*, __cdecl, void* table, const char* name)
{
	if (Vars::Misc::BypassPure.m_Var)
	{
		return nullptr;
	}

	return Hook.Original<FN>()(table, name);
}