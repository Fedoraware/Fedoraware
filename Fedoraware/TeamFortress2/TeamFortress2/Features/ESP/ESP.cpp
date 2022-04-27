#include "ESP.h"
#include "../AntiHack/CheaterDetection/CheaterDetection.h"
#include "../Vars.h"

bool CESP::ShouldRun()
{
	if (/*!Vars::ESP::Main::Active.m_Var ||*/ g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	return true;
}

void CESP::Run()
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (ShouldRun())
		{
			DrawWorld();
			DrawBuildings(pLocal);
			DrawPlayers(pLocal);
		}
	}
}

bool CESP::GetDrawBounds(CBaseEntity* pEntity, Vec3* vTrans, int& x, int& y, int& w, int& h)
{
	bool bIsPlayer = false;
	Vec3 vMins, vMaxs;

	if (pEntity->IsPlayer())
	{
		bIsPlayer = true;
		const bool bIsDucking = pEntity->IsDucking();
		vMins = g_Interfaces.GameMovement->GetPlayerMins(bIsDucking);
		vMaxs = g_Interfaces.GameMovement->GetPlayerMaxs(bIsDucking);
	}

	else
	{
		vMins = pEntity->GetCollideableMins();
		vMaxs = pEntity->GetCollideableMaxs();
	}

	const matrix3x4& transform = pEntity->GetRgflCoordinateFrame();

	const Vec3 vPoints[] =
	{
		Vec3(vMins.x, vMins.y, vMins.z),
		Vec3(vMins.x, vMaxs.y, vMins.z),
		Vec3(vMaxs.x, vMaxs.y, vMins.z),
		Vec3(vMaxs.x, vMins.y, vMins.z),
		Vec3(vMaxs.x, vMaxs.y, vMaxs.z),
		Vec3(vMins.x, vMaxs.y, vMaxs.z),
		Vec3(vMins.x, vMins.y, vMaxs.z),
		Vec3(vMaxs.x, vMins.y, vMaxs.z)
	};

	for (int n = 0; n < 8; n++)
		Math::VectorTransform(vPoints[n], transform, vTrans[n]);

	Vec3 flb, brt, blb, frt, frb, brb, blt, flt;

	if (Utils::W2S(vTrans[3], flb) && Utils::W2S(vTrans[5], brt)
		&& Utils::W2S(vTrans[0], blb) && Utils::W2S(vTrans[4], frt)
		&& Utils::W2S(vTrans[2], frb) && Utils::W2S(vTrans[1], brb)
		&& Utils::W2S(vTrans[6], blt) && Utils::W2S(vTrans[7], flt)
		&& Utils::W2S(vTrans[6], blt) && Utils::W2S(vTrans[7], flt))
	{
		const Vec3 arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

		float left = flb.x;
		float top = flb.y;
		float righ = flb.x;
		float bottom = flb.y;

		for (int n = 1; n < 8; n++)
		{
			if (left > arr[n].x)
				left = arr[n].x;

			if (top < arr[n].y)
				top = arr[n].y;

			if (righ < arr[n].x)
				righ = arr[n].x;

			if (bottom > arr[n].y)
				bottom = arr[n].y;
		}

		float x_ = left;
		const float y_ = bottom;
		float w_ = righ - left;
		const float h_ = top - bottom;

		if (bIsPlayer && Vars::ESP::Players::Box.m_Var != 3)
		{
			x_ += (righ - left) / 8.0f;
			w_ -= (righ - left) / 8.0f * 2.0f;
		}

		x = static_cast<int>(x_);
		y = static_cast<int>(y_);
		w = static_cast<int>(w_);
		h = static_cast<int>(h_);

		return !(x > g_ScreenSize.w || x + w < 0 || y > g_ScreenSize.h || y + h < 0);
	}

	return false;
}

