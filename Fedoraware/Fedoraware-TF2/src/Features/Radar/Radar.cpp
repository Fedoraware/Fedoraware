#include "Radar.h"
#include "../Vars.h"
#include "../Menu/Menu.h"

constexpr Color_t clrBlack = { 0, 0, 0, 255 };
constexpr Color_t clrWhite = { 255, 255, 255, 255 };

static void SquareConstraints(ImGuiSizeCallbackData* data) { data->DesiredSize.x = data->DesiredSize.y = std::max(data->DesiredSize.x, data->DesiredSize.y); }

void CRadar::Run()
{
	if (!ShouldRun()) { return; }

	//Draw background, handle input.
	DrawRadar();

	if (const auto& pLocal = g_EntityCache.GetLocal()) { DrawPoints(pLocal); }
}

bool CRadar::ShouldRun()
{
	if (!Vars::Radar::Main::Active.Value) { return false; }

	return true;
}

void CRadar::DrawWindow()
{
	if (!Vars::Radar::Main::Active.Value) { return; }

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::SetNextWindowSizeConstraints({ 20.f, 20.f }, { 400.f, 400.f }, SquareConstraints);

	const int activeFlags = F::Menu.IsOpen ? 0 : ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize;
	if (ImGui::Begin("Radar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | activeFlags))
	{
		RadarX = static_cast<int>(ImGui::GetWindowPos().x) + RadarSize;
		RadarY = static_cast<int>(ImGui::GetWindowPos().y) + RadarSize;

		RadarSize = static_cast<int>(ImGui::GetWindowSize().x * 0.5f);
	}
	ImGui::End();

	ImGui::PopStyleColor();
}

void CRadar::DrawRadar()
{
	// Title gradient
	if (!F::Menu.IsOpen && !Vars::Radar::Main::NoTitleGradient.Value)
	{
		g_Draw.GradientRect(RadarX - RadarSize, RadarY - RadarSize - 3,
							RadarX - RadarSize + RadarSize, RadarY - RadarSize, { 43, 43, 45, 250 },
							Vars::Menu::Colors::MenuAccent, true);

		g_Draw.GradientRect(RadarX - RadarSize + RadarSize, RadarY - RadarSize - 3,
							RadarX - RadarSize + (RadarSize * 2), RadarY - RadarSize,
							Vars::Menu::Colors::MenuAccent, { 43, 43, 45, 250 }, true);
	}

	//Build the bg color with the wanted alpha.
	const Color_t clrBack = { 36, 36, 36, static_cast<byte>(Vars::Radar::Main::BackAlpha.Value) };


	//Background
	g_Draw.Rect(RadarX - RadarSize, RadarY - RadarSize, RadarSize * 2, RadarSize * 2, clrBack);

	//Outline
	g_Draw.OutlinedRect(RadarX - RadarSize, RadarY - RadarSize, RadarSize * 2, RadarSize * 2, {
							43, 43, 45, static_cast<byte>(Vars::Radar::Main::LineAlpha.Value)
						});

	//Center lines
	g_Draw.Line(RadarX, RadarY - RadarSize, RadarX, RadarY + RadarSize - 1, {
					43, 43, 45, static_cast<byte>(Vars::Radar::Main::LineAlpha.Value)
				});
	g_Draw.Line(RadarX - RadarSize, RadarY, RadarX + RadarSize - 1, RadarY, {
					43, 43, 45, static_cast<byte>(Vars::Radar::Main::LineAlpha.Value)
				});
}

bool CRadar::GetDrawPosition(int& x, int& y, int& z, CBaseEntity* pEntity)
{
	//Calculate the delta and initial position of the entity
	const Vec3 vDelta = pEntity->GetAbsOrigin() - LocalOrigin;
	auto vPos = Vec2((vDelta.y * (-LocalCos) + vDelta.x * LocalSin),
					 (vDelta.x * (-LocalCos) - vDelta.y * LocalSin));

	//Range, keep in bounds
	//Credits are due to whoever wrote this, does what I want and is fast so idc. Code probably older than Jesus.
	//Found it in my 2015 CSGO pasta so I am sure it is made by someone else than me.
	if (fabs(vPos.x) > Range || fabs(vPos.y) > Range)
	{
		if (vPos.y > vPos.x)
		{
			if (vPos.y > -vPos.x)
			{
				vPos.x = Range * vPos.x / vPos.y;
				vPos.y = Range;
			}
			else
			{
				vPos.y = -Range * vPos.y / vPos.x;
				vPos.x = -Range;
			}
		}
		else
		{
			if (vPos.y > -vPos.x)
			{
				vPos.y = Range * vPos.y / vPos.x;
				vPos.x = Range;
			}
			else
			{
				vPos.x = -Range * vPos.x / vPos.y;
				vPos.y = -Range;
			}
		}
	}

	x = RadarX + static_cast<int>(vPos.x / Range * static_cast<float>(RadarSize));
	y = RadarY + static_cast<int>(vPos.y / Range * static_cast<float>(RadarSize));
	z = vDelta.z;

	//Just confirm that they were both set.
	return (x != -1 && y != -1);
}

void CRadar::DrawPoints(CBaseEntity* pLocal)
{
	//Update members that we use calculating the draw position in "GetDrawPosition()"
	LocalOrigin = pLocal->GetAbsOrigin();
	LocalYaw = I::EngineClient->GetViewAngles().y * (PI / 180.f);
	Range = static_cast<float>(Vars::Radar::Main::Range.Value);
	LocalCos = cos(LocalYaw), LocalSin = sin(LocalYaw);

	// Draw Ammo & Health
	if (Vars::Radar::World::Active.Value)
	{
		const int nSize = Vars::Radar::World::IconSize.Value;

		if (Vars::Radar::World::Ammo.Value)
		{
			for (const auto& ammo : g_EntityCache.GetGroup(EGroupType::WORLD_AMMO))
			{
				int nX = -1, nY = -1, nZ = 0;
				if (GetDrawPosition(nX, nY, nZ, ammo))
				{
					nX -= (nSize / 2), nY -= (nSize / 2);
					g_Draw.Texture(nX, nY, nSize, nSize, clrWhite, 55);
				}
			}
		}

		if (Vars::Radar::World::Health.Value)
		{
			for (const auto& health : g_EntityCache.GetGroup(EGroupType::WORLD_HEALTH))
			{
				int nX = -1, nY = -1, nZ = 0;
				if (GetDrawPosition(nX, nY, nZ, health))
				{
					nX -= (nSize / 2), nY -= (nSize / 2);
					g_Draw.Texture(nX, nY, nSize, nSize, clrWhite, 50);
				}
			}
		}
	}

	// Draw buildings
	if (Vars::Radar::Buildings::Active.Value)
	{
		const auto& buildings = g_EntityCache.GetGroup(Vars::Radar::Buildings::IgnoreTeam.Value
													   ? EGroupType::BUILDINGS_ENEMIES
													   : EGroupType::BUILDINGS_ALL);

		for (const auto& building : buildings)
		{
			if (const auto& pObject = reinterpret_cast<CBaseObject*>(building))
			{
				if (!pObject->IsAlive()) { continue; }

				int nX = -1, nY = -1, nZ = 0;
				if (GetDrawPosition(nX, nY, nZ, pObject))
				{
					Color_t clrDraw = Utils::GetEntityDrawColor(building, Vars::ESP::Main::EnableTeamEnemyColors.Value);

					const int nSize = Vars::Radar::Buildings::IconSize.Value;
					nX -= (nSize / 2), nY -= (nSize / 2);

					switch (pObject->GetClassID())
					{
						case ETFClassID::CObjectSentrygun:
						{
							int nTexture = (pObject->GetLevel() + 40);

							if (Vars::Radar::Buildings::Outline.Value)
							{
								g_Draw.Texture(nX - 2, nY - 2, nSize + 4, nSize + 4, clrBlack, nTexture);
							}

							g_Draw.Texture(nX, nY, nSize, nSize, clrDraw, nTexture);
							break;
						}
						case ETFClassID::CObjectDispenser:
						{
							if (Vars::Radar::Buildings::Outline.Value)
							{
								g_Draw.Texture(nX - 2, nY - 2, nSize + 4, nSize + 4, clrBlack, 44);
							}

							g_Draw.Texture(nX, nY, nSize, nSize, clrDraw, 44);
							break;
						}
						case ETFClassID::CObjectTeleporter:
						{
							int nTexture = 46; //Exit texture ID

							//If "YawToExit" is not zero, it most like is an entrance
							if (pObject->GetYawToExit())
							{
								nTexture -= 1; //In that case, -1 from "nTexture" so we get entrace texture ID
							}

							if (Vars::Radar::Buildings::Outline.Value)
							{
								g_Draw.Texture(nX - 2, nY - 2, nSize + 4, nSize + 4, clrBlack, nTexture);
							}

							g_Draw.Texture(nX, nY, nSize, nSize, clrDraw, nTexture);
							break;
						}
						default: break;
					}

					if (Vars::Radar::Buildings::Health.Value)
					{
						const int nHealth = pObject->GetHealth();
						const int nMaxHealth = pObject->GetMaxHealth();
						Color_t clrHealth = Utils::GetHealthColor(nHealth, nMaxHealth);

						const auto flHealth = static_cast<float>(nHealth);
						const auto flMaxHealth = static_cast<float>(nMaxHealth);

						static constexpr int nW = 2;

						const float flRatio = (flHealth / flMaxHealth);

						g_Draw.Rect(((nX - nW) - 1), nY, nW, nSize, Colors::OutlineESP);
						g_Draw.Rect(((nX - nW) - 1), (nY + nSize - (nSize * flRatio)), nW, (nSize * flRatio),
									clrHealth);
					}
				}
			}
		}
	}

	// Draw Players
	if (Vars::Radar::Players::Active.Value)
	{
		for (const auto& player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
		{
			if (!player->IsAlive() || player == g_EntityCache.GetObservedTarget() || player->IsAGhost() || player ==
				pLocal)
			{
				continue;
			}

			const int nEntTeam = player->GetTeamNum();
			const int nLocalTeam = pLocal->GetTeamNum();

			switch (Vars::Radar::Players::IgnoreCloaked.Value)
			{
				case 0: { break; }
				case 1:
				{
					if (player->IsCloaked()) { continue; }
					break;
				}
				case 2:
				{
					if (player->IsCloaked() && nEntTeam != nLocalTeam) { continue; }
					break;
				}
			}

			const bool bIsFriend = g_EntityCache.IsFriend(player->GetIndex());

			switch (Vars::Radar::Players::IgnoreTeam.Value)
			{
				case 0: { break; }
				case 1:
				{
					if (nEntTeam == nLocalTeam) { continue; }
					break;
				}
				case 2:
				{
					if (nEntTeam == nLocalTeam && !bIsFriend) { continue; }
					break;
				}
			}

			int nX = -1, nY = -1, nZ = 0;
			if (GetDrawPosition(nX, nY, nZ, player))
			{
				const int nSize = Vars::Radar::Players::IconSize.Value;
				nX -= (nSize / 2), nY -= (nSize / 2);

				Color_t clrDraw = Utils::GetEntityDrawColor(player, Vars::ESP::Main::EnableTeamEnemyColors.Value);

				//Background
				//Just a filled rect or a bit better looking texture RN
				//TODO:
				//Add circle background, and add outline for that
				//I think I saw a nice circle texture actually, gonna try to find that again later.
				if (Vars::Radar::Players::BackGroundType.Value)
				{
					int nTexture = 0;

					if (Vars::Radar::Players::BackGroundType.Value == 2) { nTexture += (nEntTeam + 50); }

					nTexture
						? g_Draw.Texture(nX, nY, nSize, nSize, clrDraw, nTexture)
						: g_Draw.Rect(nX, nY, nSize, nSize, clrDraw);
				}

				//Prepare the correct texture index for player icon, and draw it
				{
					PlayerInfo_t playerInfo{};
					if (Vars::Radar::Players::IconType.Value == 2 && I::EngineClient->GetPlayerInfo(player->GetIndex(), &playerInfo) && !playerInfo.fakeplayer)
					{
						// Avatar
						g_Draw.Avatar(nX, nY, nSize, nSize, playerInfo.friendsID);
					}
					else
					{
						int nTexture = player->GetClassNum();

						// Portrait
						if (Vars::Radar::Players::IconType.Value == 1)
						{
							nTexture += 10;

							if (nEntTeam == 3) { nTexture += 10; }
						}

						g_Draw.Texture(nX, nY, nSize, nSize, Colors::White, nTexture);
					}
				}

				//TODO:
				//Correct this for the circle once it's added.
				if (Vars::Radar::Players::Outline.Value)
				{
					//idk if this is kinda slow
					Color_t clrOutLine = Vars::Radar::Players::BackGroundType.Value == 1 ? clrBlack : clrDraw;
					g_Draw.OutlinedRect(nX, nY, nSize, nSize, clrOutLine);
				}

				//TODO:
				//Make the healthbar toggleable from left side to bottom.
				if (Vars::Radar::Players::Health.Value)
				{
					const int nHealth = player->GetHealth();
					const int nMaxHealth = player->GetMaxHealth();
					Color_t clrHealth = Utils::GetHealthColor(nHealth, nMaxHealth);

					auto flHealth = static_cast<float>(nHealth);
					const auto flMaxHealth = static_cast<float>(nMaxHealth);
					float flOverHeal = 0.0f;

					if (flHealth > flMaxHealth)
					{
						flOverHeal = fmod(flHealth, flMaxHealth);
						flHealth = flMaxHealth;
					}

					static constexpr int nWidth = 2;

					float flRatio = (flHealth / flMaxHealth);

					g_Draw.Rect(((nX - nWidth) - 1), nY, nWidth, nSize, Colors::OutlineESP);
					g_Draw.Rect(((nX - nWidth) - 1), (nY + nSize - (nSize * flRatio)), nWidth, (nSize * flRatio),
								clrHealth);

					if (flOverHeal > 0.0f)
					{
						flRatio = (flOverHeal / flMaxHealth);
						g_Draw.Rect(((nX - nWidth) - 1), (nY + (nSize + 1) - (nSize * flRatio)), nWidth,
									(nSize * flRatio), Colors::Overheal);
					}
				}

				// Draw height indicator (higher / lower)
				if (Vars::Radar::Players::Height.Value && std::abs(nZ) > 80.f)
				{
					const int triOffset = nZ > 0 ? -5 : 5;
					const int yPos = nZ > 0 ? nY : nY + nSize;

					g_Draw.DrawFilledTriangle({ Vec2(nX, yPos), Vec2(nX + nSize * 0.5f, yPos + triOffset), Vec2(nX + nSize, yPos) }, clrDraw);
				}
			}
		}
	}
}
