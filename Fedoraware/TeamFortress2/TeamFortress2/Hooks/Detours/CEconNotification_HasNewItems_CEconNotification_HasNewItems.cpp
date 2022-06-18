#include "../Hooks.h"

#include "../../SDK/Main/CEconNotification.h"

MAKE_HOOK(CEconNotification_HasNewItems_CEconNotification_HasNewItems, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 10 56 8B F1 89 75 F4 E8 ? ? ? ?"), void*, __fastcall,
		  void* ecx, void* edx)
{
	CEconNotification* pNotification = reinterpret_cast<CEconNotification*>(Hook.Original<FN>()(ecx, edx));
	G::NotificationVector.push_back(pNotification);
	return pNotification;
}