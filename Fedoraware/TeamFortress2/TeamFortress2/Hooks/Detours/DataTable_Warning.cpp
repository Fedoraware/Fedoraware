#include "../Hooks.h"

MAKE_HOOK(DataTable_Warning, g_Pattern.Find(L"engine.dll", L"55 8B EC B8 ? ? ? ? E8 ? ? ? ? 8D 45 0C 50 FF 75 08 8D 85 ? ? ? ? 68 ? ? ? ? 50 E8 ? ? ? ? 8D 85 ? ? ? ? 50 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 18 8B E5 5D C3 "), void, __cdecl,
	const char *pInMessage, ...)
{
	//	dont send datatable warnings
	//return Hook.Original<FN>()(pInMessage);
}