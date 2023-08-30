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
#include "../../Features/TickHandler/TickHandler.h"

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
			//static int nAvatar = 0;
			//static int nAvatarID = 0;
			//static uint32 w, h;
			//static byte removebytes[16384];
			//if (!nAvatar)
			//{
			//	nAvatar = g_SteamInterfaces.Friends015->GetMediumFriendAvatar(CSteamID(g_SteamInterfaces.User->GetSteamID()));

			//	if (g_SteamInterfaces.Utils007->GetImageSize(nAvatar, &w, &h))
			//	{
			//		const int nSize = static_cast<int>(4 * w * h * sizeof(uint8));

			//		if (g_SteamInterfaces.Utils007->GetImageRGBA(nAvatar, removebytes, nSize))
			//		{
			//			for (int i = 0; i <= 16384; i += 4)
			//			{
			//				// Do not convert these to hex >:)
			//				if (rawData[i] == 105 &&
			//					rawData[i + 1] == 20 &&
			//					rawData[i + 2] == 136 &&
			//					rawData[i + 3] == 0x01)
			//				{
			//					removebytes[i] = 0x00;
			//					removebytes[i + 1] = 0x00;
			//					removebytes[i + 2] = 0x00;
			//					removebytes[i + 3] = 0x00;
			//				}
			//			}
			//			nAvatarID = g_Draw.CreateTextureFromArray(removebytes, w, h);
			//		}
			//	}
			//}
			//if (nAvatarID)
			//{
			//	I::Surface->DrawSetTexture(nAvatarID);
			//	I::Surface->DrawTexturedRect(100, 300, w, h);
			//	g_Draw.OutlinedCircle(100 + (w / 2), 300 + (h / 2), w / 2, 300, Utils::Rainbow());
			//}

			if (I::EngineVGui->IsGameUIVisible())
			{
				if (!I::EngineClient->IsInGame())
				{
					static time_t curTime = time(0);
					static tm* curCalTime = localtime(&curTime);

					if (F::Menu.IsOpen)
					{
						g_Draw.String(FONT_MENU, 5, g_ScreenSize.h - 2  - Vars::Fonts::FONT_MENU::nTall.Value, { 200, 200, 200, 255 }, ALIGN_DEFAULT, L"Build of %hs", __DATE__ " " __TIME__);
						F::Visuals.DrawDVD();
						if (Vars::Menu::MenuCelebration.Value)
						{
							if (curCalTime->tm_mon == 11 && curCalTime->tm_mday == 25) //this *probably* works
							{
								g_Draw.String(FONT_MENU, g_ScreenSize.c, 150, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, "MERRY CHRISTMAS!!!!!!!");
							}
						}
					}

					if (Vars::Menu::MenuCelebration.Value)
					{
						if (curCalTime->tm_mon == 11 || curCalTime->tm_mon == 0 || curCalTime->tm_mon == 1) //december, january, february (winter months)
						{ //this method also sucks for getting the 3 months and can probably be shortened
							F::Visuals.DrawMenuSnow();
						}
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
				//F::Ticks.DrawDebug();
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
