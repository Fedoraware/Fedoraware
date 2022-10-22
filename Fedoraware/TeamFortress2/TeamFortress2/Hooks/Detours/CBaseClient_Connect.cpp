#include "../Hooks.h"
#include "../../Features/Chams/DMEChams.h"


MAKE_HOOK(CBaseClient_Connect, g_Pattern.Find(L"engine.dll", L"55 8B EC 53 56 57 68 ? ? ? ? 8B F1"), void, __fastcall,
		  void* ecx, void* edx, const char* szName, int nUserID, INetChannel* pNetChannel, bool bFakePlayer, int clientChallenge)
{
	F::DMEChams.CreateMaterials();
	return Hook.Original<FN>()(ecx, edx, szName, nUserID, pNetChannel, bFakePlayer, clientChallenge);
}
