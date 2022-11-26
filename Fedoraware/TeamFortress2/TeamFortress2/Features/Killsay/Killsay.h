#pragma once

#include "../../SDK.h"

class CKillsay
{
	std::vector<std::string> m_vecLoadedKillsays;
public:
	bool m_bFilled = false;
	void FireGameEvent(FNV1A_t uNameHash, CGameEvent* pEvent);
};

ADD_FEATURE(CKillsay, Killsay);