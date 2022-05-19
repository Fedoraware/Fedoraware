#include "../Hooks.h"

MAKE_HOOK(ClientModeShared_StartMessageMode, g_Pattern.Find(L"client.dll", L"55 8B EC A1 ? ? ? ? 83 EC 64"), void, __fastcall,
		  void* ecx, void* edx, int iMessageTypeMode)
{
	if (g_Interfaces.GlobalVars->maxclients != -1)
	{
		if (const auto pChatElement = g_Interfaces.ClientMode->m_pChatElement)
		{
#ifdef _DEBUG
			using fn = void(__thiscall*)(CBaseHudChat*, int);
			static auto dwCBaseHudChat__StartMessageMode = g_Pattern.Find(L"client.dll", L"55 8B EC 8B 45 08 83 EC 0C 56 57 8B F9 68 ? ? ? ?");
			fn fnCBaseHudChat__StartMessageMode = reinterpret_cast<fn>(dwCBaseHudChat__StartMessageMode);
			if (fnCBaseHudChat__StartMessageMode)
			{
				fnCBaseHudChat__StartMessageMode(pChatElement, iMessageTypeMode);
			}
#else
			pChatElement->StartMessageMode(iMessageTypeMode);
#endif
		}
	}
}