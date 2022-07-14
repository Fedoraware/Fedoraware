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
	void FOV(CViewSetup* pView);
	void ThirdPerson(CViewSetup* pView);
	void ModulateWorld();
	void RestoreWorldModulation();
	void OverrideWorldTextures();
	void PickupTimers();
	void DrawHitboxMatrix(CBaseEntity* pEntity, Color_t colourface, Color_t colouredge, float time);
	void ScopeLines(CBaseEntity* pLocal);
	void SkyboxChanger();
	void BulletTrace(CBaseEntity* pEntity, Color_t color);
	void DrawAimbotFOV(CBaseEntity* pLocal);
	void DrawDebugInfo(CBaseEntity* pLocal);
	void DrawAntiAim(CBaseEntity* pLocal);
	void DrawTickbaseInfo(CBaseEntity* pLocal);
	void DrawMenuSnow();
	void DrawDVD();
	void DrawPredictionLine();
	void DrawMovesimLine();
	void RenderLine(const Vector& v1, const Vector& v2, Color_t c, bool bZBuffer);

	void SetVisionFlags();

	float arrowUp = 0.f;
	float arrowRight = 0.f;

	class CPrecipitation
	{
	private:
		CBaseEntity* RainEntity = nullptr;
		IClientNetworkable* RainNetworkable = nullptr;
		CClientClass* GetPrecipitationClass();
	public:
		void Run();
		void Cleanup();
	};
	CPrecipitation rain;

	struct PickupData
	{
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

ADD_FEATURE(CVisuals, Visuals)