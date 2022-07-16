#include "Visuals.h"
#include "../Vars.h"

void CVisuals::DrawHitboxMatrix(CBaseEntity* pEntity, Color_t colourface, Color_t colouredge, float time)
{
	//I::DebugOverlay->ClearAllOverlays();

	const model_t* model = pEntity->GetModel();
	const studiohdr_t* hdr = I::ModelInfo->GetStudioModel(model);
	const mstudiohitboxset_t* set = hdr->GetHitboxSet(pEntity->GetHitboxSet());

	for (int i{}; i < set->numhitboxes; ++i)
	{
		const mstudiobbox_t* bbox = set->hitbox(i);
		if (!bbox)
		{
			continue;
		}

		/*if (bbox->m_radius <= 0.f) {*/
		matrix3x4 rotMatrix;
		Math::AngleMatrix(bbox->angle, rotMatrix);

		matrix3x4 matrix;
		matrix3x4 boneees[128];
		pEntity->SetupBones(boneees, 128, BONE_USED_BY_ANYTHING, I::GlobalVars->curtime);
		Math::ConcatTransforms(boneees[bbox->bone], rotMatrix, matrix);

		Vec3 bboxAngle;
		Math::MatrixAngles(matrix, bboxAngle);

		Vec3 matrixOrigin;
		Math::GetMatrixOrigin(matrix, matrixOrigin);

		I::DebugOverlay->AddBoxOverlay2(matrixOrigin, bbox->bbmin, bbox->bbmax, bboxAngle, colourface, colouredge, time);
	}
}

void CVisuals::ScopeLines(CBaseEntity* pLocal)
{
	if (pLocal->IsScoped() && Vars::Visuals::RemoveScope.Value && Vars::Visuals::ScopeLines.Value)
	{
		const int centerX = g_ScreenSize.w / 2;
		const int centerY = g_ScreenSize.h / 2;
		const Color_t line1 = { Colors::NoscopeLines1.r, Colors::NoscopeLines1.g, Colors::NoscopeLines1.b, 255 };
		const Color_t line2 = { Colors::NoscopeLines2.r, Colors::NoscopeLines2.g, Colors::NoscopeLines2.b, 255 };

		g_Draw.GradientRect(g_ScreenSize.w / 2, centerY - 1, g_ScreenSize.w, centerY + 1, line1, line2, true);
		g_Draw.GradientRect(0, centerY - 1, centerX, centerY + 1, line2, line1, true);
		g_Draw.GradientRect(centerX - 1, 0, centerX + 1, centerY, line2, line1, false);
		g_Draw.GradientRect(centerX - 1, centerY, centerX + 1, g_ScreenSize.h, line1, line2, false);

	}
}

void CVisuals::SkyboxChanger()
{
	using LoadNamedSkysFn = bool(_cdecl*)(const char*);
	static auto LoadSkys = (LoadNamedSkysFn)g_Pattern.Find(
		_(L"engine.dll"), _(L"55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 56 57 8B 01 C7 45"));

	const char* skybNames[] = {
		"Custom",
		"sky_tf2_04",
		"sky_upward",
		"sky_dustbowl_01",
		"sky_goldrush_01",
		"sky_granary_01",
		"sky_well_01",
		"sky_gravel_01",
		"sky_badlands_01",
		"sky_hydro_01",
		"sky_night_01",
		"sky_nightfall_01",
		"sky_trainyard_01",
		"sky_stormfront_01",
		"sky_morningsnow_01",
		"sky_alpinestorm_01",
		"sky_harvest_01",
		"sky_harvest_night_01",
		"sky_halloween",
		"sky_halloween_night_01",
		"sky_halloween_night2014_01",
		"sky_island_01",
		"sky_rainbow_01"
	};
	if (Vars::Visuals::SkyboxChanger.Value)
	{
		if (Vars::Skybox::SkyboxNum == 0)
		{
			if (Vars::Misc::BypassPure.Value)
			{
				LoadSkys(Vars::Skybox::SkyboxName.c_str());
			}
			else
			{
				LoadSkys(I::CVars->FindVar("sv_skyname")->GetString());
			}
		}
		else
		{
			LoadSkys(skybNames[Vars::Skybox::SkyboxNum]);
		}
	}
	else
	{
		LoadSkys(I::CVars->FindVar("sv_skyname")->GetString());
	}
}

