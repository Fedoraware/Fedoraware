#pragma once
#include "../../../SDK/SDK.h"

class CDTBar{
	bool ShouldRun();
	void Dragbar();
	bool buttonPressed = false;

public:
	void Run();
	int BarX = g_ScreenSize.c;
	int BarY = g_ScreenSize.c + 80;
};

inline CDTBar g_DTBar;