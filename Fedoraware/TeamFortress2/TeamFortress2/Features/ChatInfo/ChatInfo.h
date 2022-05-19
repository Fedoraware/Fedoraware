#pragma once

#include "../../SDK/SDK.h"

class CChatInfo
{
	enum class VoteOption {
		Text = 1 << 0,
		Console = 1 << 1,
		Chat = 1 << 2,
		Party = 1 << 3,
		AutoVote = 1 << 4,
		Verbose = 1 << 5
	};

public:
	void Event(CGameEvent* pEvent, FNV1A_t uNameHash);
	void UserMessage(UserMessageType type, bf_read& msgData);

	std::map<int, bool> KnownBots;
};

inline CChatInfo g_ChatInfo;
