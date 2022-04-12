#include "EngineVGuiHook.h"

#include "../../SDK/Includes/icons.h"
#include "../../Features/SpyWarning/SpyWarning.h"
#include "../../Features/PlayerArrows/PlayerArrows.h"
#include "../../Features/ESP/ESP.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/PlayerResource/PlayerResource.h"
#include "../../Features/CritHack/CritHack.h"
#include "../../Features/Menu/Menu.h"
#include "../../Features/Menu/SpectatorList/SpectatorList.h"
#include "../../Features/Radar/Radar.h"

void __stdcall EngineVGuiHook::Paint::Hook(int mode)
{
	static auto StartDrawing = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(
		_(L"vguimatsurface.dll"), _(L"55 8B EC 64 A1 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 14")));
	static auto FinishDrawing = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(
		_(L"vguimatsurface.dll"), _(L"55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 51 56 6A 00")));

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

			if (g_Interfaces.Client->GetPlayerView(ViewSetup))
			{
				VMatrix WorldToView = {}, ViewToProjection = {}, WorldToPixels = {};
				g_Interfaces.RenderView->GetMatricesForView(ViewSetup, &WorldToView, &ViewToProjection,
				                                            &g_GlobalInfo.m_WorldToProjection, &WorldToPixels);
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
				
				if (Vars::Aimbot::Projectile::MovementSimulation.m_Var && !g_GlobalInfo.m_vPredictedPos.IsZero())
				{
					if (Vars::Visuals::MoveSimLine.m_Var)
					for (size_t i = 0; i < g_GlobalInfo.predFutureLines.size(); i++)
					{
						Vec3 vScreenpast, vScreenfuture;
						if (Utils::W2S(g_GlobalInfo.predBeforeLines.at(i), vScreenpast))
						{
							if (Utils::W2S(g_GlobalInfo.predFutureLines.at(i), vScreenfuture))
							{
								g_Draw.Line(vScreenpast.x, vScreenpast.y, vScreenfuture.x, vScreenfuture.y,
								            {Vars::Aimbot::Projectile::PredictionColor});
							}
						}
					}
				}
				else
				{
					if (!g_GlobalInfo.m_vPredictedPos.IsZero())
					{
						if (Vars::Visuals::AimPosSquare.m_Var)
						{
							Vec3 vProjAimStart, vProjAimEnd = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);
							if (Utils::W2S(g_GlobalInfo.linearPredLine, vProjAimStart) && Utils::W2S(
								g_GlobalInfo.m_vPredictedPos, vProjAimEnd))
							{
								g_Draw.Line(
									vProjAimStart.x,
									vProjAimStart.y,
									vProjAimEnd.x,
									vProjAimEnd.y,
									{255, 255, 255, 255} //Set this to a var if u wantto idc
								);
							}
						}
					}
				}

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

							if (g_GlobalInfo.m_nWaitForShift)
							{
								color1 = Colors::DTBarIndicatorsCharging.startColour;
								color2 = Colors::DTBarIndicatorsCharging.endColour;
							}
							else
							{
								color1 = Colors::DTBarIndicatorsCharged.startColour;
								color2 = Colors::DTBarIndicatorsCharged.endColour;
							}

							if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 1)
							{
								float maxWidth = static_cast<float>(Vars::Misc::CL_Move::DTTicks.m_Var) *
									Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var;
								float dtOffset = g_ScreenSize.c - (maxWidth / 2);
								static float tickWidth = 0.f;
								static float barWidth = 0.f;
								tickWidth = (g_GlobalInfo.m_nShifted * Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var);
								barWidth = g_Draw.EaseIn(barWidth, tickWidth, 0.9f);

								g_Draw.OutlinedRect(dtOffset - 1, (g_ScreenSize.h / 2) + 49, maxWidth + 2,
								                    Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var + 2,
								                    {50, 50, 50, 210});
								g_Draw.GradientRect(dtOffset, (g_ScreenSize.h / 2) + 50, dtOffset + barWidth,
								                    (g_ScreenSize.h / 2) + 50 + Vars::Misc::CL_Move::DtbarOutlineHeight.
								                    m_Var, color1, color2, true);
							}

							else if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 3)
							{
								// put this here so we don't move menu if we r using something else, no biggie
								float rratio = (static_cast<float>(g_GlobalInfo.m_nShifted) / static_cast<float>(
									Vars::Misc::CL_Move::DTTicks.m_Var));
								static float ratio = 0.f;
								ratio = g_Draw.EaseIn(ratio, rratio, 0.9f);

								if (ratio > 1.f) { ratio = 1.f; }
								else if (ratio < 0.f) { ratio = 0.f; }
								//if the user changes ticks after charging we don't want it to be like sliding out of bounds, this stops that.

								// these are all vars in dp but fedware doesnt have the vars and i am not adding them 
								//		so i added them
								int xoff = Vars::Misc::CL_Move::DTBarX.m_Var;
								// width offset (is it called width offset who knows)
								int yoff = Vars::Misc::CL_Move::DTBarY.m_Var; // height offset
								int yscale = Vars::Misc::CL_Move::DTBarScaleY.m_Var; // height of bar
								int xscale = Vars::Misc::CL_Move::DTBarScaleX.m_Var; // width of bar

								g_Draw.OutlinedRect(g_ScreenSize.c - (xscale / 2 + 1) + xoff,
								                    nY - (yscale / 2 + 1) + yoff, (xscale + 2), (yscale + 2),
								                    Colors::DtOutline);
								g_Draw.Rect(g_ScreenSize.c - (xscale / 2) + xoff, nY - (yscale / 2) + yoff, xscale,
								            yscale, {17, 24, 26, 255});
								g_Draw.GradientRect(g_ScreenSize.c - (xscale / 2) + xoff, nY - (yscale / 2) + yoff,
								                    ((g_ScreenSize.c - (xscale / 2) + xoff) + (xscale * ratio)),
								                    (nY - (yscale / 2) + yoff + yscale), {color1}, {color2}, TRUE);
								g_Draw.String(FONT_INDICATORS, g_ScreenSize.c - (xscale / 2 + 1) + xoff,
								              nY - (yscale / 2 + 1) - 10 + yoff, {255, 255, 255, 255}, ALIGN_DEFAULT,
								              _(L"CHARGE"));
								if (g_GlobalInfo.m_nShifted == 0) // no charge no money
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
									              nY - (yscale / 2 + 1) - 10 + yoff, {255, 55, 40, 255}, ALIGN_REVERSE,
									              _(L"NO CHARGE"));
								}
								else if (g_GlobalInfo.m_bRecharging && (g_GlobalInfo.m_nWaitForShift || ratio < 1)) // charging 
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
									              nY - (yscale / 2 + 1) - 10 + yoff, {255, 126, 0, 255}, ALIGN_REVERSE,
									              _(L"CHARGING"));
								}
								else if (!g_GlobalInfo.m_nWaitForShift || ratio != 1) // activates when ready
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
									              nY - (yscale / 2 + 1) - 10 + yoff, {66, 255, 0, 255}, ALIGN_REVERSE,
									              _(L"READY"));
								}
								else // activates when waiting blah blah blahg
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
									              nY - (yscale / 2 + 1) - 10 + yoff, {255, 46, 46, 255}, ALIGN_REVERSE,
									              _(L"DT IMPOSSIBLE"));
								}
							}
						}
					}
				}

				// Build Date
				if (g_Menu.IsOpen)
				{
					g_Draw.String(FONT_MENU, 5, g_ScreenSize.h - 5 - Vars::Fonts::FONT_MENU::nTall.m_Var, { 116, 255, 48, 255 }, ALIGN_DEFAULT, _(__DATE__));
				}

				// debug
				if (Vars::Visuals::DebugInfo.m_Var)
				{
					int yoffset = 0, xoffset = 0;
					if (const int localDamage = g_PR->GetDamageByIndex(g_Interfaces.Engine->GetLocalPlayer())) {
						g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "localDamage = %d", localDamage);
						yoffset += 20;
					}

					if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon) {
						int weaponid = pWeapon->GetWeaponID();
						if (weaponid) {
							g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "weaponid = %i", weaponid);
							yoffset += 20;
						}
						int weaponindex = pWeapon->GetItemDefIndex();
						if (weaponid) {
							g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "weaponindex = %i", weaponindex);
							yoffset += 20;
						}
					}

					if (const auto& pLocal = g_EntityCache.m_pLocal) {
						int tickbase = pLocal->GetTickBase();
						if (tickbase) {
							g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "tickbase = %i", tickbase);
							yoffset += 20;
						}
						int sequence = pLocal->m_nSequence();
						if (sequence) {
							g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "sequence = %i", sequence);
							yoffset += 20;
						}
						//int animtime = pLocal->m_flAnimTime();	// unused??? always returns the same value
						//if (animtime) {
						//	g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "animtime = %i", animtime);
						//	yoffset += 20;
						//}
						float cycle = pLocal->m_flCycle();
						{
						g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "cycle = %+.1f", cycle);
						yoffset += 20;
						}
						float playbackrate = pLocal->m_flPlaybackRate();
						{
							g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "playbackrate = %+.1f", playbackrate);
							yoffset += 20;
						}
						bool clientanimations = pLocal->m_bClientSideAnimation();
						{
							Color_t clr = clientanimations ? Color_t{ 108, 255, 0, 255 } : Color_t{ 255, 118, 36, 255 };
							g_Draw.String(FONT_MENU, xoffset, yoffset, clr, ALIGN_DEFAULT, "client animating");
							yoffset += 20;
						}
						/*std::array poseparam = pLocal->GetPoseParam(); // 0 & 1, viewangles, 4 & 5, movement. and the other 20 entries do nothing?????? n1 valve
						int n = 0;
						for (; n < 24; n++) {
							g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "poseparam[%i] = %+.1f", n, poseparam[n]);
							yoffset += 20;
						}*/
					}

					/*for (const auto& Projectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
					{
						Vec3 CollideableMins = Projectile->GetCollideableMins();
						Vec3 CollideableMaxs = Projectile->GetCollideableMaxs();
						if (!CollideableMins.IsZero()) {
							g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "mins = %+.2f %+.2f %+.2f", CollideableMins.x, CollideableMins.y, CollideableMins.z);
							yoffset += 20;
						}
						if (!CollideableMaxs.IsZero()) {
							g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "maxs = %+.2f %+.2f %+.2f", CollideableMaxs.x, CollideableMaxs.y, CollideableMaxs.z);
							yoffset += 20;
						}

						model_t* pModel = Projectile->GetModel();
						if (pModel) {
							studiohdr_t* pHDR = g_Interfaces.ModelInfo->GetStudioModel(pModel);
							if (pHDR) {
								g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "hullmin = %+.2f %+.2f %+.2f", pHDR->hull_min.x, pHDR->hull_min.y, pHDR->hull_min.z);
								yoffset += 20;
								g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "hullmax = %+.2f %+.2f %+.2f", pHDR->hull_max.x, pHDR->hull_max.y, pHDR->hull_max.z);
								yoffset += 20;
							}
						}
					}*/
					
				}

				//Current Active Aimbot FOV
				if (Vars::Visuals::AimFOVAlpha.m_Var && g_GlobalInfo.m_flCurAimFOV)
				{
					if (const auto& pLocal = g_EntityCache.m_pLocal)
					{
						float flFOV = static_cast<float>(Vars::Visuals::FieldOfView.m_Var);
						float flR = tanf(DEG2RAD(g_GlobalInfo.m_flCurAimFOV) / 2.0f)
							/ tanf(
								DEG2RAD((pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.m_Var) ? 30.0f : flFOV) /
								2.0f) * g_ScreenSize.w;
						Color_t clr = Colors::FOVCircle;
						g_Draw.OutlinedCircle(g_ScreenSize.w / 2, g_ScreenSize.h / 2, flR, 68, clr);
					}
				}
			};
			OtherDraws();
			// blocks my vision to my debug info thanks
			//if (Vars::Visuals::DebugInfo.m_Var)
			//{
			//	if (const auto& pLocal = g_EntityCache.m_pLocal)
			//	{
			//		if (const int nDamage = g_PR->GetDamageByIndex(pLocal->GetIndex()))
			//		{
			//			g_Draw.String(FONT_MENU, 5, 17, { 255, 255, 255, 255 }, ALIGN_DEFAULT, _("total damage dealt: %d"), nDamage);
			//		}
			//	}
			//}
			
			g_Misc.BypassPure();
			g_ESP.Run();
			g_Visuals.PickupTimers();
			g_SpyWarning.Run();
			g_PlayerArrows.Run();
			g_SpectatorList.Run();
			g_CritHack.Draw();
			g_Radar.Run();
			
			// you can use it for more, i'm sure. - myzarfin
			g_Notifications.Think();

			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				if (pLocal->IsScoped() && Vars::Visuals::RemoveScope.m_Var && Vars::Visuals::ScopeLines.m_Var)
				{
					g_Visuals.ScopeLines();
				}
			}
		}

		static ConVar* localplayer_visionflags = g_Interfaces.CVars->FindVar("localplayer_visionflags");
		if (localplayer_visionflags)
		{
			switch (Vars::Visuals::Vision.m_Var)
			{
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
