#pragma once

#include "../../SDK/SDK.h"

class CChatInfo
{
public:
	void Event(CGameEvent* pEvent, FNV1A_t uNameHash);

	std::map<int, bool> m_known_bots;
};

inline CChatInfo g_ChatInfo;
