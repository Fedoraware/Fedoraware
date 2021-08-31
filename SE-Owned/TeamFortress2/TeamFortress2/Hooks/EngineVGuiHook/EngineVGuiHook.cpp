#include "EngineVGuiHook.h"

#include "../../SDK/Includes/icons.h"
#include "../../Features/Menu/Menu.h"
#include "../../Features/SpectatorList/SpectatorList.h"
#include "../../Features/SpyWarning/SpyWarning.h"
#include "../../Features/ESP/ESP.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Radar/Radar.h"
#include "../../Features/NewWindow/NewWindow.h"
#include "../../Features/Console/Console.h"
#include "../../Features/Aimbot/AimbotMelee/AimbotMelee.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Aimbot/AimbotProjectile/AimbotProjectile.h"

int ticksChoked = 0;





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

				//Projectile Aim's Predicted Position
				if (!g_GlobalInfo.m_vPredictedPos.IsZero())
				{
					if (Vars::Visuals::CrosshairAimPos.m_Var ? g_GlobalInfo.m_vAimPos.IsZero() : true)
					{
						static const int size = 10;
						Vec3 vecScreen = Vec3();

						if (Utils::W2S(g_GlobalInfo.m_vPredictedPos, vecScreen))
						{
							g_Draw.OutlinedRect(
								static_cast<int>(vecScreen.x - (size / 2)),
								static_cast<int>(vecScreen.y - (size / 2)),
								size, size,
								{ 0, 255, 0, 255 });

							g_Draw.OutlinedRect(
								static_cast<int>(vecScreen.x - (size / 2)) - 1,
								static_cast<int>(vecScreen.y - (size / 2)) - 1,
								size + 2, size + 2,
								Colors::OutlineESP);

							g_Draw.OutlinedRect(
								static_cast<int>(vecScreen.x - (size / 2)) + 1,
								static_cast<int>(vecScreen.y - (size / 2)) + 1,
								size - 2, size - 2,
								Colors::OutlineESP);
						}
					}
				}

				//Tickbase info
				if (Vars::Misc::CL_Move::Enabled.m_Var)
				{
					const auto& pLocal  = g_EntityCache.m_pLocal;
					const auto& pWeapon = g_EntityCache.m_pLocalWeapon;

					if (pLocal && pWeapon)
					{
						if (pLocal->GetLifeState() == LIFE_ALIVE)
						{

							const int nY = (g_ScreenSize.h / 2) + 20;

							int ticks;

							if (pLocal->GetClassNum() == CLASS_HEAVY) {

								for (int i = MAX_NEW_COMMANDS_HEAVY; i >= 0; i--) {
									//printf("i: %d\n", i);
									for (int j = MAX_NEW_COMMANDS_HEAVY - g_GlobalInfo.m_nShifted; j <= MAX_NEW_COMMANDS_HEAVY; j++) {
										//printf("j: %d\n", j);
										ticksChoked = j;
										break;
									}
								}
								ticks = MAX_NEW_COMMANDS_HEAVY;
							}
							else {
								for (int i = MAX_NEW_COMMANDS; i >= 0; i--) {
									//printf("i: %d\n", i);
									for (int j = MAX_NEW_COMMANDS - g_GlobalInfo.m_nShifted; j <= MAX_NEW_COMMANDS; j++) {
										//printf("j: %d\n", j);
										ticksChoked = j;
										break;
									}
								}
								ticks = MAX_NEW_COMMANDS;
							}

							//g_AimbotProjectile.DrawTrace(Trace);

							Color_t color1 = g_GlobalInfo.m_nWaitForShift ? Color_t{ 255, 192, 81, 180 } : Color_t{ 106, 255, 131, 180 };
							Color_t color2 = g_GlobalInfo.m_nWaitForShift ? Color_t{ 255, 134, 81, 180 } : Color_t{ 106, 255, 250, 180 };
							//g_Draw.String(FONT_MENU, g_ScreenSize.c, nY - 100, { 255, 64, 64, 255 }, ALIGN_CENTERHORIZONTAL, _(L"Ticks Choked: %i "), ticksChoked);
							//g_Draw.String(FONT_MENU, g_ScreenSize.c, nY - 100, { 255, 64, 64, 255 }, ALIGN_CENTERHORIZONTAL, Vars::Skybox::SkyboxName.c_str());
							int tickWidth = 8;
							int barWidth = (tickWidth * ticks) + 2;
							g_Draw.OutlinedRect( // Outline of bar
								g_ScreenSize.c - (barWidth / 2), // get center of screen, then subtract the width of the bar divided by 2 (to start  the bar)
								nY + 80, // vertical center + 80px
								barWidth, // width of outlined rect
								12, // height (hardcoded)
								{ 30, 30, 30, 180 } // color
							);
							g_Draw.GradientRect(
								g_ScreenSize.c - (barWidth / 2) + 1, // the bar
								nY + 81,
								(g_ScreenSize.c - (barWidth / 2) + 1) + tickWidth * ticksChoked,
								nY + 81 + 10,
								color1,
								color2,
								true
							);

						}
					}
				}
				
				//Current Active Aimbot FOV
				if (Vars::Visuals::AimFOVAlpha.m_Var && g_GlobalInfo.m_flCurAimFOV)
				{
					if (const auto &pLocal = g_EntityCache.m_pLocal)
					{
						float flFOV = static_cast<float>(Vars::Visuals::FieldOfView.m_Var);
						float flR = tanf(DEG2RAD(g_GlobalInfo.m_flCurAimFOV) / 2.0f)
							/ tanf(DEG2RAD((pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.m_Var) ? 30.0f : flFOV) / 2.0f) * g_ScreenSize.w;
						Color_t clr = Colors::FOVCircle;
						clr.a = static_cast<byte>(Vars::Visuals::AimFOVAlpha.m_Var);
						g_Draw.OutlinedCircle(g_ScreenSize.w / 2, g_ScreenSize.h / 2, flR, 68, clr);
					}
				}
			};
			OtherDraws();

			//Debug
			{

			}
			
			g_Misc.BypassPure();
			g_ESP.Run();
			g_SpyWarning.Run();
			g_SpectatorList.Run();
			g_Radar.Run();
			g_NewWindow.Run();
			g_Console.Run();
			g_Menu.Run();
		}	
		FinishDrawing(g_Interfaces.Surface);
	}
}