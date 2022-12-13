#include "Visuals.h"
#include "../Vars.h"
#include "../ESP/ESP.h"

void CVisuals::DrawHitboxMatrix(CBaseEntity* pEntity, Color_t colourface, Color_t colouredge, float time)
{
	//I::DebugOverlay->ClearAllOverlays();

	const model_t* model = pEntity->GetModel();
	const studiohdr_t* hdr = I::ModelInfoClient->GetStudioModel(model);
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

void CVisuals::DrawOnScreenConditions(CBaseEntity* pLocal)
{
	// check
	if (!Vars::Visuals::DrawOnScreenConditions.Value) { return; }
	if (!pLocal->IsAlive() || pLocal->IsAGhost()) { return; }

	const int x = Vars::Visuals::OnScreenConditions.c;
	int y = Vars::Visuals::OnScreenConditions.y;

	std::vector<std::wstring> conditionsVec = F::ESP.GetPlayerConds(pLocal);

	int nTextOffset = g_Draw.m_vecFonts[FONT_MENU].nTall;
	//int longestText = 40;
	int width, height;
	for (const std::wstring& cond : conditionsVec)
	{
		g_Draw.String(FONT_MENU, x, y + nTextOffset, { 255, 255, 255, 255 }, ALIGN_CENTER, cond.data());
		I::VGuiSurface->GetTextSize(g_Draw.m_vecFonts[FONT_MENU].dwFont, cond.data(), width, height);
		//if (width > longestText)
		//{
		//	longestText = width;
		//}
		nTextOffset += height;
	}

	//	do these matter?
	//ConditionW = longestText;
	//ConditionH = y + nTextOffset;
}

void CVisuals::DrawOnScreenPing(CBaseEntity* pLocal){
	if (!Vars::Visuals::DrawOnScreenPing.Value) { return; }
	if (!pLocal->IsAlive() || pLocal->IsAGhost()) { return; }

	CTFPlayerResource* cResource = g_EntityCache.GetPR();
	if (!cResource) { return; }

	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!iNetChan) { return; }

	const float flLatencyReal = (iNetChan->GetLatency(FLOW_INCOMING) + iNetChan->GetLatency(FLOW_OUTGOING)) * 1000;
	const int flLatencyScoreBoard = cResource->GetPing(pLocal->GetIndex());

	const int x = Vars::Visuals::OnScreenPing.x;
	const int y = Vars::Visuals::OnScreenPing.y;
	const int h = Vars::Visuals::OnScreenPing.h;

	const int nTextOffset = g_Draw.m_vecFonts[FONT_MENU].nTall;
	{
		g_Draw.String(FONT_MENU, x, y, {255, 255, 255, 255 }, ALIGN_DEFAULT, "ping real : %.0f", flLatencyReal);
		g_Draw.String(FONT_MENU, x, y + h - nTextOffset, {255, 255, 255, 255 }, ALIGN_DEFAULT,	"ping scoreboard : %d", flLatencyScoreBoard);
	}
}

void CVisuals::SkyboxChanger()
{
	using LoadNamedSkysFn = bool(_cdecl*)(const char*);
	static auto LoadSkys = (LoadNamedSkysFn)g_Pattern.Find(
		L"engine.dll", L"55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 56 57 8B 01 C7 45");

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
				LoadSkys(I::Cvar->FindVar("sv_skyname")->GetString());
			}
		}
		else
		{
			LoadSkys(skybNames[Vars::Skybox::SkyboxNum]);
		}
	}
	else
	{
		LoadSkys(I::Cvar->FindVar("sv_skyname")->GetString());
	}
}

