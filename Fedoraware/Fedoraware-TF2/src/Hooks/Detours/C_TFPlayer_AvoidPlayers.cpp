#include "../Hooks.h"

MAKE_HOOK(C_TFPlayer_AvoidPlayers, S::CTFPlayer_AvoidPlayers(), void, __fastcall, void* ecx, void* edx, CUserCmd* pCmd)
{
	switch (Vars::Misc::NoPush.Value){
	case 0: break;
	case 1: return; 
	case 2: if (pCmd->buttons & (IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT)) { return; } break;
	case 3: {
		if (pCmd->buttons & (IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT)) { return; }
		std::pair<float, float> oldMoves = {pCmd->forwardmove, pCmd->sidemove};
		Hook.Original<FN>()(ecx, edx, pCmd);
		pCmd->forwardmove += oldMoves.first; pCmd->forwardmove /= 3;
		pCmd->sidemove += oldMoves.second; pCmd->sidemove /= 3;
		return;
	}
	}
	Hook.Original<FN>()(ecx, edx, pCmd);
}
