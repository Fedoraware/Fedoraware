#include "../Hooks.h"

#include "../../Features/Misc/Misc.h"
#include "../../Features/ChatInfo/ChatInfo.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "../../Features/Visuals/Visuals.h"

static int anti_balance_attempts = 0;
static std::string previous_name;

const static std::string CLEAR_MSG("?\nServer:\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
								   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
								   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
								   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
								   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
								   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
								   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

const static std::vector<std::string> BAD_WORDS{ "cheat", "hack", "bot", "aim", "esp", "kick", "hax", "script" };

static std::string clr({ '\x7', '0', 'D', '9', '2', 'F', 'F' });
static std::string yellow({ '\x7', 'C', '8', 'A', '9', '0', '0' }); //C8A900
static std::string white({ '\x7', 'F', 'F', 'F', 'F', 'F', 'F' }); //FFFFFF
static std::string green({ '\x7', '3', 'A', 'F', 'F', '4', 'D' }); //3AFF4D

MAKE_HOOK(BaseClientDLL_DispatchUserMessage, Utils::GetVFuncPtr(I::BaseClientDLL, 36), bool, __fastcall,
		  void* ecx, void* edx, UserMessageType type, bf_read& msgData)
{
	const auto bufData = reinterpret_cast<const char*>(msgData.m_pData);
	msgData.SetAssertOnOverflow(false);

	F::ChatInfo.UserMessage(type, msgData);
	msgData.Seek(0);

	switch (type)
	{
		case SayText2:
		{
			const int nbl = msgData.GetNumBytesLeft();
			if (nbl < 5 || nbl >= 256)
			{
				break;
			}

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

			F::RSChat.PushChat(I::ClientEntityList->GetClientEntity(entIdx), Utils::ConvertUtf8ToWide(chatMessage));

			break;
		}

		case VoiceSubtitle:
		{
			int iEntityID = msgData.ReadByte();
			int iVoiceMenu = msgData.ReadByte();
			int iCommandID = msgData.ReadByte();

			if (iVoiceMenu == 1 && iCommandID == 6)
			{
				G::MedicCallers.push_back(iEntityID);
			}

			break;
		}

		case TextMsg:
		{
			if (Vars::Misc::AntiAutobal.Value && msgData.GetNumBitsLeft() > 35)
			{
				const INetChannel* server = I::EngineClient->GetNetChannelInfo();
				const std::string data(bufData);

				if (data.find("TeamChangeP") != std::string::npos && g_EntityCache.GetLocal())
				{
					const std::string serverName(server->GetAddress());
					if (serverName != previous_name)
					{
						previous_name = serverName;
						anti_balance_attempts = 0;
					}
					if (anti_balance_attempts < 2)
					{
						I::EngineClient->ClientCmd_Unrestricted("retry");
					}
					else
					{
						I::EngineClient->ClientCmd_Unrestricted(
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
			if (Vars::Visuals::RemoveMOTD.Value || Vars::Misc::AutoJoin.Value)
			{
				if (strcmp(reinterpret_cast<char*>(msgData.m_pData), "info") == 0)
				{
					I::EngineClient->ClientCmd_Unrestricted("closedwelcomemenu");
					return true;
				}
			}

			// Autojoin team / class
			if (Vars::Misc::AutoJoin.Value)
			{
				if (strcmp(reinterpret_cast<char*>(msgData.m_pData), "team") == 0)
				{
					I::EngineClient->ClientCmd_Unrestricted("autoteam");
					return true;
				}

				if (strncmp(reinterpret_cast<char*>(msgData.m_pData), "class_", 6) == 0)
				{
					static std::string classNames[] = { "scout", "soldier", "pyro", "demoman", "heavyweapons", "engineer", "medic", "sniper", "spy" };
					I::EngineClient->ClientCmd_Unrestricted(std::string("join_class").append(" ").append(classNames[Vars::Misc::AutoJoin.Value - 1]).c_str());
					return true;
				}
			}

			break;
		}

		case ForcePlayerViewAngles:
		{
			return Vars::Visuals::PreventForcedAngles.Value ? true : Hook.Original<FN>()(ecx, edx, type, msgData);
		}

		case SpawnFlyingBird:
		case PlayerGodRayEffect:
		case PlayerTauntSoundLoopStart:
		case PlayerTauntSoundLoopEnd:
		{
			return Vars::Visuals::RemoveTaunts.Value ? true : Hook.Original<FN>()(ecx, edx, type, msgData);
		}

		case Shake:
		case Fade:
		case Rumble:
		{
			return Vars::Visuals::RemoveScreenEffects.Value ? true : Hook.Original<FN>()(ecx, edx, type, msgData);
		}
	}

	msgData.Seek(0);
	return Hook.Original<FN>()(ecx, edx, type, msgData);
}