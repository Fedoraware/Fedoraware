#include "../Hooks.h"

MAKE_HOOK(CNetGraphPanel_DrawTextFields, g_Pattern.Find(L"client.dll", L"55 8B EC A1 ? ? ? ? 81 EC ? ? ? ? 83 78 30 00 57 8B F9 0F 84 ? ? ? ? A1 ? ? ? ? 83 78 30 00 74 0B 8B 8F ? ? ? ? 89 4D FC EB 09 8B 87"), void, __fastcall,
	void* ecx, void* edx, int graphvalue, int x, int y, int w, void* graph, void* cmdinfo)
{
	return Hook.Original<FN>()(ecx, edx, graphvalue, x, y, w, graph, cmdinfo);
}
