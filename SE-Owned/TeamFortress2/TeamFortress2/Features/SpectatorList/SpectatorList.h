#pragma once
#include "../../SDK/SDK.h"

class CSpectatorList
{
private:
	struct Spectator_t { std::wstring m_sName; std::wstring m_sMode; bool m_bIsFriend; int m_nTeam; int m_nIndex; };
	std::vector<Spectator_t> m_vecSpectators;
	bool GetSpectators(CBaseEntity* pLocal);
	bool ShouldRun();
	void DragSpecList(int& x, int& y, int w, int h, int offsety);
	void DrawDefault();
	void DrawClassic();

private:
	int m_nSpecListW = 160, m_nSpecListTitleBarH = 12;

public:
	int m_nSpecListX = 500, m_nSpecListY = 500;

public:
	void Run();
};

inline CSpectatorList g_SpectatorList;