#pragma once
#include "../../Feature.h"
#include "../../Colour.h"

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