#pragma once

#include "../../SDK/SDK.h"
#include "../../SDK/Interfaces/GameEvent/GameEvent.h"

class CEventListener : public CGameEventListener
{
public:
	void Setup(const std::deque<const char*>& arrEvents);
	void Destroy();

	virtual void FireGameEvent(CGameEvent* pEvent) override;
	virtual int GetEventDebugID() override { return 42; }
};

inline CEventListener g_Events;