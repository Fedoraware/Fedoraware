#include "Radar.h"
#include "../Vars.h"
#include "../Menu/Menu.h"

constexpr Color_t clrBlack = {0, 0, 0, 255};
constexpr Color_t clrWhite = {255, 255, 255, 255};

void CRadar::Run()
{
	if (!ShouldRun()) { return; }

	//Update some members before we do anything.
	//The radar draw point and background drawing is done with this information.
	RadarSize = Vars::Radar::Main::Size.m_Var;
	RadarCorrSize = (RadarSize * 2);

	//Draw background, handle input.
	DrawRadar();

	if (const auto& pLocal = g_EntityCache.m_pLocal) { DrawPoints(pLocal); }
}

bool CRadar::ShouldRun()
{
	if (!Vars::Radar::Main::Active.m_Var) { return false; }

	return true;
}

void CRadar::DrawRadar()
{
	//If menu is open, check for input and draw the titlebar.
	//The titlebar also indicates where we can drag / move the radar.
	int offset = 0;
	if (g_Menu.IsOpen)
	{
		//A quick notify, common sense but I made it by accident:
		//If on drawing, it's important to update position first before drawing
		//Causes unwanted behaviour if you draw the title first and then call "DragRadar()"
		DragRadar();
		g_Draw.Rect(RadarX - RadarSize, RadarY - RadarSize - 24, RadarSize * 2, 24, {43, 43, 45, 250});
		offset = 21;
	}

	g_Draw.GradientRect(RadarX - RadarSize, RadarY - RadarSize - 3 - offset,
	                    RadarX - RadarSize + RadarSize, RadarY - RadarSize - offset, {43, 43, 45, 250},
	                    Vars::Menu::Colors::MenuAccent, true);
	g_Draw.GradientRect(RadarX - RadarSize + RadarSize, RadarY - RadarSize - 3 - offset,
	                    RadarX - RadarSize + (RadarSize * 2), RadarY - RadarSize - offset,
	                    Vars::Menu::Colors::MenuAccent, {43, 43, 45, 250}, true);
	//Build the bg color with the wanted alpha.
	const Color_t clrBack = {36, 36, 36, static_cast<byte>(Vars::Radar::Main::BackAlpha.m_Var)};


	//Background
	g_Draw.Rect(RadarX - RadarSize, RadarY - RadarSize, RadarCorrSize, RadarCorrSize, clrBack);

	//Outline
	g_Draw.OutlinedRect(RadarX - RadarSize, RadarY - RadarSize, RadarCorrSize, RadarCorrSize, {
		                    43, 43, 45, static_cast<byte>(Vars::Radar::Main::LineAlpha.m_Var)
	                    });

	//Center lines
	g_Draw.Line(RadarX, RadarY - RadarSize, RadarX, RadarY + RadarSize - 1, {
		            43, 43, 45, static_cast<byte>(Vars::Radar::Main::LineAlpha.m_Var)
	            });
	g_Draw.Line(RadarX - RadarSize, RadarY, RadarX + RadarSize - 1, RadarY, {
		            43, 43, 45, static_cast<byte>(Vars::Radar::Main::LineAlpha.m_Var)
	            });
}

void CRadar::DragRadar()
{
	int mousex, mousey;
	I::Surface->GetCursorPos(mousex, mousey);

	static POINT pCorrect{};
	static bool isDragging = false;
	static bool isMoving = false;
	const bool bHeld = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

	if ((mousex > (RadarX - RadarSize) &&
		mousex < (RadarX - RadarSize) + (RadarSize * 2) &&
		mousey > (RadarY - RadarSize) - 24 &&
		mousey < (RadarY - RadarSize)) && bHeld)
	{
		isDragging = true;

		if (!isMoving)
		{
			pCorrect.x = mousex - RadarX;
			pCorrect.y = mousey - (RadarY - RadarSize);
			isMoving = true;
		}
	}

	if (isDragging)
	{
		RadarX = (mousex + RadarSize) - (RadarSize) - pCorrect.x;
		RadarY = (mousey + RadarSize) - pCorrect.y;
	}

	if (!bHeld)
	{
		isDragging = false;
		isMoving = false;
	}
}

bool CRadar::GetDrawPosition(int& x, int& y, CBaseEntity* pEntity)
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

	//Just confirm that they were both set.
	return (x != -1 && y != -1);
}

