#include "../Hooks.h"

#include "../../SDK/Main/CEconNotification.h"

MAKE_HOOK(CEconNotification_HasNewItems, S::CEconNotification_HasNewItems(), void*, __fastcall,
		  void* ecx, void* edx)
{
	auto* pNotification = static_cast<CEconNotification*>(Hook.Original<FN>()(ecx, edx));
	G::NotificationVector.push_back(pNotification);
	return pNotification;
}