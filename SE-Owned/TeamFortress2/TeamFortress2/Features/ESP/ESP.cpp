#include "ESP.h"
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
	int n;
	bool bIsPlayer = false;
	Vec3 vMins, vMaxs;

	if (pEntity->IsPlayer())
	{
		bIsPlayer = true;
		bool bIsDucking = pEntity->IsDucking();
		vMins = g_Interfaces.GameMovement->GetPlayerMins(bIsDucking);
		vMaxs = g_Interfaces.GameMovement->GetPlayerMaxs(bIsDucking);
	}

	else
	{
		vMins = pEntity->GetCollideableMins();
		vMaxs = pEntity->GetCollideableMaxs();
	}

	const matrix3x4& transform = pEntity->GetRgflCoordinateFrame();

	Vec3 vPoints[] =
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

	for (n = 0; n < 8; n++)
		Math::VectorTransform(vPoints[n], transform, vTrans[n]);

	Vec3 flb, brt, blb, frt, frb, brb, blt, flt;

	if (Utils::W2S(vTrans[3], flb) && Utils::W2S(vTrans[5], brt)
		&& Utils::W2S(vTrans[0], blb) && Utils::W2S(vTrans[4], frt)
		&& Utils::W2S(vTrans[2], frb) && Utils::W2S(vTrans[1], brb)
		&& Utils::W2S(vTrans[6], blt) && Utils::W2S(vTrans[7], flt)
		&& Utils::W2S(vTrans[6], blt) && Utils::W2S(vTrans[7], flt))
	{
		Vec3 arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

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
		float y_ = bottom;
		float w_ = (righ - left);
		float h_ = (top - bottom);

		if (bIsPlayer && Vars::ESP::Players::Box.m_Var != 3)
		{
			x_ += ((righ - left) / 8.0f);
			w_ -= (((righ - left) / 8.0f) * 2.0f);
		}

		x = static_cast<int>(x_);
		y = static_cast<int>(y_);
		w = static_cast<int>(w_);
		h = static_cast<int>(h_);

		return !(x > g_ScreenSize.w || (x + w) < 0 || y > g_ScreenSize.h || (y + h) < 0);
	}

	return false;
}

