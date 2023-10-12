#pragma once
#include "../Feature.h"
#include "../../SDK/Main/CEconNotification.h"

class CItems
{
	std::vector<CEconNotification*> m_NotifyQueue;

public:
	void OnNotification(CEconNotification* pNotify);

	void AddNotifcation(CEconNotification* pNotify)
	{
		m_NotifyQueue.push_back(pNotify);
	}
};

ADD_FEATURE(CItems, Items);