void CESP::DrawPlayers(CBaseEntity* pLocal)
{
	if (!Vars::ESP::Players::Active.m_Var || !Vars::ESP::Main::Active.m_Var)
		return;

	for (const auto& Player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
	{
		if (!Player->IsAlive() || Player->IsAGhost())
			continue;

		int nIndex = Player->GetIndex();
		bool bIsLocal = nIndex == g_Interfaces.Engine->GetLocalPlayer();

		if (!bIsLocal)
		{
			switch (Vars::ESP::Players::IgnoreCloaked.m_Var)
			{
			case 0: { break; }
			case 1:
			{
				if (Player->IsCloaked()) { continue; }
				break;
			}
			case 2:
			{
				if (Player->IsCloaked() && Player->GetTeamNum() != pLocal->GetTeamNum()) { continue; }
				break;
			}
			}

			switch (Vars::ESP::Players::IgnoreTeammates.m_Var)
			{
			case 0: break;
			case 1:
			{
				if (Player->GetTeamNum() == pLocal->GetTeamNum()) { continue; }
				break;
			}
			case 2:
			{
				if (Player->GetTeamNum() == pLocal->GetTeamNum() && !g_EntityCache.Friends[nIndex]) { continue; }
				break;
			}
			}
		}

		else
		{
			if (!Vars::ESP::Players::ShowLocal.m_Var)
				continue;
		}

		Color_t DrawColor = Utils::GetEntityDrawColor(Player, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);

		if (Vars::ESP::Players::Dlights.m_Var)
			CreateDLight(nIndex, DrawColor, Player->GetAbsOrigin(), Vars::ESP::Players::DlightRadius.m_Var);

		int x = 0, y = 0, w = 0, h = 0;
		Vec3 vTrans[8];
		if (GetDrawBounds(Player, vTrans, x, y, w, h))
		{
			int nHealth = Player->GetHealth(), nMaxHealth = Player->GetMaxHealth();
			Color_t HealthColor = Utils::GetHealthColor(nHealth, nMaxHealth);

			size_t FONT = FONT_ESP, FONT_NAME = FONT_ESP_NAME;

			int nTextX = x + w + 3, nTextOffset = 0, nClassNum = Player->GetClassNum();

			if (Vars::ESP::Players::Uber.m_Var == 2 && nClassNum == CLASS_MEDIC)
			{
				if (const auto& pMedGun = Player->GetWeaponFromSlot(SLOT_SECONDARY))
					nTextX += 5;
			}

			g_Interfaces.Surface->DrawSetAlphaMultiplier(Vars::ESP::Players::Alpha.m_Var);

			if (Vars::ESP::Players::Bones.m_Var)
			{
				const Color_t clrBone = Vars::ESP::Players::Bones.m_Var == 1 ? Colors::Bones : HealthColor;

				DrawBones(Player, { 8, 7, 6, 4 }, clrBone);
				DrawBones(Player, { 11, 10, 9, 4 }, clrBone);
				DrawBones(Player, { 0, 4, 1 }, clrBone);
				DrawBones(Player, { 14, 13, 1 }, clrBone);
				DrawBones(Player, { 17, 16, 1 }, clrBone);
			}

			switch (Vars::ESP::Players::Box.m_Var)
			{
			case 1:
			{
				int height = h + 1; //don't ask me /shrug

				g_Draw.OutlinedRect(x, y, w, height, DrawColor);
				if (Vars::ESP::Main::Outlinedbar.m_Var)
				{
					g_Draw.OutlinedRect(x - 1, y - 1, w + 2, height + 2, Colors::OutlineESP);
				}

				break;
			}
			case 2:
			{
				g_Draw.CornerRect(x, y, w, h, 3, 5, DrawColor);
				if (Vars::ESP::Main::Outlinedbar.m_Var)
				{
					g_Draw.CornerRect(x - 1, y - 1, w + 2, h + 2, 3, 5, Colors::OutlineESP);
				}

				break;
			}
			case 3:
			{
				Draw3DBox(vTrans, DrawColor);
				break;
			}
			default:
				break;
			}

			if (Vars::ESP::Players::Lines.m_Var)
			{
				Vec3 vScreen, vOrigin = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);

				if (g_Interfaces.Input->CAM_IsThirdPerson())
					Utils::W2S(pLocal->GetAbsOrigin(), vOrigin);

				if (Utils::W2S(Player->GetAbsOrigin(), vScreen))
					g_Draw.Line(vOrigin.x, vOrigin.y, vScreen.x, vScreen.y, DrawColor);
			}

			if (Vars::ESP::Players::Health.m_Var)
			{
				g_Draw.String(FONT, nTextX, y + nTextOffset, nHealth > nMaxHealth ? Colors::Overheal : HealthColor,
					ALIGN_DEFAULT, L"%d / %d", nHealth, nMaxHealth);
				nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
			}

			if (Vars::ESP::Players::Uber.m_Var && nClassNum == CLASS_MEDIC)
			{
				if (const auto& pMedGun = Player->GetWeaponFromSlot(SLOT_SECONDARY))
				{
					if (Vars::ESP::Players::Uber.m_Var == 1)
					{
						g_Draw.String(FONT, nTextX, y + nTextOffset, Colors::UberColor, ALIGN_DEFAULT, L"%.0f%%",
							pMedGun->GetUberCharge() * 100.0f);
						nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
					}

					if (Vars::ESP::Players::Uber.m_Var == 2 && pMedGun->GetUberCharge())
					{
						x += w + 1;

						float flUber = pMedGun->GetUberCharge() * (pMedGun->GetItemDefIndex() == Medic_s_TheVaccinator
							? 400.0f
							: 100.0f);
						float flMaxUber = 100.0f;

						if (flUber > flMaxUber)
							flUber = flMaxUber;

						static const int nWidth = 2;
						int nHeight = h + (flUber < flMaxUber ? 2 : 1);
						int nHeight2 = h + 1;

						float ratio = flUber / flMaxUber;
						g_Draw.Rect(x + nWidth, y + nHeight - nHeight * ratio, nWidth, nHeight * ratio,
							Colors::UberColor);

						if (Vars::ESP::Main::Outlinedbar.m_Var)
							g_Draw.OutlinedRect(x + nWidth - 1, y + nHeight - nHeight * ratio - 1, nWidth + 2,
								nHeight * ratio + 2, Colors::OutlineESP);

						x -= w + 1;
					}
				}
			}

			PlayerInfo_t pi;
			if (g_Interfaces.Engine->GetPlayerInfo(nIndex, &pi))
			{
				int middle = x + w / 2;
				if (Vars::ESP::Players::Name.m_Var)
				{
					int offset = g_Draw.m_vecFonts[FONT_NAME].nTall + g_Draw.m_vecFonts[FONT_NAME].nTall / 4;
					if (Vars::ESP::Players::NameBox.m_Var)
					{
						int wideth, heighth;
						g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_NAME].dwFont,
							Utils::ConvertUtf8ToWide(pi.name).data(), wideth, heighth);
						Color_t LineColor = DrawColor;
						LineColor.a = 180;
						//g_Draw.Rect((x + (w / 2) - (wideth / 2)) - 5, y - offset - 5, wideth + 10, heighth + 10, { 0,0,0,180 });
						g_Draw.Rect(middle - wideth / 2 - 5, y - offset, wideth + 10, heighth + 2, { 0, 0, 0, 180 });
						//g_Draw.Rect((x + (w / 2) - (wideth / 2)) - 5, y - offset - 7, wideth + 10, 2, LineColor);
						g_Draw.Rect(middle - wideth / 2 - 5, y - offset - 2, wideth + 10, 2, LineColor);
						offset -= 1;
					}
					if (Vars::ESP::Players::NameCustom.m_Var)
					{
						g_Draw.String(FONT_NAME, middle, y - offset, Vars::ESP::Players::NameColor, ALIGN_CENTERHORIZONTAL,
							Utils::ConvertUtf8ToWide(pi.name).data());
					}
					else
					{
						g_Draw.String(FONT_NAME, middle, y - offset, DrawColor, ALIGN_CENTERHORIZONTAL,
							Utils::ConvertUtf8ToWide(pi.name).data());
					}
				}

				if (Vars::Visuals::DebugInfo.m_Var && g_BadActors.markedcheaters[pi.friendsID])
				{
					g_Draw.String(FONT, nTextX, y + nTextOffset, { 255,0,0,255 }, ALIGN_DEFAULT, "CHEATER");
					nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
				}

				if (Vars::ESP::Players::GUID.m_Var)
				{
					g_Draw.String(FONT, nTextX, y + nTextOffset, Colors::White, ALIGN_DEFAULT, "%s", pi.guid);
					nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
				}

				
			}

			if (Vars::ESP::Players::Class.m_Var)
			{
				if (Vars::ESP::Players::Class.m_Var == 1 || Vars::ESP::Players::Class.m_Var == 3)
				{
					int offset = Vars::ESP::Players::Name.m_Var
						? g_Draw.m_vecFonts[FONT_NAME].nTall + g_Draw.m_vecFonts[FONT_NAME].nTall * 0.3f
						: 0;

					if (offset && Vars::ESP::Players::NameBox.m_Var)
						offset += 2;

					static const int nSize = 18;
					g_Draw.Texture(x + w / 2 - nSize / 2, y - offset - nSize, nSize, nSize, Colors::White,
						nClassNum);
				}

				if (Vars::ESP::Players::Class.m_Var >= 2)
				{
					g_Draw.String(FONT, nTextX, y + nTextOffset, DrawColor, ALIGN_DEFAULT, L"%ls",
						GetPlayerClass(nClassNum));
					nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
				}
			}

			if (Vars::ESP::Players::WeaponIcon.m_Var)
			{
				const auto& pWeapon = Player->GetActiveWeapon();
				if (pWeapon)
				{
					CHudTexture* pIcon = pWeapon->GetWeaponIcon();
					if (pIcon)
					{
						float fx, fy, fw, fh;
						fx = static_cast<float>(x);
						fy = static_cast<float>(y);
						fw = static_cast<float>(w);
						fh = static_cast<float>(h);
						const float iconWidth = static_cast<float>(pIcon->Width());
						// lol
						const float scale = std::clamp(fw / iconWidth, 0.5f, 0.75f);
						static float easedScale = 0.5f;
						scale > easedScale
							? easedScale = g_Draw.EaseOut(scale, easedScale, 0.99f)
							: easedScale = g_Draw.EaseIn(easedScale, scale, 0.99f);
						g_Draw.DrawHudTexture(fx + fw / 2.f - iconWidth / 2.f * scale, fy + fh + 1.f, scale, pIcon,
							Colors::WeaponIcon);
					}
				}
			}

			if (Vars::ESP::Players::Cond.m_Var)
			{
				size_t FONT = FONT_ESP_COND;
				int offset = g_Draw.m_vecFonts[FONT].nTall / 4;
				std::vector<std::wstring> cond_strings = GetPlayerConds(Player);

				if (!cond_strings.empty())
				{
					for (auto &cond_string : cond_strings) {
						g_Draw.String(FONT_ESP_COND, nTextX, y + nTextOffset, Colors::Cond, ALIGN_DEFAULT, cond_string.data());
						nTextOffset += g_Draw.m_vecFonts[FONT_ESP_COND].nTall;
					}
				}
			}

			if (Vars::ESP::Players::HealthBar.m_Var)
			{
				x -= 1;

				float flHealth = static_cast<float>(nHealth);
				float flMaxHealth = static_cast<float>(nMaxHealth);

				Gradient_t clr = flHealth > flMaxHealth ? Colors::OverhealHealthBar : Colors::GradientHealthBar;

				if (!Player->IsVulnerable())
					clr = { Colors::Invuln, Colors::Invuln };

				if (flHealth > flMaxHealth)
					flHealth = flMaxHealth;

				float ratio = flHealth / flMaxHealth;
				g_Draw.OutlinedGradientBar(x - 2 - 2, y + h, 2, h, ratio, clr.startColour, clr.endColour, Colors::OutlineESP, false);

				x += 1;
			}

			if (Vars::ESP::Players::Choked.m_Var)
			{
				x -= 1;
				static float ratio = 0.0f;
				int chokeCount = g_GlobalInfo.chokeMap[nIndex].ChokedTicks;

				Vec2 position = { (float)x - 2.f - 8.f,  (float)y + (float)h };
				ratio = chokeCount/22.0f;
				g_Draw.OutlinedGradientBar(position.x, position.y, 2, h, ratio, Colors::ChokedBar.startColour, Colors::ChokedBar.endColour, Colors::OutlineESP, false);

				x += 1;
			}

			g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);
		}
	}
}

