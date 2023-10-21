#include "Visuals.h"
#include "../Vars.h"
#include "../ESP/ESP.h"
#include "../NoSpread/NoSpread.h"
#include "../Aimbot/ProjectileSimulation/ProjectileSimulation.h"

namespace S
{
	MAKE_SIGNATURE(LoadSkys, ENGINE_DLL, "55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 56 57 8B 01 C7 45", 0x0);
	MAKE_SIGNATURE(GetServerAnimating, SERVER_DLL, "55 8B EC 8B 55 ? 85 D2 7E ? A1", 0x0);
	MAKE_SIGNATURE(DrawServerHitboxes, SERVER_DLL, "55 8B EC 83 EC ? 57 8B F9 80 BF ? ? ? ? ? 0F 85 ? ? ? ? 83 BF ? ? ? ? ? 75 ? E8 ? ? ? ? 85 C0 74 ? 8B CF E8 ? ? ? ? 8B 97", 0x0);
	MAKE_SIGNATURE(RenderLine, ENGINE_DLL, "55 8B EC 81 EC ? ? ? ? 56 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 85 F6", 0x0);
	MAKE_SIGNATURE(RenderBoxFace, ENGINE_DLL, "55 8B EC 51 8B 45 ? 8B C8 FF 75", 0x0);
	MAKE_SIGNATURE(RenderBoxEdge, ENGINE_DLL, "55 8B EC 81 EC ? ? ? ? 56 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 89 75 ? 85 F6 74 ? 8B 06 8B CE FF 50 ? A1", 0x0);
}

void CVisuals::Draw()
{
	if (const auto pLocal = g_EntityCache.GetLocal())
	{
		DrawAntiAim(pLocal);
		DrawTickbaseInfo(pLocal);
		DrawAimbotFOV(pLocal);
		ScopeLines(pLocal);
		DrawDebugInfo(pLocal);
		DrawOnScreenConditions(pLocal);
		DrawOnScreenPing(pLocal);
	}

	DrawNoSpreadIndicator();
	DrawServerHitboxes();
	DrawPredictionLine();
	PickupTimers();
	SetVisionFlags();
}

