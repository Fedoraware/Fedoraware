#include "../Hooks.h"
#include "../../Features/Chams/DMEChams.h"


MAKE_HOOK(CBaseClient_SendSignonData, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC ? 56 68 ? ? ? ? 8B F1"), bool, __fastcall,
		  void* ecx, void* edx)
{
	F::DMEChams.CreateMaterials();
	return Hook.Original<FN>()(ecx, edx);
}
