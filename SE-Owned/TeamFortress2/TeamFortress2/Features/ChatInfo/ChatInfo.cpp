#include "ChatInfo.h"

#include "../Vars.h"
#include "../Misc/Misc.h"

int attackStringW;
int attackStringH;

#define GET_PLAYER_USERID(userid) g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetPlayerForUserID(userid))
#define GET_INDEX_USERID(userid) g_Interfaces.Engine->GetPlayerForUserID(userid)

void CChatInfo::Event(CGameEvent* pEvent, const FNV1A_t uNameHash) {
	if (!g_Interfaces.Engine->IsConnected() || !g_Interfaces.Engine->IsInGame())
		return;

	static std::string clr({ '\x7', 'F', 'F', 'C', '1', '4', 'B' });

	if (const auto pLocal = g_EntityCache.m_pLocal) {
		if (Vars::Visuals::ChatInfo.m_Var) {
			if (Vars::Misc::VoteRevealer.m_Var && uNameHash == FNV1A::HashConst("vote_cast")) {
				const auto pEntity = g_Interfaces.EntityList->GetClientEntity(pEvent->GetInt("entityid"));
				if (pEntity && pEntity->IsPlayer()) {
					const bool bVotedYes = pEvent->GetInt("vote_option") == 0;
					PlayerInfo_t pi;
					g_Interfaces.Engine->GetPlayerInfo(pEntity->GetIndex(), &pi);
					// TODO: Add colors and possibly OTHER TEAM? 
					g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%s[FeD] \x3%s voted %s", clr, pi.name, bVotedYes ? "Yes" : "No").c_str());
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

					g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(nIndex, tfm::format("%s[FeD] \x3%s\x1 is now a \x3%s\x1!", clr, pi.name, Utils::GetClassByIndex(pEvent->GetInt("class"))).c_str());
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
				std::string attackString = "You hit " + std::string(pi.guid) + " for " + std::to_string(nDamage) + (bCrit ? " (crit) " : " ") + "(" + std::to_string(nHealth) + "/" + std::to_string(maxHealth) + ")";

				std::wstring wattackString = std::wstring(attackString.begin(), attackString.end());
				const wchar_t* wcattackString = wattackString.c_str();
				g_Interfaces.Surface->GetTextSize(g_Draw.m_vecFonts[FONT_INDICATORS].dwFont, wcattackString, attackStringW, attackStringH);

				if (Vars::Visuals::damageLogger.m_Var == 1 && Vars::Visuals::ChatInfo.m_Var)
					g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%s[FeD]\x3 %s", clr, attackString.c_str()).c_str());

				if (Vars::Visuals::damageLogger.m_Var == 2)
					g_notify.Add(attackString);
			}
		}

		if (uNameHash == FNV1A::HashConst("achievement_earned")) {
			const int player = pEvent->GetInt("player", 0xDEAD);
			const int achievement = pEvent->GetInt("achievement", 0xDEAD);

			// 0xCA7 is an identify and mark request.
			// 0xCA8 is a mark request.

			PlayerInfo_t info;
			if (g_Interfaces.Engine->GetPlayerInfo(player, &info) && (achievement == 0xCA7 || achievement == 0xCA8) && pLocal->GetIndex() != player) {
				if (m_known_bots.find(info.friendsID) == m_known_bots.end()) {
					g_notify.Add(tfm::format("%s is a bot!", info.name));
					if (Vars::Visuals::ChatInfo.m_Var)
						g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(player, tfm::format("%s[FeD] \x3 %s\x1 is a bot!", clr, info.name).c_str());

					{ // marked by other bots. r.i.p cl_drawline :(
						// this will be detected by fedoraware and lmaobox easily.
						// use 0xCA7 if you want to make more bots do the thing,
						// most only care about being marked.
						if (Vars::Misc::BeCat.m_Var) {
							KeyValues* kv = new KeyValues("AchievementEarned");
							kv->SetInt("achievementID", 0xCA8);
							g_Interfaces.Engine->ServerCmdKeyValues(kv);
						}
					}

					m_known_bots[info.friendsID] = true;
				}
			}
		}
	}
}