void CESP::DrawBuildings(CBaseEntity* pLocal) const
{
	if (!Vars::ESP::Buildings::Active.m_Var || !Vars::ESP::Main::Active.m_Var)
		return;

	const auto Buildings = g_EntityCache.GetGroup(Vars::ESP::Buildings::IgnoreTeammates.m_Var
		? EGroupType::BUILDINGS_ENEMIES
		: EGroupType::BUILDINGS_ALL);

	for (const auto& pBuilding : Buildings)
	{
		if (!pBuilding->IsAlive())
			continue;

		const auto& Building = reinterpret_cast<CBaseObject*>(pBuilding);

		Color_t DrawColor = Utils::GetEntityDrawColor(Building, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);

		if (Vars::ESP::Buildings::Dlights.m_Var)
			CreateDLight(Building->GetIndex(), DrawColor, Building->GetAbsOrigin(),
				Vars::ESP::Buildings::DlightRadius.m_Var);

		int x = 0, y = 0, w = 0, h = 0;
		Vec3 vTrans[8];
		if (GetDrawBounds(Building, vTrans, x, y, w, h))
		{
			auto nHealth = Building->GetHealth(), nMaxHealth = Building->GetMaxHealth(),
				nTextX = x + w + 3, nTextOffset = 0, nTextTopOffset = 0;

			Color_t HealthColor = Utils::GetHealthColor(nHealth, nMaxHealth);

			const auto nType = static_cast<EBuildingType>(Building->GetType());

			size_t FONT = FONT_ESP, FONT_NAME = FONT_ESP_NAME, FONT_COND = FONT_ESP_COND;

			const bool bIsMini = Building->GetMiniBuilding();

			g_Interfaces.Surface->DrawSetAlphaMultiplier(Vars::ESP::Buildings::Alpha.m_Var);

			switch (Vars::ESP::Buildings::Box.m_Var)
			{
			case 1:
			{
				h += 1;

				g_Draw.OutlinedRect(x, y, w, h, DrawColor);

				if (Vars::ESP::Main::Outlinedbar.m_Var)
					g_Draw.OutlinedRect(x - 1, y - 1, w + 2, h + 2, Colors::OutlineESP);

				h -= 1;
				break;
			}
			case 2:
			{
				g_Draw.CornerRect(x, y, w, h, 3, 5, DrawColor);

				if (Vars::ESP::Main::Outlinedbar.m_Var)
					g_Draw.CornerRect(x - 1, y - 1, w + 2, h + 2, 3, 5, Colors::OutlineESP);
				break;
			}
			case 3:
			{
				Draw3DBox(vTrans, DrawColor);
				break;
			}
			default:
				break;
			}

			if (Vars::ESP::Buildings::Lines.m_Var)
			{
				Vec3 vScreen, vOrigin = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);

				if (g_Interfaces.Input->CAM_IsThirdPerson())
					Utils::W2S(pLocal->GetAbsOrigin(), vOrigin);

				if (Utils::W2S(Building->GetAbsOrigin(), vScreen))
					g_Draw.Line(vOrigin.x, vOrigin.y, vScreen.x, vScreen.y, DrawColor);
			}

			if (Vars::ESP::Buildings::Name.m_Var)
			{
				const wchar_t* szName;

				switch (nType)
				{
				case EBuildingType::SENTRY:
				{
					if (bIsMini)
						szName = _(L"Mini Sentry");
					else
						szName = _(L"Sentry");
					break;
				}
				case EBuildingType::DISPENSER:
				{
					szName = _(L"Dispenser");
					break;
				}
				case EBuildingType::TELEPORTER:
				{
					if (Building->GetObjectMode())
						szName = _(L"Teleporter Out");
					else
						szName = _(L"Teleporter In");
					break;
				}
				default:
				{
					szName = _(L"Unknown");
					break;
				}
				}

				nTextTopOffset += g_Draw.m_vecFonts[FONT_NAME].nTall + g_Draw.m_vecFonts[FONT_NAME].nTall / 4;
				if (Vars::ESP::Buildings::NameBox.m_Var)
				{
					int wideth, heighth;
					const int middle = x + w / 2;
					g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_NAME].dwFont, szName, wideth, heighth);
					Color_t LineColor = DrawColor;
					LineColor.a = 180;
					//g_Draw.Rect((x + (w / 2) - (wideth / 2)) - 5, y - offset - 5, wideth + 10, heighth + 10, { 0,0,0,180 });
					g_Draw.Rect(middle - wideth / 2 - 5, y - nTextTopOffset, wideth + 10, heighth + 2,
						{ 0, 0, 0, 180 });
					//g_Draw.Rect((x + (w / 2) - (wideth / 2)) - 5, y - offset - 7, wideth + 10, 2, LineColor);
					g_Draw.Rect(middle - wideth / 2 - 5, y - nTextTopOffset - 2, wideth + 10, 2, LineColor);
				}

				g_Draw.String(FONT_NAME, x + w / 2, y - nTextTopOffset, DrawColor, ALIGN_CENTERHORIZONTAL,
					szName);
			}

			if (Vars::ESP::Buildings::Owner.m_Var && !Building->GetMapPlaced())
			{
				if (const auto& pOwner = Building->GetOwner())
				{
					PlayerInfo_t pi;
					if (g_Interfaces.Engine->GetPlayerInfo(pOwner->GetIndex(), &pi))
					{
						nTextTopOffset += g_Draw.m_vecFonts[FONT_NAME].nTall + g_Draw.m_vecFonts[FONT_NAME].nTall /
							4;
						g_Draw.String(FONT_NAME, x + w / 2, y - nTextTopOffset, DrawColor, ALIGN_CENTERHORIZONTAL,
							_(L"Built by: %ls"), Utils::ConvertUtf8ToWide(pi.name).data());
					}
				}
			}

			const float flConstructed = Building->GetConstructed() * 100.0f;


			if (Vars::ESP::Buildings::Health.m_Var)
			{
				g_Draw.String(FONT, nTextX, y + nTextOffset, HealthColor, ALIGN_DEFAULT, L"%d / %d", nHealth, nHealth, Building->GetMaxHealth());
				nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
			}

			if (flConstructed < 100.0f && static_cast<int>(flConstructed) != 0)
			{
				g_Draw.String(FONT, nTextX, y + nTextOffset, DrawColor, ALIGN_DEFAULT, _(L"Building: %0.f%%"),
					flConstructed);
				nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
			}

			if (Vars::ESP::Buildings::Level.m_Var && !bIsMini)
			{
				g_Draw.String(FONT, nTextX, y + nTextOffset, DrawColor, ALIGN_DEFAULT, L"%d/3",
					Building->GetLevel());
				nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
			}

			if (Vars::ESP::Buildings::Cond.m_Var)
			{
				std::vector<std::wstring> cond_strings{};

				if (nType == EBuildingType::SENTRY && Building->GetControlled())
					cond_strings.push_back(L"Wrangled");

				if (Building->GetSapped())
					cond_strings.push_back(L"Sapped");
				else if (Building->GetDisabled()) //Building->IsSpook() 
					cond_strings.push_back(L"Disabled");

				if (!cond_strings.empty())
				{
					for (auto &cond_string : cond_strings) {
						g_Draw.String(FONT_ESP_COND, nTextX, y + nTextOffset, Colors::Cond, ALIGN_DEFAULT, cond_string.data());
						nTextOffset += g_Draw.m_vecFonts[FONT_ESP_COND].nTall;
					}
				}
			}

			if (Vars::ESP::Buildings::HealthBar.m_Var)
			{
				x -= 1;

				float flHealth = static_cast<float>(nHealth);
				const float flMaxHealth = static_cast<float>(nMaxHealth);

				if (flHealth > flMaxHealth)
					flHealth = flMaxHealth;

				static const int nWidth = 2;
				const int nHeight = h + (flHealth < flMaxHealth ? 2 : 1);
				int nHeight2 = h + 1;

				const float ratio = flHealth / flMaxHealth;

				g_Draw.Rect(x - nWidth - 2, y + nHeight - nHeight * ratio, nWidth, nHeight * ratio,
					HealthColor);

				if (Vars::ESP::Main::Outlinedbar.m_Var)
					g_Draw.OutlinedRect(x - nWidth - 2 - 1, y + nHeight - nHeight * ratio - 1, nWidth + 2,
						nHeight * ratio + 2, Colors::OutlineESP);

				x += 1;
			}

			g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);
		}
	}
}

