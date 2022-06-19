#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"

MAKE_HOOK(ViewRender_LevelInit, Utils::GetVFuncPtr(I::ViewRender, 1), void, __fastcall,
		  void* ecx, void* edx)
{
	F::Visuals.StoreMaterialHandles();
	F::Visuals.OverrideWorldTextures();
	Hook.Original<FN>()(ecx, edx);
	F::Visuals.ModulateWorld();
}