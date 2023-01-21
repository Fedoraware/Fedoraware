#include "ESP.h"
#include "../AntiHack/CheaterDetection/CheaterDetection.h"
#include "../Vars.h"

bool CESP::ShouldRun()
{
	if (!(I::EngineClient->IsTakingScreenshot() && Vars::Visuals::CleanScreenshots.Value) && I::EngineVGui->IsGameUIVisible())
	{
		return false;
	}

	return true;
}

void CESP::Run()
{
	if (const auto& pLocal = g_EntityCache.GetLocal())
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
		vMins = I::GameMovement->GetPlayerMins(bIsDucking);
		vMaxs = I::GameMovement->GetPlayerMaxs(bIsDucking);
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
	{
		Math::VectorTransform(vPoints[n], transform, vTrans[n]);
	}

	Vec3 flb, brt, blb, frt, frb, brb, blt, flt;

	if (Utils::W2S(vTrans[3], flb) && Utils::W2S(vTrans[5], brt)
		&& Utils::W2S(vTrans[0], blb) && Utils::W2S(vTrans[4], frt)
		&& Utils::W2S(vTrans[2], frb) && Utils::W2S(vTrans[1], brb)
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
			{
				left = arr[n].x;
			}

			if (top < arr[n].y)
			{
				top = arr[n].y;
			}

			if (righ < arr[n].x)
			{
				righ = arr[n].x;
			}

			if (bottom > arr[n].y)
			{
				bottom = arr[n].y;
			}
		}

		float x_ = left;
		const float y_ = bottom;
		float w_ = righ - left;
		const float h_ = top - bottom;

		if (bIsPlayer && Vars::ESP::Players::Box.Value != 3)
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
	if (!Vars::ESP::Players::Active.Value || !Vars::ESP::Main::Active.Value)
	{
		return;
	}

	CTFPlayerResource* cResource = g_EntityCache.GetPR();
	if (!cResource) { return; }

	for (const auto& Player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
	{
		if (!Player->IsAlive() || Player->IsAGhost())
		{
			continue;
		}

		// distance things
		const Vec3 vDelta = Player->GetAbsOrigin() - pLocal->GetAbsOrigin();
		const float flDistance = vDelta.Length2D();
		if (flDistance >= (Player->GetDormant() ? Vars::ESP::Main::DormantDist.Value : Vars::ESP::Main::NetworkedDist.Value)) { continue; }
		I::VGuiSurface->DrawSetAlphaMultiplier(Vars::ESP::Main::DistanceToAlpha.Value ? Math::RemapValClamped(flDistance, (Player->GetDormant() ? Vars::ESP::Main::DormantDist.Value : Vars::ESP::Main::NetworkedDist.Value) - 256.f, (Player->GetDormant() ? Vars::ESP::Main::DormantDist.Value : Vars::ESP::Main::NetworkedDist.Value), Vars::ESP::Players::Alpha.Value, 0.f) : Vars::ESP::Players::Alpha.Value);

		if (Player->GetDormant()){
			Player->m_iHealth() = cResource->GetHealth(Player->GetIndex());
		}

		int nIndex = Player->GetIndex();
		bool bIsLocal = nIndex == I::EngineClient->GetLocalPlayer();

		if (!bIsLocal)
		{
			switch (Vars::ESP::Players::IgnoreCloaked.Value)
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

			switch (Vars::ESP::Players::IgnoreTeammates.Value)
			{
				case 0: break;
				case 1:
				{
					if (Player->GetTeamNum() == pLocal->GetTeamNum()) { continue; }
					break;
				}
				case 2:
				{
					if (Player->GetTeamNum() == pLocal->GetTeamNum() && !g_EntityCache.IsFriend(nIndex)) { continue; }
					break;
				}
			}
		}

		else
		{
			if (!Vars::ESP::Players::ShowLocal.Value)
			{
				continue;
			}
		}

		Color_t drawColor = Utils::GetEntityDrawColor(Player, Vars::ESP::Main::EnableTeamEnemyColors.Value);

		// Player lights
		if (Vars::ESP::Players::Dlights.Value)
		{
			CreateDLight(nIndex, drawColor, Player->GetAbsOrigin(), Vars::ESP::Players::DlightRadius.Value);
		}

		int x = 0, y = 0, w = 0, h = 0;
		Vec3 vTrans[8];
		if (GetDrawBounds(Player, vTrans, x, y, w, h))
		{
			int nHealth = Player->GetHealth(), nMaxHealth = Player->GetMaxHealth();
			Color_t healthColor = Utils::GetHealthColor(nHealth, nMaxHealth);

			size_t FONT = FONT_ESP, FONT_NAME = FONT_ESP_NAME;

			int nTextX = x + w + 3, nTextOffset = 0, nClassNum = Player->GetClassNum();

			if (Vars::ESP::Players::Uber.Value == 2 && nClassNum == CLASS_MEDIC)
			{
				if (const auto& pMedGun = Player->GetWeaponFromSlot(SLOT_SECONDARY))
				{
					nTextX += 5;
				}
			}

			// Bone ESP
			if (Vars::ESP::Players::Bones.Value)
			{
				const Color_t clrBone = Vars::ESP::Players::Bones.Value == 1 ? Colors::Bones : healthColor;

				DrawBones(Player, { 8, 7, 6, 4 }, clrBone);
				DrawBones(Player, { 11, 10, 9, 4 }, clrBone);
				DrawBones(Player, { 0, 4, 1 }, clrBone);
				DrawBones(Player, { 14, 13, 1 }, clrBone);
				DrawBones(Player, { 17, 16, 1 }, clrBone);
			}

			// Box ESP (Outlined, Rect, 3D)
			switch (Vars::ESP::Players::Box.Value)
			{
				case 1:
				{
					int height = h + 1; //don't ask me /shrug

					g_Draw.OutlinedRect(x, y, w, height, drawColor);
					if (Vars::ESP::Main::Outlinedbar.Value)
					{
						g_Draw.OutlinedRect(x - 1, y - 1, w + 2, height + 2, Colors::OutlineESP);
					}

					break;
				}
				case 2:
				{
					g_Draw.CornerRect(x, y, w, h, 3, 5, drawColor);
					if (Vars::ESP::Main::Outlinedbar.Value)
					{
						g_Draw.CornerRect(x - 1, y - 1, w + 2, h + 2, 3, 5, Colors::OutlineESP);
					}

					break;
				}
				case 3:
				{
					Draw3DBox(vTrans, drawColor);
					break;
				}
				default:
					break;
			}

			// Player lines
			if (Vars::ESP::Players::Lines.Value)
			{
				Vec3 vScreen, vOrigin = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);

				if (I::Input->CAM_IsThirdPerson())
				{
					Utils::W2S(pLocal->GetAbsOrigin(), vOrigin);
				}

				if (Utils::W2S(Player->GetAbsOrigin(), vScreen))
				{
					g_Draw.Line(vOrigin.x, vOrigin.y, vScreen.x, vScreen.y, drawColor);
				}
			}

			// Health Text
			if (Vars::ESP::Players::HealthText.Value == 1)
			{
				g_Draw.String(FONT, nTextX, y + nTextOffset, nHealth > nMaxHealth ? Colors::Overheal : healthColor,
							  ALIGN_DEFAULT, L"%d / %d", nHealth, nMaxHealth);
				nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
			}

			if (Vars::Debug::DebugInfo.Value)
			{
				Vec3 vPlayerVelocity{};
				Player->EstimateAbsVelocity(vPlayerVelocity);
				g_Draw.String(FONT, nTextX, y + nTextOffset, Colors::White, ALIGN_DEFAULT, L"SPEED (%.0f)", vPlayerVelocity.Length());
			}

			// Ubercharge status/bar
			if (Vars::ESP::Players::Uber.Value && nClassNum == CLASS_MEDIC)
			{
				if (const auto& pMedGun = Player->GetWeaponFromSlot(SLOT_SECONDARY))
				{
					if (Vars::ESP::Players::Uber.Value == 1)
					{
						g_Draw.String(FONT, nTextX, y + nTextOffset, Colors::UberColor, ALIGN_DEFAULT, L"%.0f%%",
									  pMedGun->GetUberCharge() * 100.0f);
						nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
					}

					if (Vars::ESP::Players::Uber.Value == 2 && pMedGun->GetUberCharge())
					{
						x += w + 1;

						float flUber = pMedGun->GetUberCharge() * (pMedGun->GetItemDefIndex() == Medic_s_TheVaccinator
																   ? 400.0f
																   : 100.0f);

						float flMaxUber = (pMedGun->GetItemDefIndex() == Medic_s_TheVaccinator ? 400.0f : 100.0f);

						if (flUber > flMaxUber)
						{
							flUber = flMaxUber;
						}

						static constexpr int RECT_WIDTH = 2;
						int nHeight = h + (flUber < flMaxUber ? 2 : 1);
						int nHeight2 = h + 1;

						float ratio = flUber / flMaxUber;
						g_Draw.Rect(x + RECT_WIDTH, y + nHeight - nHeight * ratio, RECT_WIDTH, nHeight * ratio,
									Colors::UberColor);

						if (Vars::ESP::Main::Outlinedbar.Value)
						{
							g_Draw.OutlinedRect(x + RECT_WIDTH - 1, y + nHeight - nHeight * ratio - 1, RECT_WIDTH + 2,
												nHeight * ratio + 2, Colors::OutlineESP);
						}

						x -= w + 1;
					}
				}
			}

			PlayerInfo_t pi{};
			if (I::EngineClient->GetPlayerInfo(nIndex, &pi))
			{
				// Name ESP
				int middle = x + w / 2;
				if (Vars::ESP::Players::Name.Value)
				{
					int offset = g_Draw.m_vecFonts[FONT_NAME].nTall + g_Draw.m_vecFonts[FONT_NAME].nTall / 4;
					if (Vars::ESP::Players::NameBox.Value)
					{
						int wideth, heighth;
						I::VGuiSurface->GetTextSize(g_Draw.m_vecFonts[FONT_NAME].dwFont,
													Utils::ConvertUtf8ToWide(pi.name).data(), wideth, heighth);
						Color_t LineColor = drawColor;
						LineColor.a = 180;
						//g_Draw.Rect((x + (w / 2) - (wideth / 2)) - 5, y - offset - 5, wideth + 10, heighth + 10, { 0,0,0,180 });
						g_Draw.Rect(middle - wideth / 2 - 5, y - offset, wideth + 10, heighth + 2, { 0, 0, 0, 180 });
						//g_Draw.Rect((x + (w / 2) - (wideth / 2)) - 5, y - offset - 7, wideth + 10, 2, LineColor);
						g_Draw.Rect(middle - wideth / 2 - 5, y - offset - 2, wideth + 10, 2, LineColor);
						offset -= 1;
					}
					if (Vars::ESP::Players::NameCustom.Value)
					{
						g_Draw.String(FONT_NAME, middle, y - offset, Vars::ESP::Players::NameColor, ALIGN_CENTERHORIZONTAL,
									  Utils::ConvertUtf8ToWide(pi.name).data());
					}
					else
					{
						g_Draw.String(FONT_NAME, middle, y - offset, drawColor, ALIGN_CENTERHORIZONTAL,
									  Utils::ConvertUtf8ToWide(pi.name).data());
					}
				}

				// Cheater detection ESP
				if (G::PlayerPriority[pi.friendsID].Mode == 4 && Vars::ESP::Players::CheaterDetection.Value)
				{
					g_Draw.String(FONT, middle, y - 28, { 255, 0, 0, 255 }, ALIGN_CENTERHORIZONTAL, "CHEATER");
					nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
				}

				// GUID ESP
				if (Vars::ESP::Players::GUID.Value)
				{
					g_Draw.String(FONT, nTextX, y + nTextOffset, Colors::White, ALIGN_DEFAULT, "%s", pi.guid);
					nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
				}
			}

			// Class ESP
			if (Vars::ESP::Players::Class.Value)
			{
				if (Vars::ESP::Players::Class.Value == 1 || Vars::ESP::Players::Class.Value == 3)
				{
					int offset = Vars::ESP::Players::Name.Value
						? g_Draw.m_vecFonts[FONT_NAME].nTall + g_Draw.m_vecFonts[FONT_NAME].nTall * 0.3f
						: 0;

					if (offset && Vars::ESP::Players::NameBox.Value)
					{
						offset += 2;
					}

					static constexpr int TEXTURE_SIZE = 18;
					if (Vars::ESP::Players::CheaterDetection.Value && G::PlayerPriority[pi.friendsID].Mode == 4)
					{
						g_Draw.Texture(x + w / 2 - TEXTURE_SIZE / 2, y - 30 - TEXTURE_SIZE, TEXTURE_SIZE, TEXTURE_SIZE, Colors::White,
									   nClassNum);
					}
					else
						g_Draw.Texture(x + w / 2 - TEXTURE_SIZE / 2, y - offset - TEXTURE_SIZE, TEXTURE_SIZE, TEXTURE_SIZE, Colors::White,
									   nClassNum);
				}

				if (Vars::ESP::Players::Class.Value >= 2)
				{
					g_Draw.String(FONT, nTextX, y + nTextOffset, drawColor, ALIGN_DEFAULT, L"%ls",
								  GetPlayerClass(nClassNum));
					nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
				}
			}

			const auto& pWeapon = Player->GetActiveWeapon();
			if (pWeapon)
			{
				int weaponoffset = 0;
				// Weapon text
				if (Vars::ESP::Players::WeaponText.Value)
				{
					static auto CTFPlayerSharedUtils_GetEconItemViewByLoadoutSlot = reinterpret_cast<void* (__cdecl*)(void*, int, void**)>(
						g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 08 53 56 57 8B 7D 08 33 F6 8B 87 ? ? ? ? 81 C7 ? ? ? ? 89 45 FC 8D 49 00")
						);

					static auto C_EconItemView_GetItemName = reinterpret_cast<const char* (__thiscall*)(void*)>(
						g_Pattern.Find(L"client.dll", L"56 8B F1 C6 86 ? ? ? ? ? E8 ? ? ? ? 8B 8E ? ? ? ? 5E 85 C9 75 06")
						);

					int iWeaponSlot = pWeapon->GetSlot();
					int iPlayerClass = Player->GetClassNum();

					const char* szItemName = "";

					switch (iPlayerClass)
					{
						case CLASS_SPY:
						{
							switch (iWeaponSlot)
							{
								case 0: // Primary (gun)
								{
									iWeaponSlot = 1;
									break;
								}
								case 1: // Sapper ????
								{
									iWeaponSlot = 4;
									break;
								}
								// case 2: // Knife
								case 3: // Disguise kit
								{
									iWeaponSlot = 5;
									break;
								}
							}
							break;
						}
						case CLASS_ENGINEER:
						{
							switch (iWeaponSlot)
							{
								case 3:
								{
									iWeaponSlot = 5;
									break;
								}
								case 4:
								{
									iWeaponSlot = 6;
									break;
								}
							}
							break;
						}
						default: break;
					}

					void* pCurItemData = CTFPlayerSharedUtils_GetEconItemViewByLoadoutSlot(Player, iWeaponSlot, 0);
					if (pCurItemData)
					{
						int offset = 0;
						if (Vars::ESP::Players::Distance.Value)
						{
							offset = 10;
						}
						szItemName = C_EconItemView_GetItemName(pCurItemData);
						g_Draw.String(FONT_ESP, x + (w / 2), y + h + offset, Colors::WeaponIcon, ALIGN_CENTERHORIZONTAL, "%ls", szItemName);
						weaponoffset += Vars::Fonts::FONT_ESP::nTall.Value;
					}


					//static auto C_EconItemView_GetStaticData = reinterpret_cast<void* (__thiscall*)(void*)>(
					//	g_Pattern.Find(L"client.dll", L"0F B7 41 24 50 E8 ? ? ? ? 8B C8 E8 ? ? ? ? 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 50 E8 ? ? ? ? 83 C4 14 C3")
					//	);

					//void* pItem = pWeapon->m_Item();

					//void* pItemDefinition = C_EconItemView_GetStaticData(pItem);

					//if (auto szItemName = C_EconItemView_GetItemName(pItemDefinition))
					//{
					//	
					//}

					/*wchar_t szItemName[128]{};
					if (pWeapon->GetLocalizedBaseItemName(szItemName))
					{
						g_Draw.String(FONT_ESP, x + (w / 2), y + h, Colors::WeaponIcon, ALIGN_CENTERHORIZONTAL, "%ls", szItemName);
						weaponoffset += Vars::Fonts::FONT_ESP::nTall.Value;
					}*/
				}

				// Weapon icons
				if (Vars::ESP::Players::WeaponIcon.Value)
				{

					CHudTexture* pIcon = pWeapon->GetWeaponIcon();
					if (pIcon)
					{
						int offset = 0;
						if (Vars::ESP::Players::Distance.Value)
						{
							offset = 10;
						}
						float fx, fy, fw, fh;
						fx = static_cast<float>(x);
						fy = static_cast<float>(y);
						fw = static_cast<float>(w);
						fh = static_cast<float>(h);
						const auto iconWidth = static_cast<float>(pIcon->Width());
						// lol
						const float scale = std::clamp(fw / iconWidth, 0.5f, 0.75f);
						static float easedScale = 0.5f;
						scale > easedScale
							? easedScale = g_Draw.EaseOut(scale, easedScale, 0.99f)
							: easedScale = g_Draw.EaseIn(easedScale, scale, 0.99f);
						g_Draw.DrawHudTexture(fx + fw / 2.f - iconWidth / 2.f * scale, fy + fh + 1.f + weaponoffset + offset, scale, pIcon,
											  Colors::WeaponIcon);
					}
				}
			}

			//Distance ESP
			if (Vars::ESP::Players::Distance.Value)
			{
				const Vec3 vDelta = Player->GetAbsOrigin() - pLocal->GetAbsOrigin();
				const float flDistance = vDelta.Length2D() * 0.01905; // 1 m = 52.49 hu, so this is accurate *enough*
				const int Distance = std::round(flDistance); //return as an int, so it doesnt show a shitty decimal number

				g_Draw.String(FONT_ESP, x + (w / 2), y + h, Colors::White, ALIGN_CENTERHORIZONTAL, L"[%d M]", Distance);
			}



			// Player conditions
			if (Vars::ESP::Players::Cond.Value)
			{
				size_t FONT = FONT_ESP_COND;
				int offset = g_Draw.m_vecFonts[FONT].nTall / 4;
				std::vector<std::wstring> cond_strings = GetPlayerConds(Player);

				if (!cond_strings.empty())
				{
					for (auto& condString : cond_strings)
					{
						g_Draw.String(FONT_ESP_COND, nTextX, y + nTextOffset, Colors::Cond, ALIGN_DEFAULT, condString.data());
						nTextOffset += g_Draw.m_vecFonts[FONT_ESP_COND].nTall;
					}
				}
			}
			
			if (g_EntityCache.IsFriend(nIndex) && Vars::ESP::Players::FriendCond.Value)
			{
				size_t FONT = FONT_ESP_COND;
				int offset = g_Draw.m_vecFonts[FONT].nTall / 4;
				std::vector<std::wstring> cond_strings = GetPlayerConds(Player);

			g_Draw.String(FONT_ESP_COND, nTextX, y + nTextOffset, Colors::Cond, ALIGN_DEFAULT, 
				L"FRIEND"); // ez pz lemon squeezy
			nTextOffset += g_Draw.m_vecFonts[FONT_ESP_COND].nTall;
		    }

			// Health bar
			if (Vars::ESP::Players::HealthBar.Value)
			{
				x -= 1;

				auto flHealth = static_cast<float>(nHealth);
				auto flMaxHealth = static_cast<float>(nMaxHealth);

				Gradient_t clr = flHealth > flMaxHealth ? Colors::GradientOverhealBar : Colors::GradientHealthBar;

				Color_t HealthColor = flHealth > flMaxHealth ? Colors::Overheal : Utils::GetHealthColor(nHealth, nMaxHealth);

				if (!Player->IsVulnerable())
				{
					clr = { Colors::Invuln, Colors::Invuln };
				}

				if (flHealth > flMaxHealth)
				{
					flHealth = flMaxHealth;
				}

				float ratio = flHealth / flMaxHealth;

				if (Vars::ESP::Players::HealthBarStyle.Value == 0 && Vars::ESP::Players::HealthBar.Value)
				{
					g_Draw.OutlinedGradientBar(x - 2 - 2, y + h, 2, h, ratio, clr.startColour, clr.endColour, Colors::OutlineESP, false);
				}

				else if (Vars::ESP::Players::HealthBarStyle.Value == 1 && Vars::ESP::Players::HealthBar.Value)
				{
					g_Draw.RectOverlay(x - 2 - 2, y + h, 2, h, ratio, HealthColor, Colors::OutlineESP, false);
				}

				if (Vars::ESP::Players::HealthText.Value == 2)
				{
					if (Vars::ESP::Players::Choked.Value) {					
						nTextOffset = 10; 
					}
					if (nHealth > nMaxHealth) {
						g_Draw.String(FONT, x - (15 + nTextOffset), (y + h) - (ratio * h) - 2, Colors::Overheal, ALIGN_CENTERHORIZONTAL, "+%d", nHealth - nMaxHealth);
					} 
					else {
						g_Draw.String(FONT, x - (15 + nTextOffset), (y + h) - (ratio * h) - 2, Colors::White, ALIGN_CENTERHORIZONTAL, "%d", nHealth); 
					}
				}

				x += 1;
			}

			// Choked packets indicator/bar
			if (Vars::ESP::Players::Choked.Value)
			{
				x -= 1;
				static float ratio = 0.0f;
				int chokeCount = G::ChokeMap[nIndex];

				Vec2 position = { static_cast<float>(x) - 2.f - 8.f, static_cast<float>(y + h) };
				ratio = chokeCount / 22.f;
				Math::Clamp(ratio, 0.f, 22.f);
				g_Draw.OutlinedGradientBar(position.x, position.y, 2, h, ratio, Colors::ChokedBar.startColour, Colors::ChokedBar.endColour, Colors::OutlineESP, false);

				x += 1;
			}
		}
	}
	I::VGuiSurface->DrawSetAlphaMultiplier(1.0f);
}

