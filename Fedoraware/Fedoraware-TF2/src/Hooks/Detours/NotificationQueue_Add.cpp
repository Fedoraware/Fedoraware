#include "../Hooks.h"

#include "../../SDK/Main/CEconNotification.h"

MAKE_HOOK(NotificationQueue_Add, S::NotificationQueue_Add(), int, __cdecl, CEconNotification* pNotification)
{
	if (!G::NotificationVector.empty())
	{
		for (const auto& Notification : G::NotificationVector)
		{
			if (Notification == pNotification && Vars::Misc::AutoAcceptItemDrops.Value)
			{
				pNotification->Accept();
				pNotification->Trigger();
				pNotification->UpdateTick();
				pNotification->MarkForDeletion();
				G::NotificationVector.clear();
				return 0;
			}
		}
	}

	return Hook.Original<FN>()(pNotification);
}