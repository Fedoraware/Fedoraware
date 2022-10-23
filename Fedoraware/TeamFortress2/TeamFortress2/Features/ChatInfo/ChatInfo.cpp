#include "ChatInfo.h"

#include "../Vars.h"
#include "../Misc/Notifications/Notifications.h"
#include "../../Features/Visuals/Visuals.h"

int attackStringW;
int attackStringH;

static std::string yellow({ '\x7', 'C', '8', 'A', '9', '0', '0' }); //C8A900 //CACACA
static std::string blue({ '\x7', '0', 'D', '9', '2', 'F', 'F' }); //0D92FF
static std::string red({ '\x7', 'F', 'F', '3', 'A', '3', 'A' }); //FF3A3A
static std::string green({ '\x7', '3', 'A', 'F', 'F', '4', 'D' }); //3AFF4D
static std::string white({ '\x7', 'F', 'F', 'F', 'F', 'F', 'F' }); //FFFFFF
static std::string gsyellow({ '\x7', 'C', 'A', 'C', 'A', 'C', 'A' }); //CACACA
static std::string gsblue({ '\x7', '7', '6', '7', '6', '7', '6' }); //767676
static std::string gsred({ '\x7', '7', '5', '7', '5', '7', '5' }); //757575
static std::string gsgreen({ '\x7', 'B', '0', 'B', '0', 'B', '0' }); //b0b0b0

enum VoteLogger
{
	VoteText = 1 << 0,
	VoteConsole = 1 << 1,
	VoteChat = 1 << 2,
	VoteParty = 1 << 3,
	VoteVerbose = 1 << 5
};

