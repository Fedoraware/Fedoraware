#include "../Hooks.h"



MAKE_HOOK(CServerGameClients_ProcessUsercmds, g_Pattern.Find(L"server.dll", L"55 8B EC B8 ? ? ? ? E8 ? ? ? ? B9 ? ? ? ? 8D 85 ? ? ? ? EB 06"), float, __fastcall,
	void* ecx, void* edx, void* player, bf_read* buf, int numcmds, int totalcmds, int dropped_packets, bool ignore, bool paused)
{
	return Hook.Original<FN>()(ecx, edx, player, buf, numcmds, totalcmds, dropped_packets, ignore, paused);
}