void CESP::DrawWorld() const
{
	if (!Vars::ESP::World::Active.m_Var || !Vars::ESP::Main::Active.m_Var)
		return;

	Vec3 vScreen = {};
	const size_t FONT = FONT_ESP_PICKUPS;

	g_Interfaces.Surface->DrawSetAlphaMultiplier(Vars::ESP::World::Alpha.m_Var);

	if (Vars::ESP::World::HealthText.m_Var)
	{
		for (const auto& Health : g_EntityCache.GetGroup(EGroupType::WORLD_HEALTH))
		{
			int x = 0, y = 0, w = 0, h = 0;
			Vec3 vTrans[8];
			if (GetDrawBounds(Health, vTrans, x, y, w, h)) {
				if (Utils::W2S(Health->GetVecOrigin(), vScreen))
					g_Draw.String(FONT, vScreen.x, y + h, Colors::Health, ALIGN_CENTER, _(L"Health"));
			} // obviously a health pack isn't going to be upside down, this just looks nicer.
		}
	}

	if (Vars::ESP::World::AmmoText.m_Var)
	{
		for (const auto& Ammo : g_EntityCache.GetGroup(EGroupType::WORLD_AMMO))
		{
			int x = 0, y = 0, w = 0, h = 0;
			Vec3 vTrans[8];
			if (GetDrawBounds(Ammo, vTrans, x, y, w, h)) {
				if (Utils::W2S(Ammo->GetVecOrigin(), vScreen))
					g_Draw.String(FONT, vScreen.x, y + h, Colors::Ammo, ALIGN_CENTER, _(L"Ammo"));
			}
		}
	}

	g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);
}

