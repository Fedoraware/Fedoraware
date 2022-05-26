#pragma once
#include "../../SDK/SDK.h"

class CPlayerArrows
{
private:
	bool ShouldRun(CBaseEntity* pLocal);
	void DrawArrowTo(const Vec3& vecFromPos, const Vec3& vecToPos, Color_t color);

	std::vector<Vec3> m_vecPlayers = {};

public:
	void Run();
};

ADD_FEATURE(CPlayerArrows, PlayerArrows)