bool CVisuals::RemoveScope(int nPanel)
{
	if (!Vars::Visuals::RemoveScope.Value) { return false; }

	if (!m_nHudZoom && Hash::IsHudScope(I::VGuiPanel->GetName(nPanel)))
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
			if (!I::EngineVGui->IsGameUIVisible() && !I::VGuiSurface->IsCursorVisible())
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
			const Vec3 viewangles = I::EngineClient->GetViewAngles(); // Use engine view angles so anti aim doesn't make your camera go crazy mode
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

void DebugLine(const char* title, const char* value, std::pair<int, int> offsets, Color_t clr = { 255, 255, 255, 255 })
{
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

		if (!G::LastUserCmd){ return; }
		const float flLastFwd = G::LastUserCmd->forwardmove;
		const float flLastSde = G::LastUserCmd->sidemove;
		{
			g_Draw.String(FONT_MENU, xoffset, yoffset += 15, {255, 255, 255, 255}, ALIGN_DEFAULT, "%.0f", flLastFwd);
			g_Draw.String(FONT_MENU, xoffset, yoffset += 15, {255, 255, 255, 255}, ALIGN_DEFAULT, "%.0f", flLastSde);
		}
	}
}

void CVisuals::DrawAntiAim(CBaseEntity* pLocal)
{
	if (!pLocal->IsAlive() || !I::Input->CAM_IsThirdPerson())
	{
		return;
	}

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
			{
				g_Draw.Line(vScreen1.x, vScreen1.y, vScreen2.x, vScreen2.y, realColour);
			}

			if (Utils::W2S(Utils::GetRotatedPosition(vOrigin, G::FakeViewAngles.y, distance), vScreen2))
			{
				g_Draw.Line(vScreen1.x, vScreen1.y, vScreen2.x, vScreen2.y, fakeColour);
			}
		}
	}
}

#include "../../Resources/DVD-Icon.h"

