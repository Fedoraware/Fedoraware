#pragma once
#include "../../SDK/SDK.h"

// radar
class CRadar
{
public:
	void Run();
	int m_nRadarX = 300, m_nRadarY = 300;

private:
	bool ShouldRun();
	bool GetDrawPosition(int& x, int& y, CBaseEntity* pEntity);
	void DragRadar();
	void DrawPoints(CBaseEntity* pLocal);
	void DrawRadar();

	int m_nRadarSize, m_nRadarCorrSize;
	float m_flLocalCos, m_flLocalSin, m_flRange, m_flLocalYaw;
	Vec3 m_vLocalOrigin;
};

inline CRadar g_Radar;
