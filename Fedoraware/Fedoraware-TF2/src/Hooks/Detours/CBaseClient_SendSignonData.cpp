#include "../Hooks.h"
#include "../../Features/Chams/DMEChams.h"


MAKE_HOOK(CBaseClient_SendSignonData, S::CBaseClient_SendSignonData(), bool, __fastcall, void* ecx, void* edx)
{
	F::DMEChams.CreateMaterials();
	return Hook.Original<FN>()(ecx, edx);
}
