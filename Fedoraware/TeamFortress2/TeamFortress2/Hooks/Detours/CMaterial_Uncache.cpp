#include "../Hooks.h"
#include "../../Features/Chams/DMEChams.h"

MAKE_HOOK(CMaterial_Uncache, g_Pattern.E8(L"MaterialSystem.dll", L"E8 ? ? ? ? 83 7E 1C 00 "), void, __fastcall,
		  IMaterial* ecx, void* edx, bool bPreserveVars)
{
	if (ecx)
	{
		if (std::find(F::DMEChams.v_MatListGlobal.begin(), F::DMEChams.v_MatListGlobal.end(), ecx) != F::DMEChams.v_MatListGlobal.end()){
			return;
		}
	}

	Hook.Original<FN>()(ecx, edx, bPreserveVars);
}