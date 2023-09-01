#include "../Hooks.h"
#include "../../Features/Chams/DMEChams.h"


MAKE_HOOK(CBaseClient_Disconnect, g_Pattern.Find(L"engine.dll", L"55 8B EC 81 EC ? ? ? ? 57 8B 7D 08 83 BF ? ? ? ? ? 0F 84 ? ? ? ? 56 8D 77 FC 56 E8 ? ? ? ? 8B C8 E8 ? ? ? ? C7 86 ? ? ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? 8B"), void, __fastcall,
		  void* ecx, void* edx, const char* fmt, ...)
{
	F::DMEChams.DeleteMaterials();
	return Hook.Original<FN>()(ecx, edx, fmt);
}