void CESP::DrawPlayers(CBaseEntity* pLocal)
{
	if (!Vars::ESP::Players::Active.m_Var)
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
			case 1: { if (Player->IsCloaked()) { continue; } break; }
			case 2: { if (Player->IsCloaked() && Player->GetTeamNum() != pLocal->GetTeamNum()) { continue; } break; }
			}

			switch (Vars::ESP::Players::IgnoreTeammates.m_Var)
			{
			case 0: break;
			case 1: { if (Player->GetTeamNum() == pLocal->GetTeamNum()) { continue; } break; }
			case 2: { if (Player->GetTeamNum() == pLocal->GetTeamNum() && !g_EntityCache.Friends[nIndex]) { continue; } break; }
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

		int x = 0, y = 0, w = 0, h = 0; Vec3 vTrans[8];
		if (GetDrawBounds(Player, vTrans, x, y, w, h))
		{
			int nHealth = Player->GetHealth(), nMaxHealth = Player->GetMaxHealth();
			Color_t HealthColor = Utils::GetHealthColor(nHealth, nMaxHealth);

			size_t FONT = FONT_ESP, FONT_NAME = FONT_ESP_NAME;

			int nTextX = ((x + w) + 3), nTextOffset = 0, nClassNum = Player->GetClassNum();

			if (Vars::ESP::Players::Uber.m_Var == 2 && nClassNum == ETFClass::CLASS_MEDIC) {
				if (const auto& pMedGun = Player->GetWeaponFromSlot(EWeaponSlots::SLOT_SECONDARY))
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

			switch (Vars::ESP::Players::Box.m_Var) {
			case 1: {
				int height = (h + 1); //don't ask me /shrug

				g_Draw.OutlinedRect(x, y, w, height, DrawColor);
				if (Vars::ESP::Main::Outlinedbar.m_Var) {
					g_Draw.OutlinedRect((x - 1), (y - 1), (w + 2), (height + 2), Colors::OutlineESP);
				}

				break;
			}
			case 2: {
				g_Draw.CornerRect(x, y, w, h, 3, 5, DrawColor);
				if (Vars::ESP::Main::Outlinedbar.m_Var) {
					g_Draw.CornerRect((x - 1), (y - 1), (w + 2), (h + 2), 3, 5, Colors::OutlineESP);
				}

				break;
			}
			case 3: {
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
				g_Draw.String(FONT, nTextX, (y + nTextOffset), nHealth > nMaxHealth ? Colors::Overheal : HealthColor, ALIGN_DEFAULT, L"%d / %d", nHealth, nMaxHealth);
				nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
			}

			if (Vars::ESP::Players::Uber.m_Var && nClassNum == ETFClass::CLASS_MEDIC)
			{
				if (const auto& pMedGun = Player->GetWeaponFromSlot(EWeaponSlots::SLOT_SECONDARY))
				{
					if (Vars::ESP::Players::Uber.m_Var == 1)
					{
						g_Draw.String(FONT, nTextX, (y + nTextOffset), Colors::UberColor, ALIGN_DEFAULT, L"%.0f%%", (pMedGun->GetUberCharge() * 100.0f));
						nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
					}

					if (Vars::ESP::Players::Uber.m_Var == 2 && pMedGun->GetUberCharge())
					{
						x += w + 1;

						float flUber = pMedGun->GetUberCharge() * (pMedGun->GetItemDefIndex() == Medic_s_TheVaccinator ? 400.0f : 100.0f);
						float flMaxUber = 100.0f;

						if (flUber > flMaxUber)
							flUber = flMaxUber;

						static const int nWidth = 2;
						int nHeight = (h + (flUber < flMaxUber ? 2 : 1));
						int nHeight2 = (h + 1);

						float ratio = (flUber / flMaxUber);
						g_Draw.Rect((x + nWidth), (y + nHeight - (nHeight * ratio)), nWidth, (nHeight * ratio), Colors::UberColor);

						if (Vars::ESP::Main::Outlinedbar.m_Var)
							g_Draw.OutlinedRect((x + nWidth) - 1, (y + nHeight - (nHeight * ratio)) - 1, nWidth + 2, (nHeight * ratio) + 2, Colors::OutlineESP);

						x -= w + 1;
					}
				}
			}

			PlayerInfo_t pi;
			if (g_Interfaces.Engine->GetPlayerInfo(nIndex, &pi))
			{
				int middle = (x + (w / 2));
				if (Vars::ESP::Players::Name.m_Var)
				{
					int offset = (g_Draw.m_vecFonts[FONT_NAME].nTall + (g_Draw.m_vecFonts[FONT_NAME].nTall / 4));
					if (Vars::ESP::Players::NameBox.m_Var) {
						int wideth, heighth;
						g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_NAME].dwFont, Utils::ConvertUtf8ToWide(pi.name).data(), wideth, heighth);
						Color_t LineColor = DrawColor;
						LineColor.a = 180;
						//g_Draw.Rect((x + (w / 2) - (wideth / 2)) - 5, y - offset - 5, wideth + 10, heighth + 10, { 0,0,0,180 });
						g_Draw.Rect((middle - (wideth / 2)) - 5, y - offset, wideth + 10, heighth + 2, { 0,0,0,180 });
						//g_Draw.Rect((x + (w / 2) - (wideth / 2)) - 5, y - offset - 7, wideth + 10, 2, LineColor);
						g_Draw.Rect((middle - (wideth / 2)) - 5, y - offset - 2, wideth + 10, 2, LineColor);
						offset -= 1;
					}
					g_Draw.String(FONT_NAME, middle, (y - offset), DrawColor, ALIGN_CENTERHORIZONTAL, Utils::ConvertUtf8ToWide(pi.name).data());
				}

				if (Vars::ESP::Players::GUID.m_Var)
				{
					g_Draw.String(FONT, nTextX, (y + nTextOffset), Colors::White, ALIGN_DEFAULT, "%s", pi.guid);
					nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
				}
			}

			if (Vars::ESP::Players::Class.m_Var)
			{
				if (Vars::ESP::Players::Class.m_Var == 1 || Vars::ESP::Players::Class.m_Var == 3)
				{
					int offset = (Vars::ESP::Players::Name.m_Var ? g_Draw.m_vecFonts[FONT_NAME].nTall + (g_Draw.m_vecFonts[FONT_NAME].nTall * 0.3f) : 0);

					if (offset && Vars::ESP::Players::NameBox.m_Var)
						offset += 2;

					static const int nSize = 18;
					g_Draw.Texture((x + (w / 2) - (nSize / 2)), ((y - offset) - nSize), nSize, nSize, Colors::White, nClassNum);
				}

				if (Vars::ESP::Players::Class.m_Var >= 2)
				{
					g_Draw.String(FONT, nTextX, (y + nTextOffset), DrawColor, ALIGN_DEFAULT, L"%ls", GetPlayerClass(nClassNum));
					nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
				}
			}

			if (Vars::ESP::Players::WeaponIcon.m_Var)
			{
				const auto& pWeapon = Player->GetActiveWeapon();
				if (pWeapon) {
					CHudTexture* pIcon = pWeapon->GetWeaponIcon();
					if (pIcon) {
						float fx, fy, fw, fh;
						fx = (float)x; fy = (float)y; fw = (float)w; fh = (float)h;
						const float iconWidth = (float)pIcon->Width();
						// lol
						const float scale = std::clamp(fw / iconWidth, 0.5f, 0.75f);
						static float easedScale = 0.5f;
						scale > easedScale ? easedScale = g_Draw.EaseOut(scale, easedScale, 0.99f) : easedScale = g_Draw.EaseIn(easedScale, scale, 0.99f);
						g_Draw.DrawHudTexture(fx + (fw / 2.f) - (iconWidth / 2.f) * scale, fy + fh + 1.f, scale, pIcon, Colors::WeaponIcon);
					}
				}
			}

			if (Vars::ESP::Players::Cond.m_Var)
			{
				size_t FONT = FONT_ESP_COND;
				int offset = g_Draw.m_vecFonts[FONT].nTall / 4;
				std::wstring cond_str = GetPlayerConds(Player);

				if (!cond_str.empty())
				{
					cond_str.erase((cond_str.end() - 1), cond_str.end());
					g_Draw.String(FONT, (x + (w / 2)), ((y + h) + offset), Colors::Cond, ALIGN_CENTERHORIZONTAL, cond_str.data());
				}
			}

			if (Vars::ESP::Players::HealthBar.m_Var)
			{
				x -= 1;

				float flHealth = static_cast<float>(nHealth);
				float flMaxHealth = static_cast<float>(nMaxHealth);

				Color_t clr = flHealth > flMaxHealth ? Colors::Overheal : HealthColor;

				if (!Player->IsVulnerable())
					clr = Colors::Invuln;

				if (flHealth > flMaxHealth)
					flHealth = flMaxHealth;

				static const int nWidth = 2;
				int nHeight = (h + (flHealth < flMaxHealth ? 2 : 1));
				int nHeight2 = (h + 1);

				float ratio = (flHealth / flMaxHealth);
				g_Draw.Rect(((x - nWidth) - 2), (y + nHeight - (nHeight * ratio)), nWidth, (nHeight * ratio), clr);

				if (Vars::ESP::Main::Outlinedbar.m_Var)
					g_Draw.OutlinedRect(((x - nWidth) - 2) - 1, (y + nHeight - (nHeight * ratio)) - 1, nWidth + 2, (nHeight * ratio) + 2, Colors::OutlineESP);

				x += 1;
			}

			g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);
		}
	}
}

