#pragma once
#include "../../Includes/Includes.h"

class CEngineVGui
{
public:
	virtual					~CEngineVGui(void) { }
	virtual unsigned int	GetPanel(EVGuiPanel type) = 0;
	virtual bool			IsGameUIVisible() = 0;
};

#define VENGINE_VGUI_VERSION "VEngineVGui002"