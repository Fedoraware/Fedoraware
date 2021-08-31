#pragma once
#include "../../SDK/SDK.h"

class CVisuals
{
private:
	int m_nHudZoom = 0;
	IMaterial* m_pMatDev;

public:
	bool RemoveScope(int nPanel);
	void Init();
	void FOV(CViewSetup *pView);
	void ThirdPerson();
	void ModulateWorld();
	void UpdateWorldModulation();
	void RestoreWorldModulation();
	void OverrideWorldTextures();
	void SkyboxChanger();
	void BulletTrace(CBaseEntity* pEntity, Color_t color);
};

inline CVisuals g_Visuals;