void CESP::DrawBuildings(CBaseEntity* pLocal)
{
	if (!Vars::ESP::Buildings::Active.m_Var)
		return;

	const auto Buildings = g_EntityCache.GetGroup(Vars::ESP::Buildings::IgnoreTeammates.m_Var ? EGroupType::BUILDINGS_ENEMIES : EGroupType::BUILDINGS_ALL);

	for (const auto& pBuilding : Buildings)
	{
		if (!pBuilding->IsAlive())
			continue;

		const auto& Building = reinterpret_cast<CBaseObject*>(pBuilding);

		Color_t DrawColor = Utils::GetEntityDrawColor(Building, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);

		if (Vars::ESP::Buildings::Dlights.m_Var)
			CreateDLight(Building->GetIndex(), DrawColor, Building->GetAbsOrigin(), Vars::ESP::Buildings::DlightRadius.m_Var);

		int x = 0, y = 0, w = 0, h = 0; Vec3 vTrans[8];
		if (GetDrawBounds(Building, vTrans, x, y, w, h))
		{
			auto nHealth = Building->GetHealth(), nMaxHealth = Building->GetMaxHealth(),
				nTextX = ((x + w) + 3), nTextOffset = 0, nTextTopOffset = 0;

			Color_t HealthColor = Utils::GetHealthColor(nHealth, nMaxHealth);

			auto nType = EBuildingType(Building->GetType());

			size_t FONT = FONT_ESP, FONT_NAME = FONT_ESP_NAME, FONT_COND = FONT_ESP_COND;

			bool bIsMini = Building->GetMiniBuilding();

			g_Interfaces.Surface->DrawSetAlphaMultiplier(Vars::ESP::Buildings::Alpha.m_Var);

			switch (Vars::ESP::Buildings::Box.m_Var) {
			case 1: {
				h += 1;

				g_Draw.OutlinedRect(x, y, w, h, DrawColor);

				if (Vars::ESP::Main::Outlinedbar.m_Var)
					g_Draw.OutlinedRect((x - 1), (y - 1), (w + 2), (h + 2), Colors::OutlineESP);

				h -= 1;
				break;
			}
			case 2: {
				g_Draw.CornerRect(x, y, w, h, 3, 5, DrawColor);

				if (Vars::ESP::Main::Outlinedbar.m_Var)
					g_Draw.CornerRect((x - 1), (y - 1), (w + 2), (h + 2), 3, 5, Colors::OutlineESP);
				break;
			}
			case 3: {
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
					{
						szName = _(L"Mini Sentry");
						break;
					}

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
					szName = _(L"Teleporter");
					break;
				}
				default:
				{
					szName = _(L"Unknown");
					break;
				}
				}

				nTextTopOffset += (g_Draw.m_vecFonts[FONT_NAME].nTall + (g_Draw.m_vecFonts[FONT_NAME].nTall / 4));
				if (Vars::ESP::Buildings::NameBox.m_Var) {
					int wideth, heighth;
					int middle = (x + (w / 2));
					g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_NAME].dwFont, szName, wideth, heighth);
					Color_t LineColor = DrawColor;
					LineColor.a = 180;
					//g_Draw.Rect((x + (w / 2) - (wideth / 2)) - 5, y - offset - 5, wideth + 10, heighth + 10, { 0,0,0,180 });
					g_Draw.Rect((middle - (wideth / 2)) - 5, y - nTextTopOffset, wideth + 10, heighth + 2, { 0,0,0,180 });
					//g_Draw.Rect((x + (w / 2) - (wideth / 2)) - 5, y - offset - 7, wideth + 10, 2, LineColor);
					g_Draw.Rect((middle - (wideth / 2)) - 5, y - nTextTopOffset - 2, wideth + 10, 2, LineColor);
				}

				g_Draw.String(FONT_NAME, (x + (w / 2)), (y - nTextTopOffset), DrawColor, ALIGN_CENTERHORIZONTAL, szName);
			}

			if (Vars::ESP::Buildings::Owner.m_Var && !Building->GetMapPlaced())
			{
				if (const auto& pOwner = Building->GetOwner())
				{
					PlayerInfo_t pi;
					if (g_Interfaces.Engine->GetPlayerInfo(pOwner->GetIndex(), &pi))
					{
						nTextTopOffset += (g_Draw.m_vecFonts[FONT_NAME].nTall + (g_Draw.m_vecFonts[FONT_NAME].nTall / 4));
						g_Draw.String(FONT_NAME, (x + (w / 2)), (y - nTextTopOffset), DrawColor, ALIGN_CENTERHORIZONTAL, _(L"Built by: %ls"), Utils::ConvertUtf8ToWide(pi.name).data());
					}
				}
			}

			float flConstructed = Building->GetConstructed() * 100.0f;
			if (flConstructed < 100.0f && static_cast<int>(flConstructed) != 0)
			{
				g_Draw.String(FONT_NAME, nTextX, (y + nTextOffset), DrawColor, ALIGN_DEFAULT, _(L"Building: %0.f%%"), flConstructed);
				nTextOffset += g_Draw.m_vecFonts[FONT_NAME].nTall;
			}

			if (Vars::ESP::Buildings::Health.m_Var)
			{
				g_Draw.String(FONT_NAME, nTextX, (y + nTextOffset), HealthColor, ALIGN_DEFAULT, L"%d", nHealth);
				nTextOffset += g_Draw.m_vecFonts[FONT_NAME].nTall;
			}

			if (Vars::ESP::Buildings::Level.m_Var && !bIsMini)
			{
				g_Draw.String(FONT_NAME, x + (w / 2), (y + h + nTextOffset), DrawColor, ALIGN_CENTERHORIZONTAL, L"%d/3", Building->GetLevel());
				nTextOffset += g_Draw.m_vecFonts[FONT_NAME].nTall;
			}

			if (Vars::ESP::Buildings::Cond.m_Var)
			{
				int offset = g_Draw.m_vecFonts[FONT_COND].nTall / 4;

				std::wstring cond_str = L"";

				if (nType == EBuildingType::SENTRY && Building->GetControlled())
					cond_str += _(L"CONTROLLED "); //dispensers are also apparently controlled sometimes
				//They are controlled when player is in their range, maybe add cond for them "ACTIVE" or something
				//indicating there is a player nearby.

				if (Building->GetDisabled()) //Building->IsSpook()
					cond_str += _(L"DISABLED ");

				if (!cond_str.empty())
				{
					cond_str.erase((cond_str.end() - 1), cond_str.end());
					g_Draw.String(FONT_COND, (x + (w / 2)), (y + h) + offset, Colors::Cond, ALIGN_CENTERHORIZONTAL, cond_str.data());
				}
			}

			if (Vars::ESP::Buildings::HealthBar.m_Var)
			{
				x -= 1;

				float flHealth = static_cast<float>(nHealth);
				float flMaxHealth = static_cast<float>(nMaxHealth);

				if (flHealth > flMaxHealth)
					flHealth = flMaxHealth;

				static const int nWidth = 2;
				int nHeight = (h + (flHealth < flMaxHealth ? 2 : 1));
				int nHeight2 = (h + 1);

				float ratio = (flHealth / flMaxHealth);

				g_Draw.Rect(((x - nWidth) - 2), (y + nHeight - (nHeight * ratio)), nWidth, (nHeight * ratio), HealthColor);

				if (Vars::ESP::Main::Outlinedbar.m_Var)
					g_Draw.OutlinedRect(((x - nWidth) - 2) - 1, (y + nHeight - (nHeight * ratio)) - 1, nWidth + 2, (nHeight * ratio) + 2, Colors::OutlineESP);

				x += 1;
			}

			g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);
		}
	}
}