typedef int ETFCond;

template < typename tIntType >
class CConditionVars
{
public:
	CConditionVars(tIntType& nPlayerCond, tIntType& nPlayerCondEx, tIntType& nPlayerCondEx2, tIntType& nPlayerCondEx3, ETFCond eCond)
	{
		if (eCond >= 96)
		{
			if (eCond < 96 + 32)
			{
				m_pnCondVar = &nPlayerCondEx3;
				m_nCondBit = eCond - 96;
			}
		}
		else if (eCond >= 64)
		{
			if (eCond < (64 + 32)) {
				m_pnCondVar = &nPlayerCondEx2;
				m_nCondBit = eCond - 64;
			}
		}
		else if (eCond >= 32)
		{
			if (eCond < (32 + 32)) {
				m_pnCondVar = &nPlayerCondEx;
				m_nCondBit = eCond - 32;
			}
		}
		else
		{
			m_pnCondVar = &nPlayerCond;
			m_nCondBit = eCond;
		}
	}

	tIntType& CondVar() const
	{
		return *m_pnCondVar;
	}

	int CondBit() const
	{
		return 1 << m_nCondBit;
	}

private:
	tIntType* m_pnCondVar;
	int m_nCondBit;
};

class CTFCondition;



class CTFConditionList
{
public:
	CTFConditionList();

