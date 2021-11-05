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

						Utils::W2S(g_GlobalInfo.m_vClubPenguinClubPenguinClubPenguinClubPenguinClubPenguinClubPenguinClubPenguinClubPenguin, vProjAimStart);
						Utils::W2S(g_GlobalInfo.m_vPredictedPos, vProjAimEnd);
						g_Draw.Line(
							vProjAimStart.x,
							vProjAimStart.y,
							vProjAimEnd.x,
							vProjAimEnd.y,
							{ 255, 255, 255, 255 } //Set this to a var if u wantto idc
						);
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

							static Color_t color1, color2;

							if (g_GlobalInfo.m_nWaitForShift) {
								color1 = Colors::DtChargingLeft;
								color2 = Colors::DtChargingRight;
							}
							else {
								color1 = Colors::DtChargedLeft;
								color2 = Colors::DtChargedRight;
							}

							static int tickWidth = Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var;
							int barWidth = (tickWidth * g_GlobalInfo.dtTicks) + 2;
							if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 1) {
								g_Draw.OutlinedRect(
									g_ScreenSize.c - (barWidth / 2),
									nY + 80,
									barWidth,
									Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var,
									Colors::DtOutline
								);
								g_Draw.GradientRect(
									g_ScreenSize.c - (barWidth / 2) + 1,
									nY + 81,
									(g_ScreenSize.c - (barWidth / 2) + 1) + tickWidth * g_GlobalInfo.m_nShifted,
									nY + 81 + Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var - 2,
									color1,
									color2,
									true
								);
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
		}
		FinishDrawing(g_Interfaces.Surface);
	}
}
