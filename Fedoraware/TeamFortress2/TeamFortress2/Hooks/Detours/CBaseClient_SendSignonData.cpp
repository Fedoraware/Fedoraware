#include "../Hooks.h"
#include "../../Features/Chams/DMEChams.h"


MAKE_HOOK(CBaseClient_SendSignonData, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC 1C 56 68 ? ? ? ? 8B F1 FF 15 ? ? ? ? 83 C4 04 E8 ? ? ? ? 6A 14 8B C8 8B 10 FF 52 68 8B 86 ? ? ? ? 80 B8 ? ? ? ? ? 74 21 8B 80"), bool, __fastcall,
	void* ecx, void* edx)
{
	F::DMEChams.CreateMaterials();
	return Hook.Original<FN>()(ecx, edx);
}
