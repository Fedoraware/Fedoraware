#pragma once
#include "../../SDK/SDK.h"

class CFollowbot {
	bool ValidTarget(CBaseEntity* pTarget, CBaseEntity* pLocal);
	void OptimizePath(CBaseEntity* pLocal);
	CBaseEntity* FindTarget(CBaseEntity* pLocal);

	struct PathNode {
		Vec3 Location{};
		bool OnGround = false;
	};

	CBaseEntity* CurrentTarget = nullptr;

public:
	void Run(CUserCmd* pCmd);
	void Draw();

	std::deque<PathNode> FollowPath; // We have to expose this for drawing
};

inline CFollowbot g_Followbot;