	bool InCond(ETFCond type) const;

	CUtlVector<CTFCondition*> _conditions;

	int _condition_bits;
	int _old_condition_bits;
};

bool CTFConditionList::InCond(ETFCond type) const
{
	return ((_condition_bits & (1 << type)) != 0);
}

class CTFCondition
{
public:
	CTFCondition(ETFCond type, float duration, CBaseEntity* outer, CBaseEntity* provider = NULL);
	virtual ~CTFCondition();

	virtual void Add(float duration);

	virtual void OnAdded() = 0;
	virtual void OnRemoved() = 0;
	virtual void OnThink() = 0;
	virtual void OnServerThink() = 0;

	// Condition Traits
	virtual bool IsHealable() { return false; }
	virtual bool UsesMinDuration() { return false; }

	ETFCond	GetType() { return _type; }
	float	GetMaxDuration() { return _max_duration; }
	void	SetMaxDuration(float val) { _max_duration = val; }
	float	GetMinDuration() { return _min_duration; }
	void	SetMinDuration(float val) { if (UsesMinDuration()) { _min_duration = val; } }
	CBaseEntity* GetOuter() { return _outer; }
	void	SetProvider(CBaseEntity* provider) { _provider = provider; }
	CBaseEntity* GetProvider() { return _provider; }

private:
	float			_min_duration;
	float			_max_duration;
	const ETFCond	_type;
	CBaseEntity* _outer;
	CBaseEntity* _provider;
};

