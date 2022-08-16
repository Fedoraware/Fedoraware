#include "../Hooks.h"
#include "../../Features/Chams/DMEChams.h"

MAKE_HOOK(CMaterial_DeleteIfUnreferenced, g_Pattern.Find(L"materialsystem.dll", L"56 8B F1 83 7E 1C 00 7F 51"), void, __fastcall,
	IMaterial* ecx, void* edx)
{
	if (ecx) {
		if (std::find(F::DMEChams.v_MatListGlobal.begin(), F::DMEChams.v_MatListGlobal.end(), ecx) != F::DMEChams.v_MatListGlobal.end())
		{
			return;
		}
	}

	return Hook.Original<FN>()(ecx, edx);
}
