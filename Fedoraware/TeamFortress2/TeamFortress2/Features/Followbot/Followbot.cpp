#include "Followbot.h"

/*
 * This followbot is very basic and could need some improvements.
 * But it's better than nothing...
 * - lnx00
 */

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
	std::lock_guard guard(PathMutex);

	for (size_t i = 0; i < PathNodes.size(); i++)
	{
		auto& currentNode = PathNodes[i];
		if (pLocal->GetAbsOrigin().Dist2D(currentNode.Location) < NODE_DISTANCE)
		{
			int garbageNodes = static_cast<int>(i);
			while (garbageNodes > 1 && !PathNodes.empty())
			{
				PathNodes.pop_front();
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
		if (Vars::Misc::Followbot::FriendsOnly.Value && !g_EntityCache.IsFriend(pPlayer->GetIndex())) { continue; }

		if (ValidTarget(pPlayer, pLocal))
		{
			return pPlayer;
		}
	}

	return nullptr;
}

void CFollowbot::Run(CUserCmd* pCmd)
{
	const auto& pLocal = g_EntityCache.GetLocal();
	if (!Vars::Misc::Followbot::Enabled.Value || !pLocal || !pLocal->IsAlive())
	{
		Reset();
		return;
	}

	// Find a new target if we don't have one
	if (!ValidTarget(CurrentTarget, pLocal) || PathNodes.size() >= 300)
	{
		Reset();
		CurrentTarget = FindTarget(pLocal);
	}
	if (!CurrentTarget) { return; }

	// Store the target position and follow him
	static Timer pathTimer{};
	if (pathTimer.Run(100))
	{
		if (PathNodes.empty())
		{
			PathNodes.push_back({ CurrentTarget->GetAbsOrigin(), CurrentTarget->OnSolid() });
		}
		else
		{
			const auto& lastNode = PathNodes.back();
			if (CurrentTarget->GetAbsOrigin().DistTo(lastNode.Location) >= 5.f)
			{
				PathNodes.push_back({ CurrentTarget->GetAbsOrigin(), CurrentTarget->OnSolid() });
			}
		}
	}

	// Walk to the next node
	if (!PathNodes.empty())
	{
		auto& currentNode = PathNodes.front();
		const Vec3 localPos = pLocal->GetAbsOrigin();

		if (localPos.Dist2D(currentNode.Location) >= NODE_DISTANCE)
		{
			if (localPos.DistTo(CurrentTarget->GetAbsOrigin()) >= Vars::Misc::Followbot::Distance.Value)
			{
				Utils::WalkTo(pCmd, pLocal, currentNode.Location);
			}

			if (!currentNode.OnGround)
			{
				if (!pLocal->OnSolid())
				{
					currentNode.OnGround = true;
				}
				else
				{
					pCmd->buttons |= IN_JUMP;
				}
			}
		}
		else
		{
			PathNodes.pop_front();
		}

		OptimizePath(pLocal);
	}
}

void CFollowbot::Reset()
{
	CurrentTarget = nullptr;
	PathNodes.clear();
}

void CFollowbot::Draw()
{
	if (!Vars::Misc::Followbot::Enabled.Value || !CurrentTarget) { return; }
	if (PathNodes.size() < 2) { return; }

	const std::deque<PathNode> tmpPath = PathNodes;
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