bool InCond(CBaseEntity* pEntity, int eCond)
{
	if (eCond >= 0 && eCond < 122) {

		void* condList = pEntity->m_ConditionList();
		const auto& conditionList = *reinterpret_cast<CTFConditionList*>(condList);

		if (conditionList._conditions.Count() > 0) {

			// Old condition system, only used for the first 32 conditions
			if (eCond < 32 && conditionList.InCond(eCond))
				return true;

			CConditionVars<const int> cPlayerCond(pEntity->GetCond(), pEntity->GetCondEx(), pEntity->GetCondEx2(), pEntity->GetCondEx3(), eCond);
			return (cPlayerCond.CondVar() & cPlayerCond.CondBit()) != 0;
		}
	}
	return false;
}

std::vector<std::wstring> CESP::GetPlayerConds(CBaseEntity* pEntity) const
{
	std::vector<std::wstring> szCond{};
	int nCond = pEntity->GetCond(), nCondEx = pEntity->GetCondEx(), nCondEx2 = pEntity->GetCondEx2();
	int nFlag = pEntity->GetFlags();

	if (InCond(pEntity, 61)) {
		szCond.push_back(L"Blast resistance");
	}

	if (InCond(pEntity, 62)) {
		szCond.push_back(L"Bullet resistance");
	}

	if (InCond(pEntity, 63)) {
		szCond.push_back(L"Fire resistance");
	}


	if (nCond & TFCond_Slowed)
	{
		if (const auto& pWeapon = pEntity->GetActiveWeapon())
		{
			if (pWeapon->GetWeaponID() == TF_WEAPON_MINIGUN)
				szCond.push_back(L"Revved");
		}
	}

	if (nFlag & FL_DUCKING)
	{
		szCond.push_back(L"Ducking");
	}

	if (pEntity->GetHealth() > pEntity->GetMaxHealth())
		szCond.push_back(L"Overhealed");

	if (nCond & TFCond_Ubercharged || nCond & TFCondEx_PhlogUber)
		szCond.push_back(L"Ubered");

	if (nCond & TFCond_MegaHeal)
		szCond.push_back(L"Megahealed");

	if (nCond & TFCond_Bonked)
		szCond.push_back(L"Bonked");

	if (nCond & TFCond_Kritzkrieged || nCond & TFCond_MiniCrits ||
		nCondEx & TFCondEx_CritCanteen || nCondEx & TFCondEx_CritOnFirstBlood || nCondEx & TFCondEx_CritOnWin ||
		nCondEx & TFCondEx_CritOnKill || nCondEx & TFCondEx_CritDemoCharge || nCondEx & TFCondEx_CritOnFlagCapture ||
		nCondEx & TFCondEx_HalloweenCritCandy || nCondEx & TFCondEx_PyroCrits)
		szCond.push_back(L"Crit boosted");

	if (nCond & TFCond_Cloaked)
		szCond.push_back(L"Cloaked");

	if (nCond & TFCond_Zoomed)
		szCond.push_back(L"Scoped");

	if (nCond & TFCond_Taunting)
		szCond.push_back(L"Taunting");

	if (nCond & TFCond_Disguised)
		szCond.push_back(L"Disguised");

	if (nCond & TFCond_Milked)
		szCond.push_back(L"Milked");

	if (nCond & TFCond_Jarated)
		szCond.push_back(L"Jarated");

	if (nCond & TFCond_Bleeding)
		szCond.push_back(L"Bleeding");

	return szCond;
}

