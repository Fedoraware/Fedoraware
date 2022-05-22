#include "../Hooks.h"

MAKE_HOOK(CL_NameCvarChanged, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC 30 8D 4D F8"), void, __cdecl,
		  IConVar* pConvar)
{
	Hook.Original<FN>()(pConvar);

	if (const auto name = I::CVars->FindVar("name"))
	{
		I::CVars->ConsolePrintf("[FeD] Name set to: %s\n", name->GetString());
	}
}