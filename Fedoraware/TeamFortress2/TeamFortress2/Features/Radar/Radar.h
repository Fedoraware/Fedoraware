#pragma once
#include "../../SDK/SDK.h"

class CRadar {
public:
	void Run();

	int RadarX = 100, RadarY = 500;

private:
	bool ShouldRun();
	void DrawRadar();
	void DragRadar();
	bool GetDrawPosition(int& x, int& y, CBaseEntity* pEntity);
	void DrawPoints(CBaseEntity* pLocal);

	int RadarSize = 0, RadarCorrSize = 0;
	float LocalCos = 0, LocalSin = 0, Range = 0, LocalYaw = 0;
	Vec3 LocalOrigin;
};

inline CRadar g_Radar;