void CVisuals::DrawTickbaseInfo(CBaseEntity* pLocal)
{
	//Tickbase info
	if (Vars::Misc::CL_Move::Enabled.Value && Vars::Misc::CL_Move::DTBarStyle.Value)
	{

		const auto& pWeapon = g_EntityCache.GetWeapon();

		if (pWeapon)
		{
			if (pLocal->GetLifeState() == LIFE_ALIVE)
			{
				const int nY = (g_ScreenSize.h / 2) + 20;
				const DragBox_t DTBox = Vars::Misc::CL_Move::DTIndicator;
				const float ratioCurrent = std::clamp(((float)G::ShiftedTicks / (float)Vars::Misc::CL_Move::DTTicks.Value), 0.0f, 1.0f);
				static float ratioInterp = 0.00f; ratioInterp = g_Draw.EaseIn(ratioInterp, ratioCurrent, 0.95f); Math::Clamp(ratioInterp, 0.00f, 1.00f);

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
					case 1:
					{
						g_Draw.OutlinedRect(DTBox.x, DTBox.y, DTBox.w, DTBox.h, Colors::DtOutline);
						g_Draw.GradientRectWH(DTBox.x + 1, DTBox.y + 1, ratioInterp * (DTBox.w - 2), DTBox.h - 2, color1, color2, true);
						break;
					}
					case 2:
					{
						const auto fontHeight = Vars::Fonts::FONT_INDICATORS::nTall.Value;
						const int drawX = DTBox.x;
						g_Draw.String(FONT_INDICATORS, DTBox.c, DTBox.y - fontHeight - 3, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"Ticks %d/%d", G::ShiftedTicks, Vars::Misc::CL_Move::DTTicks.Value);
						g_Draw.RoundedBoxStatic(DTBox.x, DTBox.y, DTBox.w, DTBox.h, 4, Colors::DtOutline);
						if (G::ShiftedTicks && ratioCurrent)
						{
							g_Draw.RoundedBoxStatic(DTBox.x + 2, DTBox.y + 2, ratioCurrent * (DTBox.w - 4), DTBox.h - 4, 4, Vars::Menu::Colors::MenuAccent);
						}
						break;
					}
					case 3:
					{
						g_Draw.OutlinedRect(DTBox.x, DTBox.y, DTBox.w, DTBox.h, Colors::DtOutline);	//	draw the outline
						g_Draw.Rect(DTBox.x + 1, DTBox.y + 1, DTBox.w - 2, DTBox.h - 2, { 28, 29, 38, 255 });	//	draw the background
						g_Draw.GradientRectWH(DTBox.x + 1, DTBox.y + 1, ratioInterp * (DTBox.w - 2), DTBox.h - 2, color1, color2, true);
						g_Draw.String(FONT_INDICATORS, DTBox.x, DTBox.y - 10, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"CHARGE");

						if (G::ShiftedTicks == 0) // chargless
						{
							g_Draw.String(FONT_INDICATORS, DTBox.x + DTBox.w, DTBox.y - 10, { 255, 55, 40, 255 }, ALIGN_REVERSE, L"NO CHARGE");
						}
						else if (G::Recharging) // charging 
						{
							g_Draw.String(FONT_INDICATORS, DTBox.x + DTBox.w, DTBox.y - 10, { 255, 126, 0, 255 }, ALIGN_REVERSE, L"CHARGING");
						}
						else if (!G::WaitForShift && ratioCurrent == 1) // ready (only show if we are fully charged)
						{
							g_Draw.String(FONT_INDICATORS, DTBox.x + DTBox.w, DTBox.y - 10, { 66, 255, 0, 255 }, ALIGN_REVERSE, L"READY");

						}
						else	//waiting 
						{
							g_Draw.String(FONT_INDICATORS, DTBox.x + DTBox.w, DTBox.y - 10, { 255, 46, 46, 255 }, ALIGN_REVERSE, L"DT IMPOSSIBLE");
						}
						break;
					}
					case 4:
					{
						if (G::ShiftedTicks == 0 || G::Recharging)
						{
							g_Draw.String(FONT_INDICATORS, DTBox.c, DTBox.y - 10, { 255, 64, 64, 255 }, ALIGN_CENTERHORIZONTAL, L"Recharge! (%i / %i)", G::ShiftedTicks, Vars::Misc::CL_Move::DTTicks.Value);
						}
						else if (G::WaitForShift)
						{
							g_Draw.String(FONT_INDICATORS, DTBox.c, DTBox.y - 10, { 255, 178, 0, 255 }, ALIGN_CENTERHORIZONTAL, L"Wait! (%i / 25)", G::WaitForShift);
						}
						else
						{
							g_Draw.String(FONT_INDICATORS, DTBox.c, DTBox.y - 10, { 153, 255, 153, 255 }, ALIGN_CENTERHORIZONTAL, L"Shift ready!");
						}
						break;
					}
					case 5:
					{
						g_Draw.String(FONT_INDICATORS, DTBox.c, DTBox.y - 3, { 255, 255, 255, 255 }, ALIGN_CENTERHORIZONTAL, L"%i/%i", G::ShiftedTicks, Vars::Misc::CL_Move::DTTicks.Value);
						break;
					}
						//hhhs0j — Today at 15:19
						//Add a dt indicator but only with numbers

				}

			}
		}
	}
}

