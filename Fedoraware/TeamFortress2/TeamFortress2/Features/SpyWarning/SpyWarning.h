#pragma once
#include "../../SDK/SDK.h"

class CSpyWarning
{
private:
	bool ShouldRun(CBaseEntity* pLocal);
	void DrawArrowTo(const Vec3& vecFromPos, const Vec3& vecToPos);

	std::vector<Vec3> m_vecSpies = {};

public:
	void Run();
};

ADD_FEATURE(CSpyWarning, SpyWarning)