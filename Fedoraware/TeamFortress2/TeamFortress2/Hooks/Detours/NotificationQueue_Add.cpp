#include "../Hooks.h"

#include "../../SDK/Main/CEconNotification.h"

MAKE_HOOK(NotificationQueue_Add, g_Pattern.Find(L"client.dll", L"55 8B EC 8B 0D ? ? ? ? 56 57 8B 01 8B 40 68 FF D0 8B 75 08 84 C0 75 09 80 BE ? ? ? ? ? 75 29"), int, __cdecl,
		  CEconNotification* pNotification)
{
	if (G::NotificationVector.size() > 0)
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