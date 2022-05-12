#include "Followbot.h"

constexpr float NODE_DISTANCE = 20.f;

bool CFollowbot::ValidTarget(CBaseEntity* pTarget, CBaseEntity* pLocal)
{
	if (!pTarget) { return false; }
	if (pTarget->GetDormant()) { return false; }
	if (!pTarget->IsAlive()) { return false; }
	if (pLocal->GetAbsOrigin().DistTo(pTarget->GetAbsOrigin()) > 900.f) { return false; } // We're too slow or got stuck

	return true;
}

// Optimizes the path and removes useless Nodes
void CFollowbot::OptimizePath(CBaseEntity* pLocal)
{
	for (size_t i = 0; i < FollowPath.size(); i++)
	{
		auto& currentNode = FollowPath[i];
		if (pLocal->GetAbsOrigin().Dist2D(currentNode.Location) < NODE_DISTANCE)
		{
			int garbageNodes = static_cast<int>(i);
			while (garbageNodes > 1 && !FollowPath.empty())
			{
				FollowPath.pop_front();
				garbageNodes--;
			}
			return;
		}
	}
}

CBaseEntity* CFollowbot::FindTarget(CBaseEntity* pLocal)
{
	for (const auto& pPlayer : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
	{
		if (!pPlayer || !pPlayer->IsAlive()) { continue; }
		if (pPlayer->GetIndex() == pLocal->GetIndex()) { continue; }
		if (pLocal->GetAbsOrigin().DistTo(pPlayer->GetAbsOrigin()) > 280.f) { continue; }

		if (ValidTarget(pPlayer, pLocal))
		{
			return pPlayer;
		}
	}

	return nullptr;
}

void CFollowbot::Run(CUserCmd* pCmd)
{
	if (!Vars::Misc::Followbot::Enabled.m_Var)
	{
		CurrentTarget = nullptr;
		return;
	}

	const auto& pLocal = g_EntityCache.m_pLocal;
	if (!pLocal || !pLocal->IsAlive()) { return; }

	// Find a new target if we don't have one
	if (!ValidTarget(CurrentTarget, pLocal) || FollowPath.size() >= 300)
	{
		FollowPath.clear();
		CurrentTarget = FindTarget(pLocal);
	}
	if (!CurrentTarget) { return; }

	// Store the target position and follow him
	static Timer pathTimer{};
	if (pathTimer.Run(100))
	{
		FollowPath.push_back({ CurrentTarget->GetAbsOrigin(), CurrentTarget->IsOnGround() });
	}

	if (!FollowPath.empty())
	{
		auto& currentNode = FollowPath.front();
		const Vec3 localPos = pLocal->GetAbsOrigin();

		if (localPos.Dist2D(currentNode.Location) >= NODE_DISTANCE)
		{
			if (localPos.DistTo(CurrentTarget->GetAbsOrigin()) >= Vars::Misc::Followbot::Distance.m_Var)
			{
				Utils::WalkTo(pCmd, pLocal, currentNode.Location);
			}

			if (!currentNode.OnGround)
			{
				if (!pLocal->IsOnGround())
				{
					currentNode.OnGround = true;
				} else
				{
					pCmd->buttons |= IN_JUMP;
				}
			}
		} else
		{
			FollowPath.pop_front();
		}

		OptimizePath(pLocal);
	}
}

void CFollowbot::Draw()
{
	if (!Vars::Misc::Followbot::Enabled.m_Var || !CurrentTarget) { return; }
	if (FollowPath.size() < 2) { return; }

	const std::deque<PathNode> tmpPath = FollowPath;
	for (size_t i = 1; i < tmpPath.size(); i++)
	{
		auto& currentNode = tmpPath[i];
		auto& lastNode = tmpPath[i - 1];

		Vec3 vcScreen, vlScreen;
		if (Utils::W2S(currentNode.Location, vcScreen) && Utils::W2S(lastNode.Location, vlScreen))
		{
			g_Draw.Line(vlScreen.x, vlScreen.y, vcScreen.x, vcScreen.y, { 255, 255, 255, 255 });
		}
	}
}
