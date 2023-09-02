#pragma once
#include "../Feature.h"

class CRadar
{
	bool ShouldRun();
	void DrawRadar();
	bool GetDrawPosition(int& x, int& y, int& z, CBaseEntity* pEntity);
	void DrawPoints(CBaseEntity* pLocal);

	int RadarSize = 0;
	float LocalCos = 0, LocalSin = 0, Range = 0, LocalYaw = 0;
	Vec3 LocalOrigin;

public:
	void Draw();
	void DrawWindow();

	int RadarX = 100, RadarY = 500;
};

ADD_FEATURE(CRadar, Radar)