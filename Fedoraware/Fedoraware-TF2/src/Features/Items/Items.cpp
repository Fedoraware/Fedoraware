#include "Items.h"

void CItems::OnNotification(CEconNotification* pNotify)
{
	if (!Vars::Misc::AutoAcceptItemDrops.Value)
	{
		m_NotifyQueue.clear();
	}

	if (m_NotifyQueue.empty()) { return; }

	for (const auto& notification : m_NotifyQueue)
	{
		if (notification == pNotify)
		{
			pNotify->Accept();
			pNotify->Trigger();
			pNotify->UpdateTick();
			pNotify->MarkForDeletion();
			m_NotifyQueue.clear();
			return;
		}
	}
}