bool CVisuals::RemoveScope(int nPanel)
{
	if (!Vars::Visuals::RemoveScope.Value) { return false; }

	if (!m_nHudZoom && Hash::IsHudScope(I::Panel->GetName(nPanel)))
	{
		m_nHudZoom = nPanel;
	}

	return (nPanel == m_nHudZoom);
}

void CVisuals::FOV(CViewSetup* pView)
{
	CBaseEntity* pLocal = g_EntityCache.GetLocal();

	if (pLocal && pView)
	{
		if (pLocal->GetObserverMode() == OBS_MODE_FIRSTPERSON || (pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.Value))
		{
			return;
		}

		pView->fov = static_cast<float>(Vars::Visuals::FieldOfView.Value);

		if (pLocal->IsAlive())
		{
			pLocal->SetFov(Vars::Visuals::FieldOfView.Value);
			pLocal->m_iDefaultFOV() = Vars::Visuals::FieldOfView.Value;
		}
	}
}

void CVisuals::ThirdPerson(CViewSetup* pView)
{
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		// Toggle key
		if (Vars::Visuals::ThirdPersonKey.Value)
		{
			if (!I::EngineVGui->IsGameUIVisible() && !I::Surface->IsCursorVisible())
			{
				static KeyHelper tpKey{ &Vars::Visuals::ThirdPersonKey.Value };
				if (tpKey.Pressed())
				{
					Vars::Visuals::ThirdPerson.Value = !Vars::Visuals::ThirdPerson.Value;
				}
			}
		}

		const bool bIsInThirdPerson = I::Input->CAM_IsThirdPerson();

		if (!Vars::Visuals::ThirdPerson.Value
			|| ((!Vars::Visuals::RemoveScope.Value || !Vars::Visuals::RemoveZoom.Value) && pLocal->IsScoped()))
		{
			if (bIsInThirdPerson)
			{
				pLocal->ForceTauntCam(0);
			}

			return;
		}

		if (!bIsInThirdPerson)
		{
			pLocal->ForceTauntCam(1);
		}

		// Thirdperson angles
		if (bIsInThirdPerson && Vars::Visuals::ThirdPersonSilentAngles.Value)
		{
			I::Prediction->SetLocalViewAngles(G::RealViewAngles);
			if (Vars::Visuals::ThirdPersonInstantYaw.Value)
			{
				if (const auto& pAnimState = pLocal->GetAnimState())
				{
					pAnimState->m_flCurrentFeetYaw = G::RealViewAngles.y;
				}
			}
		}

		// Thirdperson offset
		if (bIsInThirdPerson && Vars::Visuals::ThirdpersonOffset.Value)
		{
			const Vec3 viewangles = I::Engine->GetViewAngles(); // Use engine view angles so anti aim doesn't make your camera go crazy mode
			Vec3 vForward, vRight, vUp;
			Math::AngleVectors(viewangles, &vForward, &vRight, &vUp);
			static KeyHelper offsetKey{ &Vars::Visuals::ThirdpersonArrowOffsetKey.Value };

			if (Vars::Visuals::ThirdpersonOffsetWithArrows.Value)
			{
				if (offsetKey.Down())
				{
					if (GetAsyncKeyState(VK_UP) & 0x8000)
					{
						arrowUp += 1.5f;
					}

					if (GetAsyncKeyState(VK_DOWN) & 0x8000)
					{
						arrowUp -= 1.5f;
					}

					if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
					{
						arrowRight += 1.5f;
					}

					if (GetAsyncKeyState(VK_LEFT) & 0x8000)
					{
						arrowRight -= 1.5f;
					}
				}

				pView->origin += vRight * arrowRight;
				pView->origin += vUp * arrowUp;
				pView->origin += vForward * Vars::Visuals::ThirdpersonDist.Value;
			}
			else
			{
				pView->origin += vRight * Vars::Visuals::ThirdpersonRight.Value;
				pView->origin += vUp * Vars::Visuals::ThirdpersonUp.Value;
				pView->origin += vForward * Vars::Visuals::ThirdpersonDist.Value;
			}
		}
	}
}