// Event info
void CChatInfo::Event(CGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (!I::EngineClient->IsConnected() || !I::EngineClient->IsInGame())
	{
		return;
	}

	if (const auto pLocal = g_EntityCache.GetLocal())
	{
		// Class change
		if ((Vars::Visuals::ChatInfoText.Value || Vars::Visuals::ChatInfoChat.Value) && uNameHash == FNV1A::HashConst("player_changeclass"))
		{
			if (const auto& pEntity = I::ClientEntityList->GetClientEntity(
				I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"))))
			{
				if (pEntity == pLocal) { return; }
				int nIndex = pEntity->GetIndex();

				PlayerInfo_t pi{};
				I::EngineClient->GetPlayerInfo(nIndex, &pi);

				std::string classString = tfm::format("%s %s is now a %s", Vars::Menu::CheatPrefix.c_str(), pi.name, Utils::GetClassByIndex(pEvent->GetInt("class")));
				auto wclassString = std::wstring(classString.begin(), classString.end());

				if (Vars::Visuals::ChatInfoText.Value)
				{
					F::Notifications.Add(classString);
				}

				if (Vars::Visuals::ChatInfoChat.Value)
				{
					const std::string changeClassString = tfm::format("%s %s %s is now a %s", Vars::Menu::Colors::MenuAccent.to_hex(), Vars::Menu::CheatPrefix.c_str(), pi.name, Utils::GetClassByIndex(pEvent->GetInt("class")));
					I::ClientModeShared->m_pChatElement->ChatPrintf(0, changeClassString.c_str());
				}
			}
		}

		// Vote options
		if (Vars::Misc::VotingOptions.Value && uNameHash == FNV1A::HashConst("vote_cast"))
		{
			const auto pEntity = I::ClientEntityList->GetClientEntity(pEvent->GetInt("entityid"));
			if (pEntity && pEntity->IsPlayer())
			{
				const bool bVotedYes = pEvent->GetInt("vote_option") == 0;
				const int votingOptions = Vars::Misc::VotingOptions.Value;
				PlayerInfo_t pi{};
				I::EngineClient->GetPlayerInfo(pEntity->GetIndex(), &pi);
				auto voteLine = tfm::format("%s %s %s voted %s", Vars::Menu::CheatPrefix.c_str(), (pEntity->GetTeamNum() == pLocal->GetTeamNum()) ? "" : "(Enemy)", pi.name, bVotedYes ? "Yes" : "No");

				if (votingOptions & VoteText) // text
				{
					F::Notifications.Add(voteLine);
				}
				if (votingOptions & VoteConsole) // console
				{
					I::Cvar->ConsoleColorPrintf({ 133, 255, 66, 255 }, tfm::format("%s \n", voteLine).c_str());
				}
				if (votingOptions & VoteChat) // chat
				{
					I::ClientModeShared->m_pChatElement->ChatPrintf(0, tfm::format("%s %s \x3%s %svoted %s%s", Vars::Menu::Colors::MenuAccent.to_hex(), Vars::Menu::CheatPrefix.c_str(), pi.name, (Vars::Visuals::ChatInfoGrayScale.Value ? gsyellow : yellow), bVotedYes ? (Vars::Visuals::ChatInfoGrayScale.Value ? gsgreen : green) : (Vars::Visuals::ChatInfoGrayScale.Value ? gsred : red), bVotedYes ? "Yes" : "No").c_str());
				}
				if (votingOptions & VoteParty) // party
				{
					I::EngineClient->ClientCmd_Unrestricted(tfm::format("tf_party_chat \"%s\"", voteLine).c_str());
				}
			}
		}

		// Damage logger
		if ((Vars::Visuals::DamageLoggerConsole.Value || Vars::Visuals::DamageLoggerChat.Value) && uNameHash == FNV1A::HashConst("player_hurt"))
		{
			if (const auto pEntity = I::ClientEntityList->GetClientEntity(
				I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"))))
			{
				const auto nAttacker = pEvent->GetInt("attacker");
				const auto nHealth = pEvent->GetInt("health");
				const auto nDamage = pEvent->GetInt("damageamount");
				const auto bCrit = pEvent->GetBool("crit");
				const int nIndex = pEntity->GetIndex();
				if (pEntity == pLocal) { return; }


				PlayerInfo_t pi{};

				{
					I::EngineClient->GetPlayerInfo(I::EngineClient->GetLocalPlayer(), &pi);
					if (nAttacker != pi.userID) { return; }
				}

				I::EngineClient->GetPlayerInfo(nIndex, &pi);

				const auto maxHealth = pEntity->GetMaxHealth();
				std::string attackString = "You hit " + std::string(pi.name) + " for " + std::to_string(nDamage) + (
					bCrit ? " (crit) " : " ") + "(" + std::to_string(nHealth) + "/" + std::to_string(maxHealth) + ")";

				auto wattackString = std::wstring(attackString.begin(), attackString.end());
				const wchar_t* wcattackString = wattackString.c_str();
				I::VGuiSurface->GetTextSize(g_Draw.m_vecFonts[FONT_ESP_COND].dwFont, wcattackString,
											attackStringW, attackStringH);
				const std::string chatAttackString(
					Vars::Menu::Colors::MenuAccent.to_hex() + Vars::Menu::CheatPrefix + (Vars::Visuals::ChatInfoGrayScale.Value ? gsyellow : yellow) + " You hit \x3" + pi.name + (Vars::Visuals::ChatInfoGrayScale.Value ? gsyellow : yellow) + " for " + (Vars::Visuals::ChatInfoGrayScale.Value ? gsred : red) + std::to_string(nDamage) + " damage " + (bCrit ? (Vars::Visuals::ChatInfoGrayScale.Value ? gsgreen : green) + "(crit) " : "") + (Vars::Visuals::ChatInfoGrayScale.Value ? gsyellow : yellow) + "(" +
					std::to_string(nHealth) + "/" + std::to_string(maxHealth) + ")");

				if (Vars::Visuals::DamageLoggerChat.Value)
				{
					I::ClientModeShared->m_pChatElement->ChatPrintf(0,
																	chatAttackString.c_str());
				}

				if (Vars::Visuals::DamageLoggerConsole.Value)
				{
					I::Cvar->ConsoleColorPrintf({ 219, 145, 59, 255 }, "%s\n", attackString.c_str());
				}

				if (Vars::Visuals::DamageLoggerText.Value)
				{
					F::Notifications.Add(attackString);
				}
			}
		}

		// Show Hitboxes on hit
		if (Vars::Aimbot::Global::showHitboxes.Value && uNameHash == FNV1A::HashConst("player_hurt"))
		{
			if (Vars::Aimbot::Global::ClearPreviousHitbox.Value) { I::DebugOverlay->ClearAllOverlays(); }
			auto time = Vars::Aimbot::Global::HitboxLifetime.Value;
			// alpha is how "filled" the hitbox render is, looks bad at anything non-zero (rijin moment)
			auto pEntity = I::ClientEntityList->GetClientEntity(
				I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid")));
			const auto nAttacker = I::ClientEntityList->GetClientEntity(
				I::EngineClient->GetPlayerForUserID(pEvent->GetInt("attacker")));
			if (pEntity == pLocal) { return; }
			if (pLocal != nAttacker) { return; }
			F::Visuals.DrawHitboxMatrix(pEntity, Colors::HitboxFace, Colors::HitboxEdge, time);
		}
	}
}

// User Message info
void CChatInfo::UserMessage(UserMessageType type, bf_read& msgData)
{
	switch (type)
	{
		case VoteStart:
		{
			const int team = msgData.ReadByte();
			const int voteID = msgData.ReadLong();
			const int caller = msgData.ReadByte();
			char reason[64], voteTarget[64];
			msgData.ReadString(reason, 64);
			msgData.ReadString(voteTarget, 64);
			const int target = static_cast<unsigned char>(msgData.ReadByte()) >> 1;

			PlayerInfo_t infoTarget{}, infoCaller{};
			if (const auto& pLocal = g_EntityCache.GetLocal())
			{
				if (target && caller && I::EngineClient->GetPlayerInfo(target, &infoTarget) && I::EngineClient->GetPlayerInfo(caller, &infoCaller))
				{
					const bool bSameTeam = team == pLocal->GetTeamNum();

					const auto bluntLine = tfm::format("%s %s called a vote on %s", bSameTeam ? "" : "(Enemy)", infoCaller.name, infoTarget.name);
					const auto verboseLine = tfm::format("%s %s [U:1:%s] called a vote on %s [U:1:%s]", bSameTeam ? "" : "(Enemy)", infoCaller.name, infoCaller.friendsID, infoTarget.name,
														 infoTarget.friendsID);

					const int votingOptions = Vars::Misc::VotingOptions.Value;
					const bool verboseVoting = votingOptions & VoteVerbose;

					const auto chosenLine = verboseVoting ? verboseLine.c_str() : bluntLine.c_str();

					// Log to Text/Notifications
					if (votingOptions & VoteText)
					{
						F::Notifications.Add(chosenLine);
					}

					// Log to Console
					if (votingOptions & VoteConsole)
					{
						I::Cvar->ConsoleColorPrintf({ 133, 255, 66, 255 }, tfm::format("%s \n", chosenLine).c_str());
					}

					// Log to Chat
					if (votingOptions & VoteChat)
					{
						I::ClientModeShared->m_pChatElement->ChatPrintf(0, chosenLine);
					}

					// Log to Party
					if (votingOptions & VoteParty)
					{
						I::EngineClient->ClientCmd_Unrestricted(tfm::format("tf_party_chat \"%s\"", chosenLine).c_str());
					}

					// Auto Vote
					if (Vars::Misc::AutoVote.Value && bSameTeam && target != I::EngineClient->GetLocalPlayer())
					{
						if (G::IsIgnored(infoTarget.friendsID) || g_EntityCache.IsFriend(target))
						{
							I::EngineClient->ClientCmd_Unrestricted(tfm::format("vote %d option2", voteID).c_str()); //f2 on ignored and steam friends
						}
						else
						{
							I::EngineClient->ClientCmd_Unrestricted(tfm::format("vote %d option1", voteID).c_str()); //f1 on everyone else
						}
					}
				}
			}
			break;
		}
	}
}
