#pragma once
#include "../../../SDK/SDK.h"

struct ListPlayer {
	PlayerInfo_t Info{ };
	Color_t Color{ };
	int Team = -1;
	int Index = -1;
};

class CPlayerList {
	bool SortByTeam(const ListPlayer& a, const ListPlayer& b);

	std::array<ListPlayer, 64> PlayerCache;
	
public:
	void UpdatePlayers();
	void Render();

	bool IsOpen = false;
};

inline CPlayerList g_PlayerList;