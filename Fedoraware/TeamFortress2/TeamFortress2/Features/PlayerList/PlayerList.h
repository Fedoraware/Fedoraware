#pragma once
#include "../../SDK/Includes/Includes.h"

struct plistPlayer
{
	PlayerInfo_t info;
	Color_t color;
	int teamNum;
	int index = -1;
};

class CPlayerList
{
public:
	bool showWindow;
	void GetPlayers();
	void Render();
	std::array<plistPlayer, 64> players;
};

inline CPlayerList g_PlayerList;
