#include "../Hooks.h"

#include "../../SDK/Includes/icons.h"
#include "../../Features/SpyWarning/SpyWarning.h"
#include "../../Features/PlayerArrows/PlayerArrows.h"
#include "../../Features/ESP/ESP.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/CritHack/CritHack.h"
#include "../../Features/Menu/Menu.h"
#include "../../Features/Menu/SpectatorList/SpectatorList.h"
#include "../../Features/Radar/Radar.h"
#include "../../Features/Followbot/Followbot.h"
#include "../../Features/AutoQueue/AutoQueue.h"
#include "../../Features/Chams/DMEChams.h"
#include "../../Features/Menu/MaterialEditor/MaterialEditor.h"
#include "../../Features/Menu/Playerlist/Playerlist.h"

#include "../../Resources/DVD-Icon.h"

MAKE_HOOK(EngineVGui_Paint, Utils::GetVFuncPtr(I::EngineVGui, 13), void, __fastcall,
		  void* ecx, void* edx, int iMode)
{
	static auto StartDrawing = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(
		_(L"vguimatsurface.dll"), _(L"55 8B EC 64 A1 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 14")));
	static auto FinishDrawing = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(
		_(L"vguimatsurface.dll"), _(L"55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 51 56 6A 00")));

	if (!g_ScreenSize.w || !g_ScreenSize.h)
	{
		g_ScreenSize.Update();
	}

	//HACK: for some reason we need to do this
	{
		static bool bInitIcons = false;

		if (!bInitIcons)
		{
			for (int nIndex = 0; nIndex < ICONS::TEXTURE_AMOUNT; nIndex++)
			{
				ICONS::ID[nIndex] = -1;
				g_Draw.Texture(-200, 0, 18, 18, Colors::White, nIndex);
			}

			bInitIcons = true;
		}
	}

	Hook.Original<FN>()(ecx, edx, iMode);

	if (iMode & PAINT_UIPANELS)
	{
		//Update W2S
		{
			CViewSetup viewSetup = {};

			if (I::Client->GetPlayerView(viewSetup))
			{
				VMatrix worldToView = {}, viewToProjection = {}, worldToPixels = {};
				I::RenderView->GetMatricesForView(viewSetup, &worldToView, &viewToProjection,
												  &G::WorldToProjection, &worldToPixels);
			}
		}

		StartDrawing(I::Surface);
		{
			if (I::EngineVGui->IsGameUIVisible())
			{
				if (!I::Engine->IsInGame())
				{
					static bool bOpenedMenuOnce = false;
					if (F::Menu.IsOpen)
					{
						g_Draw.String(FONT_MENU, 5, g_ScreenSize.h - 5 - Vars::Fonts::FONT_MENU::nTall.Value, { 116, 255, 48, 255 }, ALIGN_DEFAULT, __DATE__);
						F::Visuals.DrawDVD();
						bOpenedMenuOnce = true;
					}

					static time_t curTime = time(0);
					static tm* curCalTime = localtime(&curTime);

					if (curCalTime->tm_mon == 11)
					{
						if (!bOpenedMenuOnce)
						{
							g_Draw.String(FONT_MENU, g_ScreenSize.c, 150, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, "Happy Non-Discriminatory December Seasonal Holiday!");
						}
						F::Visuals.DrawMenuSnow();
					}
				}
			}

			if (CBaseEntity* pLocal = g_EntityCache.GetLocal())
			{
				F::Visuals.DrawAntiAim(pLocal);
				F::Visuals.DrawTickbaseInfo(pLocal);
				F::Visuals.DrawAimbotFOV(pLocal);
				F::Visuals.ScopeLines(pLocal);
				F::Visuals.DrawDebugInfo(pLocal);
			}

			F::Visuals.DrawPredictionLine();
			F::ESP.Run();
			F::Visuals.PickupTimers();
			F::SpyWarning.Run();
			F::PlayerArrows.Run();
			F::AutoQueue.Run();
			F::Followbot.Draw();
			F::SpectatorList.Run();
			F::CritHack.Draw();
			F::Radar.Run();
			F::PlayerList.Run();
			F::Notifications.Think();
			F::Visuals.SetVisionFlags();
		}
		FinishDrawing(I::Surface);
	}
}