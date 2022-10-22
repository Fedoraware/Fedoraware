#pragma once
#include "../../../SDK/SDK.h"

class CPlayerList
{
	void Save();
	void Load();

public:
	void UpdatePlayers();
	void Run();
	void Render();

	bool ShouldSave = false;
	bool ShouldLoad = true;
};

ADD_FEATURE(CPlayerList, PlayerList)