void CVisuals::DrawHitboxMatrix(CBaseEntity* pEntity, Color_t colourface, Color_t colouredge, float time)
{
	if (Vars::Aimbot::Global::ClearPreviousHitbox.Value) {
		I::DebugOverlay->ClearAllOverlays();
	}

	const model_t* model = pEntity->GetModel();
	const studiohdr_t* hdr = I::ModelInfoClient->GetStudioModel(model);
	const mstudiohitboxset_t* set = hdr->GetHitboxSet(pEntity->GetHitboxSet());

	for (int i = 0; i < set->numhitboxes; ++i)
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
		const Color_t line1 = {Vars::Colours::NoscopeLines1.Value.r, Vars::Colours::NoscopeLines1.Value.g, Vars::Colours::NoscopeLines1.Value.b, 255};
		const Color_t line2 = {Vars::Colours::NoscopeLines2.Value.r, Vars::Colours::NoscopeLines2.Value.g, Vars::Colours::NoscopeLines2.Value.b, 255};

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

	const int x = Vars::Visuals::OnScreenConditions.Value.c;
	const int y = Vars::Visuals::OnScreenConditions.Value.y;

	const std::vector<std::wstring> conditionsVec = F::ESP.GetPlayerConds(pLocal);

	const auto& menuFont = g_Draw.GetFont(FONT_MENU);
	int nTextOffset = menuFont.nTall;
	//int longestText = 40;
	int width, height;
	for (const std::wstring& cond : conditionsVec)
	{
		g_Draw.String(menuFont, x, y + nTextOffset, {255, 255, 255, 255}, ALIGN_CENTER, cond.data());
		I::VGuiSurface->GetTextSize(menuFont.dwFont, cond.data(), width, height);
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

void CVisuals::DrawOnScreenPing(CBaseEntity* pLocal)
{
	if (!Vars::Visuals::DrawOnScreenPing.Value) { return; }
	if (!pLocal->IsAlive() || pLocal->IsAGhost()) { return; }

	CTFPlayerResource* cResource = g_EntityCache.GetPR();
	if (!cResource) { return; }

	const INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!iNetChan) { return; }

	const float flLatencyReal = (iNetChan->GetLatency(FLOW_INCOMING) + iNetChan->GetLatency(FLOW_OUTGOING)) * 1000;
	const int flLatencyScoreBoard = cResource->GetPing(pLocal->GetIndex());

	const int x = Vars::Visuals::OnScreenPing.Value.x;
	const int y = Vars::Visuals::OnScreenPing.Value.y;
	const int h = Vars::Visuals::OnScreenPing.Value.h;

	const auto& menuFont = g_Draw.GetFont(FONT_MENU);
	const int nTextOffset = menuFont.nTall;
	{
		g_Draw.String(menuFont, x, y, {255, 255, 255, 255}, ALIGN_DEFAULT, "ping real : %.0f", flLatencyReal);
		g_Draw.String(menuFont, x, y + h - nTextOffset, {255, 255, 255, 255}, ALIGN_DEFAULT, "ping scoreboard : %d", flLatencyScoreBoard);
	}
}

void CVisuals::DrawNoSpreadIndicator()
{
	if (!Vars::NoSpread::Indicator.Value)
		return;
	if (!I::EngineVGui->IsGameUIVisible()) {
		std::wstring text = L"NoSpread Syncing..";
		Color_t clr = { 255, 255, 255, 255 };
		if (!Vars::NoSpread::Hitscan.Value)
		{
			text = L"NoSpread Not Active.";
			clr = { 220, 20, 60, 255 };
		}
		else if (G::BadMantissa) {
			text = L"NoSpread Not Synced, server uptime too low.";
			clr = { 204, 204, 0, 255 };
		}
		else
		{
			switch (G::NoSpreadSynced)
			{
			case SYNCED:
				text = L"NoSpread Synced.";
				clr = { 60, 179, 113, 255 };
				break;
			case NOT_SYNCED:
				text = L"NoSpread Not Synced.";
				break;
			case DEAD_SYNC:
				text = L"NoSpread Dead Sync";
				clr = { 220, 20, 60, 255 };
				break;
			case CORRECTING:
				text = L"NoSpread Resyncing..";
				clr = { 204, 204, 0, 255 };
				break;
			default:
				text = L"NoSpread Not Synced.";
				break;
			}
		}
		int step = (int)F::NoSpread.CalcMantissaStep(G::SentClientFloatTime * 1000.0);
		Color_t mclr = { 204, 204, 0, 255 };

		const auto& menuFont = g_Draw.GetFont(FONT_MENU);

		g_Draw.String(menuFont, 50, 30, clr, ALIGN_DEFAULT, text.data());
		g_Draw.String(menuFont, 50, 45, mclr, ALIGN_DEFAULT, "Mantissa step size: %d", step);
	}
}

void CVisuals::SkyboxChanger()
{
	using LoadNamedSkysFn = bool(_cdecl*)(const char*);
	static auto fnLoadSkys = S::LoadSkys.As<LoadNamedSkysFn>();

	static const char* skybNames[] = {
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
		if (Vars::Skybox::SkyboxNum.Value == 0)
		{
			if (Vars::Misc::BypassPure.Value)
			{
				fnLoadSkys(Vars::Skybox::SkyboxName.Value.c_str());
			}
			else
			{
				fnLoadSkys(I::Cvar->FindVar("sv_skyname")->GetString());
			}
		}
		else
		{
			fnLoadSkys(skybNames[Vars::Skybox::SkyboxNum.Value]);
		}
	}
	else
	{
		fnLoadSkys(I::Cvar->FindVar("sv_skyname")->GetString());
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
				static KeyHelper tpKey{&Vars::Visuals::ThirdPersonKey.Value};
				if (tpKey.Pressed())
				{
					Vars::Visuals::ThirdPerson.Value = !Vars::Visuals::ThirdPerson.Value;
				}
			}
		}
		const bool bScoped = (Vars::Visuals::RemoveScope.Value || Vars::Visuals::RemoveZoom.Value) && pLocal->IsScoped();
		const bool bThirdPersonVar = Vars::Visuals::ThirdPerson.Value;
		const bool bFollowingProjectile = Vars::Visuals::ProjectileCameraKey.Value && GetAsyncKeyState(Vars::Visuals::ProjectileCameraKey.Value) & 0x8000 && !g_EntityCache.GetGroup(EGroupType::LOCAL_PROJECTILES).empty();
		const bool bFreecam = G::FreecamActive;
		const bool bShouldThirdPerson = (bThirdPersonVar || bFollowingProjectile || bFreecam) && (!bScoped || bFreecam);
		const bool bIsInThirdPerson = I::Input->CAM_IsThirdPerson();

		if (!bShouldThirdPerson) {
			if (bIsInThirdPerson) {
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
			static KeyHelper offsetKey{&Vars::Visuals::ThirdpersonArrowOffsetKey.Value};

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

void CVisuals::ProjectileTrace() // make it ignore other projectiles
{
	const auto& pLocal = g_EntityCache.GetLocal();
	const auto& pWeapon = g_EntityCache.GetWeapon();
	if (!pWeapon || !pLocal)
		return;

	ProjectileInfo projInfo = {};
	if (!F::ProjSim.GetInfo(pLocal, pWeapon, I::EngineClient->GetViewAngles(), projInfo, true))
		return;

	if (!F::ProjSim.Initialize(projInfo))
		return;

	for (int n = 0; n < TIME_TO_TICKS(projInfo.m_lifetime); n++)
	{
		Vec3 Old = F::ProjSim.GetOrigin();
		F::ProjSim.RunTick(projInfo);
		Vec3 New = F::ProjSim.GetOrigin();

		CGameTrace trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pLocal;
		Utils::TraceHull(Old, New, projInfo.m_hull * -1.f, projInfo.m_hull, MASK_SOLID, &filter, &trace);
		if (trace.DidHit())
		{
			Vec3 angles;
			Math::VectorAngles(trace.Plane.normal, angles);

			const Vec3 size = { projInfo.m_hull.x * 2.f, 16.f, 16.f };
			RenderBox(trace.vEndPos, size / -2, size / 2, angles, Vars::Aimbot::Projectile::ProjectileColor.Value, { 0, 0, 0, 0 });

			projInfo.PredictionLines.push_back({ trace.vEndPos, Math::GetRotatedPosition(trace.vEndPos, Math::VelocityToAngles(F::ProjSim.GetVelocity() * Vec3(1, 1, 0)).Length2D() + 90, Vars::Visuals::SeperatorLength.Value) });

			break;
		}
	}

	for (size_t i = 1; i < projInfo.PredictionLines.size(); i++)
		RenderLine(projInfo.PredictionLines.at(i - 1).first, projInfo.PredictionLines.at(i).first, Vars::Aimbot::Projectile::ProjectileColor.Value, false);
}

void DebugLine(const char* title, const char* value, std::pair<int, int> offsets, Color_t clr = {255, 255, 255, 255})
{
	const auto& menuFont = g_Draw.GetFont(FONT_MENU);
	g_Draw.String(menuFont, offsets.first, offsets.second += 15, clr, ALIGN_DEFAULT, title);
	g_Draw.String(menuFont, offsets.first + 125, offsets.second, clr, ALIGN_DEFAULT, value);
}

void CVisuals::DrawDebugInfo(CBaseEntity* pLocal)
{
	// Debug info
	if (Vars::Debug::DebugInfo.Value)
	{
		int yoffset = 10, xoffset = 10;
		const auto& menuFont = g_Draw.GetFont(FONT_MENU);
		const auto& indFont = g_Draw.GetFont(FONT_INDICATORS);

		{
			g_Draw.String(indFont, xoffset, yoffset += 15, Utils::Rainbow(), ALIGN_DEFAULT, "Fedoraware");
		}
		{
			g_Draw.String(menuFont, xoffset, yoffset += 15, {119, 255, 225, 255}, ALIGN_DEFAULT, "Local Player"); // header
		}
		// alive
		{
			const bool alive = pLocal->IsAlive();
			Color_t clr = alive ? Color_t{153, 232, 0, 255} : Color_t{167, 0, 0, 255};
			g_Draw.String(menuFont, xoffset, yoffset += 15, clr, ALIGN_DEFAULT, "%s", alive ? "ALIVE" : "DEAD");
		}

		if (!G::LastUserCmd) { return; }
		const float flLastFwd = G::LastUserCmd->forwardmove;
		const float flLastSde = G::LastUserCmd->sidemove;
		{
			g_Draw.String(menuFont, xoffset, yoffset += 15, {255, 255, 255, 255}, ALIGN_DEFAULT, "%.0f", flLastFwd);
			g_Draw.String(menuFont, xoffset, yoffset += 15, {255, 255, 255, 255}, ALIGN_DEFAULT, "%.0f", flLastSde);
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
		static constexpr Color_t REAL_COLOUR = {0, 255, 0, 255};
		static constexpr Color_t FAKE_COLOUR = {255, 0, 0, 255};

		const auto& vOrigin = pLocal->GetAbsOrigin();

		Vec3 vScreen1, vScreen2;
		if (Utils::W2S(vOrigin, vScreen1))
		{
			constexpr auto distance = 50.f;
			if (Utils::W2S(Utils::GetRotatedPosition(vOrigin, G::RealViewAngles.y, distance), vScreen2))
			{
				g_Draw.Line(vScreen1.x, vScreen1.y, vScreen2.x, vScreen2.y, REAL_COLOUR);
			}

			if (Utils::W2S(Utils::GetRotatedPosition(vOrigin, G::FakeViewAngles.y, distance), vScreen2))
			{
				g_Draw.Line(vScreen1.x, vScreen1.y, vScreen2.x, vScreen2.y, FAKE_COLOUR);
			}
		}
	}
}

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
				const DragBox_t DTBox = Vars::Misc::CL_Move::DTIndicator.Value;
				const float ratioCurrent = std::clamp((static_cast<float>(G::ShiftedTicks) / static_cast<float>(Vars::Misc::CL_Move::DTTicks.Value)), 0.0f, 1.0f);
				static float ratioInterp = 0.00f;
				const auto& indFont = g_Draw.GetFont(FONT_INDICATORS);

				ratioInterp = g_Draw.EaseIn(ratioInterp, ratioCurrent, 0.95f);
				Math::Clamp(ratioInterp, 0.00f, 1.00f);

				static Color_t color1, color2;

				if (G::WaitForShift)
				{
					color1 = Vars::Colours::DTBarIndicatorsCharging.Value.startColour;
					color2 = Vars::Colours::DTBarIndicatorsCharging.Value.endColour;
				}
				else
				{
					color1 = Vars::Colours::DTBarIndicatorsCharged.Value.startColour;
					color2 = Vars::Colours::DTBarIndicatorsCharged.Value.endColour;
				}

				switch (Vars::Misc::CL_Move::DTBarStyle.Value)
				{
				case 1:
					{
						g_Draw.OutlinedRect(DTBox.x, DTBox.y, DTBox.w, DTBox.h, Vars::Colours::DtOutline.Value);
						g_Draw.GradientRectWH(DTBox.x + 1, DTBox.y + 1, ratioInterp * (DTBox.w - 2), DTBox.h - 2, color1, color2, true);
						break;
					}
				case 2:
					{
						const auto fontHeight = Vars::Fonts::FONT_INDICATORS::nTall.Value;
						const int drawX = DTBox.x;
						g_Draw.String(indFont, DTBox.c, DTBox.y - fontHeight - 3, {255, 255, 255, 255}, ALIGN_CENTERHORIZONTAL, L"Ticks %d/%d", G::ShiftedTicks,
						              Vars::Misc::CL_Move::DTTicks.Value);
						g_Draw.RoundedBoxStatic(DTBox.x, DTBox.y, DTBox.w, DTBox.h, 4, Vars::Colours::DtOutline.Value);
						if (G::ShiftedTicks && ratioCurrent)
						{
							g_Draw.RoundedBoxStatic(DTBox.x + 2, DTBox.y + 2, ratioCurrent * (DTBox.w - 4), DTBox.h - 4, 4, Vars::Menu::Colors::MenuAccent.Value);
						}
						if (G::WaitForShift)
						{
							g_Draw.String(indFont, DTBox.c, DTBox.y + fontHeight + DTBox.h, {255, 255, 255, 255}, ALIGN_CENTERHORIZONTAL, L"Not Ready");
						}
						break;
					}
				case 3:
					{
						g_Draw.OutlinedRect(DTBox.x, DTBox.y, DTBox.w, DTBox.h, Vars::Colours::DtOutline.Value); //	draw the outline
						g_Draw.Rect(DTBox.x + 1, DTBox.y + 1, DTBox.w - 2, DTBox.h - 2, {28, 29, 38, 255}); //	draw the background
						g_Draw.GradientRectWH(DTBox.x + 1, DTBox.y + 1, ratioInterp * (DTBox.w - 2), DTBox.h - 2, color1, color2, true);
						g_Draw.String(indFont, DTBox.x, DTBox.y - 10, {255, 255, 255, 255}, ALIGN_DEFAULT, L"CHARGE");

						if (G::ShiftedTicks == 0) // chargless
						{
							g_Draw.String(indFont, DTBox.x + DTBox.w, DTBox.y - 10, {255, 55, 40, 255}, ALIGN_REVERSE, L"NO CHARGE");
						}
						else if (G::Recharging) // charging 
						{
							g_Draw.String(indFont, DTBox.x + DTBox.w, DTBox.y - 10, {255, 126, 0, 255}, ALIGN_REVERSE, L"CHARGING");
						}
						else if (G::WaitForShift) // waiting
						{
							g_Draw.String(indFont, DTBox.x + DTBox.w, DTBox.y - 10, {255, 46, 46, 255}, ALIGN_REVERSE, L"DT IMPOSSIBLE");
						}
						else // ready
						{
							g_Draw.String(indFont, DTBox.x + DTBox.w, DTBox.y - 10, {66, 255, 0, 255}, ALIGN_REVERSE, L"READY");
						}
						break;
					}
				case 4:
					{
						if (G::ShiftedTicks == 0 || G::Recharging)
						{
							g_Draw.String(indFont, DTBox.c, DTBox.y - 10, {255, 64, 64, 255}, ALIGN_CENTERHORIZONTAL, L"Recharge! (%i / %i)", G::ShiftedTicks,
							              Vars::Misc::CL_Move::DTTicks.Value);
						}
						else if (G::WaitForShift)
						{
							g_Draw.String(indFont, DTBox.c, DTBox.y - 10, {255, 178, 0, 255}, ALIGN_CENTERHORIZONTAL, L"Wait! (%i / 25)", G::WaitForShift);
						}
						else
						{
							g_Draw.String(indFont, DTBox.c, DTBox.y - 10, {153, 255, 153, 255}, ALIGN_CENTERHORIZONTAL, L"Shift ready!");
						}
						break;
					}
				case 5:
					{
						g_Draw.String(indFont, DTBox.c, DTBox.y - 3, {255, 255, 255, 255}, ALIGN_CENTERHORIZONTAL, L"%i/%i", G::ShiftedTicks, Vars::Misc::CL_Move::DTTicks.Value);
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
	if (iOldTick == I::GlobalVars->tickcount) { return; }
	iOldTick = I::GlobalVars->tickcount;
	// draw our serverside hitbox on local servers, used to test fakelag & antiaim
	if (I::Input->CAM_IsThirdPerson() && Vars::Visuals::ThirdPersonServerHitbox.Value)
	{
		//	i have no idea what this is
		using GetServerAnimating_t = void* (*)(int);
		static auto GetServerAnimating = S::GetServerAnimating.As<GetServerAnimating_t>();

		using DrawServerHitboxes_t = void(__thiscall*)(void*, float, bool); // C_BaseAnimating, Duration, MonoColour
		static auto DrawServerHitboxes = S::DrawServerHitboxes.As<DrawServerHitboxes_t>();

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
	//	menu snow
	struct SnowFlakeT
	{
		int X;
		int Y;
	};

	static std::vector<SnowFlakeT> vSnowFlakes;
	constexpr int snowCount = 1000;

	static bool bInit = false;
	if (!bInit)
	{
		for (int i = 0; i < snowCount; i++)
		{
			vSnowFlakes.emplace_back(Utils::RandIntSimple(0, g_ScreenSize.w), Utils::RandIntSimple(0, g_ScreenSize.h / 2.f));
		}
		bInit = true;
	}

	const auto& menuFont = g_Draw.GetFont(FONT_MENU);
	for (auto& flake : vSnowFlakes)
	{
		//	do gravity
		constexpr int drift = 1;
		flake.X += Utils::RandIntSimple(-drift, drift);
		flake.Y += drift;

		//	calculate alpha
		const float alpha = Math::MapFloat(flake.Y, 0.0f, g_ScreenSize.h / 2.f, 1.0f, 0.0f);
		//
		//	recreate snow flakes that are gone
		if (alpha <= 0.f || flake.X >= g_ScreenSize.w || flake.X <= 0)
		{
			flake.X = Utils::RandIntSimple(0, g_ScreenSize.w);
			flake.Y = Utils::RandIntSimple(0, 100);
		} //

		Color_t flakeColour = {255, 255, 255, static_cast<byte>(alpha * 255.0f)};
		g_Draw.String(menuFont, flake.X, flake.Y, flakeColour, ALIGN_DEFAULT, "*");
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
					{255, 255, 255, 255}
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
					RenderLine(G::PredLinesBackup.at(i - 1).first, G::PredLinesBackup.at(i).first, Vars::Aimbot::Projectile::PredictionColor.Value, false);
				}
			}
			else
			{
				for (size_t i = 1; i < G::PredLinesBackup.size(); i++)
				{
					const auto& vStart = G::PredLinesBackup[i - 1].first;
					const auto& vRotate = G::PredLinesBackup[i - 1].second; //	splirp vec
					const auto& vEnd = G::PredLinesBackup[i].first;
					if ((i % Vars::Visuals::SeperatorSpacing.Value) == 0) { RenderLine(vStart, vRotate, Vars::Aimbot::Projectile::PredictionColor.Value, false); }
					RenderLine(vStart, vEnd, Vars::Aimbot::Projectile::PredictionColor.Value, false);
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
		G::DormantPlayerESP[iEntIndex] = {vOrigin, I::EngineClient->Time()};
	}
}

void CVisuals::RenderLine(const Vector& v1, const Vector& v2, Color_t c, bool bZBuffer)
{
	using FN = void(__cdecl*)(const Vector&, const Vector&, Color_t, bool);
	static auto fnRenderLine = S::RenderLine.As<FN>();
	fnRenderLine(v1, v2, c, bZBuffer);
}

void CVisuals::RenderBox(const Vec3& vPos, const Vec3& vMins, const Vec3& vMaxs, const Vec3& vOrientation, Color_t cEdge, Color_t cFace)
{
	{
		using FN = void(__cdecl*)(const Vec3&, const Vec3&, const Vec3&, const Vec3&, Color_t, bool, bool);
		static auto fnRenderLine = S::RenderBoxFace.As<FN>();
		fnRenderLine(vPos, vOrientation, vMins, vMaxs, cFace, false, false);
	}
	{
		using FN = void(__cdecl*)(const Vec3&, const Vec3&, const Vec3&, const Vec3&, Color_t, bool);
		static auto fnRenderLine = S::RenderBoxEdge.As<FN>();
		fnRenderLine(vPos, vOrientation, vMins, vMaxs, cEdge, false);
	}
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
		Vec3 vForward;
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
			Vec3 vShootPos = pEnemy->GetShootPos();
			Vec3 vAngle = pEnemy->GetEyeAngles();
			Math::AngleVectors(vAngle, &vForward);
			Vec3 vShootEnd = vShootPos + (vForward * 8192.f);

			Utils::Trace(vShootPos, vShootEnd, MASK_SHOT, &filter, &trace);

			m_SightLines[pEnemy->GetIndex()] = {vShootPos, trace.vEndPos, GetEntityDrawColour(pEnemy, Vars::ESP::Main::EnableTeamEnemyColors.Value), true};
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
	m_vecBulletTracers.push_front({vFrom, vTo, clr, I::GlobalVars->curtime});
}

void CVisuals::PruneBulletTracers()
{
	if (m_vecBulletTracers.empty()) { return; }
	if (fabsf(m_vecBulletTracers.back().m_flTimeCreated - I::GlobalVars->curtime) > 5.f) { m_vecBulletTracers.pop_back(); }
}

void CVisuals::DrawBulletTracers()
{
	const float curTime = I::GlobalVars->curtime;

	if (m_vecBulletTracers.empty()) { return; }
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

	const Vec3 vecPos = G::CurWeaponType == EWeaponType::PROJECTILE ? G::PredictedPos : position;
	const Color_t tracerColor = Vars::Visuals::BulletTracerRainbow.Value ? Utils::Rainbow() : Vars::Colours::BulletTracer.Value;
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
		const Color_t clr = Vars::Colours::FOVCircle.Value;
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
	static auto oldW = Vars::Colours::WorldModulation.Value;
	static auto oldS = Vars::Colours::SkyModulation.Value;

	if (Vars::Colours::WorldModulation.Value != oldW || Vars::Colours::SkyModulation.Value != oldS)
	{
		oldW = Vars::Colours::WorldModulation.Value;
		oldS = Vars::Colours::SkyModulation.Value;

		return true;
	}
	return false;
}

bool ModSetChanged() // check if modulation has been switched
{
	static auto oldS = Vars::Visuals::SkyModulation.Value;
	static auto oldW = Vars::Visuals::WorldModulation.Value;

	if (Vars::Visuals::SkyModulation.Value != oldS || Vars::Visuals::WorldModulation.Value != oldW)
	{
		oldW = Vars::Visuals::WorldModulation.Value;
		oldS = Vars::Visuals::SkyModulation.Value;
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
		Vars::Visuals::WorldModulation.Value ? ApplyModulation(Vars::Colours::WorldModulation.Value) : ApplyModulation({255, 255, 255, 255});
		Vars::Visuals::SkyModulation.Value ? ApplySkyboxModulation(Vars::Colours::SkyModulation.Value) : ApplySkyboxModulation({255, 255, 255, 255});
		oConnectionState = connectionState;
		shouldModulate = false;
	}
	if (!shouldModulate)
	{
		if (!Vars::Visuals::WorldModulation.Value)
		{
			ApplyModulation({255, 255, 255, 255});
			shouldModulate = true;
		}
		if (!Vars::Visuals::SkyModulation.Value)
		{
			ApplySkyboxModulation({255, 255, 255, 255});
			shouldModulate = true;
		}
	}
	//else if (!Vars::Visuals::WorldModulation.Value)
	//{
	//	if (!shouldModulate)
	//	{
	//		ApplyModulation({ 255, 255, 255, 255 });
	//		shouldModulate = true;
	//	}
	//} // i don't know why i need to do this
}

void CVisuals::RestoreWorldModulation() // keep this because its mentioned in @DLLMain.cpp if you find a better way to do this, remove it ig.
{
	ApplyModulation({255, 255, 255, 255});
	ApplySkyboxModulation({255, 255, 255, 255});
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

	const auto& pickupFont = g_Draw.GetFont(FONT_ESP_PICKUPS);
	for (auto pickupData = PickupDatas.begin(); pickupData != PickupDatas.end();)
	{
		const float timeDiff = I::EngineClient->Time() - pickupData->Time;
		if (timeDiff > 10.f)
		{
			pickupData = PickupDatas.erase(pickupData);
			continue;
		}

		auto timerText = std::format("{:.1f}", 10.f - timeDiff);
		auto color = pickupData->Type ? Vars::Colours::Health.Value : Vars::Colours::Ammo.Value;

		Vec3 vScreen;
		if (Utils::W2S(pickupData->Location, vScreen))
		{
			g_Draw.String(pickupFont, vScreen.x, vScreen.y, color, ALIGN_CENTER, timerText.c_str());
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
	const auto& osrsFont = g_Draw.GetFont(FONT_OSRS);
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
				Vec3 vHeadpos = chat.m_pEntity->GetHitboxPos(HITBOX_HEAD);
				if (!vHeadpos.IsZero())
				{
					vHeadpos.z += 20;
					if (Utils::W2S(vHeadpos, vScreen))
					{
						Color_t col = {255, 255, 0, 255};
						switch (chat.m_Colour)
						{
						case eRS_RED:
							{
								col = {255, 0, 0, 255};
								break;
							}
						case eRS_GREEN:
							{
								col = {0, 255, 0, 255};
								break;
							}
						case eRS_CYAN:
							{
								col = {0, 255, 255, 255};
								break;
							}
						case eRS_PURPLE:
							{
								col = {255, 0, 255, 255};
								break;
							}
						case eRS_WHITE:
							{
								col = {255, 255, 255, 255};
								break;
							}
						default: break;
						}

						g_Draw.String(osrsFont, vScreen.x, vScreen.y - (14 * chat.m_nOffset), col, ALIGN_CENTERHORIZONTAL, L"%ls", chat.m_szChatText.c_str());
					}
				}
			}
		}
	}

	for (const auto& pos : vecRemovals)
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
	if (!pEntity)
	{
		return;
	}

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
	for (const auto& chat : m_vecChats)
	{
		if (chat.m_pEntity == pEntity)
		{
			if (chat.m_nOffset >= highestOffset)
			{
				highestOffset = chat.m_nOffset + 1;
			}
		}
	}

	const Chat_t push = {pEntity, I::GlobalVars->curtime, highestOffset, col, szChatText};
	m_vecChats.push_back(push);
}
