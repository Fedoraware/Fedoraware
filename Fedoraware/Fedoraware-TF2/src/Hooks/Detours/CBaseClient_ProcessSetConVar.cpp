#include "../Hooks.h"

MAKE_HOOK(CBaseClient_ProcessSetConVar, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC 10 8B 45 08 53 8B D9 C7 45 ? ? ? ? ? 83 78 20 00 0F 8E ? ? ? ? 33 D2 56 89 55 F8 57"), bool, __fastcall,
	void* eax, void* edi, NET_SetConVar* msg)
{
	return /*Vars::Visuals::RemoveForcedConvars.Value ? true : */Hook.Original<FN>()(eax, edi, msg);
}
