#include "../Hooks.h"

#include "../../Features/Misc/Misc.h"
#include "../../Features/ChatInfo/ChatInfo.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>

static int anti_balance_attempts = 0;
static std::string previous_name;

const static std::string CLEAR_MSG("?\nServer:\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
								   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
								   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
								   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
								   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
								   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
								   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

const static std::vector<std::string> BAD_WORDS{ _("cheat"), _("hack"), _("bot"), _("aim"), _("esp"), _("kick"), _("hax"), _("script") };

static std::string clr({ '\x7', '0', 'D', '9', '2', 'F', 'F' });
static std::string yellow({ '\x7', 'C', '8', 'A', '9', '0', '0' }); //C8A900
static std::string white({ '\x7', 'F', 'F', 'F', 'F', 'F', 'F' }); //FFFFFF
static std::string green({ '\x7', '3', 'A', 'F', 'F', '4', 'D' }); //3AFF4D

MAKE_HOOK(BaseClientDLL_FispatchUserMessage, Utils::GetVFuncPtr(I::Client, 36), bool, __fastcall,
		  void* ecx, void* edx, UserMessageType type, bf_read& msgData)
{
	const auto bufData = reinterpret_cast<const char*>(msgData.m_pData);
	msgData.SetAssertOnOverflow(false);

	g_ChatInfo.UserMessage(type, msgData);

	switch (type)
	{
		case SayText2:
		{
			const int nbl = msgData.GetNumBytesLeft();
			if (nbl < 5 || nbl >= 256)
			{
				break;
			}

			msgData.Seek(0);
			const int entIdx = msgData.ReadByte();
			msgData.Seek(8);
			char typeBuffer[256], nameBuffer[256], msgBuffer[256];
			if (msgData.GetNumBytesLeft() == 0) { break; }
			msgData.ReadString(typeBuffer, 256);
			if (msgData.GetNumBytesLeft() == 0) { break; }
			msgData.ReadString(nameBuffer, 256);
			if (msgData.GetNumBytesLeft() == 0) { break; }
			msgData.ReadString(msgBuffer, 256);

			std::string chatType(typeBuffer);
			std::string playerName(nameBuffer);
			std::string chatMessage(msgBuffer);

			if (Vars::Misc::ChatCensor.m_Var)
			{
				PlayerInfo_t senderInfo{};
				if (I::Engine->GetPlayerInfo(entIdx, &senderInfo))
				{
					if (entIdx == I::Engine->GetLocalPlayer()) { break; }
					if (g_GlobalInfo.IsIgnored(senderInfo.friendsID) || g_EntityCache.IsFriend(entIdx))
					{
						break;
					}

					const std::vector<std::string> toReplace = { " ", "4", "3", "0", "6", "5", "7", "@", ".", ",", "-", "!" };
					const std::vector<std::string> replaceWith = { "", "a", "e", "o", "g", "s", "t", "a", "", "", "", "i" };

					for (std::vector<int>::size_type i = 0; i != toReplace.size(); i++)
					{
						boost::replace_all(chatMessage, toReplace[i], replaceWith[i]);
					}

					for (auto& word : BAD_WORDS)
					{
						if (boost::contains(chatMessage, word))
						{
							const std::string cmd = "say_team \"" + CLEAR_MSG + "\"";
							I::Engine->ServerCmd(cmd.c_str(), true);
							I::ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%s[FeD] \x3 %s\x1 wrote\x3 %s", clr, playerName, chatMessage).c_str());
							break;
						}
					}
				}
			}

			break;
		}

		case TextMsg:
		{
			if (Vars::Misc::AntiAutobal.m_Var && msgData.GetNumBitsLeft() > 35)
			{
				const INetChannel* server = I::Engine->GetNetChannelInfo();
				const std::string data(bufData);

				if (data.find("TeamChangeP") != std::string::npos && g_EntityCache.m_pLocal)
				{
					const std::string serverName(server->GetAddress());
					if (serverName != previous_name)
					{
						previous_name = serverName;
						anti_balance_attempts = 0;
					}
					if (anti_balance_attempts < 2)
					{
						I::Engine->ClientCmd_Unrestricted("retry");
					}
					else
					{
						I::Engine->ClientCmd_Unrestricted(
							"tf_party_chat \"I will be autobalanced in 3 seconds\"");
					}
					anti_balance_attempts++;
				}
			}
			break;
		}

		case VGUIMenu:
		{
			// Remove MOTD
			if (Vars::Visuals::RemoveMOTD.m_Var || Vars::Misc::AutoJoin.m_Var)
			{
				if (strcmp(reinterpret_cast<char*>(msgData.m_pData), "info") == 0)
				{
					I::Engine->ClientCmd_Unrestricted("closedwelcomemenu");
					return true;
				}
			}

			// Autojoin team / class
			if (Vars::Misc::AutoJoin.m_Var)
			{
				if (strcmp(reinterpret_cast<char*>(msgData.m_pData), "team") == 0)
				{
					I::Engine->ClientCmd_Unrestricted("autoteam");
					return true;
				}

				if (strncmp(reinterpret_cast<char*>(msgData.m_pData), "class_", 6) == 0)
				{
					static std::string classNames[] = { "scout", "soldier", "pyro", "demoman", "heavyweapons", "engineer", "medic", "sniper", "spy" };
					I::Engine->ClientCmd_Unrestricted(std::string("join_class").append(" ").append(classNames[Vars::Misc::AutoJoin.m_Var - 1]).c_str());
					return true;
				}
			}

			break;
		}

		case ForcePlayerViewAngles:
		{
			return Vars::Visuals::PreventForcedAngles.m_Var ? true : Hook.Original<FN>()(ecx, edx, type, msgData);
		}

		case SpawnFlyingBird:
		case PlayerGodRayEffect:
		case PlayerTauntSoundLoopStart:
		case PlayerTauntSoundLoopEnd:
		{
			return Vars::Visuals::RemoveTaunts.m_Var ? true : Hook.Original<FN>()(ecx, edx, type, msgData);
		}

		case Shake:
		case Fade:
		case Rumble:
		{
			return Vars::Visuals::RemoveScreenEffects.m_Var ? true : Hook.Original<FN>()(ecx, edx, type, msgData);
		}
	}

	msgData.Seek(0);
	return Hook.Original<FN>()(ecx, edx, type, msgData);
}