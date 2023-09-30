#include "../Hooks.h"

MAKE_HOOK(CL_ReadPackets, S::CL_ReadPackets(), void, __cdecl, bool bFinalTick)
{
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!iNetChan || iNetChan->IsLoopback() || !I::EngineClient->IsInGame() || !Vars::Misc::FixInputDelay.Value){ 
		return Hook.Original<FN>()(bFinalTick); 
	}


	//const float flCurTimeBackup = I::GlobalVars->curtime;
	//Hook.Original<FN>()(bFinalTick);
	//I::GlobalVars->curtime = flCurTimeBackup;
	return;
}