#pragma once
#include "../../SDK/SDK.h"

class CVisuals
{
private:
	int m_nHudZoom = 0;
	IMaterial* m_pMatDev;

public:
	bool RemoveScope(int nPanel);
	void FOV(CViewSetup* pView);
	void ThirdPerson(CViewSetup* pView);
	void ModulateWorld();
	void RestoreWorldModulation();
	void OverrideWorldTextures();
	void PickupTimers();
	void DrawHitboxMatrix(CBaseEntity* pEntity, Color_t colourface, Color_t colouredge, float time);
	void ScopeLines();
	void SkyboxChanger();
	void BigHeads(float headSize, float torsoSize, float handSize);
	void BulletTrace(CBaseEntity* pEntity, Color_t color);
	float arrowUp = 0.f;
	float arrowRight = 0.f;
	class CPrecipitation {
	private:
		CBaseEntity* RainEntity = nullptr;
		IClientNetworkable* RainNetworkable = nullptr;
	private:
		CClientClass* GetPrecipitationClass();
	public:
		void Run();
		void Cleanup();
	};

	struct PickupData {
		int Type = 0;
		float Time = 0.f;
		Vec3 Location;
	};
	std::vector<PickupData> PickupDatas;

	CPrecipitation rain;
};

inline CVisuals g_Visuals;
