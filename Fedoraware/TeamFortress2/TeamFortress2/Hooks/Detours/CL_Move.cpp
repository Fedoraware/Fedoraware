#include "../Hooks.h"
#include "../../Features/TickHandler/TickHandler.h"

MAKE_HOOK(CL_Move, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC ? 83 3D ? ? ? ? 02 0F 8C ? ? 00 00 E8 ? ? ? 00 84 C0"), void, __cdecl,
	float accumulated_extra_samples, bool bFinalTick)
{
	static Hooks::CL_Move::FN oClMove = Hook.Original<FN>();

	if (!Vars::Misc::CL_Move::Enabled.Value)
	{
		oClMove(accumulated_extra_samples, bFinalTick);
	}
	else {
		F::Ticks.CLMove(accumulated_extra_samples, bFinalTick);
	}
	
	I::EngineClient->FireEvents();
}