#include "../Hooks.h"

MAKE_HOOK(BaseClientDLL_VoiceStatus, Utils::GetVFuncPtr(I::Client, 33), void, __fastcall,
	void* ecx, void* edx, int entindex, int bTalking)	//	bTalking = typedef int qboolean;
{
	if (const int iLocalIndex = I::Engine->GetLocalPlayer()) {
		bTalking = iLocalIndex == entindex ? 0 : bTalking;
	}
	Hook.Original<FN>()(ecx, edx, entindex, bTalking);
}