#include "../Hooks.h"
#include "../../Features/Chams/DMEChams.h"

MAKE_HOOK(CMaterial_Uncache, S::CMaterial_Uncache(), void, __fastcall, IMaterial* ecx, void* edx, bool bPreserveVars)
{
	if (ecx)
	{
		if (std::ranges::find(F::DMEChams.v_MatListGlobal, ecx) != F::DMEChams.v_MatListGlobal.end())
		{
			return;
		}
	}

	Hook.Original<FN>()(ecx, edx, bPreserveVars);
}