void CESP::DrawBuildings(CBaseEntity* pLocal) const
{
	if (!Vars::ESP::Buildings::Active.Value || !Vars::ESP::Main::Active.Value)
	{
		return;
	}

	for (const auto& pBuilding : g_EntityCache.GetGroup(Vars::ESP::Buildings::IgnoreTeammates.Value ? EGroupType::BUILDINGS_ENEMIES : EGroupType::BUILDINGS_ALL))
	{
		if (!pBuilding->IsAlive())
		{
			continue;
		}

		// distance things
		const Vec3 vDelta = pBuilding->GetAbsOrigin() - pLocal->GetAbsOrigin();
		const float flDistance = vDelta.Length2D();
		if (flDistance >= Vars::ESP::Main::NetworkedDist.Value) { continue; }
		I::VGuiSurface->DrawSetAlphaMultiplier(Vars::ESP::Main::DistanceToAlpha.Value ? Math::RemapValClamped(flDistance, Vars::ESP::Main::NetworkedDist.Value - 256.f, Vars::ESP::Main::NetworkedDist.Value, Vars::ESP::Buildings::Alpha.Value, 0.f) : Vars::ESP::Buildings::Alpha.Value);


		const auto& building = reinterpret_cast<CBaseObject*>(pBuilding);

		Color_t drawColor = Utils::GetEntityDrawColor(building, Vars::ESP::Main::EnableTeamEnemyColors.Value);

		// Building lights
		if (Vars::ESP::Buildings::Dlights.Value)
		{
			CreateDLight(building->GetIndex(), drawColor, building->GetAbsOrigin(),
						 Vars::ESP::Buildings::DlightRadius.Value);
		}

		int x = 0, y = 0, w = 0, h = 0;
		Vec3 vTrans[8];
		if (GetDrawBounds(building, vTrans, x, y, w, h))
		{
			const auto nHealth = building->GetHealth();
			const auto nMaxHealth = building->GetMaxHealth();
			auto nTextOffset = 0, nTextTopOffset = 0;
			const auto nTextX = x + w + 3;

			Color_t healthColor = Utils::GetHealthColor(nHealth, nMaxHealth);

			const auto nType = static_cast<EBuildingType>(building->GetType());

			size_t FONT = FONT_ESP, FONT_NAME = FONT_ESP_NAME, FONT_COND = FONT_ESP_COND;

			const bool bIsMini = building->GetMiniBuilding();

			// Box ESP (Rect, Corners, 3D)
			switch (Vars::ESP::Buildings::Box.Value)
			{
				case 1:
				{
					h += 1;

					g_Draw.OutlinedRect(x, y, w, h, drawColor);

					if (Vars::ESP::Main::Outlinedbar.Value)
					{
						g_Draw.OutlinedRect(x - 1, y - 1, w + 2, h + 2, Colors::OutlineESP);
					}

					h -= 1;
					break;
				}
				case 2:
				{
					g_Draw.CornerRect(x, y, w, h, 3, 5, drawColor);

					if (Vars::ESP::Main::Outlinedbar.Value)
					{
						g_Draw.CornerRect(x - 1, y - 1, w + 2, h + 2, 3, 5, Colors::OutlineESP);
					}
					break;
				}
				case 3:
				{
					Draw3DBox(vTrans, drawColor);
					break;
				}
				default:
					break;
			}

			// Building lines
			if (Vars::ESP::Buildings::Lines.Value)
			{
				Vec3 vScreen, vOrigin = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);

				if (I::Input->CAM_IsThirdPerson())
				{
					Utils::W2S(pLocal->GetAbsOrigin(), vOrigin);
				}

				if (Utils::W2S(building->GetAbsOrigin(), vScreen))
				{
					g_Draw.Line(vOrigin.x, vOrigin.y, vScreen.x, vScreen.y, drawColor);
				}
			}

			// Name ESP
			if (Vars::ESP::Buildings::Name.Value)
			{
				const wchar_t* szName;

				switch (nType)
				{
					case EBuildingType::SENTRY:
					{
						if (bIsMini)
						{
							szName = L"Mini Sentry";
						}
						else
						{
							szName = L"Sentry";
						}
						break;
					}
					case EBuildingType::DISPENSER:
					{
						szName = L"Dispenser";
						break;
					}
					case EBuildingType::TELEPORTER:
					{
						if (building->GetObjectMode())
						{
							szName = L"Teleporter Out";
						}
						else
						{
							szName = L"Teleporter In";
						}
						break;
					}
					default:
					{
						szName = L"Unknown";
						break;
					}
				}

				nTextTopOffset += g_Draw.m_vecFonts[FONT_NAME].nTall + g_Draw.m_vecFonts[FONT_NAME].nTall / 4;
				if (Vars::ESP::Buildings::NameBox.Value)
				{
					int wideth, heighth;
					const int middle = x + w / 2;
					I::VGuiSurface->GetTextSize(g_Draw.m_vecFonts[FONT_NAME].dwFont, szName, wideth, heighth);
					Color_t LineColor = drawColor;
					LineColor.a = 180;
					//g_Draw.Rect((x + (w / 2) - (wideth / 2)) - 5, y - offset - 5, wideth + 10, heighth + 10, { 0,0,0,180 });
					g_Draw.Rect(middle - wideth / 2 - 5, y - nTextTopOffset, wideth + 10, heighth + 2,
								{ 0, 0, 0, 180 });
					//g_Draw.Rect((x + (w / 2) - (wideth / 2)) - 5, y - offset - 7, wideth + 10, 2, LineColor);
					g_Draw.Rect(middle - wideth / 2 - 5, y - nTextTopOffset - 2, wideth + 10, 2, LineColor);
				}
				if (Vars::ESP::Buildings::NameCustom.Value)
				{
					g_Draw.String(FONT_NAME, x + w / 2, y - nTextTopOffset, Vars::ESP::Buildings::NameColor, ALIGN_CENTERHORIZONTAL,
								  szName);
				}
				else
				{
					g_Draw.String(FONT_NAME, x + w / 2, y - nTextTopOffset, drawColor, ALIGN_CENTERHORIZONTAL,
								  szName);
				}
			}

			//Distance ESP
			if (Vars::ESP::Buildings::Distance.Value)
			{
				const Vec3 vDelta = building->GetAbsOrigin() - pLocal->GetAbsOrigin();
				const float flDistance = vDelta.Length2D() * 0.01905; // 1 m = 52.49 hu, so this is accurate *enough*
				const int Distance = std::round(flDistance); //return as an int, so it doesnt show a shitty decimal number

				g_Draw.String(FONT_ESP, x + (w / 2), y + h, Colors::White, ALIGN_CENTERHORIZONTAL, L"[%d M]", Distance);
			}

			// Building owner ESP
			if (Vars::ESP::Buildings::Owner.Value && !building->GetMapPlaced())
			{
				if (const auto& pOwner = building->GetOwner())
				{
					PlayerInfo_t pi;
					if (I::EngineClient->GetPlayerInfo(pOwner->GetIndex(), &pi))
					{
						nTextTopOffset += g_Draw.m_vecFonts[FONT_NAME].nTall + g_Draw.m_vecFonts[FONT_NAME].nTall /
							4;
						g_Draw.String(FONT_NAME, x + w / 2, y - nTextTopOffset, drawColor, ALIGN_CENTERHORIZONTAL,
									  L"Built by: %ls", Utils::ConvertUtf8ToWide(pi.name).data());
					}
				}
			}

			const float flConstructed = building->GetConstructed() * 100.0f;

			// Health text
			if (Vars::ESP::Buildings::Health.Value)
			{
				g_Draw.String(FONT, nTextX, y + nTextOffset, healthColor, ALIGN_DEFAULT, L"%d / %d", nHealth, nMaxHealth);
				nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
			}

			if (flConstructed < 100.0f && static_cast<int>(flConstructed) != 0)
			{
				g_Draw.String(FONT, nTextX, y + nTextOffset, drawColor, ALIGN_DEFAULT, L"Building: %0.f%%",
							  flConstructed);
				nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
			}

			// Building level
			if (Vars::ESP::Buildings::Level.Value && !bIsMini)
			{
				g_Draw.String(FONT, nTextX, y + nTextOffset, drawColor, ALIGN_DEFAULT, L"%d/3",
							  building->GetLevel());
				nTextOffset += g_Draw.m_vecFonts[FONT].nTall;
			}

			// Building conditions
			if (Vars::ESP::Buildings::Cond.Value)
			{
				std::vector<std::wstring> condStrings{};

				if (nType == EBuildingType::SENTRY && building->GetControlled())
				{
					condStrings.emplace_back(L"Wrangled");
				}

				if (building->GetSapped())
				{
					condStrings.emplace_back(L"Sapped");
				}
				else if (building->GetDisabled()) //Building->IsSpook()
				{
					condStrings.emplace_back(L"Disabled");
				}

				if (building->IsSentrygun() && !building->GetConstructing())
				{
					int iShells;
					int iMaxShells;
					int iRockets;
					int iMaxRockets;

					building->GetAmmoCount(iShells, iMaxShells, iRockets, iMaxRockets);

					if (iShells == 0)
						condStrings.emplace_back(L"No Ammo");

					if (!bIsMini && iRockets == 0)
						condStrings.emplace_back(L"No Rockets");
				}

				if (!condStrings.empty())
				{
					for (auto& condString : condStrings)
					{
						g_Draw.String(FONT_ESP_COND, nTextX, y + nTextOffset, Colors::Cond, ALIGN_DEFAULT, condString.data());
						nTextOffset += g_Draw.m_vecFonts[FONT_ESP_COND].nTall;
					}
				}
			}

			// Healthbar
			if (Vars::ESP::Buildings::HealthBar.Value)
			{
				x -= 1;

				auto flHealth = static_cast<float>(nHealth);
				const auto flMaxHealth = static_cast<float>(nMaxHealth);

				if (flHealth > flMaxHealth)
				{
					flHealth = flMaxHealth;
				}

				static constexpr int RECT_WIDTH = 2;
				const int nHeight = h + (flHealth < flMaxHealth ? 2 : 1);
				int nHeight2 = h + 1;

				const float ratio = flHealth / flMaxHealth;

				g_Draw.Rect(x - RECT_WIDTH - 2, y + nHeight - nHeight * ratio, RECT_WIDTH, nHeight * ratio,
							healthColor);

				if (Vars::ESP::Main::Outlinedbar.Value)
				{
					g_Draw.OutlinedRect(x - RECT_WIDTH - 2 - 1, y + nHeight - nHeight * ratio - 1, RECT_WIDTH + 2,
										nHeight * ratio + 2, Colors::OutlineESP);
				}

				x += 1;
			}

			// Teleport exit direction
			if (Vars::ESP::Buildings::TeleExitDir.Value)
			{
				if (building->IsTeleporter() && building->GetObjectMode() == 1)
				{
					const Vec3 Origin = building->GetAbsOrigin();

					Vec3 Forward{};
					Vec3 Right{};
					Vec3 Up{};

					Math::AngleVectors(building->GetAbsAngles(), &Forward, &Right, &Up);

					const Vec3 ArrowTop = Forward * 55.0f + Origin;
					const Vec3 ArrowLeft = Right * -5.0f + Forward * 32.5f + Origin;
					const Vec3 ArrowRight = Right * 5.0f + Forward * 32.5f + Origin;

					Vec3 ArrowLeftScrn, ArrowRightScrn, ArrowTopScrn;

					if (Utils::W2S(ArrowLeft, ArrowLeftScrn) &&
						Utils::W2S(ArrowRight, ArrowRightScrn) &&
						Utils::W2S(ArrowTop, ArrowTopScrn))
					{
						const std::array<Vec2, 3> Points{ Vec2(ArrowLeftScrn.x, ArrowLeftScrn.y),
													Vec2(ArrowTopScrn.x, ArrowTopScrn.y),
													Vec2(ArrowRightScrn.x, ArrowRightScrn.y) };
						g_Draw.DrawOutlinedTriangle(Points, Vars::ESP::Buildings::TeleExitDirColor);
					}
				}
			}

		}
	}
	I::VGuiSurface->DrawSetAlphaMultiplier(1.0f);
}

