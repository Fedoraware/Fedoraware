#pragma once
#include "../../SDK/SDK.h"

class CVisuals
{
private:
	int m_nHudZoom = 0;

public:
	bool RemoveScope(int nPanel);
	void FOV(CViewSetup *pView);
	void ThirdPerson();
	void ModulateWorld();
	void UpdateWorldModulation();
	void RestoreWorldModulation();
};

inline CVisuals g_Visuals;