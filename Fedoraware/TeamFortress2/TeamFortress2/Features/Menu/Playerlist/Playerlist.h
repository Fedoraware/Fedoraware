#pragma once
#include "../../../SDK/SDK.h"

class CPlayerList {
public:
	void UpdatePlayers();
	void Render();
};

ADD_FEATURE(CPlayerList, PlayerList)