void CVisuals::DrawServerHitboxes()
{
	static int iOldTick = I::GlobalVars->tickcount;
	if (iOldTick == I::GlobalVars->tickcount) { return; } iOldTick = I::GlobalVars->tickcount;
	// draw our serverside hitbox on local servers, used to test fakelag & antiaim
	if (I::Input->CAM_IsThirdPerson() && Vars::Visuals::ThirdPersonServerHitbox.Value)
	{
		//	i have no idea what this is
		using GetServerAnimating_t = void* (*)(int);
		static auto GetServerAnimating = reinterpret_cast<GetServerAnimating_t>(g_Pattern.Find(L"server.dll", L"55 8B EC 8B 55 ? 85 D2 7E ? A1"));

		using DrawServerHitboxes_t = void(__thiscall*)(void*, float, bool);	// C_BaseAnimating, Duration, MonoColour
		static auto DrawServerHitboxes = reinterpret_cast<DrawServerHitboxes_t>(g_Pattern.Find(L"server.dll", L"55 8B EC 83 EC ? 57 8B F9 80 BF ? ? ? ? ? 0F 85 ? ? ? ? 83 BF ? ? ? ? ? 75 ? E8 ? ? ? ? 85 C0 74 ? 8B CF E8 ? ? ? ? 8B 97"));

		const auto pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());
		if (pLocal && pLocal->IsAlive())
		{
			void* server_animating = GetServerAnimating(pLocal->GetIndex());
			if (server_animating)
			{
				DrawServerHitboxes(server_animating, I::GlobalVars->interval_per_tick, true);
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

			I::VGuiSurface->DrawSetTexture(iDVD);
			I::VGuiSurface->DrawSetColor(Utils::Rainbow());
			I::VGuiSurface->DrawTexturedRect(logoPos.x, logoPos.y, DVDIcon::Width, DVDIcon::Height);
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
			if (!Vars::Visuals::MoveSimSeperators.Value)
			{
				for (size_t i = 1; i < G::PredLinesBackup.size(); i++)
				{
					RenderLine(G::PredLinesBackup.at(i - 1).first, G::PredLinesBackup.at(i).first, Vars::Aimbot::Projectile::PredictionColor, false);
				}
			}
			else
			{
				for (size_t i = 1; i < G::PredLinesBackup.size(); i++)
				{
					const auto& vStart = G::PredLinesBackup[i - 1].first;
					const auto& vRotate = G::PredLinesBackup[i - 1].second;	//	splirp vec
					const auto& vEnd = G::PredLinesBackup[i].first;
					if ((i % Vars::Visuals::SeperatorSpacing.Value) == 0) { RenderLine(vStart, vRotate, Vars::Aimbot::Projectile::PredictionColor, false); }
					RenderLine(vStart, vEnd, Vars::Aimbot::Projectile::PredictionColor, false);
				}
			}
		}
	}
}

// Credits go to reestart entirely
void CVisuals::ManualNetwork(const StartSoundParams_t& params)
{
	if (!Vars::ESP::Main::DormantSoundESP.Value) { return; }
	if (params.soundsource <= 0) { return; }

	Vector vOrigin;
	VectorCopy(params.origin, vOrigin);
	const int iEntIndex = params.soundsource;
	CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(iEntIndex);

	if (pEntity
		&& iEntIndex != I::EngineClient->GetLocalPlayer()
		&& pEntity->GetDormant()
		&& pEntity->GetClassID() == ETFClassID::CTFPlayer)
	{
		G::DormantPlayerESP[iEntIndex] = { vOrigin, I::EngineClient->Time() };
	}
}

void CVisuals::RenderLine(const Vector& v1, const Vector& v2, Color_t c, bool bZBuffer)
{
	static auto RenderLineFn = reinterpret_cast<void(__cdecl*)(const Vector&, const Vector&, Color_t, bool)>(g_Pattern.Find(L"engine.dll", L"55 8B EC 81 EC ? ? ? ? 56 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 85 F6"));
	RenderLineFn(v1, v2, c, bZBuffer);
}

void CVisuals::DrawSightlines()
{
	if (Vars::ESP::Players::SniperSightlines.Value)
	{
		if (!m_SightLines.empty())
		{
			for (const auto& sightline : m_SightLines)
			{
				if (sightline.m_bDraw)
				{
					RenderLine(sightline.m_vStart, sightline.m_vEnd, sightline.m_Color, false);
				}
			}
		}
	}
}