void CRadar::DrawPoints(CBaseEntity* pLocal)
{
	//Update members that we use calculating the draw position in "GetDrawPosition()"
	LocalOrigin = pLocal->GetAbsOrigin();
	LocalYaw = I::Engine->GetViewAngles().y * (PI / 180.f);
	Range = static_cast<float>(Vars::Radar::Main::Range.m_Var);
	LocalCos = cos(LocalYaw), LocalSin = sin(LocalYaw);

	if (Vars::Radar::World::Active.m_Var)
	{
		const int nSize = Vars::Radar::World::IconSize.m_Var;

		if (Vars::Radar::World::Ammo.m_Var)
		{
			for (const auto& ammo : g_EntityCache.GetGroup(EGroupType::WORLD_AMMO))
			{
				int nX = -1, nY = -1;
				if (GetDrawPosition(nX, nY, ammo))
				{
					nX -= (nSize / 2), nY -= (nSize / 2);
					g_Draw.Texture(nX, nY, nSize, nSize, clrWhite, 55);
				}
			}
		}

		if (Vars::Radar::World::Health.m_Var)
		{
			for (const auto& health : g_EntityCache.GetGroup(EGroupType::WORLD_HEALTH))
			{
				int nX = -1, nY = -1;
				if (GetDrawPosition(nX, nY, health))
				{
					nX -= (nSize / 2), nY -= (nSize / 2);
					g_Draw.Texture(nX, nY, nSize, nSize, clrWhite, 50);
				}
			}
		}
	}

	if (Vars::Radar::Buildings::Active.m_Var)
	{
		const auto& buildings = g_EntityCache.GetGroup(Vars::Radar::Buildings::IgnoreTeam.m_Var
			                                              ? EGroupType::BUILDINGS_ENEMIES
			                                              : EGroupType::BUILDINGS_ALL);

		for (const auto& building : buildings)
		{
			if (const auto& pObject = reinterpret_cast<CBaseObject*>(building))
			{
				if (!pObject->IsAlive()) { continue; }

				int nX = -1, nY = -1;
				if (GetDrawPosition(nX, nY, pObject))
				{
					Color_t clrDraw = Utils::GetEntityDrawColor(building, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);

					const int nSize = Vars::Radar::Buildings::IconSize.m_Var;
					nX -= (nSize / 2), nY -= (nSize / 2);

					switch (pObject->GetClassID())
					{
					case ETFClassID::CObjectSentrygun:
						{
							int nTexture = (pObject->GetLevel() + 40);

							if (Vars::Radar::Buildings::Outline.m_Var)
							{
								g_Draw.Texture(nX - 2, nY - 2, nSize + 4, nSize + 4, clrBlack, nTexture);
							}

							g_Draw.Texture(nX, nY, nSize, nSize, clrDraw, nTexture);
							break;
						}
					case ETFClassID::CObjectDispenser:
						{
							if (Vars::Radar::Buildings::Outline.m_Var)
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

							if (Vars::Radar::Buildings::Outline.m_Var)
							{
								g_Draw.Texture(nX - 2, nY - 2, nSize + 4, nSize + 4, clrBlack, nTexture);
							}

							g_Draw.Texture(nX, nY, nSize, nSize, clrDraw, nTexture);
							break;
						}
					default: break;
					}

					if (Vars::Radar::Buildings::Health.m_Var)
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

	if (Vars::Radar::Players::Active.m_Var)
	{
		for (const auto& player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
		{
			if (!player->IsAlive() || player == g_EntityCache.m_pObservedTarget || player->IsAGhost() || player ==
				pLocal) { continue; }

			const int nEntTeam = player->GetTeamNum();
			const int nLocalTeam = pLocal->GetTeamNum();

			switch (Vars::Radar::Players::IgnoreCloaked.m_Var)
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

			switch (Vars::Radar::Players::IgnoreTeam.m_Var)
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

			int nX = -1, nY = -1;
			if (GetDrawPosition(nX, nY, player))
			{
				const int nSize = Vars::Radar::Players::IconSize.m_Var;
				nX -= (nSize / 2), nY -= (nSize / 2);

				Color_t clrDraw = Utils::GetEntityDrawColor(player, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);

				//Background
				//Just a filled rect or a bit better looking texture RN
				//TODO:
				//Add circle background, and add outline for that
				//I think I saw a nice circle texture actually, gonna try to find that again later.
				if (Vars::Radar::Players::BackGroundType.m_Var)
				{
					int nTexture = 0;

					if (Vars::Radar::Players::BackGroundType.m_Var == 2) { nTexture += (nEntTeam + 50); }

					nTexture
						? g_Draw.Texture(nX, nY, nSize, nSize, clrDraw, nTexture)
						: g_Draw.Rect(nX, nY, nSize, nSize, clrDraw);
				}

				//Prepare the correct texture index for player icon, and draw it
				{
					PlayerInfo_t playerInfo{};
					if (Vars::Radar::Players::IconType.m_Var == 2 && I::Engine->GetPlayerInfo(player->GetIndex(), &playerInfo) && !playerInfo.fakeplayer)
					{
						// Avatar
						g_Draw.Avatar(nX, nY, nSize, nSize, playerInfo.friendsID);
					}
					else
					{
						int nTexture = player->GetClassNum();

						// Portrait
						if (Vars::Radar::Players::IconType.m_Var == 1)
						{
							nTexture += 10;

							if (nEntTeam == 3) { nTexture += 10; }
						}

						g_Draw.Texture(nX, nY, nSize, nSize, Colors::White, nTexture);
					}
				}

				//TODO:
				//Correct this for the circle once it's added.
				if (Vars::Radar::Players::Outline.m_Var)
				{
					//idk if this is kinda slow
					Color_t clrOutLine = Vars::Radar::Players::BackGroundType.m_Var == 1 ? clrBlack : clrDraw;
					g_Draw.OutlinedRect(nX, nY, nSize, nSize, clrOutLine);
				}

				//TODO:
				//Make the healthbar toggleable from left side to bottom.
				if (Vars::Radar::Players::Health.m_Var)
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
			}
		}
	}
}
