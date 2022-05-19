#pragma once
#include "../../../SDK/SDK.h"

struct ListPlayer {
	const char* Name{};
	int UserID{};
	uint32_t FriendsID{};
	bool FakePlayer{};
	Color_t Color{};
	int Health{};
	int MaxHealth{};
	int Class{};
	bool Alive{};
};

class CPlayerList {
public:
	void UpdatePlayers();
	void Render();

	std::multimap<int, ListPlayer> PlayerCache{};
};

inline CPlayerList g_PlayerList;