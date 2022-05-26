#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"

MAKE_HOOK(ViewRender_LevelShutdown, Utils::GetVFuncPtr(I::ViewRender, 2), void, __fastcall,
		  void* ecx, void* edx)
{
	F::Visuals.ClearMaterialHandles();
	Hook.Original<FN>()(ecx, edx);
}