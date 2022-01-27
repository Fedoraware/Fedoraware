#include "Radar.h"
#include "../Vars.h"
#include "../Menu/Menu.h"

constexpr Color_t clrBlack = { 0, 0, 0, 255 };
constexpr Color_t clrWhite = { 255, 255, 255, 255 };

void CRadar::Run()
{
	if (!ShouldRun())
		return;

	//Update some members before we do anything.
	//The radar draw point and background drawing is done with this information.
	m_nRadarSize = Vars::Radar::Main::Size.m_Var;
	m_nRadarCorrSize = (m_nRadarSize * 2);

	//Draw background, handle input.
	DrawRadar();

	if (const auto& pLocal = g_EntityCache.m_pLocal)
		DrawPoints(pLocal);
}

void CRadar::DrawRadar()
{
	//If menu is open, check for input and draw the titlebar.
	//The titlebar also indicates where we can drag / move the radar.
	if (g_Menu.m_bOpen || g_Menu.m_flFadeElapsed < g_Menu.m_flFadeDuration)
	{
		//A quick notify, common sense but I made it by accident:
		//If on drawing, it's important to update position first before drawing
		//Causes unwanted behaviour if you draw the title first and then call "DragRadar()"
		DragRadar();

		g_Interfaces.Surface->DrawSetAlphaMultiplier(g_Menu.m_flFadeAlpha);
		g_Draw.Rect(m_nRadarX - m_nRadarSize, m_nRadarY - m_nRadarSize - 10, m_nRadarSize * 2, 10, Vars::Menu::Colors::TitleBar);
		g_Interfaces.Surface->DrawSetAlphaMultiplier(1.0f);
	}

	//Build the bg color with the wanted alpha.
	Color_t clrBack = { 30, 30, 30, static_cast<byte>(Vars::Radar::Main::BackAlpha.m_Var) };
	Color_t clrLines = { Vars::Menu::Colors::TitleBar.r, Vars::Menu::Colors::TitleBar.g, Vars::Menu::Colors::TitleBar.b, static_cast<byte>(Vars::Radar::Main::LineAlpha.m_Var) };



	//Background
	g_Draw.Rect(m_nRadarX - m_nRadarSize, m_nRadarY - m_nRadarSize, m_nRadarCorrSize, m_nRadarCorrSize, clrBack);

	//Outline
	g_Draw.OutlinedRect(m_nRadarX - m_nRadarSize, m_nRadarY - m_nRadarSize, m_nRadarCorrSize, m_nRadarCorrSize, clrLines);

	//Center lines
	g_Draw.Line(m_nRadarX, m_nRadarY - m_nRadarSize, m_nRadarX, m_nRadarY + m_nRadarSize - 1, clrLines);
	g_Draw.Line(m_nRadarX - m_nRadarSize, m_nRadarY, m_nRadarX + m_nRadarSize - 1, m_nRadarY, clrLines);
}

bool CRadar::GetDrawPosition(int& x, int& y, CBaseEntity* pEntity)
{
	//Calculate the delta and initial position of the entity
	Vec3 vDelta = pEntity->GetAbsOrigin() - m_vLocalOrigin;
	Vec2 vPos = Vec2((vDelta.y * (-m_flLocalCos) + vDelta.x * m_flLocalSin),
		(vDelta.x * (-m_flLocalCos) - vDelta.y * m_flLocalSin));

	//Range, keep in bounds
	//Credits are due to whoever wrote this, does what I want and is fast so idc. Code probably older than Jesus.
	//Found it in my 2015 CSGO pasta so I am sure it is made by someone else than me.
	if (fabs(vPos.x) > m_flRange || fabs(vPos.y) > m_flRange)
	{
		if (vPos.y > vPos.x)
		{
			if (vPos.y > -vPos.x)
			{
				vPos.x = m_flRange * vPos.x / vPos.y;
				vPos.y = m_flRange;
			}
			else
			{
				vPos.y = -m_flRange * vPos.y / vPos.x;
				vPos.x = -m_flRange;
			}
		}
		else
		{
			if (vPos.y > -vPos.x)
			{
				vPos.y = m_flRange * vPos.y / vPos.x;
				vPos.x = m_flRange;
			}
			else
			{
				vPos.x = -m_flRange * vPos.x / vPos.y;
				vPos.y = -m_flRange;
			}
		}
	}

	x = (m_nRadarX + static_cast<int>(vPos.x / m_flRange * m_nRadarSize));
	y = (m_nRadarY + static_cast<int>(vPos.y / m_flRange * m_nRadarSize));

	//Just confirm that they were both set.
	return (x != -1 && y != -1);
}

