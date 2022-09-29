#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Misc/Misc.h"

MAKE_HOOK(ViewRender_LevelShutdown, Utils::GetVFuncPtr(I::ViewRender, 2), void, __fastcall,
		  void* ecx, void* edx)
{
	F::Visuals.ClearMaterialHandles();
	F::Statistics.Submit();
	Hook.Original<FN>()(ecx, edx);
}