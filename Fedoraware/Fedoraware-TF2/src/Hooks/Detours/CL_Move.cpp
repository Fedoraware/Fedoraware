#include "../Hooks.h"
#include "../../Features/TickHandler/TickHandler.h"

MAKE_HOOK(CL_Move, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC ? 83 3D ? ? ? ? 02 0F 8C ? ? 00 00 E8 ? ? ? 00 84 C0"), void, __cdecl,
		  float accumulated_extra_samples, bool bFinalTick)
{
	static FN oClMove = Hook.Original<FN>();

	F::Ticks.CLMove(accumulated_extra_samples, bFinalTick);
	I::EngineClient->FireEvents();
}