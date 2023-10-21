#pragma once
#include "../Feature.h"
#include "../Colour.h"

#include <map>

struct Sightline_t
{
	Vec3 m_vStart = { 0,0,0 };
	Vec3 m_vEnd = { 0,0,0 };
	Color_t m_Color = { 0,0,0,0 };
	bool m_bDraw = false;
};

struct BulletTracer_t
{
	Vec3 m_vStartPos;
	Vec3 m_vEndPos;
	Color_t m_Color;
	float m_flTimeCreated;
};


class CVisuals
{
	void DrawAntiAim(CBaseEntity* pLocal);
	void DrawTickbaseInfo(CBaseEntity* pLocal);
	void DrawAimbotFOV(CBaseEntity* pLocal);
	void ScopeLines(CBaseEntity* pLocal);
	void DrawDebugInfo(CBaseEntity* pLocal);
	void DrawOnScreenConditions(CBaseEntity* pLocal);
	void DrawOnScreenPing(CBaseEntity* pLocal);
	void DrawNoSpreadIndicator();

	void DrawServerHitboxes();
	void DrawPredictionLine();
	void PickupTimers();
	void SetVisionFlags();

	int m_nHudZoom = 0;
	int m_nHudMotd = 0;
	IMaterial* m_pMatDev = nullptr;

public:
	void Draw();

	bool RemoveScope(int nPanel);
	void FOV(CViewSetup* pView);
	void ThirdPerson(CViewSetup* pView);
	void ModulateWorld();
	void RestoreWorldModulation();
	void OverrideWorldTextures();
	void DrawHitboxMatrix(CBaseEntity* pEntity, Color_t colourface, Color_t colouredge, float time);

	void SkyboxChanger();
	void BulletTrace(CBaseEntity* pEntity, Color_t color);
	void ProjectileTrace();
	void DrawMenuSnow();
	void DrawMovesimLine();
	void ManualNetwork(const StartSoundParams_t& params); // Credits: reestart
	void RenderLine(const Vector& v1, const Vector& v2, Color_t c, bool bZBuffer);
	void RenderBox(const Vec3& vPos, const Vec3& vMins, const Vec3& vMaxs, const Vec3& vOrientation, Color_t cEdge, Color_t cFace);
	void DrawSightlines();
	void FillSightlines();
	void AddBulletTracer(const Vec3& vFrom, const Vec3& vTo, const Color_t& clr);
	void PruneBulletTracers();
	void DrawBulletTracers();
	void DrawProjectileTracer(CBaseEntity* pLocal, const Vec3& position);

	std::deque<BulletTracer_t> m_vecBulletTracers;

	float arrowUp = 0.f;
	float arrowRight = 0.f;

	std::array<Sightline_t, 64> m_SightLines;

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

class CRunescapeChat
{
	enum EChatColour
	{
		eRS_YELLOW,
		eRS_RED,
		eRS_GREEN,
		eRS_CYAN,
		eRS_PURPLE,
		eRS_WHITE
	};

	struct Chat_t
	{
		CBaseEntity* m_pEntity = nullptr;
		float m_flTimeCreated;
		int m_nOffset;
		EChatColour m_Colour;
		std::wstring m_szChatText;
	};

	std::deque<Chat_t> m_vecChats;
public:
	void Draw();
	void PushChat(CBaseEntity* pEntity, std::wstring szChatText);
};

ADD_FEATURE(CVisuals, Visuals)

ADD_FEATURE(CRunescapeChat, RSChat);