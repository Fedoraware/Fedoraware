#pragma once

#include "../../SDK/SDK.h"

class CChatInfo
{
public:
	void Event(CGameEvent* pEvent, FNV1A_t uNameHash);
	void UserMessage(UserMessageType type, bf_read& msgData);

	std::unordered_map<int, bool> KnownBots;
};

ADD_FEATURE(CChatInfo, ChatInfo)