void CESP::DrawWorld()
{
	if (!Vars::ESP::World::Active.m_Var)
		return;

	Vec3 vScreen = {};
	size_t FONT = FONT_ESP_PICKUPS;

	g_Interfaces.Surface->DrawSetAlphaMultiplier(Vars::ESP::World::Alpha.m_Var);

	if (Vars::ESP::World::HealthText.m_Var)
	{
		for (const auto& Health : g_EntityCache.GetGroup(EGroupType::WORLD_HEALTH))
		{
			if (Utils::W2S(Health->GetWorldSpaceCenter(), vScreen))
				g_Draw.String(FONT, vScreen.x, vScreen.y, Colors::Health, ALIGN_CENTER, _(L"HEALTH"));
		}
	}

	if (Vars::ESP::World::AmmoText.m_Var)
	{
		for (const auto& Ammo : g_EntityCache.GetGroup(EGroupType::WORLD_AMMO))
		{
			if (Utils::W2S(Ammo->GetWorldSpaceCenter(), vScreen))
				g_Draw.String(FONT, vScreen.x, vScreen.y, Colors::Ammo, ALIGN_CENTER, _(L"AMMO"));
		}
	}

	g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);
}

std::wstring CESP::GetPlayerConds(CBaseEntity* pEntity)
{
	std::wstring szCond = L"";
	int nCond = pEntity->GetCond(), nCondEx = pEntity->GetCondEx(), nCondEx2 = pEntity->GetCondEx2();
	int nFlag = pEntity->GetFlags();

	if (nCond & TFCond_Slowed)
	{
		if (const auto& pWeapon = pEntity->GetActiveWeapon())
		{
			if (pWeapon->GetWeaponID() == TF_WEAPON_MINIGUN)
				szCond += _(L"REV  ");
		}
	}

	if (nFlag & FL_DUCKING)
	{
		szCond += _(L"DUCKING ");
	}

	if (nCondEx2 & TFCondEx2_BlastImmune)
		szCond += _(L"BLAST IMMUNE ");

	if (nCondEx2 & TFCondEx2_BulletImmune)
		szCond += _(L"BULLET IMMUNE ");

	if (nCondEx2 & TFCondEx2_FireImmune)
		szCond += _(L"FIRE IMMUNE ");

	if (pEntity->GetHealth() > pEntity->GetMaxHealth())
		szCond += _(L"HP+ ");

	if ((nCond & TFCond_Ubercharged) || (nCond & TFCondEx_PhlogUber))
		szCond += _(L"UBER ");

	if (nCond & TFCond_MegaHeal)
		szCond += _(L"MEGAHEAL ");

	if (nCond & TFCond_Bonked)
		szCond += _(L"BONK ");

	if (nCond & TFCond_Kritzkrieged || nCond & TFCond_MiniCrits ||
		nCondEx & TFCondEx_CritCanteen || nCondEx & TFCondEx_CritOnFirstBlood || nCondEx & TFCondEx_CritOnWin ||
		nCondEx & TFCondEx_CritOnKill || nCondEx & TFCondEx_CritDemoCharge || nCondEx & TFCondEx_CritOnFlagCapture ||
		nCondEx & TFCondEx_HalloweenCritCandy || nCondEx & TFCondEx_PyroCrits)
		szCond += _(L"CRIT ");

	if (nCond & TFCond_Cloaked)
		szCond += _(L"CLOAK ");

	if (nCond & TFCond_Zoomed)
		szCond += _(L"SCOPE ");

	if (nCond & TFCond_Taunting)
		szCond += _(L"TAUNT ");

	if (nCond & TFCond_Disguised)
		szCond += _(L"DISG ");

	if (nCond & TFCond_Milked)
		szCond += _(L"MILK ");

	if (nCond & TFCond_Jarated)
		szCond += _(L"JARATE ");

	if (nCond & TFCond_Bleeding)
		szCond += _(L"BLEED ");

	return szCond;
}

const wchar_t* CESP::GetPlayerClass(int nClassNum)
{
	static const wchar_t* szClasses[] = { L"unknown", L"scout", L"sniper", L"soldier", L"demoman",
										  L"medic",   L"heavy", L"pyro",   L"spy",     L"engineer" };

	return (nClassNum < 10 && nClassNum > 0) ? szClasses[nClassNum] : szClasses[0];
}

void CESP::CreateDLight(int nIndex, Color_t DrawColor, const Vec3& vOrigin, float flRadius)
{
	auto pLight = g_Interfaces.EngineEffects->CL_AllocDlight(nIndex);
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
	size_t n, nMax = vecBones.size(), nLast = (nMax - 1);
	for (n = 0; n < nMax; n++)
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