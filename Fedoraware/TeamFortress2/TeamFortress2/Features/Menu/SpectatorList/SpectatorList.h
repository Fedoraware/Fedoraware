#pragma once
#include "../../../SDK/SDK.h"

class CSpectatorList {
private:
	struct Spectator_t
	{
		std::wstring Name;
		std::wstring Mode;
		bool IsFriend;
		int Team;
		int Index;
	};

	std::vector<Spectator_t> Spectators;
	int SpecListW = 160, SpecListTitleBarH = 24;

public:
	int SpecListX = 30, SpecListY = 100;
	bool GetSpectators(CBaseEntity* pLocal);
	bool ShouldRun();
	void Run();
	void DragSpecList(int& x, int& y, int w, int h, int offsety);
	void DrawDefault();
	void DrawClassic();
};

inline CSpectatorList g_SpectatorList;