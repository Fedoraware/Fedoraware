#include "../Hooks.h"
#include "../../Features/TickHandler/NetworkFix/NetworkFix.h"

MAKE_HOOK(CL_ReadPackets, S::CL_ReadPackets(), void, __cdecl, bool bFinalTick)
{
	if (F::NetworkFix.ShouldReadPackets()) {
		return Hook.Original<FN>()(bFinalTick);
	}
	
	return;
}