void CVisuals::BulletTrace(CBaseEntity* pEntity, Color_t color)
{
	Vector forward, src, dst;
	CGameTrace tr;
	Ray_t ray;
	CTraceFilterHitscan filter;

	Math::AngleVectors(pEntity->GetEyeAngles(), &forward);
	filter.pSkip = pEntity;
	const Vector src3D = pEntity->GetBonePos(6) - Vector(0, 0, 0);
	const Vector dst3D = src3D + (forward * 1000);

	ray.Init(src3D, dst3D);

	I::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	if (!Utils::W2S(src3D, src) || !Utils::W2S(tr.vEndPos, dst))
	{
		return;
	}

	//I::Surface->DrawLine(src.x, src.y, dst.x, dst.y);
	g_Draw.Line(src.x, src.y, dst.x, dst.y, color);
}

void DebugLine(const char* title, const char* value, std::pair<int, int> offsets, Color_t clr = {255, 255, 255, 255}) {
	g_Draw.String(FONT_MENU, offsets.first, offsets.second += 15, clr, ALIGN_DEFAULT, title);
	g_Draw.String(FONT_MENU, offsets.first + 125, offsets.second, clr, ALIGN_DEFAULT, value);
}

void CVisuals::DrawDebugInfo(CBaseEntity* pLocal)
{
	// Debug info
	if (Vars::Debug::DebugInfo.Value)
	{
		int yoffset = 10, xoffset = 10;

			// header
		{
			g_Draw.String(FONT_MENU, xoffset, yoffset += 15, { 119, 255, 225, 255 }, ALIGN_DEFAULT, "Local Player");
		}
		// alive
		{
			const bool alive = pLocal->IsAlive();
			Color_t clr = alive ? Color_t{ 153, 232, 0, 255 } : Color_t{ 167, 0, 0, 255 };
			g_Draw.String(FONT_MENU, xoffset, yoffset += 15, clr, ALIGN_DEFAULT, "%s", alive ? "ALIVE" : "DEAD");
		}
		if (const int tickcount = I::GlobalVars->tickcount)
		{
			DebugLine("TickCount", tfm::format(": %i", tickcount).c_str(), { xoffset, yoffset }); yoffset += 15;
		}

		{	//	movement data to help me make epic strafe prediction!
			const Vec3 m_vecVelocity = pLocal->m_vecVelocity();
			const Vec3 m_vecViewOffset = pLocal->m_vecViewOffset();
			const Vec3 m_vecOrigin = pLocal->m_vecOrigin();
			const Vec3 eyePosition = pLocal->GetEyePosition();;
			DebugLine("m_vecVelocity", tfm::format(": [%.1f, %.1f, %.1f]", m_vecVelocity.x, m_vecVelocity.y, m_vecVelocity.z).c_str(), { xoffset, yoffset }); yoffset += 15;
			DebugLine("playerVelocity", tfm::format(": [%.1f]", m_vecVelocity.Length2D()).c_str(), { xoffset, yoffset }); yoffset += 15;
			DebugLine("m_vecViewOffset", tfm::format(": [%.1f, %.1f, %.1f]", m_vecViewOffset.x, m_vecViewOffset.y, m_vecViewOffset.z).c_str(), { xoffset, yoffset }); yoffset += 15;
			DebugLine("m_vecOrigin", tfm::format(": [%.1f, %.1f, %.1f]", m_vecOrigin.x, m_vecOrigin.y, m_vecOrigin.z).c_str(), { xoffset, yoffset }); yoffset += 15;
			DebugLine("eyePosition", tfm::format(": [%.1f, %.1f, %.1f]", eyePosition.x, eyePosition.y, eyePosition.z).c_str(), { xoffset, yoffset }); yoffset += 15;
		}

		{
			const int MaxSpeed = pLocal->GetMaxSpeed();
			DebugLine("MaxSpeed", tfm::format(": %d", MaxSpeed).c_str(), { xoffset, yoffset }); yoffset += 15;
		}
	}
}

