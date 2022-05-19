#pragma once
#include "../../SDK/SDK.h"

class CVisuals
{
private:
	int m_nHudZoom = 0;
	int m_nHudMotd = 0;
	IMaterial* m_pMatDev;

public:
	bool RemoveScope(int nPanel);
	bool RemoveMOTD(int nPanel);
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
	CPrecipitation rain;

	struct PickupData {
		int Type = 0;
		float Time = 0.f;
		Vec3 Location;
	};
	std::vector<PickupData> PickupDatas;

	struct MaterialHandleData
	{
		enum class EMatGroupType
		{
			GROUP_OTHER,
			GROUP_WORLD,
			GROUP_SKY
		};

		MaterialHandle_t Handle;
		IMaterial* Material;
		std::string_view Group;
		std::string_view Name;
		EMatGroupType	 GroupType;
		bool			 ShouldOverrideTextures;
	};
	std::vector<MaterialHandleData> MaterialHandleDatas;

	void StoreMaterialHandles();
	void ClearMaterialHandles();


};

inline CVisuals g_Visuals;
