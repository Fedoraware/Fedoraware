#include "SpectatorList.h"

#include "../Vars.h"
#include "../Menu/Menu.h"

bool CSpectatorList::GetSpectators(CBaseEntity* pLocal)
{
	m_vecSpectators.clear();

	for (const auto& pTeammate : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
	{
		CBaseEntity* pObservedPlayer = g_Interfaces.EntityList->GetClientEntityFromHandle(pTeammate->GetObserverTarget());

		if (pTeammate && !pTeammate->IsAlive() && pObservedPlayer == pLocal)
		{
			std::wstring szMode = L"";

			switch (pTeammate->GetObserverMode()) {
			case OBS_MODE_FIRSTPERSON: { szMode = _(L"1st"); break; }
			case OBS_MODE_THIRDPERSON: { szMode = _(L"3rd"); break; }
			default: continue;
			}

			PlayerInfo_t PlayerInfo;
			if (g_Interfaces.Engine->GetPlayerInfo(pTeammate->GetIndex(), &PlayerInfo))
				m_vecSpectators.push_back({ Utils::ConvertUtf8ToWide(PlayerInfo.name), szMode, g_EntityCache.Friends[pTeammate->GetIndex()], pTeammate->GetTeamNum(), pTeammate->GetIndex() });
		}
	}

	return !m_vecSpectators.empty();
}

bool CSpectatorList::ShouldRun()
{
	return Vars::Visuals::SpectatorList.m_Var && !g_Interfaces.EngineVGui->IsGameUIVisible();
}

void CSpectatorList::Run()
{
	if (!ShouldRun())
		return;

	if (Vars::Visuals::SpectatorList.m_Var == 1)
		DrawDefault();
	else
		DrawClassic();
}

void CSpectatorList::DragSpecList(int& x, int& y, int w, int h, int offsety)
{
	if (!g_Menu.m_bOpen)
		return;

	int mousex, mousey;
	g_Interfaces.Surface->GetCursorPos(mousex, mousey);

	static POINT delta;
	static bool drag = false;
	static bool move = false;
	bool held = GetKey(VK_LBUTTON);

	if ((mousex > x && mousex < x + w && mousey > y - offsety && mousey < y - offsety + h) && held)
	{
		drag = true;

		if (!move) {
			delta.x = mousex - x;
			delta.y = mousey - y;
			move = true;
		}
	}

	if (drag) {
		x = mousex - delta.x;
		y = mousey - delta.y;
	}

	if (!held) {
		drag = false;
		move = false;
	}
}

void CSpectatorList::DrawDefault()
{
	DragSpecList(
		m_nSpecListX,
		m_nSpecListY,
		m_nSpecListW,
		m_nSpecListTitleBarH,
		0);

	g_Draw.Rect(m_nSpecListX, m_nSpecListY, m_nSpecListW, m_nSpecListTitleBarH, Vars::Menu::Colors::TitleBar);

	g_Draw.String(FONT_MENU,
		m_nSpecListX + (m_nSpecListW / 2),
		m_nSpecListY + (m_nSpecListTitleBarH / 2),
		Vars::Menu::Colors::Text,
		ALIGN_CENTER,
		"%hs", _("Spectators"));

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!pLocal->IsAlive() || !GetSpectators(pLocal))
			return;

		int nFontTall = g_Draw.m_vecFonts[FONT_MENU].nTall;
		int nSpacing = 5;
		int nModeX = m_nSpecListX + nSpacing;
		int nModeW = 15;
		int nNameX = nModeX + nModeW + (nSpacing * 2);
		int y = m_nSpecListY + m_nSpecListTitleBarH;
		int h = nFontTall * m_vecSpectators.size();

		g_Draw.Rect(m_nSpecListX, y, m_nSpecListW, h, Vars::Menu::Colors::WindowBackground);
		g_Draw.Line(nModeX + nSpacing + nModeW, y, nModeX + nSpacing + nModeW, y + h - 1, Vars::Menu::Colors::Text);

		for (size_t n = 0; n < m_vecSpectators.size(); n++)
		{
			if (m_vecSpectators[n].m_sName.length() > 20)
				m_vecSpectators[n].m_sName.replace(20, m_vecSpectators[n].m_sName.length(), _(L"..."));

			y = m_nSpecListY + m_nSpecListTitleBarH + (nFontTall * n);

			g_Draw.String(FONT_MENU, nModeX, y, Vars::Menu::Colors::Text, ALIGN_DEFAULT, m_vecSpectators[n].m_sMode.data());
			g_Draw.String(FONT_MENU, nNameX, y, Vars::Menu::Colors::Text, ALIGN_DEFAULT, m_vecSpectators[n].m_sName.data());
		}
	}
}

void CSpectatorList::DrawClassic()
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!pLocal->IsAlive() || !GetSpectators(pLocal))
			return;

		int nDrawY = (g_ScreenSize.h / 2) - 300;
		int centerr = g_ScreenSize.c;
		int addyy = g_Draw.m_vecFonts[FONT_ESP_NAME].nTall;

		g_Draw.String(
			FONT_ESP_NAME,
			centerr, nDrawY - addyy,
			{ 255,255,255,255 },
			ALIGN_CENTERHORIZONTAL,
			L"Spectating you");

		for (const auto& Spectator : m_vecSpectators)
		{
			int nDrawX = g_ScreenSize.c;

			int w, h;
			g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_ESP_NAME].dwFont, (Spectator.m_sMode + Spectator.m_sName).c_str(), w, h);

			int nAddX = 0, nAddY = g_Draw.m_vecFonts[FONT_ESP_NAME].nTall;
			if (Vars::Visuals::SpectatorList.m_Var == 3)
			{
				//nDrawX -= 55;



				PlayerInfo_t pi;
				if (!g_Interfaces.Engine->GetPlayerInfo(Spectator.m_nIndex, &pi))
					continue;

				g_Draw.Avatar(nDrawX - (w / 2) - 25, nDrawY, 24, 24, pi.friendsID); // center - half the width of the string
				nDrawY += 6;

				//nAddX = 25;
				//nAddY = 14;
			}

			g_Draw.String(
				FONT_ESP_NAME,
				nDrawX, nDrawY,
				Spectator.m_bIsFriend ? Colors::Friend : Utils::GetTeamColor(Spectator.m_nTeam, Vars::ESP::Main::EnableTeamEnemyColors.m_Var),
				ALIGN_CENTERHORIZONTAL,
				L"[%ls] %ls", Spectator.m_sMode.data(), Spectator.m_sName.data());

			nDrawY += nAddY;
		}
	}
}