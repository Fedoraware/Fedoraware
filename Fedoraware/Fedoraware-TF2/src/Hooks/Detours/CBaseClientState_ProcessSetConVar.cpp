#include "../Hooks.h"

MAKE_HOOK(CBaseClientState_ProcessSetConVar, g_Pattern.Find(L"engine.dll", L"55 8B EC 8B 49 08 83 EC 0C 8B 01 8B 40 18 FF D0 84 C0 0F 85 ? ? ? ? 8B 45 08 53 33 DB 39 58 20 0F 8E ? ? ? ? 33 C9 56 89 4D FC 57 8B"), bool, __fastcall,
	void* ecx, void* edx, NET_SetConVar* msg)
{
	return /*Vars::Visuals::RemoveForcedConvars.Value ? true : */Hook.Original<FN>()(ecx, edx, msg);	//	weird crash
}