void CVisuals::DrawAntiAim(CBaseEntity* pLocal)
{
	if (Vars::AntiHack::AntiAim::Active.Value)
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

#include "../../Features/Menu/DTBar/DTBar.h"
#include "../../Resources/DVD-Icon.h"

void CVisuals::DrawTickbaseInfo(CBaseEntity* pLocal)
{
	//Tickbase info
	if (Vars::Misc::CL_Move::Enabled.Value)
	{

		const auto& pWeapon = g_EntityCache.GetWeapon();

		if (pWeapon)
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

				switch (Vars::Misc::CL_Move::DTBarStyle.Value)
				{
					case 0:
						return;
					case 1:
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
						break;
					}
					case 2:
					{
						const auto fontHeight = Vars::Fonts::FONT_INDICATORS::nTall.Value;
						g_Draw.String(FONT_INDICATORS, g_ScreenSize.c, 800 - fontHeight - 3, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"Ticks %d/22", G::ShiftedTicks);
						g_Draw.RoundedBoxStatic(g_ScreenSize.c - 50, 800, 100, 12, 4, { 0,0,0,170 });
						const int chargeWidth = Math::RemapValClamped(G::ShiftedTicks, 0, 22, 0, 96);
						if (G::ShiftedTicks)
						{
							g_Draw.RoundedBoxStatic(g_ScreenSize.c - 48, 802, chargeWidth, 8, 4, Vars::Menu::Colors::MenuAccent);
						}
						break;
					}
					case 3:
					{
						F::DTBar.Run();
					// put this here so we don't move menu if we r using something else, no biggie
						const float rratio = (static_cast<float>(G::ShiftedTicks) / static_cast<float>(
							Vars::Misc::CL_Move::DTTicks.Value));
						static float ratio = 0.f;
						ratio = g_Draw.EaseIn(ratio, rratio, .995f);

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
						else if (!G::WaitForShift && ratio == 1) // activates when ready
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
						break;
					}
				}
			}
		}
	}
}

void CVisuals::DrawMenuSnow()
{
	{	//	menu snow
		struct snowFlake
		{
			std::pair<int, int> position;
		};

		static std::vector<snowFlake> vSnowFlakes;
		constexpr int snowCount = 1000;

		static bool bInit = false;
		if (!bInit)
		{
			for (int i = 0; i < snowCount; i++)
			{
				vSnowFlakes.push_back({ {Utils::RandIntSimple(0, g_ScreenSize.w), Utils::RandIntSimple(0, g_ScreenSize.h / 2.f)} });
			}
			bInit = true;
		}

		for (snowFlake& flake : vSnowFlakes)
		{
//	do gravity
			constexpr int drift = 1;
			flake.position.first += Utils::RandFloatRange(-drift, drift);
			flake.position.second += drift;

			//	calculate alpha
			float Alpha = Math::MapFloat(flake.position.second, 0.0f, g_ScreenSize.h / 2.f, 1.0f, 0.0f);
			//
			//	recreate snow flakes that are gone
			if (Alpha <= 0.f || flake.position.first >= g_ScreenSize.w || flake.position.first <= 0)
			{
				flake = { {
						Utils::RandIntSimple(0, g_ScreenSize.w),
						Utils::RandIntSimple(0, 100),
				},
				};
			}//

			Color_t flakeColour = { 255, 255, 255, static_cast<byte>(Alpha * 255.0f) };
			g_Draw.String(FONT_MENU, flake.position.first, flake.position.second, flakeColour, ALIGN_DEFAULT, "*");
		}
	}
}

void CVisuals::DrawDVD()
{
	{
		static int iDVD = g_Draw.CreateTextureFromArray(DVDIcon::rawData, 237, 139);

				// DVD Logo
		if (iDVD && Vars::Menu::ShowDVD.Value)
		{
			static Vec2 logoPos = { 1, 1 };
			static Vec2 logoVelocity = { 1, -1 };

			if (logoPos.y <= 0 || logoPos.y >= (g_ScreenSize.h - DVDIcon::Height))
			{
				logoVelocity.y = -logoVelocity.y;
			}
			if (logoPos.x <= 0 || logoPos.x >= (g_ScreenSize.w - DVDIcon::Width))
			{
				logoVelocity.x = -logoVelocity.x;
			}
			logoPos += logoVelocity;

			I::Surface->DrawSetTexture(iDVD);
			const Color_t rainbow = Utils::Rainbow();
			I::Surface->SetDrawColor(rainbow.r, rainbow.g, rainbow.b, 255);
			I::Surface->DrawTexturedRect(logoPos.x, logoPos.y, DVDIcon::Width, DVDIcon::Height);
		}
	}
}

