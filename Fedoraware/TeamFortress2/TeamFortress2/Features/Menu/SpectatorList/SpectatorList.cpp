#include "SpectatorList.h"

#include "../../Vars.h"
#include "../Menu.h"

bool CSpectatorList::GetSpectators(CBaseEntity* pLocal)
{
	Spectators.clear();

	for (const auto& pTeammate : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
	{
		CBaseEntity* pObservedPlayer = I::EntityList->GetClientEntityFromHandle(
			pTeammate->GetObserverTarget());

		if (pTeammate && !pTeammate->IsAlive() && pObservedPlayer == pLocal)
		{
			std::wstring szMode;
			switch (pTeammate->GetObserverMode())
			{
			case OBS_MODE_FIRSTPERSON:
				{
					szMode = _(L"1st");
					break;
				}
			case OBS_MODE_THIRDPERSON:
				{
					szMode = _(L"3rd");
					break;
				}
			default: continue;
			}

			PlayerInfo_t playerInfo{ };
			if (I::Engine->GetPlayerInfo(pTeammate->GetIndex(), &playerInfo))
			{
				Spectators.push_back({
					Utils::ConvertUtf8ToWide(playerInfo.name), szMode, g_EntityCache.IsFriend(pTeammate->GetIndex()),
					pTeammate->GetTeamNum(), pTeammate->GetIndex()
				});
			}
		}
	}

	return !Spectators.empty();
}

bool CSpectatorList::ShouldRun()
{
	return Vars::Visuals::SpectatorList.m_Var/* && !I::EngineVGui->IsGameUIVisible()*/;
}

void CSpectatorList::Run()
{
	if (!ShouldRun()) { return; }

	if (Vars::Visuals::SpectatorList.m_Var == 1) { DrawDefault(); }
	else { DrawClassic(); }
}

void CSpectatorList::DragSpecList(int& x, int& y, int w, int h, int offsety)
{
	if (!g_Menu.IsOpen) { return; }

	int mousex, mousey;
	I::Surface->GetCursorPos(mousex, mousey);

	static POINT delta{};
	static bool drag = false;
	static bool move = false;
	const bool held = GetKey(VK_LBUTTON);

	if ((mousex > x && mousex < x + w && mousey > y - offsety && mousey < y - offsety + h) && held)
	{
		drag = true;

		if (!move)
		{
			delta.x = mousex - x;
			delta.y = mousey - y;
			move = true;
		}
	}

	if (drag)
	{
		x = mousex - delta.x;
		y = mousey - delta.y;
	}

	if (!held)
	{
		drag = false;
		move = false;
	}
}

void CSpectatorList::DrawDefault()
{
	DragSpecList(
		SpecListX,
		SpecListY,
		SpecListW,
		SpecListTitleBarH,
		0);

	// 38 to 43
	g_Draw.Rect(SpecListX, SpecListY, SpecListW, SpecListTitleBarH, {43, 43, 45, 250});

	g_Draw.String(FONT_IMGUI,
	              SpecListX + (SpecListW / 2),
	              SpecListY + (SpecListTitleBarH / 2),
	              Vars::Menu::Colors::MenuAccent,
	              ALIGN_CENTER,
	              "%hs", _("Spectators"));

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!pLocal->IsAlive() || !GetSpectators(pLocal)) { return; }

		constexpr int nSpacing = 5;
		constexpr int nModeW = 15;
		const int nFontTall = g_Draw.m_vecFonts[FONT_MENU].nTall;
		const int nModeX = SpecListX + nSpacing;
		const int nNameX = nModeX + nModeW + (nSpacing * 2);
		int y = SpecListY + SpecListTitleBarH;
		const int h = nFontTall * Spectators.size();

		// 25 to 31
		g_Draw.Rect(SpecListX, y, SpecListW, h, {36, 36, 36, 255});
		g_Draw.Line(nModeX + nSpacing + nModeW, y, nModeX + nSpacing + nModeW, y + h - 1, {255, 255, 255, 255});

		for (size_t n = 0; n < Spectators.size(); n++)
		{
			if (Spectators[n].Name.length() > 20)
			{
				Spectators[n].Name.replace(20, Spectators[n].Name.length(), _(L"..."));
			}

			y = SpecListY + SpecListTitleBarH + (nFontTall * n);

			g_Draw.String(FONT_MENU, nModeX, y, {255, 255, 255, 255}, ALIGN_DEFAULT, Spectators[n].Mode.data());
			g_Draw.String(FONT_MENU, nNameX, y, {255, 255, 255, 255}, ALIGN_DEFAULT, Spectators[n].Name.data());
		}
	}
}

void CSpectatorList::DrawClassic()
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!pLocal->IsAlive() || !GetSpectators(pLocal)) { return; }

		int nDrawY = (g_ScreenSize.h / 2) - 300;
		const int centerr = g_ScreenSize.c;
		const int addyy = g_Draw.m_vecFonts[FONT_ESP_NAME].nTall;

		g_Draw.String(
			FONT_ESP_NAME,
			centerr, nDrawY - addyy,
			{255, 255, 255, 255},
			ALIGN_CENTERHORIZONTAL,
			L"Spectating you");

		for (const auto& Spectator : Spectators)
		{
			int nDrawX = g_ScreenSize.c;

			int w, h;
			I::Surface->GetTextSize(g_Draw.m_vecFonts[FONT_ESP_NAME].dwFont,
			                                  (Spectator.Mode + Spectator.Name).c_str(), w, h);

			const int nAddY = g_Draw.m_vecFonts[FONT_ESP_NAME].nTall;
			if (Vars::Visuals::SpectatorList.m_Var == 3)
			{
				PlayerInfo_t pi{};
				if (!I::Engine->GetPlayerInfo(Spectator.Index, &pi)) { continue; }

				g_Draw.Avatar(nDrawX - (w / 2) - 25, nDrawY, 24, 24, pi.friendsID);
				// center - half the width of the string
				nDrawY += 6;
			}

			g_Draw.String(
				FONT_ESP_NAME,
				nDrawX, nDrawY,
				Spectator.IsFriend
					? Colors::Friend
					: Utils::GetTeamColor(Spectator.Team, Vars::ESP::Main::EnableTeamEnemyColors.m_Var),
				ALIGN_CENTERHORIZONTAL,
				L"[%ls] %ls", Spectator.Mode.data(), Spectator.Name.data());

			nDrawY += nAddY;
		}
	}
}
