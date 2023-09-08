#pragma once
#include "../Feature.h"

class CPlayerArrows
{
	bool ShouldRun(CBaseEntity* pLocal);
	void DrawArrowTo(const Vec3& vecFromPos, const Vec3& vecToPos, Color_t color);

	std::vector<Vec3> m_vecPlayers = {};

public:
	void Draw();
};

ADD_FEATURE(CPlayerArrows, PlayerArrows)