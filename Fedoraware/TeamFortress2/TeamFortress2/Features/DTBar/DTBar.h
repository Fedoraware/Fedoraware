#pragma once
#include "../../SDK/SDK.h"

#include <fstream>
#include <filesystem>

class CDTBar
{
public:
	void Run();
	int m_nBarX = g_ScreenSize.c, m_nBarY = g_ScreenSize.c + 80;
private:
	bool ShouldRun();
	void Dragbar();
	void Drawbar();
	bool buttonPressed = false;
};

inline CDTBar g_DTBar;