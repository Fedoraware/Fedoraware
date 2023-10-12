#pragma once
#include "Vars.h"
//TODO: move this lol

__inline Color_t GetTeamColour(int nTeamNum, bool otherColors)
{
	if (otherColors) {
		if (const auto& pLocal = g_EntityCache.GetLocal()) {
			// Enemy/Team based colors
			const auto lPlayerTeam = pLocal->GetTeamNum();

			if (pLocal->IsInValidTeam() && (nTeamNum == 2 || nTeamNum == 3))
			{
				if (lPlayerTeam == nTeamNum)
				{
					return Vars::Colours::Friendly.Value;
				}

				return Vars::Colours::Enemy.Value;
			}

			return Colors::White;
		}
	}
	else {
		switch (nTeamNum)
		{
		case 2: return Vars::Colours::TeamRed.Value;
		case 3: return Vars::Colours::TeamBlu.Value;
		default: return Colors::White;
		}
	}
	return Colors::White;
}

__inline Color_t GetEntityDrawColour(CBaseEntity* pEntity, bool enableOtherColors)
{
	Color_t out = GetTeamColour(pEntity->GetTeamNum(), enableOtherColors);
	PlayerInfo_t info{}; I::EngineClient->GetPlayerInfo(pEntity->GetIndex(), &info);

	if (pEntity->IsNPC()) { out = Vars::Colours::NPC.Value; }
	if (pEntity->IsBomb()) { out = Vars::Colours::Bomb.Value; }

	if (pEntity->IsPlayer())
	{
		if (g_EntityCache.GetLocal()->GetIndex() == pEntity->GetIndex())
		{
			out = Vars::Colours::Local.Value;
		}

		else if (g_EntityCache.IsFriend(pEntity->GetIndex()) || pEntity == g_EntityCache.GetLocal())
		{
			out = Vars::Colours::Friend.Value;
		}

		else if (G::IsIgnored(info.friendsID))
		{
			out = Vars::Colours::Ignored.Value;
		}

		else if (pEntity->IsCloaked())
		{
			out = Vars::Colours::Cloak.Value;
		}

		else if (!pEntity->IsVulnerable())
		{
			out = Vars::Colours::Invuln.Value;
		}
	}

	if (pEntity->GetIndex() == G::CurrentTargetIdx)
	{
		out = Vars::Colours::Target.Value;
	}

	return out;
}