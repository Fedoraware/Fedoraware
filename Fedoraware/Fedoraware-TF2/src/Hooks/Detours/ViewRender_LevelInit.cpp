#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Backtrack/Backtrack.h"
#include "../../Features/TickHandler/TickHandler.h"
#include "../../Features/AntiHack/CheaterDetection/CheaterDetection.h"
#include "../../Features/NoSpread/NoSpread.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Chams/DMEChams.h"
#include "../../Features/Glow/Glow.h"

MAKE_HOOK(ViewRender_LevelInit, Utils::GetVFuncPtr(I::ViewRender, 1), void, __fastcall,
		  void* ecx, void* edx)
{
	F::Visuals.StoreMaterialHandles();
	F::Visuals.OverrideWorldTextures();
	F::NoSpread.Reset();
	//F::Statistics.Clear();

	F::DMEChams.CreateMaterials();
	F::Glow.CreateMaterials();

	F::Backtrack.Restart();
	F::Ticks.Reset();
	F::BadActors.OnLoad();

	G::NextSafeTick = 0;

	Hook.Original<FN>()(ecx, edx);

	F::Visuals.ModulateWorld();
}