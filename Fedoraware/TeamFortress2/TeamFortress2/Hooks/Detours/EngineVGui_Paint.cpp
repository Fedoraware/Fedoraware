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
#include "../../Features/Menu/DTBar/DTBar.h"
#include "../../Features/Radar/Radar.h"
#include "../../Features/Followbot/Followbot.h"

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
			auto OtherDraws = [&]() -> void
			{
				if (I::EngineVGui->IsGameUIVisible())
				{
					if (!I::Engine->IsInGame())
					{
						g_Draw.String(FONT_INDICATORS, g_ScreenSize.c, 200, Utils::Rainbow(), EStringAlign::ALIGN_CENTERHORIZONTAL, "Happy pride month!!!!!!");
					}
					return;
				}

				//Proj aim line
				//This could use alot of improvement, but still subjectively better than a flying rec
				//Credits to JAGNEmk aka me x)

				if (!G::PredictedPos.IsZero())
				{
					if (Vars::Visuals::MoveSimLine.Value)
					{
						for (size_t i = 0; i < G::PredFutureLines.size(); i++)
						{
							Vec3 vScreenpast, vScreenfuture;
							if (Utils::W2S(G::PredBeforeLines.at(i), vScreenpast))
							{
								if (Utils::W2S(G::PredFutureLines.at(i), vScreenfuture))
								{
									g_Draw.Line(vScreenpast.x, vScreenpast.y, vScreenfuture.x, vScreenfuture.y,
												{ Vars::Aimbot::Projectile::PredictionColor });
								}
							}
						}
					}
				}
				else
				{
					if (!G::PredictedPos.IsZero())
					{
						if (Vars::Visuals::AimPosSquare.Value)
						{
							Vec3 vProjAimStart, vProjAimEnd = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);
							if (Utils::W2S(G::LinearPredLine, vProjAimStart) && Utils::W2S(
								G::PredictedPos, vProjAimEnd))
							{
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
				}

				//Tickbase info
				if (Vars::Misc::CL_Move::Enabled.Value)
				{
					const auto& pLocal = g_EntityCache.GetLocal();
					const auto& pWeapon = g_EntityCache.GetWeapon();

					if (pLocal && pWeapon)
					{
						if (pLocal->GetLifeState() == LIFE_ALIVE)
						{
							const int nY = (g_ScreenSize.h / 2) + 20;

							static Color_t color1, color2;

							if (G::WaitForShift)
							{
								color1 = Colors::DTBarIndicatorsCharging.startColour;
								color2 = Colors::DTBarIndicatorsCharging.endColour;
							}
							else
							{
								color1 = Colors::DTBarIndicatorsCharged.startColour;
								color2 = Colors::DTBarIndicatorsCharged.endColour;
							}

							// Default DT Bar
							if (Vars::Misc::CL_Move::DTBarStyle.Value == 1)
							{
								const auto maxWidth = static_cast<float>(Vars::Misc::CL_Move::DTTicks.Value * Vars::Misc::CL_Move::DtbarOutlineWidth.Value);
								const float dtOffset = g_ScreenSize.c - (maxWidth / 2);
								static float tickWidth = 0.f;
								static float barWidth = 0.f;
								tickWidth = (G::ShiftedTicks * Vars::Misc::CL_Move::DtbarOutlineWidth.Value);
								barWidth = g_Draw.EaseIn(barWidth, tickWidth, 0.9f);

								g_Draw.OutlinedRect(dtOffset - 1, (g_ScreenSize.h / 2) + 49, maxWidth + 2,
													Vars::Misc::CL_Move::DtbarOutlineHeight.Value + 2,
													{ 50, 50, 50, 210 });
								g_Draw.GradientRect(dtOffset, (g_ScreenSize.h / 2) + 50, dtOffset + barWidth,
													(g_ScreenSize.h / 2) + 50 + Vars::Misc::CL_Move::DtbarOutlineHeight.
													Value, color1, color2, true);
							}

							// Rijin DT Bar
							else if (Vars::Misc::CL_Move::DTBarStyle.Value == 3)
							{
								F::DTBar.Run();
								// put this here so we don't move menu if we r using something else, no biggie
								const float rratio = (static_cast<float>(G::ShiftedTicks) / static_cast<float>(
									Vars::Misc::CL_Move::DTTicks.Value));
								static float ratio = 0.f;
								ratio = g_Draw.EaseIn(ratio, rratio, 0.9f);

								if (ratio > 1.f) { ratio = 1.f; }
								else if (ratio < 0.f) { ratio = 0.f; }
								//if the user changes ticks after charging we don't want it to be like sliding out of bounds, this stops that.

								// these are all vars in dp but fedware doesnt have the vars and i am not adding them 
								//		so i added them
								const int xoff = Vars::Misc::CL_Move::DTBarX.Value;
								// width offset (is it called width offset who knows)
								const int yoff = Vars::Misc::CL_Move::DTBarY.Value; // height offset
								const int yscale = Vars::Misc::CL_Move::DTBarScaleY.Value; // height of bar
								const int xscale = Vars::Misc::CL_Move::DTBarScaleX.Value; // width of bar

								g_Draw.OutlinedRect(g_ScreenSize.c - (xscale / 2 + 1) + xoff,
													nY - (yscale / 2 + 1) + yoff, (xscale + 2), (yscale + 2),
													Colors::DtOutline);
								g_Draw.Rect(g_ScreenSize.c - (xscale / 2) + xoff, nY - (yscale / 2) + yoff, xscale,
											yscale, { 17, 24, 26, 255 });
								g_Draw.GradientRect(g_ScreenSize.c - (xscale / 2) + xoff, nY - (yscale / 2) + yoff,
													((g_ScreenSize.c - (xscale / 2) + xoff) + (xscale * ratio)),
													(nY - (yscale / 2) + yoff + yscale), { color1 }, { color2 }, TRUE);
								g_Draw.String(FONT_INDICATORS, g_ScreenSize.c - (xscale / 2 + 1) + xoff,
											  nY - (yscale / 2 + 1) - 10 + yoff, { 255, 255, 255, 255 }, ALIGN_DEFAULT,
											  _(L"CHARGE"));
								if (G::ShiftedTicks == 0) // no charge no money
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
												  nY - (yscale / 2 + 1) - 10 + yoff, { 255, 55, 40, 255 }, ALIGN_REVERSE,
												  _(L"NO CHARGE"));
								}
								else if (G::Recharging && (G::WaitForShift || ratio < 1)) // charging 
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
												  nY - (yscale / 2 + 1) - 10 + yoff, { 255, 126, 0, 255 }, ALIGN_REVERSE,
												  _(L"CHARGING"));
								}
								else if (!G::WaitForShift || ratio != 1) // activates when ready
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
												  nY - (yscale / 2 + 1) - 10 + yoff, { 66, 255, 0, 255 }, ALIGN_REVERSE,
												  _(L"READY"));
								}
								else // activates when waiting blah blah blahg
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
												  nY - (yscale / 2 + 1) - 10 + yoff, { 255, 46, 46, 255 }, ALIGN_REVERSE,
												  _(L"DT IMPOSSIBLE"));
								}
							}
						}
					}
				}

				// Build Date
				if (F::Menu.IsOpen)
				{
					g_Draw.String(FONT_MENU, 5, g_ScreenSize.h - 5 - Vars::Fonts::FONT_MENU::nTall.Value, { 116, 255, 48, 255 }, ALIGN_DEFAULT, _(__DATE__));
				}

				// Debug info
				if (Vars::Debug::DebugInfo.Value)
				{
					int yoffset = 20, xoffset = 10;

					if (const auto& pLocal = g_EntityCache.GetLocal())
					{
						// header
						{
							g_Draw.String(FONT_MENU, xoffset, yoffset += 15, { 119, 255, 225, 255 }, ALIGN_DEFAULT, "Local Player");
						}
						// alive
						{
							const bool alive = pLocal->IsAlive();
							Color_t clr = alive ? Color_t{ 153, 232, 0, 255} : Color_t{ 167, 0, 0, 255};
							g_Draw.String(FONT_MENU, xoffset, yoffset += 15, clr, ALIGN_DEFAULT, "%s", alive ? "ALIVE" : "DEAD");
						}
						if (const int tickcount = I::GlobalVars->tickcount)
						{
							g_Draw.String(FONT_MENU, xoffset, yoffset += 15, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "tickcount = %i", tickcount);
						}
						// rfgl
						if (const matrix3x4& RgflCoordinateFrame = pLocal->GetRgflCoordinateFrame()) {
							for (int i = 0; i < 3; i++) {
								g_Draw.String(FONT_MENU, xoffset, yoffset += 15, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "%i  : [%.1f, %.1f, %.1f, %.1f]", i, RgflCoordinateFrame[i][0], RgflCoordinateFrame[i][1], RgflCoordinateFrame[i][2], RgflCoordinateFrame[i][3]);
							}//
						}
					}
				}

				//Current Active Aimbot FOV
				if (Vars::Visuals::AimFOVAlpha.Value && G::CurAimFOV)
				{
					if (const auto& pLocal = g_EntityCache.GetLocal())
					{
						const float flFOV = static_cast<float>(Vars::Visuals::FieldOfView.Value);
						const float flR = tanf(DEG2RAD(G::CurAimFOV) / 2.0f)
							/ tanf(
							DEG2RAD((pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.Value) ? 30.0f : flFOV) /
							2.0f) * g_ScreenSize.w;
						const Color_t clr = Colors::FOVCircle;
						g_Draw.OutlinedCircle(g_ScreenSize.w / 2, g_ScreenSize.h / 2, flR, 68, clr);
					}
				}
			};
			OtherDraws();
			
			F::ESP.Run();
			F::Visuals.PickupTimers();
			F::SpyWarning.Run();
			F::PlayerArrows.Run();
			F::Followbot.Draw();
			F::SpectatorList.Run();
			F::CritHack.Draw();
			F::Radar.Run();

			if (Vars::AntiHack::AntiAim::Active.Value)
			{
				if (const auto& pLocal = g_EntityCache.GetLocal())
				{
					static constexpr Color_t realColour = { 0, 255,0, 255 };
					static constexpr Color_t fakeColour = { 255, 0, 0, 255 };

					const auto& vOrigin = pLocal->GetAbsOrigin();

					Vec3 vScreen1, vScreen2;
					if (Utils::W2S(vOrigin, vScreen1))
					{
						constexpr auto distance = 50.f;
						if (Utils::W2S(Utils::GetRotatedPosition(vOrigin, G::RealViewAngles.y, distance), vScreen2))
							g_Draw.Line(vScreen1.x, vScreen1.y, vScreen2.x, vScreen2.y, realColour);

						if (Utils::W2S(Utils::GetRotatedPosition(vOrigin, G::FakeViewAngles.y, distance), vScreen2))
							g_Draw.Line(vScreen1.x, vScreen1.y, vScreen2.x, vScreen2.y, fakeColour);
					}
				}
			}

			// you can use it for more, i'm sure. - myzarfin
			F::Notifications.Think();

			if (const auto& pLocal = g_EntityCache.GetLocal())
			{
				if (pLocal->IsScoped() && Vars::Visuals::RemoveScope.Value && Vars::Visuals::ScopeLines.Value)
				{
					F::Visuals.ScopeLines();
				}
			}
		}

		static ConVar* localplayer_visionflags = I::CVars->FindVar("localplayer_visionflags");
		if (localplayer_visionflags)
		{
			switch (Vars::Visuals::Vision.Value)
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

		FinishDrawing(I::Surface);
	}
}