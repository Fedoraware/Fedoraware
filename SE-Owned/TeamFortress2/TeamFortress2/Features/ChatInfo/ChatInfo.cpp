#include "ChatInfo.h"

#include "../Vars.h"
#include "../Misc/Misc.h"

#define GET_PLAYER_USERID(userid) g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(userid))
#define GET_INDEX_USERID(userid) g_Interfaces.Engine->GetPlayerForUserID(userid)

void CChatInfo::Event(CGameEvent* pEvent, const FNV1A_t uNameHash) {
	if (!g_Interfaces.Engine->IsConnected() || !g_Interfaces.Engine->IsInGame())
		return;

	if (const auto pLocal = g_EntityCache.m_pLocal) {
		if (Vars::Visuals::ChatInfo.m_Var) {
			if (Vars::Misc::VoteRevealer.m_Var && uNameHash == FNV1A::HashConst("vote_cast")) {
				const auto pEntity = g_Interfaces.EntityList->GetClientEntity(pEvent->GetInt("entityid"));
				if (pEntity && pEntity->IsPlayer()) {
					const bool bVotedYes = pEvent->GetInt("vote_option") == 0;
					PlayerInfo_t pi;
					g_Interfaces.Engine->GetPlayerInfo(pEntity->GetIndex(), &pi);
					g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("\x4[FeD] \x3%s voted %s", pi.name, bVotedYes ? "Yes" : "No").c_str());
					if (Vars::Misc::VotesInChat.m_Var) {
						g_Interfaces.Engine->ClientCmd_Unrestricted(tfm::format("say_party \"%s voted %s\"", pi.name, bVotedYes ? "Yes" : "No").c_str());
					}
				}
			}

			if (uNameHash == FNV1A::HashConst("player_changeclass")) {
				if (const auto& pEntity = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(pEvent->GetInt("userid")))) {
					int nIndex = pEntity->GetIndex();

					PlayerInfo_t pi;
					g_Interfaces.Engine->GetPlayerInfo(nIndex, &pi);

					g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(nIndex, tfm::format("\x4[FeD] \x3%s\x1 is now a \x3%s\x1!", pi.name, Utils::GetClassByIndex(pEvent->GetInt("class"))).c_str());
				}
			}

			if (uNameHash == FNV1A::HashConst("player_connect")) {
				g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(GET_INDEX_USERID(pEvent->GetInt(_("userid"))), tfm::format("\x3%s\x1 connected. (%s)", pEvent->GetString("name"), pEvent->GetString("address")).c_str());
			}
		}

		if (Vars::Visuals::damageLogger.m_Var && uNameHash == FNV1A::HashConst("player_hurt")) {
			if (const auto pEntity = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(pEvent->GetInt("userid")))) {
				const auto nAttacker = pEvent->GetInt("attacker");
				const auto nHealth = pEvent->GetInt("health");
				const auto nDamage = pEvent->GetInt("damageamount");
				const auto bCrit = pEvent->GetBool("crit");
				const int  nIndex = pEntity->GetIndex();

				PlayerInfo_t pi;

				{
					g_Interfaces.Engine->GetPlayerInfo(g_Interfaces.Engine->GetLocalPlayer(), &pi);
					if (nAttacker != pi.userID)
						return;
				}

				g_Interfaces.Engine->GetPlayerInfo(nIndex, &pi);

				const auto maxHealth = pEntity->GetMaxHealth();
				std::string attackString = "You hit " + std::string(pi.name) + " for " + std::to_string(nDamage) + (bCrit ? " (crit) " : " ") + "(" + std::to_string(nHealth) + "/" + std::to_string(maxHealth) + ")";

				if (Vars::Visuals::damageLogger.m_Var == 1 && Vars::Visuals::ChatInfo.m_Var)
					g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("\x4[FeD]\x3 %s", attackString.c_str()).c_str());

				if (Vars::Visuals::damageLogger.m_Var == 2)
					g_notify.Add(attackString);
			}
		}

		if (uNameHash == FNV1A::HashConst("cl_drawline")) {
			const int nPlayer	= pEvent->GetInt("player", 0xDEAD);
			const int nLine		= pEvent->GetInt("line");
			const int nPanel	= pEvent->GetInt("panel");
			const float flX		= pEvent->GetFloat("x");
			const float flY		= pEvent->GetFloat("y");

			if (nPlayer != 0xDEAD && nLine == 0 && nPanel == 2) {
				if ((flX == 0xCA8 || flX == 0xCA7) && flY == 1234567.f) {
					PlayerInfo_t info;
					if (g_Interfaces.Engine->GetPlayerInfo(nPlayer, &info)) {
						if (m_known_bots.find(info.friendsID) == m_known_bots.end()) {
							if (Vars::Visuals::ChatInfo.m_Var)
								g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(nPlayer, tfm::format("\x4[FeD] \x3 %s\x1 is a bot!", info.name).c_str());
							m_known_bots[info.friendsID] = true;
						}
					}
				}
			}
		}
	}
}