void CESP::DrawWorld() const
{
	CBaseEntity* pLocal = g_EntityCache.GetLocal();

	if (!Vars::ESP::World::Active.Value || !Vars::ESP::Main::Active.Value)
	{
		return;
	}

	Vec3 vScreen = {};
	constexpr size_t FONT = FONT_ESP_PICKUPS;

	I::VGuiSurface->DrawSetAlphaMultiplier(Vars::ESP::World::Alpha.Value);

	for (const auto& health : g_EntityCache.GetGroup(EGroupType::WORLD_HEALTH))
	{
		// distance things
		const Vec3 vDelta = health->GetAbsOrigin() - pLocal->GetAbsOrigin();
		const float flDistance = vDelta.Length2D();
		if (flDistance >= Vars::ESP::Main::NetworkedDist.Value) { continue; }
		I::VGuiSurface->DrawSetAlphaMultiplier(Vars::ESP::Main::DistanceToAlpha.Value ? Math::RemapValClamped(flDistance, Vars::ESP::Main::NetworkedDist.Value - 256.f, Vars::ESP::Main::NetworkedDist.Value, Vars::ESP::World::Alpha.Value, 0.f) : Vars::ESP::World::Alpha.Value);

		int x = 0, y = 0, w = 0, h = 0;
		Vec3 vTrans[8];
		if (GetDrawBounds(health, vTrans, x, y, w, h))
		{
			if (Vars::ESP::World::HealthName.Value)
			{
				if (Utils::W2S(health->GetVecOrigin(), vScreen))
					g_Draw.String(FONT, vScreen.x, y + h, Colors::Health, ALIGN_CENTER, L"Health");
			}

			if (Vars::ESP::World::HealthLine.Value)
			{
				Vec3 vScreen, vOrigin = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);

				if (I::Input->CAM_IsThirdPerson())
					Utils::W2S(pLocal->GetAbsOrigin(), vOrigin);

				if (Utils::W2S(health->GetAbsOrigin(), vScreen))
					g_Draw.Line(vOrigin.x, vOrigin.y, vScreen.x, vScreen.y, Colors::Health);
			}

			switch (Vars::ESP::World::HealthBox.Value)
			{
				case 1:
				{
					h += 1;

					g_Draw.OutlinedRect(x, y, w, h, Colors::Health);

					if (Vars::ESP::Main::Outlinedbar.Value)
						g_Draw.OutlinedRect(x - 1, y - 1, w + 2, h + 2, Colors::OutlineESP);

					h -= 1;
					break;
				}
				case 2:
				{
					g_Draw.CornerRect(x, y, w, h, 3, 5, Colors::Health);

					if (Vars::ESP::Main::Outlinedbar.Value)
						g_Draw.CornerRect(x - 1, y - 1, w + 2, h + 2, 3, 5, Colors::OutlineESP);

					break;
				}
				case 3:
				{
					Draw3DBox(vTrans, Colors::Health);
					break;
				}
				default: break;
			}

			//Distance ESP
			if (Vars::ESP::World::HealthDistance.Value)
			{
				const Vec3 vDelta = health->GetAbsOrigin() - pLocal->GetAbsOrigin();
				const float flDistance = vDelta.Length2D() * 0.01905; // 1 m = 52.49 hu, so this is accurate *enough*
				const int Distance = std::round(flDistance); //return as an int, so it doesnt show a shitty decimal number

				g_Draw.String(FONT_ESP, x + (w / 2), y + h, Colors::White, ALIGN_CENTERHORIZONTAL, L"[%d M]", Distance);
			}
		}
	}

	for (const auto& ammo : g_EntityCache.GetGroup(EGroupType::WORLD_AMMO))
	{
		// distance things
		const Vec3 vDelta = ammo->GetAbsOrigin() - pLocal->GetAbsOrigin();
		const float flDistance = vDelta.Length2D();
		if (flDistance >= Vars::ESP::Main::NetworkedDist.Value) { continue; }
		I::VGuiSurface->DrawSetAlphaMultiplier(Vars::ESP::Main::DistanceToAlpha.Value ? Math::RemapValClamped(flDistance, Vars::ESP::Main::NetworkedDist.Value - 256.f, Vars::ESP::Main::NetworkedDist.Value, Vars::ESP::World::Alpha.Value, 0.f) : Vars::ESP::World::Alpha.Value);

		int x = 0, y = 0, w = 0, h = 0;
		Vec3 vTrans[8];
		if (GetDrawBounds(ammo, vTrans, x, y, w, h))
		{
			if (Vars::ESP::World::AmmoName.Value)
			{
				if (Utils::W2S(ammo->GetVecOrigin(), vScreen))
					g_Draw.String(FONT, vScreen.x, y + h, Colors::Ammo, ALIGN_CENTER, L"Ammo");
			}

			if (Vars::ESP::World::AmmoLine.Value)
			{
				Vec3 vScreen, vOrigin = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);

				if (I::Input->CAM_IsThirdPerson())
					Utils::W2S(pLocal->GetAbsOrigin(), vOrigin);

				if (Utils::W2S(ammo->GetAbsOrigin(), vScreen))
					g_Draw.Line(vOrigin.x, vOrigin.y, vScreen.x, vScreen.y, Colors::Ammo);
			}

			switch (Vars::ESP::World::AmmoBox.Value)
			{
				case 1:
				{
					h += 1;

					g_Draw.OutlinedRect(x, y, w, h, Colors::Ammo);

					if (Vars::ESP::Main::Outlinedbar.Value)
						g_Draw.OutlinedRect(x - 1, y - 1, w + 2, h + 2, Colors::OutlineESP);

					h -= 1;
					break;
				}
				case 2:
				{
					g_Draw.CornerRect(x, y, w, h, 3, 5, Colors::Ammo);

					if (Vars::ESP::Main::Outlinedbar.Value)
						g_Draw.CornerRect(x - 1, y - 1, w + 2, h + 2, 3, 5, Colors::OutlineESP);

					break;
				}
				case 3:
				{
					Draw3DBox(vTrans, Colors::Ammo);
					break;
				}
				default: break;
			}

			//Distance ESP
			if (Vars::ESP::World::AmmoDistance.Value)
			{
				const Vec3 vDelta = ammo->GetAbsOrigin() - pLocal->GetAbsOrigin();
				const float flDistance = vDelta.Length2D() * 0.01905; // 1 m = 52.49 hu, so this is accurate *enough*
				const int Distance = std::round(flDistance); //return as an int, so it doesnt show a shitty decimal number

				g_Draw.String(FONT_ESP, x + (w / 2), y + h, Colors::White, ALIGN_CENTERHORIZONTAL, L"[%d M]", Distance);
			}
		}
	}

	for (const auto& NPC : g_EntityCache.GetGroup(EGroupType::WORLD_NPC))
	{
		// distance things
		const Vec3 vDelta = NPC->GetAbsOrigin() - pLocal->GetAbsOrigin();
		const float flDistance = vDelta.Length2D();
		if (flDistance >= Vars::ESP::Main::NetworkedDist.Value) { continue; }
		I::VGuiSurface->DrawSetAlphaMultiplier(Vars::ESP::Main::DistanceToAlpha.Value ? Math::RemapValClamped(flDistance, Vars::ESP::Main::NetworkedDist.Value - 256.f, Vars::ESP::Main::NetworkedDist.Value, Vars::ESP::World::Alpha.Value, 0.f) : Vars::ESP::World::Alpha.Value);

		int x = 0, y = 0, w = 0, h = 0;
		Vec3 vTrans[8];
		if (GetDrawBounds(NPC, vTrans, x, y, w, h))
		{
			int nTextTopOffset = 0;

			if (Vars::ESP::World::NPCName.Value)
			{
				const wchar_t* szName;

				switch (NPC->GetClassID())
				{
					case ETFClassID::CHeadlessHatman:
					{
						szName = L"Horseless Headless Horsemann";
						break;
					}
					case ETFClassID::CTFTankBoss:
					{
						szName = L"Tank";
						break;
					}
					case ETFClassID::CMerasmus:
					{
						szName = L"Merasmus";
						break;
					}
					case ETFClassID::CZombie:
					{
						szName = L"Skeleton";
						break;
					}
					case ETFClassID::CEyeballBoss:
					{
						szName = L"Monoculus";
						break;
					}
					default:
					{
						szName = L"Unknown";
						break;
					}
				}

				nTextTopOffset += g_Draw.m_vecFonts[FONT].nTall + g_Draw.m_vecFonts[FONT].nTall / 4;
				g_Draw.String(FONT, x + w / 2, y - nTextTopOffset, Utils::GetEntityDrawColor(NPC, true), ALIGN_CENTERHORIZONTAL, szName);
			}

			if (Vars::ESP::World::NPCLine.Value)
			{
				Vec3 vScreen, vOrigin = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);

				if (I::Input->CAM_IsThirdPerson())
					Utils::W2S(pLocal->GetAbsOrigin(), vOrigin);

				if (Utils::W2S(NPC->GetAbsOrigin(), vScreen))
					g_Draw.Line(vOrigin.x, vOrigin.y, vScreen.x, vScreen.y, Utils::GetEntityDrawColor(NPC, true));
			}

			switch (Vars::ESP::World::NPCBox.Value)
			{
				case 1:
				{
					h += 1;

					g_Draw.OutlinedRect(x, y, w, h, Utils::GetEntityDrawColor(NPC, true));

					if (Vars::ESP::Main::Outlinedbar.Value)
						g_Draw.OutlinedRect(x - 1, y - 1, w + 2, h + 2, Colors::OutlineESP);

					h -= 1;
					break;
				}
				case 2:
				{
					g_Draw.CornerRect(x, y, w, h, 3, 5, Utils::GetEntityDrawColor(NPC, true));

					if (Vars::ESP::Main::Outlinedbar.Value)
						g_Draw.CornerRect(x - 1, y - 1, w + 2, h + 2, 3, 5, Colors::OutlineESP);

					break;
				}
				case 3:
				{
					Draw3DBox(vTrans, Utils::GetEntityDrawColor(NPC, true));
					break;
				}
				default: break;
			}

			//Distance ESP
			if (Vars::ESP::World::NPCDistance.Value)
			{
				const Vec3 vDelta = NPC->GetAbsOrigin() - pLocal->GetAbsOrigin();
				const float flDistance = vDelta.Length2D() * 0.01905; // 1 m = 52.49 hu, so this is accurate *enough*
				const int Distance = std::round(flDistance); //return as an int, so it doesnt show a shitty decimal number

				g_Draw.String(FONT_ESP, x + (w / 2), y + h, Colors::White, ALIGN_CENTERHORIZONTAL, L"[%d M]", Distance);
			}
		}
	}

	for (const auto& Bombs : g_EntityCache.GetGroup(EGroupType::WORLD_BOMBS))
	{
		// distance things
		const Vec3 vDelta = Bombs->GetAbsOrigin() - pLocal->GetAbsOrigin();
		const float flDistance = vDelta.Length2D();
		if (flDistance >= Vars::ESP::Main::NetworkedDist.Value) { continue; }
		I::VGuiSurface->DrawSetAlphaMultiplier(Vars::ESP::Main::DistanceToAlpha.Value ? Math::RemapValClamped(flDistance, Vars::ESP::Main::NetworkedDist.Value - 256.f, Vars::ESP::Main::NetworkedDist.Value, Vars::ESP::World::Alpha.Value, 0.f) : Vars::ESP::World::Alpha.Value);

		int x = 0, y = 0, w = 0, h = 0;
		Vec3 vTrans[8];
		if (GetDrawBounds(Bombs, vTrans, x, y, w, h))
		{
			int nTextTopOffset = 0;

			if (Vars::ESP::World::BombName.Value)
			{
				const wchar_t* szName;

				switch (Bombs->GetClassID())
				{
					case ETFClassID::CTFPumpkinBomb:
					{
						szName = L"Pumpkin Bomb";
						break;
					}
					case ETFClassID::CTFGenericBomb:
					{
						szName = L"Bomb";
						break;
					}
					default:
					{
						szName = L"Unknown";
						break;
					}
				}

				nTextTopOffset += g_Draw.m_vecFonts[FONT].nTall + g_Draw.m_vecFonts[FONT].nTall / 4;
				g_Draw.String(FONT, x + w / 2, y - nTextTopOffset, Utils::GetEntityDrawColor(Bombs, true), ALIGN_CENTERHORIZONTAL, szName);
			}

			if (Vars::ESP::World::BombLine.Value)
			{
				Vec3 vScreen, vOrigin = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);

				if (I::Input->CAM_IsThirdPerson())
					Utils::W2S(pLocal->GetAbsOrigin(), vOrigin);

				if (Utils::W2S(Bombs->GetAbsOrigin(), vScreen))
					g_Draw.Line(vOrigin.x, vOrigin.y, vScreen.x, vScreen.y, Utils::GetEntityDrawColor(Bombs, true));
			}

			switch (Vars::ESP::World::BombBox.Value)
			{
				case 1:
				{
					h += 1;

					g_Draw.OutlinedRect(x, y, w, h, Utils::GetEntityDrawColor(Bombs, true));

					if (Vars::ESP::Main::Outlinedbar.Value)
						g_Draw.OutlinedRect(x - 1, y - 1, w + 2, h + 2, Colors::OutlineESP);

					h -= 1;
					break;
				}
				case 2:
				{
					g_Draw.CornerRect(x, y, w, h, 3, 5, Utils::GetEntityDrawColor(Bombs, true));

					if (Vars::ESP::Main::Outlinedbar.Value)
						g_Draw.CornerRect(x - 1, y - 1, w + 2, h + 2, 3, 5, Colors::OutlineESP);

					break;
				}
				case 3:
				{
					Draw3DBox(vTrans, Utils::GetEntityDrawColor(Bombs, true));
					break;
				}
				default: break;
			}

			//Distance ESP
			if (Vars::ESP::World::BombDistance.Value)
			{
				const Vec3 vDelta = Bombs->GetAbsOrigin() - pLocal->GetAbsOrigin();
				const float flDistance = vDelta.Length2D() * 0.01905; // 1 m = 52.49 hu, so this is accurate *enough*
				const int Distance = std::round(flDistance); //I think this method is better than doing it the normal way

				g_Draw.String(FONT_ESP, x + (w / 2), y + h, Colors::White, ALIGN_CENTERHORIZONTAL, L"[%d M]", Distance);
			}
		}
	}

	I::VGuiSurface->DrawSetAlphaMultiplier(1.0f);
}

