#include "../Hooks.h"
#include "../../Features/Items/Items.h"

MAKE_HOOK(NotificationQueue_Add, S::NotificationQueue_Add(), int, __cdecl, CEconNotification* pNotification)
{
	F::Items.OnNotification(pNotification);
	return Hook.Original<FN>()(pNotification);
}