void CVisuals::FillSightlines()
{
	if (Vars::ESP::Players::SniperSightlines.Value)
	{
		Vec3 vShootPos, vAngle, vForward, vShootEnd;
		CTraceFilterHitscan filter{};
		CGameTrace trace{};
		for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
		{
			const int iEntityIndex = pEnemy->GetIndex();
			if (!(pEnemy->IsAlive()) ||
				!(pEnemy->GetClassNum() == CLASS_SNIPER) ||
				!(pEnemy->GetCond() & TFCond_Zoomed) ||
				(pEnemy->GetDormant()))
			{
				m_SightLines[iEntityIndex] = { Vec3{0,0,0}, Vec3{0,0,0}, Color_t{0,0,0,0}, false };
				continue;
			}
			vShootPos = pEnemy->GetShootPos();
			vAngle = pEnemy->GetEyeAngles();
			Math::AngleVectors(vAngle, &vForward);
			vShootEnd = vShootPos + (vForward * 8192.f);

			Utils::Trace(vShootPos, vShootEnd, MASK_SHOT, &filter, &trace);

			m_SightLines[pEnemy->GetIndex()] = { vShootPos, trace.vEndPos, Utils::GetEntityDrawColor(pEnemy, Vars::ESP::Main::EnableTeamEnemyColors.Value), true };
		}
	}
}

