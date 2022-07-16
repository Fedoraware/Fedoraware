#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Backtrack/Backtrack.h"

MAKE_HOOK(ViewRender_LevelInit, Utils::GetVFuncPtr(I::ViewRender, 1), void, __fastcall,
		  void* ecx, void* edx)
{
	F::Visuals.StoreMaterialHandles();
	F::Visuals.OverrideWorldTextures();

	Hook.Original<FN>()(ecx, edx);

	F::Visuals.ModulateWorld();
	F::Backtrack.ResetLatency();
}