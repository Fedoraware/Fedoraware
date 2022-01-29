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
	void UpdateSkyModulation();
	void RestoreWorldModulation();
	void OverrideWorldTextures();
	void TransparentProps();
	void DrawHitboxMatrix(CBaseEntity* pEntity, Color_t colour, float time);
	void ScopeLines();
	void SkyboxChanger();
	void BigHeads(float headSize, float torsoSize, float handSize);
	void BulletTrace(CBaseEntity* pEntity, Color_t color);
};

inline CVisuals g_Visuals;