void CVisuals::SetVisionFlags()
{
	static ConVar* localplayer_visionflags = I::Cvar->FindVar("localplayer_visionflags");
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

void CVisuals::AddBulletTracer(const Vec3& vFrom, const Vec3& vTo, const Color_t& clr)
{
	m_vecBulletTracers.push_back({ vFrom, vTo, clr, I::GlobalVars->curtime });
}

void CVisuals::PruneBulletTracers()
{
	const float curtime = I::GlobalVars->curtime;

	if (m_vecBulletTracers.size())
	{
		for (size_t i = 0; i < m_vecBulletTracers.size(); i++)
		{
			const auto& bulletTracer = m_vecBulletTracers.at(i);
			if (curtime > bulletTracer.m_flTimeCreated + 5)
			{
				m_vecBulletTracers.erase(m_vecBulletTracers.begin(), m_vecBulletTracers.begin() + 1);
			}
		}
	}
}

void CVisuals::DrawBulletTracers()
{
	const float curTime = I::GlobalVars->curtime;
	if (m_vecBulletTracers.size())
	{
		for (const auto& bulletTracer : m_vecBulletTracers)
		{
			Color_t tracerColor = bulletTracer.m_Color;
			const float flDistance = curTime - bulletTracer.m_flTimeCreated;
			if (flDistance < 0)
			{
				tracerColor.a = 255;
			}
			else
			{
				//I::Cvar->ConsolePrintf("%f\n", flDistance);
				tracerColor.a = Math::RemapValClamped(flDistance, 0, 1, 255, 0);
			}
			/*I::Cvar->ConsolePrintf("a: %d\n", tracerColor.a);*/
			RenderLine(bulletTracer.m_vStartPos, bulletTracer.m_vEndPos, tracerColor, false);
		}
	}
}

void CVisuals::DrawProjectileTracer(CBaseEntity* pLocal, const Vec3& position)
{
	if (pLocal->IsTaunting())
	{
		return;
	}

	if (!I::Prediction->m_bFirstTimePredicted)
	{
		return;
	}

	static int lastTickcount = 0;
	if (lastTickcount == I::GlobalVars->tickcount)
	{
		return;
	}

	lastTickcount = I::GlobalVars->tickcount;
	const Vec3 vecPos = G::CurWeaponType == EWeaponType::PROJECTILE ? G::PredictedPos : position;
	const Color_t tracerColor = Vars::Visuals::BulletTracerRainbow.Value ? Utils::Rainbow() : Colors::BulletTracer;
	Vec3 shootPos;
	const int iAttachment = pLocal->GetActiveWeapon()->LookupAttachment("muzzle");
	pLocal->GetActiveWeapon()->GetAttachment(iAttachment, shootPos);
	AddBulletTracer(shootPos, vecPos, tracerColor);
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
	for (MaterialHandle_t h = I::MaterialSystem->First(); h != I::MaterialSystem->Invalid(); h = I::MaterialSystem->Next(h))
	{
		if (const auto& pMaterial = I::MaterialSystem->Get(h))
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
	static bool oConnectionState = (I::EngineClient->IsConnected() && I::EngineClient->IsInGame());
	const bool connectionState = (I::EngineClient->IsConnected() && I::EngineClient->IsInGame());
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
		kv->SetString("$basetexture", "dev/dev_measuregeneric01b");
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
		const float timeDiff = I::EngineClient->Time() - pickupData->Time;
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
		for (auto pClass = I::BaseClientDLL->GetAllClasses(); pClass; pClass = pClass->m_pNext)
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

	const auto* pRainEntity = I::ClientEntityList->GetClientEntity(PRECIPITATION_INDEX);

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

		RainEntity = I::ClientEntityList->GetClientEntity(PRECIPITATION_INDEX);

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

void CRunescapeChat::Draw()
{
	if (!Vars::Misc::RunescapeChat.Value)
	{
		return;
	}
	std::vector<size_t> vecRemovals;
	const float curTime = I::GlobalVars->curtime;
	Vec3 vHeadpos;
	Vec3 vScreen;
	for (size_t i = 0; i < m_vecChats.size(); i++)
	{
		auto& chat = m_vecChats.at(i);
		if (chat.m_flTimeCreated + 4 < curTime)
		{
			vecRemovals.push_back(i);
		}
		else
		{
			if (chat.m_pEntity && chat.m_pEntity->IsAlive())
			{
				vHeadpos = chat.m_pEntity->GetHitboxPos(HITBOX_HEAD);
				if (!vHeadpos.IsZero())
				{
					vHeadpos.z += 20;
					if (Utils::W2S(vHeadpos, vScreen))
					{
						Color_t col = { 255, 255, 0, 255 };
						switch (chat.m_Colour)
						{
							case eRS_RED:
							{
								col = { 255, 0, 0, 255 };
								break;
							}
							case eRS_GREEN:
							{
								col = { 0, 255, 0, 255 };
								break;
							}
							case eRS_CYAN:
							{
								col = { 0, 255, 255, 255 };
								break;
							}
							case eRS_PURPLE:
							{
								col = { 255, 0, 255, 255 };
								break;
							}
							case eRS_WHITE:
							{
								col = { 255,255,255,255 };
								break;
							}
							default:break;
						}
						g_Draw.String(FONT_OSRS, vScreen.x, vScreen.y - (14 * chat.m_nOffset), col, ALIGN_CENTERHORIZONTAL, L"%ls", chat.m_szChatText.c_str());
					}
				}
			}
		}
	}

	for (auto& pos : vecRemovals)
	{
		m_vecChats.erase(m_vecChats.begin() + pos);
	}
}

void CRunescapeChat::PushChat(CBaseEntity* pEntity, std::wstring szChatText)
{
	if (!Vars::Misc::RunescapeChat.Value)
	{
		return;
	}
	if (!pEntity) return;

	EChatColour col = eRS_YELLOW;

	if (!szChatText.rfind(L"red:", 0))
	{
		col = eRS_RED;
		szChatText.erase(0, 4);
	}
	else if (!szChatText.rfind(L"green:", 0))
	{
		col = eRS_GREEN;
		szChatText.erase(0, 6);
	}
	else if (!szChatText.rfind(L"cyan:", 0))
	{
		col = eRS_CYAN;
		szChatText.erase(0, 5);
	}
	else if (!szChatText.rfind(L"purple:", 0))
	{
		col = eRS_PURPLE;
		szChatText.erase(0, 7);
	}
	else if (!szChatText.rfind(L"white:", 0))
	{
		col = eRS_WHITE;
		szChatText.erase(0, 6);
	}




	int highestOffset = 0;
	for (auto& chat : m_vecChats)
	{
		if (chat.m_pEntity == pEntity)
		{
			if (chat.m_nOffset >= highestOffset)
			{
				highestOffset = chat.m_nOffset + 1;
			}
		}
	}
	Chat_t push = { pEntity, I::GlobalVars->curtime, highestOffset, col, szChatText };
	m_vecChats.push_back(push);
}
