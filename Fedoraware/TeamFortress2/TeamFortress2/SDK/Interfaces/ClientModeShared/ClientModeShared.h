#pragma once

#include "../../Includes/Includes.h"

class CBaseHudChat
{
public:
	void ChatPrintf(int pIndex, const char *fmt, ...)
	{
		typedef void(_cdecl* FN)(void*, int, int, const char*, ...);
		GetVFunc<FN>(this, 19)(this, pIndex, 0, fmt);
	}
};

class CClientModeShared
{
public:
	char szPad[28];
	CBaseHudChat *m_pChatElement;

	bool IsChatPanelOutOfFocus()
	{
		void* CHudChat = GetVFunc<void*(__thiscall *)(void*)>(this, 19)(this);
		return (CHudChat && !*(float *)((DWORD)CHudChat + 0xFC));
	}
};