#pragma once
#include "../../SDK/Includes/Includes.h"

struct plistPlayer
{
	PlayerInfo_t info;
	Color_t color;
	int teamNum;
	int index;
};

class CPlayerList
{
public:
	bool showWindow;
	void GetPlayers();
	void Render();
	std::vector<plistPlayer> players;
};

inline CPlayerList g_PlayerList;