using ETFCond = int;

template <typename tIntType>
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
			if (eCond < (64 + 32))
			{
				m_pnCondVar = &nPlayerCondEx2;
				m_nCondBit = eCond - 64;
			}
		}
		else if (eCond >= 32)
		{
			if (eCond < (32 + 32))
			{
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
	CTFCondition(ETFCond type, float duration, CBaseEntity* outer, CBaseEntity* provider = nullptr);
	virtual ~CTFCondition();

	virtual void Add(float duration);

	virtual void OnAdded() = 0;
	virtual void OnRemoved() = 0;
	virtual void OnThink() = 0;
	virtual void OnServerThink() = 0;

	// Condition Traits
	virtual bool IsHealable() { return false; }
	virtual bool UsesMinDuration() { return false; }

	ETFCond GetType() { return _type; }
	float GetMaxDuration() { return _max_duration; }
	void SetMaxDuration(float val) { _max_duration = val; }
	float GetMinDuration() { return _min_duration; }
	void SetMinDuration(float val) { if (UsesMinDuration()) { _min_duration = val; } }
	CBaseEntity* GetOuter() { return _outer; }
	void SetProvider(CBaseEntity* provider) { _provider = provider; }
	CBaseEntity* GetProvider() { return _provider; }

private:
	float _min_duration;
	float _max_duration;
	const ETFCond _type;
	CBaseEntity* _outer;
	CBaseEntity* _provider;
};

bool InCond(CBaseEntity* pEntity, int eCond)
{
	if (eCond >= 0 && eCond < 122)
	{
		void* condList = pEntity->m_ConditionList();
		const auto& conditionList = *reinterpret_cast<CTFConditionList*>(condList);

		if (conditionList._conditions.Count() > 0)
		{
			// Old condition system, only used for the first 32 conditions
			if (eCond < 32 && conditionList.InCond(eCond))
			{
				return true;
			}

			CConditionVars<const int> cPlayerCond(pEntity->GetCond(), pEntity->GetCondEx(), pEntity->GetCondEx2(), pEntity->GetCondEx3(), eCond);
			return (cPlayerCond.CondVar() & cPlayerCond.CondBit()) != 0;
		}
	}
	return false;
}

std::vector<std::wstring> CESP::GetPlayerConds(CBaseEntity* pEntity) const
{
	std::vector<std::wstring> szCond{};
	const int nCond = pEntity->GetCond();
	const int nCondEx = pEntity->GetCondEx();
	const int nFlag = pEntity->GetFlags();

	{
		const float flTickVelSqr = pow(pEntity->TickVelocity2D(), 2);
		if (flTickVelSqr > 4096.f) { szCond.emplace_back(L"Can't Hit"); }
	}

	if (const wchar_t* rune = pEntity->GetRune())
	{	// I want to see if they are the king before anything else.
		szCond.emplace_back(rune);
	}

	if (pEntity->GetDormant())
	{
		szCond.emplace_back(L"Dormant");
	}

	if (InCond(pEntity, 61))
	{
		szCond.emplace_back(L"Bullet resistance");
	}

	if (InCond(pEntity, 62))
	{
		szCond.emplace_back(L"Blast resistance");
	}

	if (InCond(pEntity, 63))
	{
		szCond.emplace_back(L"Fire resistance");
	}


	if (nCond & TFCond_Slowed)
	{
		if (const auto& pWeapon = pEntity->GetActiveWeapon())
		{
			if (pWeapon->GetWeaponID() == TF_WEAPON_MINIGUN)
			{
				szCond.emplace_back(L"Revved");
			}
		}
	}

	if (nFlag & FL_DUCKING)
	{
		szCond.emplace_back(L"Ducking");
	}

	if (pEntity->GetHealth() > pEntity->GetMaxHealth())
	{
		szCond.emplace_back(L"Overhealed");
	}

	if (nCond & TFCond_OnFire)
	{
		szCond.emplace_back(L"Burning");
	}

	if (pEntity->IsUbered())
	{
		szCond.emplace_back(L"Ubered");
	}

	if (nCond & TFCond_MegaHeal)
	{
		szCond.emplace_back(L"Megahealed");
	}

	if (nCond & TFCond_Bonked)
	{
		szCond.emplace_back(L"Bonked");
	}

	if (nCond & TFCond_Kritzkrieged || 
		nCondEx & TFCondEx_CritCanteen || nCondEx & TFCondEx_CritOnFirstBlood || nCondEx & TFCondEx_CritOnWin ||
		nCondEx & TFCondEx_CritOnKill || nCondEx & TFCondEx_CritDemoCharge || nCondEx & TFCondEx_CritOnFlagCapture ||
		nCondEx & TFCondEx_HalloweenCritCandy || nCondEx & TFCondEx_PyroCrits)
	{
		szCond.emplace_back(L"Crit boosted");
	}
	if (nCond & TFCond_MiniCrits)
	{
	 szCond.emplace_back(L"Mini-Crits");
	}

	if (nCond & TFCond_Cloaked)
	{
		szCond.emplace_back(L"Cloaked");
	}

	if (nCond & TFCond_Zoomed)
	{
		szCond.emplace_back(L"Scoped");
	}

	if (nCond & TFCond_Taunting)
	{
		szCond.emplace_back(L"Taunting");
	}

	if (nCond & TFCond_Disguised)
	{
		szCond.emplace_back(L"Disguised");
	}

	if (nCond & TFCond_Milked)
	{
		szCond.emplace_back(L"Milked");
	}

	if (nCond & TFCond_Jarated)
	{
		szCond.emplace_back(L"Jarated");
	}

	if (nCond & TFCond_Bleeding)
	{
		szCond.emplace_back(L"Bleeding");
	}

	if (pEntity->GetFeignDeathReady())
	{
		szCond.emplace_back(L"Dead Ringer");
	}

	if (pEntity->IsBuffedByKing())
	{
		szCond.emplace_back(L"Buffed by King");
	}

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
	const auto pLight = I::EngineEffects->CL_AllocDlight(nIndex);
	pLight->m_flDie = I::EngineClient->Time() + 0.5f;
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
void CESP::Draw3DBox(const Vec3* vPoints, Color_t clr)
{
	Vector vStart, vEnd;

	for (int i = 0; i < 3; i++)
	{
		if (Utils::W2S(vPoints[i], vStart))
		{
			if (Utils::W2S(vPoints[i + 1], vEnd))
			{
				g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
			}
		}
	}

	if (Utils::W2S(vPoints[0], vStart))
	{
		if (Utils::W2S(vPoints[3], vEnd))
		{
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
		}
	}

	for (int i = 4; i < 7; i++)
	{
		if (Utils::W2S(vPoints[i], vStart))
		{
			if (Utils::W2S(vPoints[i + 1], vEnd))
			{
				g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
			}
		}
	}

	if (Utils::W2S(vPoints[4], vStart))
	{
		if (Utils::W2S(vPoints[7], vEnd))
		{
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
		}
	}

	if (Utils::W2S(vPoints[0], vStart))
	{
		if (Utils::W2S(vPoints[6], vEnd))
		{
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
		}
	}

	if (Utils::W2S(vPoints[1], vStart))
	{
		if (Utils::W2S(vPoints[5], vEnd))
		{
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
		}
	}

	if (Utils::W2S(vPoints[2], vStart))
	{
		if (Utils::W2S(vPoints[4], vEnd))
		{
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
		}
	}

	if (Utils::W2S(vPoints[3], vStart))
	{
		if (Utils::W2S(vPoints[7], vEnd))
		{
			g_Draw.Line(vStart.x, vStart.y, vEnd.x, vEnd.y, clr);
		}
	}
}

void CESP::DrawBones(CBaseEntity* pPlayer, const std::vector<int>& vecBones, Color_t clr)
{
	const size_t nMax = vecBones.size(), nLast = nMax - 1;
	for (size_t n = 0; n < nMax; n++)
	{
		if (n == nLast)
		{
			continue;
		}

		const auto vBone = pPlayer->GetHitboxPos(vecBones[n]);
		const auto vParent = pPlayer->GetHitboxPos(vecBones[n + 1]);

		Vec3 vScreenBone, vScreenParent;

		if (Utils::W2S(vBone, vScreenBone) && Utils::W2S(vParent, vScreenParent))
		{
			g_Draw.Line(vScreenBone.x, vScreenBone.y, vScreenParent.x, vScreenParent.y, clr);
		}
	}
}
