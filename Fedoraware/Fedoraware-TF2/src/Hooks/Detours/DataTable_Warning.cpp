#include "../Hooks.h"

MAKE_HOOK(DataTable_Warning, S::DataTable_Warning(), void, __cdecl, const char *pInMessage, ...)
{
	//	dont send datatable warnings
	//return Hook.Original<FN>()(pInMessage);
}