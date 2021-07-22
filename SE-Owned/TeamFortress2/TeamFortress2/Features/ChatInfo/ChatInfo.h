#pragma once

#include "../../SDK/SDK.h"

class CChatInfo : public CGameEventListener
{
public:
	void FireGameEvent(CGameEvent* pEvent) override;

	void AddListeners();
	void RemoveListeners();
};

inline CChatInfo g_ChatInfo;