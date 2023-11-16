#include "../Hooks.h"

#include "../../SDK/Includes/icons.h"
#include "../../Features/SpyWarning/SpyWarning.h"
#include "../../Features/PlayerArrows/PlayerArrows.h"
#include "../../Features/ESP/ESP.h"
#include "../../Features/Misc/Notifications/Notifications.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/CritHack/CritHack.h"
#include "../../Features/Menu/Menu.h"
#include "../../Features/Menu/SpectatorList/SpectatorList.h"
#include "../../Features/Radar/Radar.h"
#include "../../Features/Followbot/Followbot.h"
#include "../../Features/AutoQueue/AutoQueue.h"
#include "../../Features/Menu/MaterialEditor/MaterialEditor.h"
#include "../../Features/Menu/Playerlist/Playerlist.h"

namespace S
{
	MAKE_SIGNATURE(StartDrawing, MATSURFACE_DLL, "55 8B EC 64 A1 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 14", 0x0);
	MAKE_SIGNATURE(FinishDrawing, MATSURFACE_DLL, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 51 56 6A 00", 0x0);
}

MAKE_HOOK(EngineVGui_Paint, Utils::GetVFuncPtr(I::EngineVGui, 14), void, __fastcall,
		  void* ecx, void* edx, int iMode)
{
	static auto StartDrawing = reinterpret_cast<void(__thiscall*)(void*)>(S::StartDrawing());
	static auto FinishDrawing = reinterpret_cast<void(__thiscall*)(void*)>(S::FinishDrawing());

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

			if (I::BaseClientDLL->GetPlayerView(viewSetup))
			{
				VMatrix worldToView = {}, viewToProjection = {}, worldToPixels = {};
				I::RenderView->GetMatricesForView(viewSetup, &worldToView, &viewToProjection,
												  &G::WorldToProjection, &worldToPixels);
			}
		}

		StartDrawing(I::VGuiSurface);
		{
			// Anti Screenshot
			if (Vars::Visuals::CleanScreenshots.Value && I::EngineClient->IsTakingScreenshot())
			{
				FinishDrawing(I::VGuiSurface);
				return;
			}

			// Main Menu stuff
			if (I::EngineVGui->IsGameUIVisible())
			{
				if (!I::EngineClient->IsInGame())
				{
					static time_t curTime = time(nullptr);
					static tm* curCalTime = localtime(&curTime);

					if (F::Menu.IsOpen)
					{
						const auto& menuFont = g_Draw.GetFont(FONT_MENU);
						g_Draw.String(menuFont, 5, g_ScreenSize.h - 2  - Vars::Fonts::FONT_MENU::nTall.Value, { 200, 200, 200, 255 }, ALIGN_DEFAULT, L"Build of %hs", __DATE__ " " __TIME__);
						if (curCalTime->tm_mon == 11 && curCalTime->tm_mday == 25) //this *probably* works
						{
							g_Draw.String(menuFont, g_ScreenSize.c, 150, Utils::Rainbow(), ALIGN_CENTERHORIZONTAL, "MERRY CHRISTMAS!!!!!!!");
						}
					}
					if(Vars::Menu::DrawWeather.Value)
					{ 
						if (curCalTime->tm_mon == 11 || curCalTime->tm_mon == 0 || curCalTime->tm_mon == 1) //december, january, february (winter months)
						{ //this method also sucks for getting the 3 months and can probably be shortened
							F::Visuals.DrawMenuSnow();
						}

						if (curCalTime->tm_mon == 8 || curCalTime->tm_mon == 9 || curCalTime->tm_mon == 10)
						{
							F::Visuals.DrawMenuRain();
						}
					}
				}
			}

			if (I::EngineClient->IsInGame())
			{
				F::RSChat.Draw();
			}

			F::Visuals.Draw();
			F::ESP.Draw();
			F::PlayerArrows.Draw();
			F::Followbot.Draw();
			F::SpectatorList.Draw();
			F::CritHack.Draw();
			F::Radar.Draw();
			F::AutoQueue.Run();
			F::SpyWarning.Run();
			F::PlayerList.Run();
			F::Notifications.Draw();
		}
		FinishDrawing(I::VGuiSurface);
	}
}
