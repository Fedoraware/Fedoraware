#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Glow/Glow.h"
#include "../../Features/Chams/Chams.h"
#include "../../Features/Chams/DMEChams.h"
#include "../../Features/Camera/CameraWindow.h"

MAKE_HOOK(ViewRender_LevelInit, Utils::GetVFuncPtr(I::ViewRender, 1), void, __fastcall,
		  void* ecx, void* edx)
{
	static bool done = false;
	// Initialize features
	if (!done)
	{
		F::Glow.Init();
		F::Chams.Init();
		F::DMEChams.Init();
		F::CameraWindow.Init();
		done = true;
	}

	F::Visuals.StoreMaterialHandles();
	F::Visuals.OverrideWorldTextures();
	Hook.Original<FN>()(ecx, edx);
	F::Visuals.ModulateWorld();
}