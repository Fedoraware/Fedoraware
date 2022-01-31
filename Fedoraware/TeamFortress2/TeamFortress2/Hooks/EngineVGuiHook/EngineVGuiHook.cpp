#include "EngineVGuiHook.h"

#include "../../SDK/Includes/icons.h"
#include "../../Features/Menu/Menu.h"
#include "../../Features/SpectatorList/SpectatorList.h"
#include "../../Features/SpyWarning/SpyWarning.h"
#include "../../Features/PlayerArrows/PlayerArrows.h"
#include "../../Features/ESP/ESP.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Radar/Radar.h"
#include "../../Features/Keybinds/Keybinds.h"
#include "../../Features/Aimbot/AimbotMelee/AimbotMelee.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Crits/Crits.h"
#include "../../Features/Aimbot/AimbotProjectile/AimbotProjectile.h"
#include "../../Features/Crits/Crits.h"

void __stdcall EngineVGuiHook::Paint::Hook(int mode)
{
	static auto StartDrawing = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(_(L"vguimatsurface.dll"), _(L"55 8B EC 64 A1 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 14")));
	static auto FinishDrawing = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(_(L"vguimatsurface.dll"), _(L"55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 51 56 6A 00")));

	if (!g_ScreenSize.w || !g_ScreenSize.h)
		g_ScreenSize.Update();

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

	Table.Original<fn>(index)(g_Interfaces.EngineVGui, mode);

	if (mode & PAINT_UIPANELS)
	{
		//Update W2S
		{
			CViewSetup ViewSetup = {};

			if (g_Interfaces.Client->GetPlayerView(ViewSetup)) {
				VMatrix WorldToView = {}, ViewToProjection = {}, WorldToPixels = {};
				g_Interfaces.RenderView->GetMatricesForView(ViewSetup, &WorldToView, &ViewToProjection, &g_GlobalInfo.m_WorldToProjection, &WorldToPixels);
			}
		}

		StartDrawing(g_Interfaces.Surface);
		{
			auto OtherDraws = [&]() -> void
			{
				if (g_Interfaces.EngineVGui->IsGameUIVisible())
					return;

				//Proj aim line
				//This could use alot of improvement, but still subjectively better than a flying rec
				//Credits to JAGNEmk aka me x)
				// Fuck you.
				if (!g_GlobalInfo.m_vPredictedPos.IsZero())
				{
					if (Vars::Visuals::AimPosSquare.m_Var) {
						Vec3 vProjAimStart, vProjAimEnd = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);
						if (Utils::W2S(g_GlobalInfo.jagnepredshit, vProjAimStart) && Utils::W2S(g_GlobalInfo.m_vPredictedPos, vProjAimEnd)) {
							g_Draw.Line(
								vProjAimStart.x,
								vProjAimStart.y,
								vProjAimEnd.x,
								vProjAimEnd.y,
								{ 255, 255, 255, 255 } //Set this to a var if u wantto idc
							);
						}
						
					}
				}

				// for damage logger. 
				// you can use it for more, i'm sure. - myzarfin
				g_notify.Think();

				//Tickbase info
				if (Vars::Misc::CL_Move::Enabled.m_Var)
				{
					const auto& pLocal = g_EntityCache.m_pLocal;
					const auto& pWeapon = g_EntityCache.m_pLocalWeapon;

					if (pLocal && pWeapon)
					{
						if (pLocal->GetLifeState() == LIFE_ALIVE)
						{
							const int nY = (g_ScreenSize.h / 2) + 20;

							static Color_t color1, color2, color3;

							if (g_GlobalInfo.m_nWaitForShift) {
								color1 = Colors::DtChargingLeft;
								color2 = Colors::DtChargingRight;
							}
							else {
								color1 = Colors::DtChargedLeft;
								color2 = Colors::DtChargedRight;
							}

							if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 1) {
								float maxWidth = 24 * Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var;
								float dtOffset = g_ScreenSize.c - (maxWidth / 2);
								static float tickWidth = 0.f;
								static float barWidth = 0.f;
								tickWidth = (g_GlobalInfo.m_nShifted * Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var);
								barWidth = g_Draw.EaseIn(barWidth, tickWidth, 0.9f); 

								g_Draw.OutlinedRect(dtOffset - 1, (g_ScreenSize.h / 2)  + 49, maxWidth + 2, Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var + 2, { 50,50,50,210 });
								g_Draw.GradientRect(dtOffset, (g_ScreenSize.h / 2) + 50, dtOffset + barWidth, (g_ScreenSize.h / 2) + 50 + Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var, color1, color2, true);
							}

							else if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 3) { // literally directly pasted from deathpole and not tested so PLEASE M-FED TEST THIS
								float ratio = ((float)g_GlobalInfo.m_nShifted / (float)24);

								if (ratio > 1) { ratio = 1; }
								else if (ratio < 0) { ratio = 0; } //failsafe (has an actual purpose if u do custom dt ticks better)
								
								// these are all vars in dp but fedware doesnt have the vars and i am not adding them
								int xoff = 0; // width offset (is it called width offset who knows)
								int yoff = 402; // height offset
								int xscale = 14; // height of bar
								int yscale = 300; // width of bar

								color3 = { 255,255,255,255 };

								g_Draw.OutlinedRect(g_ScreenSize.c - (yscale / 2 + 1) + xoff, nY - (xscale / 2 + 1) + yoff, (yscale + 2), (xscale + 2), color3);
								g_Draw.GradientRect(g_ScreenSize.c - (yscale / 2) + xoff, nY - (xscale / 2) + yoff, (g_ScreenSize.c - (yscale / 2) + xoff + yscale), (nY - (xscale / 2) + yoff + xscale), { color1 }, { color2 }, TRUE);
								g_Draw.String(FONT_ESP_COND, g_ScreenSize.c - (yscale / 2 + 1) + xoff, nY - (xscale / 2 + 1) - 10 + yoff, { 255, 255, 255, 255 }, ALIGN_DEFAULT, _(L"CHARGE"));
								if (g_GlobalInfo.m_nShifted == 0) // no charge no money
								{
									g_Draw.String(FONT_ESP_COND, (g_ScreenSize.c - (yscale / 2) + xoff + yscale), nY - (xscale / 2 + 1) - 10 + yoff, { 255, 55, 40, 255 }, ALIGN_REVERSE, _(L"NO CHARGE"));
									g_Draw.Rect(g_ScreenSize.c - (yscale / 2) + xoff, nY - (xscale / 2) + yoff, yscale, xscale, { 17, 24, 26, 255 });
								}
								else if (g_GlobalInfo.m_bRecharging) // charging 
								{
									g_Draw.String(FONT_ESP_COND, (g_ScreenSize.c - (yscale / 2) + xoff + yscale), nY - (xscale / 2 + 1) - 10 + yoff, { 255, 126, 0, 255 }, ALIGN_REVERSE, _(L"CHARGING"));
									g_Draw.Rect(g_ScreenSize.c - (yscale / 2) + (yscale * ratio) + xoff, nY - (xscale / 2) + yoff, yscale - (yscale * ratio) + 1 /*float is kind of shit*/, xscale, {17, 24, 26, 255});
								}
								else if (ratio != 1) // disgusting btw
								{
									g_Draw.String(FONT_ESP_COND, (g_ScreenSize.c - (yscale / 2) + xoff + yscale), nY - (xscale / 2 + 1) - 10 + yoff, { 66, 255, 0, 255 }, ALIGN_REVERSE, _(L"READY"));
									g_Draw.Rect(g_ScreenSize.c - (yscale / 2) + (yscale * ratio) + xoff, nY - (xscale / 2) + yoff, yscale - (yscale * ratio) + 1 /*float is kind of shit*/, xscale, { 17, 24, 26, 255 });
								}
								else if (!g_GlobalInfo.m_nWaitForShift) // activates when ready
								{
									g_Draw.String(FONT_ESP_COND, (g_ScreenSize.c - (yscale / 2) + xoff + yscale), nY - (xscale / 2 + 1) - 10 + yoff, { 66, 255, 0, 255 }, ALIGN_REVERSE, _(L"READY"));
								}
								else // activates when waiting blah blah blahg
								{
									g_Draw.String(FONT_ESP_COND, (g_ScreenSize.c - (yscale / 2) + xoff + yscale), nY - (xscale / 2 + 1) - 10 + yoff, { 255, 46, 46, 255 }, ALIGN_REVERSE, _(L"DT IMPOSSIBLE"));
								}
							}

						}
					}
				}

				//Current Active Aimbot FOV
				if (Vars::Visuals::AimFOVAlpha.m_Var && g_GlobalInfo.m_flCurAimFOV)
				{
					if (const auto& pLocal = g_EntityCache.m_pLocal)
					{
						float flFOV = static_cast<float>(Vars::Visuals::FieldOfView.m_Var);
						float flR = tanf(DEG2RAD(g_GlobalInfo.m_flCurAimFOV) / 2.0f)
							/ tanf(DEG2RAD((pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.m_Var) ? 30.0f : flFOV) / 2.0f) * g_ScreenSize.w;
						Color_t clr = Colors::FOVCircle;
						g_Draw.OutlinedCircle(g_ScreenSize.w / 2, g_ScreenSize.h / 2, flR, 68, clr);
					}
				}
			};
			OtherDraws();
			g_Misc.BypassPure();
			g_ESP.Run();
			g_SpyWarning.Run();
			g_PlayerArrows.Run();
			g_SpectatorList.Run();
			g_Radar.Run();
			g_Crits.Frame();
			g_Menu.Run();

			if (const auto& pLocal = g_EntityCache.m_pLocal) {
				if (pLocal->IsScoped() && Vars::Visuals::RemoveScope.m_Var && Vars::Visuals::ScopeLines.m_Var) {
					g_Visuals.ScopeLines();
				}
			}
		}

		static ConVar* localplayer_visionflags = g_Interfaces.CVars->FindVar("localplayer_visionflags");
		if (localplayer_visionflags) {
			switch (Vars::Visuals::Vision.m_Var) {
			case 1:
				localplayer_visionflags->SetValue(1);
				break;
			case 2:
				localplayer_visionflags->SetValue(2);
				break;
			case 3:
				localplayer_visionflags->SetValue(4);
				break;
			default:
				break;

			}
		}

		//static ConVar* localplayer_visionflags = g_Interfaces.CVars->FindVar("localplayer_visionflags");
		//if (localplayer_visionflags) {
		//	localplayer_visionflags->nFlags |= ~(1 << 1); //FCVAR_DEVELOPMENTONLY
		//	g_Interfaces.CVars->ConsolePrintf("localplayer_visionflags found");
		//	localplayer_visionflags->SetValue("1");
		//}
		FinishDrawing(g_Interfaces.Surface);
	}
}