const wchar_t* CESP::GetPlayerClass(int nClassNum)
{
	static const wchar_t* szClasses[] = {
		L"unknown", L"scout", L"sniper", L"soldier", L"demoman",
		L"medic", L"heavy", L"pyro", L"spy", L"engineer"
	};

	return nClassNum < 10 && nClassNum > 0 ? szClasses[nClassNum] : szClasses[0];
}

void CESP::CreateDLight(int nIndex, Color_t DrawColor, const Vec3& vOrigin, float flRadius)
{
	const auto pLight = g_Interfaces.EngineEffects->CL_AllocDlight(nIndex);
	pLight->m_flDie = g_Interfaces.Engine->Time() + 0.5f;
	pLight->m_flRadius = flRadius;
	pLight->m_Color.r = DrawColor.r;
	pLight->m_Color.g = DrawColor.g;
	pLight->m_Color.b = DrawColor.b;
	pLight->m_Color.m_Exponent = 5;
	pLight->m_nKey = nIndex;
	pLight->m_flDecay = 512.0f;
	pLight->m_vOrigin = vOrigin + Vec3(0.0f, 0.0f, 50.0f);
}

//Got this from dude719, who got it from somewhere else
void CESP::Draw3DBox(Vec3* vPoints, Color_t clr)
{
	Vector vStart, vEnd;

	for (int i = 0; i < 3; i++)
	{
		if (Utils::W2S(vPoints[i], vStart))
			if (Utils::W2S(vPoints[i + 1], vEnd))
				g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
	}

	if (Utils::W2S(vPoints[0], vStart))
		if (Utils::W2S(vPoints[3], vEnd))
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);

	for (int i = 4; i < 7; i++)
	{
		if (Utils::W2S(vPoints[i], vStart))
			if (Utils::W2S(vPoints[i + 1], vEnd))
				g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
	}

	if (Utils::W2S(vPoints[4], vStart))
		if (Utils::W2S(vPoints[7], vEnd))
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);

	if (Utils::W2S(vPoints[0], vStart))
		if (Utils::W2S(vPoints[6], vEnd))
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);

	if (Utils::W2S(vPoints[1], vStart))
		if (Utils::W2S(vPoints[5], vEnd))
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);

	if (Utils::W2S(vPoints[2], vStart))
		if (Utils::W2S(vPoints[4], vEnd))
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);

	if (Utils::W2S(vPoints[3], vStart))
		if (Utils::W2S(vPoints[7], vEnd))
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
}

void CESP::DrawBones(CBaseEntity* pPlayer, const std::vector<int>& vecBones, Color_t clr)
{
	const size_t nMax = vecBones.size(), nLast = nMax - 1;
	for (size_t n = 0; n < nMax; n++)
	{
		if (n == nLast)
			continue;

		const auto vBone = pPlayer->GetHitboxPos(vecBones[n]);
		const auto vParent = pPlayer->GetHitboxPos(vecBones[n + 1]);

		Vec3 vScreenBone, vScreenParent;

		if (Utils::W2S(vBone, vScreenBone) && Utils::W2S(vParent, vScreenParent))
			g_Draw.Line(vScreenBone.x, vScreenBone.y, vScreenParent.x, vScreenParent.y, clr);
	}
}