void CVisuals::DrawPredictionLine()
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
					{ 255, 255, 255, 255 }
				);
			}
		}
	}
}

void CVisuals::DrawMovesimLine()
{
	if (Vars::Visuals::MoveSimLine.Value)
	{
		if (!G::PredLinesBackup.empty())
		{
			for (size_t i = 1; i < G::PredLinesBackup.size(); i++)
			{
				RenderLine(G::PredLinesBackup.at(i - 1), G::PredLinesBackup.at(i), Vars::Aimbot::Projectile::PredictionColor, false);
			}
		}
	}
}

void CVisuals::RenderLine(const Vector& v1, const Vector& v2, Color_t c, bool bZBuffer)
{
	static auto RenderLineFn = reinterpret_cast<void(__cdecl*)(const Vector&, const Vector&, Color_t, bool)>(g_Pattern.Find(L"engine.dll", L"55 8B EC 81 EC ? ? ? ? 56 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 85 F6"));
	RenderLineFn(v1, v2, c, bZBuffer);
}

void CVisuals::SetVisionFlags()
{
	static ConVar* localplayer_visionflags = I::CVars->FindVar("localplayer_visionflags");
	if (localplayer_visionflags)
	{
		switch (Vars::Visuals::VisionModifier.Value)
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
}

void CVisuals::DrawAimbotFOV(CBaseEntity* pLocal)
{
	//Current Active Aimbot FOV
	if (Vars::Visuals::AimFOVAlpha.Value && Vars::Aimbot::Global::AimFOV.Value)
	{
		const float flFOV = static_cast<float>(Vars::Visuals::FieldOfView.Value);
		const float flR = tanf(DEG2RAD(Vars::Aimbot::Global::AimFOV.Value) / 2.0f)
			/ tanf(
			DEG2RAD((pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.Value) ? 30.0f : flFOV) /
			2.0f) * g_ScreenSize.w;
		const Color_t clr = Colors::FOVCircle;
		g_Draw.OutlinedCircle(g_ScreenSize.w / 2, g_ScreenSize.h / 2, flR, 68, clr);
	}
}


void CVisuals::StoreMaterialHandles()
{
	for (MaterialHandle_t h = I::MatSystem->First(); h != I::MatSystem->Invalid(); h = I::MatSystem->Next(h))
	{
		if (const auto& pMaterial = I::MatSystem->Get(h))
		{
			if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
			{
				continue;
			}

			MaterialHandleData data;

			data.Handle = h;

			data.Material = pMaterial;

			auto sGroup = std::string_view(pMaterial->GetTextureGroupName());

			data.Group = sGroup;

			auto sName = std::string_view(pMaterial->GetName());

			data.Name = sName;

			if (sGroup._Starts_with("SkyBox"))
			{
				data.GroupType = MaterialHandleData::EMatGroupType::GROUP_SKY;
			}
			else if (sGroup._Starts_with("World"))
			{
				data.GroupType = MaterialHandleData::EMatGroupType::GROUP_WORLD;
			}
			else
			{
				data.GroupType = MaterialHandleData::EMatGroupType::GROUP_OTHER;
			}

			if (sName.find("water") != std::string_view::npos || sName.find("glass") != std::string_view::npos
				|| sName.find("door") != std::string_view::npos || sName.find("tools") != std::string_view::npos
				|| sName.find("player") != std::string_view::npos || sName.find("chicken") != std::string_view::npos
				|| sName.find("wall28") != std::string_view::npos || sName.find("wall26") != std::string_view::npos
				|| sName.find("decal") != std::string_view::npos || sName.find("overlay") != std::string_view::npos
				|| sName.find("hay") != std::string_view::npos)
			{
				data.ShouldOverrideTextures = false;
			}
			else
			{
				data.ShouldOverrideTextures = true;
			}

			MaterialHandleDatas.push_back(data);
		}
	}
}

void CVisuals::ClearMaterialHandles()
{
	MaterialHandleDatas.clear();
}

// this whole section below is for world modulation
bool ModColChanged() // check if colours have been changed
{
	static auto oldW = Colors::WorldModulation;
	static auto oldS = Colors::SkyModulation;
	const auto curW = Colors::WorldModulation;
	const auto curS = Colors::SkyModulation;

	if (curW.r != oldW.r || curW.g != oldW.g || curW.b != oldW.b || curS.r != oldS.r || curS.g != oldS.g || curS.b != oldS.b)
	{
		oldW = curW;
		oldS = curS;
		return true;
	}
	return false;
}

bool ModSetChanged() // check if modulation has been switched
{
	static auto oldS = Vars::Visuals::SkyModulation.Value;
	static auto oldW = Vars::Visuals::WorldModulation.Value;
	const auto curS = Vars::Visuals::SkyModulation.Value;
	const auto curW = Vars::Visuals::WorldModulation.Value;

	if (curS != oldS || curW != oldW)
	{
		oldW = curW;
		oldS = curS;
		return true;
	}
	return false;
}

void ApplyModulation(const Color_t& clr)
{
	//for (MaterialHandle_t h = I::MatSystem->First(); h != I::MatSystem->Invalid(); h = I::
	//	MatSystem->Next(h))
	//{
	//	if (const auto& pMaterial = I::MatSystem->Get(h))
	//	{
	//		if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
	//			continue;

	//		std::string_view group(pMaterial->GetTextureGroupName());

	//		if (group.find(_(TEXTURE_GROUP_WORLD)) != group.npos)
	//		{
	//			pMaterial->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
	//		}
	//	}
	//}
	if (F::Visuals.MaterialHandleDatas.empty())
	{
		return;
	}

	for (const auto& material : F::Visuals.MaterialHandleDatas)
	{
		if (material.Material)
		{
			if (material.GroupType != CVisuals::MaterialHandleData::EMatGroupType::GROUP_WORLD)
			{
				continue;
			}
			if (material.Material->IsErrorMaterial() || !material.Material->IsPrecached())
			{
				continue;
			}
			material.Material->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
		}
	}
}

void ApplySkyboxModulation(const Color_t& clr)
{
	//for (MaterialHandle_t h = I::MatSystem->First(); h != I::MatSystem->Invalid(); h = I::
	//	MatSystem->Next(h))
	//{
	//	const auto& pMaterial = I::MatSystem->Get(h);

	//	if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
	//		continue;

	//	std::string_view group(pMaterial->GetTextureGroupName());

	//	if (group._Starts_with("SkyBox"))
	//	{
	//		pMaterial->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
	//	}
	//}
	if (F::Visuals.MaterialHandleDatas.empty())
	{
		return;
	}

	for (const auto& material : F::Visuals.MaterialHandleDatas)
	{
		if (material.Material)
		{
			if (material.GroupType != CVisuals::MaterialHandleData::EMatGroupType::GROUP_SKY)
			{
				continue;
			}
			if (material.Material->IsErrorMaterial() || !material.Material->IsPrecached())
			{
				continue;
			}
			material.Material->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
		}
	}
}

void CVisuals::ModulateWorld()
{
	static bool oConnectionState = (I::Engine->IsConnected() && I::Engine->IsInGame());
	const bool connectionState = (I::Engine->IsConnected() && I::Engine->IsInGame());
	const bool isUnchanged = connectionState == oConnectionState;
	static bool shouldModulate = false;

	if (ModColChanged() || ModSetChanged() || !isUnchanged)
	{
		Vars::Visuals::WorldModulation.Value ? ApplyModulation(Colors::WorldModulation) : ApplyModulation({ 255, 255, 255, 255 });
		Vars::Visuals::SkyModulation.Value ? ApplySkyboxModulation(Colors::SkyModulation) : ApplySkyboxModulation({ 255, 255, 255, 255 });
		oConnectionState = connectionState;
		shouldModulate = false;
	}
	else if (!Vars::Visuals::WorldModulation.Value)
	{
		if (!shouldModulate)
		{
			ApplyModulation({ 255, 255, 255, 255 });
			shouldModulate = true;
		}
	} // i don't know why i need to do this
}

void CVisuals::RestoreWorldModulation() // keep this because its mentioned in @DLLMain.cpp if you find a better way to do this, remove it ig.
{
	ApplyModulation({ 255, 255, 255, 255 });
	ApplySkyboxModulation({ 255, 255, 255, 255 });
}

// all world mod stuff above

void CVisuals::OverrideWorldTextures()
{
	static KeyValues* kv = nullptr;
	if (!kv)
	{
		kv = new KeyValues("LightmappedGeneric");
		//kv->SetString("$basetexture", "dev/dev_measuregeneric01b"); //Nitro (dev texture)
		kv->SetString("$basetexture", "vgui/white_additive"); //flat 
		kv->SetString("$color2", "[0.12 0.12 0.15]"); //grey
	}

	if (Vars::Visuals::OverrideWorldTextures.Value)
	{
		for (const auto& data : MaterialHandleDatas)
		{
			if (data.Material == nullptr)
			{
				continue;
			}

			if (data.Material->IsTranslucent() || data.Material->IsSpriteCard() || data.GroupType != MaterialHandleData::EMatGroupType::GROUP_WORLD)
			{
				continue;
			}

			if (!data.ShouldOverrideTextures)
			{
				continue;
			}

			data.Material->SetShaderAndParams(kv);
		}
	}
}

void CVisuals::PickupTimers()
{
	if (!Vars::Visuals::PickupTimers.Value) { return; }

	for (auto pickupData = PickupDatas.begin(); pickupData != PickupDatas.end();)
	{
		const float timeDiff = I::Engine->Time() - pickupData->Time;
		if (timeDiff > 10.f)
		{
			pickupData = PickupDatas.erase(pickupData);
			continue;
		}

		auto timerText = tfm::format("%.1f", 10.f - timeDiff);
		auto color = pickupData->Type ? Colors::Health : Colors::Ammo;

		Vec3 vScreen;
		if (Utils::W2S(pickupData->Location, vScreen))
		{
			g_Draw.String(FONT_ESP_PICKUPS, vScreen.x, vScreen.y, color, ALIGN_CENTER, timerText.c_str());
		}

		++pickupData;
	}
}

CClientClass* CVisuals::CPrecipitation::GetPrecipitationClass()
{
	static CClientClass* pReturn = nullptr;

	if (!pReturn)
	{
		for (auto pClass = I::Client->GetAllClasses(); pClass; pClass = pClass->m_pNext)
		{
			if (pClass->m_ClassID == static_cast<int>(ETFClassID::CPrecipitation))
			{
				pReturn = pClass;
				break;
			}
		}
	}

	return pReturn;
}

void CVisuals::CPrecipitation::Run()
{
	constexpr auto PRECIPITATION_INDEX = (MAX_EDICTS - 1);

	const auto* pRainEntity = I::EntityList->GetClientEntity(PRECIPITATION_INDEX);

	if (!pRainEntity)
	{
		const auto pClass = GetPrecipitationClass();

		if (!pClass || !pClass->m_pCreateFn)
		{
			return;
		}

		RainNetworkable = reinterpret_cast<IClientNetworkable * (__cdecl*)(int, int)>(pClass->m_pCreateFn)(PRECIPITATION_INDEX, 0);

		if (!RainNetworkable)
		{
			return;
		}

		RainEntity = I::EntityList->GetClientEntity(PRECIPITATION_INDEX);

		if (!RainEntity)
		{
			return;
		}

		static auto dwOff = GetNetVar("CPrecipitation", "m_nPrecipType");

		*reinterpret_cast<int*>(RainEntity + dwOff) = Vars::Visuals::Rain.Value - 1;

		RainEntity->Networkable()->PreDataUpdate(DATA_UPDATE_CREATED);
		RainEntity->Networkable()->OnPreDataChanged(DATA_UPDATE_CREATED);

		RainEntity->m_vecMins() = Vec3(-32767.0f, -32767.0f, -32767.0f);
		RainEntity->m_vecMaxs() = Vec3(32767.0f, 32767.0f, 32767.0f);

		RainEntity->Networkable()->OnDataChanged(DATA_UPDATE_CREATED);
		RainEntity->Networkable()->PostDataUpdate(DATA_UPDATE_CREATED);
	}
}

void CVisuals::CPrecipitation::Cleanup()
{
	RainEntity = nullptr;
	RainNetworkable = nullptr;
}
