#include "../Hooks.h"

#include "../../SDK/Includes/icons.h"
#include "../../Features/SpyWarning/SpyWarning.h"
#include "../../Features/PlayerArrows/PlayerArrows.h"
#include "../../Features/ESP/ESP.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Misc/Notifications/Notifications.h"
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
#include "../../Features/AntiHack/AntiAim.h"

#include "../../Resources/DVD-Icon.h"
#include "../../Resources/64x64_Circle_Mask.h"

MAKE_HOOK(EngineVGui_Paint, Utils::GetVFuncPtr(I::EngineVGui, 14), void, __fastcall,
	void* ecx, void* edx, int iMode)
{
	static auto StartDrawing = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(
		L"vguimatsurface.dll", L"55 8B EC 64 A1 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 14"));
	static auto FinishDrawing = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(
		L"vguimatsurface.dll", L"55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 51 56 6A 00"));

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
			if (g_Draw.m_vecFonts.empty())
			{
				g_Draw.RemakeFonts
				({
					{ 0x0, Vars::Fonts::FONT_ESP::szName.c_str(), Vars::Fonts::FONT_ESP::nTall.Value, Vars::Fonts::FONT_ESP::nWeight.Value, Vars::Fonts::FONT_ESP::nFlags.Value},
					{ 0x0, Vars::Fonts::FONT_ESP_NAME::szName.c_str(), Vars::Fonts::FONT_ESP_NAME::nTall.Value, Vars::Fonts::FONT_ESP_NAME::nWeight.Value, Vars::Fonts::FONT_ESP_NAME::nFlags.Value },
					{ 0x0, Vars::Fonts::FONT_ESP_COND::szName.c_str(), Vars::Fonts::FONT_ESP_COND::nTall.Value, Vars::Fonts::FONT_ESP_COND::nWeight.Value, Vars::Fonts::FONT_ESP_COND::nFlags.Value },
					{ 0x0, Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(), Vars::Fonts::FONT_ESP_PICKUPS::nTall.Value, Vars::Fonts::FONT_ESP_PICKUPS::nWeight.Value, Vars::Fonts::FONT_ESP_PICKUPS::nFlags.Value },
					{ 0x0, Vars::Fonts::FONT_MENU::szName.c_str(), Vars::Fonts::FONT_MENU::nTall.Value, Vars::Fonts::FONT_MENU::nWeight.Value, Vars::Fonts::FONT_MENU::nFlags.Value},
					{ 0x0, Vars::Fonts::FONT_INDICATORS::szName.c_str(), Vars::Fonts::FONT_INDICATORS::nTall.Value, Vars::Fonts::FONT_INDICATORS::nWeight.Value, Vars::Fonts::FONT_INDICATORS::nFlags.Value},
					{ 0x0, "Verdana", 18, 1600, FONTFLAG_ANTIALIAS},
					{ 0x0, "Verdana", 12, 800, FONTFLAG_DROPSHADOW},
					});
			}


			if (I::EngineVGui->IsGameUIVisible())
			{
				if (!I::EngineClient->IsInGame())
				{
					static time_t curTime = time(0);
					static tm* curCalTime = localtime(&curTime);

					if (F::Menu.IsOpen)
					{
						g_Draw.String(FONT_MENU, 5, g_ScreenSize.h - 5 - Vars::Fonts::FONT_MENU::nTall.Value, { 116, 255, 48, 255 }, ALIGN_DEFAULT, __DATE__);
						F::Visuals.DrawDVD();
						if (curCalTime->tm_mon == 11)
						{
							g_Draw.String(FONT_MENU, g_ScreenSize.c, 150, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, "MERRY CHRISTMAS!!!!!!!");
						}
					}

					if (curCalTime->tm_mon == 11)
					{
						F::Visuals.DrawMenuSnow();
					}
				}
			}

			if (I::EngineClient->IsInGame())
			{
				F::RSChat.Draw();
			}

			if (CBaseEntity* pLocal = g_EntityCache.GetLocal())
			{
				if (I::EngineClient->IsTakingScreenshot() && Vars::Visuals::CleanScreenshots.Value) { return FinishDrawing(I::VGuiSurface); }
				F::Visuals.DrawAntiAim(pLocal);
				F::Visuals.DrawTickbaseInfo(pLocal);
				F::Visuals.DrawAimbotFOV(pLocal);
				F::Visuals.ScopeLines(pLocal);
				F::Visuals.DrawDebugInfo(pLocal);
				F::Visuals.DrawOnScreenConditions(pLocal);
				F::Visuals.DrawOnScreenPing(pLocal);
				F::Visuals.DrawServerHitboxes();
				F::AntiAim.Draw(pLocal);
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
		FinishDrawing(I::VGuiSurface);
	}
}