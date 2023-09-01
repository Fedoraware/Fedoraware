#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Camera/CameraWindow.h"

MAKE_HOOK(Panel_PaintTraverse, Utils::GetVFuncPtr(I::VGuiPanel, 41), void, __fastcall,
		  void* ecx, void* edx, unsigned int vgui_panel, bool force_repaint, bool allow_force)
{
	if (I::EngineClient->IsTakingScreenshot() && Vars::Visuals::CleanScreenshots.Value)
	{
		return Hook.Original<FN>()(ecx, edx, vgui_panel, force_repaint, allow_force);
	}
	F::CameraWindow.Draw();
	//if (F::Visuals.RemoveScope(vgui_panel)) { return; }

	Hook.Original<FN>()(ecx, edx, vgui_panel, force_repaint, allow_force);
}

//if (!I::Engine->IsInGame())
//{
//	static auto UpdateInventory = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 53 56 33 DB"));
//	static int delay = 0;
//	if (I::BackpackPanel)
//	{
//		delay++;
//		if (delay == 30000)
//		{
//			UpdateInventory(I::BackpackPanel);
//			static int count = 0;
//			count++;
//
//			if (count > 6) { count = 0; }
//			switch (count)
//			{
//				case 0: G::BackpackQuality = 22; break;
//				case 1: G::BackpackQuality = 11; break;
//				case 2: G::BackpackQuality = 6; break;
//				case 3: G::BackpackQuality = 1; break;
//				case 4: G::BackpackQuality = 3; break;
//				case 5: G::BackpackQuality = 20; break;
//				case 6: G::BackpackQuality = 21; break;
//			}
//
//		}
//
//		if (delay > 30000)
//		{
//			delay = 0;
//		}
//	}
//}