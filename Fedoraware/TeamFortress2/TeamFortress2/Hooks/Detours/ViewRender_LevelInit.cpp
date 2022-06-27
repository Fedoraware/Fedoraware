#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Glow/Glow.h"
#include "../../Features/Chams/Chams.h"
#include "../../Features/Chams/DMEChams.h"
#include "../../Features/Camera/CameraWindow.h"

#include <mutex>

MAKE_HOOK(ViewRender_LevelInit, Utils::GetVFuncPtr(I::ViewRender, 1), void, __fastcall,
		  void* ecx, void* edx)
{
	static std::once_flag onceFlag;
	std::call_once(onceFlag, [] {
		F::Glow.Init();
		F::Chams.Init();
		F::DMEChams.Init();
		F::CameraWindow.Init();
	});

	F::Visuals.StoreMaterialHandles();
	F::Visuals.OverrideWorldTextures();
	Hook.Original<FN>()(ecx, edx);
	F::Visuals.ModulateWorld();
}