void CRadar::DrawPoints(CBaseEntity* pLocal)
{
	//Update members that we use calculating the draw position in "GetDrawPosition()"
	m_vLocalOrigin = pLocal->GetAbsOrigin();
	m_flLocalYaw = g_Interfaces.Engine->GetViewAngles().y * (PI / 180.0f),
		m_flRange = static_cast<float>(Vars::Radar::Main::Range.m_Var);
	m_flLocalCos = cos(m_flLocalYaw), m_flLocalSin = sin(m_flLocalYaw);

	if (Vars::Radar::World::Active.m_Var)
	{
		const int nSize = Vars::Radar::World::IconSize.m_Var;

		if (Vars::Radar::World::Ammo.m_Var)
		{
			for (const auto& Ammo : g_EntityCache.GetGroup(EGroupType::WORLD_AMMO))
			{
				int nX = -1, nY = -1;
				if (GetDrawPosition(nX, nY, Ammo))
				{
					nX -= (nSize / 2), nY -= (nSize / 2);
					g_Draw.Texture(nX, nY, nSize, nSize, clrWhite, 55);
				}
			}
		}

		if (Vars::Radar::World::Health.m_Var)
		{
			for (const auto& Health : g_EntityCache.GetGroup(EGroupType::WORLD_HEALTH))
			{
				int nX = -1, nY = -1;
				if (GetDrawPosition(nX, nY, Health))
				{
					nX -= (nSize / 2), nY -= (nSize / 2);
					g_Draw.Texture(nX, nY, nSize, nSize, clrWhite, 50);
				}
			}
		}
	}

	if (Vars::Radar::Buildings::Active.m_Var)
	{
		const auto Buildings = g_EntityCache.GetGroup(Vars::Radar::Buildings::IgnoreTeam.m_Var ? EGroupType::BUILDINGS_ENEMIES : EGroupType::BUILDINGS_ALL);

		for (const auto& Building : Buildings)
		{
			if (const auto& pObject = reinterpret_cast<CBaseObject*>(Building))
			{
				if (!pObject->IsAlive())
					continue;

				int nX = -1, nY = -1;
				if (GetDrawPosition(nX, nY, pObject))
				{
					Color_t clrDraw = Utils::GetEntityDrawColor(Building, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);

					const int nSize = Vars::Radar::Buildings::IconSize.m_Var;
					nX -= (nSize / 2), nY -= (nSize / 2);

					switch (pObject->GetClassID())
					{
					case ETFClassID::CObjectSentrygun:
					{
						int nTexture = (pObject->GetLevel() + 40);

						if (Vars::Radar::Buildings::Outline.m_Var)
							g_Draw.Texture(nX - 2, nY - 2, nSize + 4, nSize + 4, clrBlack, nTexture);

						g_Draw.Texture(nX, nY, nSize, nSize, clrDraw, nTexture);
						break;
					}
					case ETFClassID::CObjectDispenser:
					{
						if (Vars::Radar::Buildings::Outline.m_Var)
							g_Draw.Texture(nX - 2, nY - 2, nSize + 4, nSize + 4, clrBlack, 44);

						g_Draw.Texture(nX, nY, nSize, nSize, clrDraw, 44);
						break;
					}
					case ETFClassID::CObjectTeleporter:
					{
						int nTexture = 46; //Exit texture ID

						//If "m_flYawToExit" is not zero, it most like is an entrance
						if (pObject->GetYawToExit())
							nTexture -= 1; //In that case, -1 from "nTexture" so we get entrace texture ID

						if (Vars::Radar::Buildings::Outline.m_Var)
							g_Draw.Texture(nX - 2, nY - 2, nSize + 4, nSize + 4, clrBlack, nTexture);

						g_Draw.Texture(nX, nY, nSize, nSize, clrDraw, nTexture);
						break;
					}
					default: break;
					}

					if (Vars::Radar::Buildings::Health.m_Var)
					{
						int nHealth = pObject->GetHealth(), nMaxHealth = pObject->GetMaxHealth();
						Color_t clrHealth = Utils::GetHealthColor(nHealth, nMaxHealth);

						float flHealth = static_cast<float>(nHealth);
						float flMaxHealth = static_cast<float>(nMaxHealth);

						static const int nW = 2;

						float flRatio = (flHealth / flMaxHealth);

						g_Draw.Rect(((nX - nW) - 1), nY, nW, nSize, Colors::OutlineESP);
						g_Draw.Rect(((nX - nW) - 1), (nY + nSize - (nSize * flRatio)), nW, (nSize * flRatio), clrHealth);
					}
				}
			}
		}
	}

	if (Vars::Radar::Players::Active.m_Var)
	{
		for (const auto& Player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
		{
			if (!Player->IsAlive() || Player == g_EntityCache.m_pObservedTarget || Player->IsAGhost() || Player == pLocal)
				continue;

			int nEntTeam = Player->GetTeamNum(), nLocalTeam = pLocal->GetTeamNum();

			switch (Vars::Radar::Players::IgnoreCloaked.m_Var)
			{
			case 0: { break; }
			case 1: { if (Player->IsCloaked()) { continue; } break; }
			case 2: { if (Player->IsCloaked() && nEntTeam != nLocalTeam) { continue; } break; }
			}

			bool bIsFriend = g_EntityCache.Friends[Player->GetIndex()];

			switch (Vars::Radar::Players::IgnoreTeam.m_Var)
			{
			case 0: { break; }
			case 1: { if (nEntTeam == nLocalTeam) { continue; } break; }
			case 2: { if (nEntTeam == nLocalTeam && !bIsFriend) { continue; } break; }
			}

			int nX = -1, nY = -1;
			if (GetDrawPosition(nX, nY, Player))
			{
				const int nSize = Vars::Radar::Players::IconSize.m_Var;
				nX -= (nSize / 2), nY -= (nSize / 2);

				Color_t clrDraw = Utils::GetEntityDrawColor(Player, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);

				//Background
				//Just a filled rect or a bit better looking texture RN
				//TODO:
				//Add circle background, and add outline for that
				//I think I saw a nice circle texture actually, gonna try to find that again later.
				if (Vars::Radar::Players::BackGroundType.m_Var)
				{
					int nTexture = 0;

					if (Vars::Radar::Players::BackGroundType.m_Var == 2)
					{
						nTexture += (nEntTeam + 50);
					}

					nTexture ? g_Draw.Texture(nX, nY, nSize, nSize, clrDraw, nTexture) : g_Draw.Rect(nX, nY, nSize, nSize, clrDraw);
				}

				//Prepare the correct texture index for player icon, and draw it
				{
					if (Vars::Radar::Players::IconType.m_Var == 2)
					{
						PlayerInfo_t pi;
						if (g_Interfaces.Engine->GetPlayerInfo(Player->GetIndex(), &pi))
							g_Draw.Avatar(nX, nY, nSize, nSize, pi.friendsID);
					}
					else
					{
						int nTexture = Player->GetClassNum();

						if (Vars::Radar::Players::IconType.m_Var == 1)
						{
							nTexture += 10;

							if (nEntTeam == 3)
								nTexture += 10;
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
					int nHealth = Player->GetHealth(), nMaxHealth = Player->GetMaxHealth();
					Color_t clrHealth = Utils::GetHealthColor(nHealth, nMaxHealth);

					float flHealth = static_cast<float>(nHealth);
					float flMaxHealth = static_cast<float>(nMaxHealth);
					float flOverHeal = 0.0f;

					if (flHealth > flMaxHealth)
					{
						flOverHeal = fmod(flHealth, flMaxHealth);
						flHealth = flMaxHealth;
					}

					static const int nWidth = 2;

					float flRatio = (flHealth / flMaxHealth);

					g_Draw.Rect(((nX - nWidth) - 1), nY, nWidth, nSize, Colors::OutlineESP);
					g_Draw.Rect(((nX - nWidth) - 1), (nY + nSize - (nSize * flRatio)), nWidth, (nSize * flRatio), clrHealth);

					if (flOverHeal > 0.0f)
					{
						flRatio = (flOverHeal / flMaxHealth);
						g_Draw.Rect(((nX - nWidth) - 1), (nY + (nSize + 1) - (nSize * flRatio)), nWidth, (nSize * flRatio), Colors::Overheal);
					}
				}
			}
		}
	}
}

bool CRadar::ShouldRun()
{
	if (!Vars::Radar::Main::Active.m_Var || g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	return true;
}

void CRadar::DragRadar()
{
	int mousex, mousey;
	g_Interfaces.Surface->GetCursorPos(mousex, mousey);

	static POINT pCorrect;
	static bool m_bDrag = false;
	static bool m_bMove = false;
	bool bHeld = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

	if ((mousex > (m_nRadarX - m_nRadarSize) &&
		mousex < (m_nRadarX - m_nRadarSize) + (m_nRadarSize * 2) &&
		mousey >(m_nRadarY - m_nRadarSize) - 10 &&
		mousey < (m_nRadarY - m_nRadarSize)) && bHeld)
	{
		m_bDrag = true;

		if (!m_bMove)
		{
			pCorrect.x = mousex - m_nRadarX;
			pCorrect.y = mousey - (m_nRadarY - m_nRadarSize);
			m_bMove = true;
		}
	}

	if (m_bDrag)
	{
		m_nRadarX = (mousex + m_nRadarSize) - (m_nRadarSize)-pCorrect.x;
		m_nRadarY = (mousey + m_nRadarSize) - pCorrect.y;
	}

	if (!bHeld)
	{
		m_bDrag = false;
		m_bMove = false;
	}
}