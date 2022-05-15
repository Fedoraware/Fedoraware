#include "ClientHook.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "../../Features/Misc/Misc.h"
#include "../../Features/AntiHack/CheaterDetection/CheaterDetection.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AttributeChanger/AttributeChanger.h"
#include "../../Features/Resolver/Resolver.h"
#include "../../Features/Menu/Playerlist/Playerlist.h"

const static std::string CLEAR_MSG("?\nServer:\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

const static std::vector<std::string> BAD_WORDS{ _("cheat"), _("hack"), _("bot"), _("aim"), _("esp"), _("kick"), _("hax"), _("script")};

static std::string clr({'\x7', '0', 'D', '9', '2', 'F', 'F'});
static std::string yellow({ '\x7', 'C', '8', 'A', '9', '0', '0' }); //C8A900
static std::string white({ '\x7', 'F', 'F', 'F', 'F', 'F', 'F' }); //FFFFFF
static std::string green({ '\x7', '3', 'A', 'F', 'F', '4', 'D' }); //3AFF4D

void __stdcall ClientHook::PreEntity::Hook(const char* szMapName)
{
	Table.Original<fn>(index)(g_Interfaces.Client, szMapName);
}

void __stdcall ClientHook::PostEntity::Hook()
{
	Table.Original<fn>(index)(g_Interfaces.Client);
	g_Interfaces.Engine->ClientCmd_Unrestricted(_("r_maxdlights 69420"));
	g_Interfaces.Engine->ClientCmd_Unrestricted(_("r_dynamic 1"));

}

void __stdcall ClientHook::ShutDown::Hook()
{
	Table.Original<fn>(index)(g_Interfaces.Client);
	g_EntityCache.Clear();
	g_Visuals.rain.Cleanup();
	g_GlobalInfo.partyPlayerESP.clear();
	g_Resolver.ResolveData.clear();
	g_GlobalInfo.chokeMap.clear();
}


void __stdcall ClientHook::FrameStageNotify::Hook(EClientFrameStage FrameStage)
{
	switch (FrameStage)
	{
	case EClientFrameStage::FRAME_RENDER_START:
		{
			g_GlobalInfo.m_vPunchAngles = Vec3();

			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				// Handle freecam position
				if (g_GlobalInfo.m_bFreecamActive && Vars::Visuals::FreecamKey.m_Var && GetAsyncKeyState(Vars::Visuals::FreecamKey.m_Var) & 0x8000) {
					pLocal->SetVecOrigin(g_GlobalInfo.m_vFreecamPos);
					pLocal->SetAbsOrigin(g_GlobalInfo.m_vFreecamPos);
				}

				// Remove punch effect
				if (Vars::Visuals::RemovePunch.m_Var)
				{
					g_GlobalInfo.m_vPunchAngles = pLocal->GetPunchAngles();
					//Store punch angles to be compesnsated for in aim
					pLocal->ClearPunchAngle(); //Clear punch angles for visual no-recoil
				}
			}

			// Resolver
			g_Resolver.Run();

			g_Visuals.SkyboxChanger();

			break;
		}

	default: break;
	}

	Table.Original<fn>(index)(g_Interfaces.Client, FrameStage);

	switch (FrameStage)
	{
	case EClientFrameStage::FRAME_NET_UPDATE_START:
		{
			g_EntityCache.Clear();

			break;
		}


	case EClientFrameStage::FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		{
			g_AttributeChanger.Run();

			break;
		}


	case EClientFrameStage::FRAME_NET_UPDATE_END:
		{
			g_EntityCache.Fill();
			g_PlayerList.UpdatePlayers();

			g_GlobalInfo.m_bLocalSpectated = false;

			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				for (const auto& Teammate : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
				{
					if (Teammate->IsAlive() || g_EntityCache.Friends[Teammate->GetIndex()])
						continue;

					CBaseEntity* pObservedPlayer = g_Interfaces.EntityList->GetClientEntityFromHandle(
						Teammate->GetObserverTarget());

					if (pObservedPlayer == pLocal)
					{
						switch (Teammate->GetObserverMode())
						{
						case OBS_MODE_FIRSTPERSON: break;
						case OBS_MODE_THIRDPERSON: break;
						default: continue;
						}

						g_GlobalInfo.m_bLocalSpectated = true;
						break;
					}
				}
			}

			for (int i =0;i < g_Interfaces.Engine->GetMaxClients(); i++) {
				if (const auto& Player = g_Interfaces.EntityList->GetClientEntity(i)) {
					VelFixRecord record = { Player->m_vecOrigin(), Player->m_fFlags(), Player->GetSimulationTime() };
					g_GlobalInfo.velFixRecord[Player] = record;
				}
			}

			break;
		}

	case EClientFrameStage::FRAME_RENDER_START:
		{
			if (!g_GlobalInfo.unloadWndProcHook)
			{
				if (Vars::Visuals::Rain.m_Var > 0) {
					g_Visuals.rain.Run();
				}
				static bool modded = false;
				if (Vars::Visuals::SkyModulation.m_Var || Vars::Visuals::WorldModulation.m_Var) { g_Visuals.ModulateWorld(); modded = true; }
				else if (modded) { modded = false; g_Visuals.ModulateWorld(); } // genius method i swear
			}
			break;
		}

	default: break;
	}
}

static int anti_balance_attempts = 0;
static std::string previous_name;

bool __stdcall ClientHook::DispatchUserMessage::Hook(int type, bf_read& msgData)
{
	auto bufData = reinterpret_cast<const char*>(msgData.m_pData);
	msgData.SetAssertOnOverflow(false);

	switch (type)
	{
	case SayText2:
		{
			int nbl = msgData.GetNumBytesLeft();
			if (nbl < 5 || nbl >= 256)
			{
				break;
			}

			msgData.Seek(0);
			int entIdx = msgData.ReadByte();
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
				if (g_Interfaces.Engine->GetPlayerInfo(entIdx, &senderInfo))
				{
					if (entIdx == g_Interfaces.Engine->GetLocalPlayer()) { break; }
					if (g_GlobalInfo.ignoredPlayers.find(senderInfo.friendsID) != g_GlobalInfo.ignoredPlayers.end()
						|| (entIdx > 0 && entIdx <= 128 && g_EntityCache.Friends[entIdx]))
					{
						break;
					}

					const std::vector<std::string> toReplace = { " ", "4", "3", "0", "6", "5", "7", "@", ".", ",", "-", "!" };
					const std::vector<std::string> replaceWith = { "", "a", "e", "o", "g", "s", "t", "a", "", "", "", "i" };

					for (std::vector<int>::size_type i = 0; i != toReplace.size(); i++) {
						boost::replace_all(chatMessage, toReplace[i], replaceWith[i]);
					}

					for (auto& word : BAD_WORDS)
					{
						if (boost::contains(chatMessage, word))
						{
							const std::string cmd = "say_team \"" + CLEAR_MSG + "\"";
							g_Interfaces.Engine->ServerCmd(cmd.c_str(), true);
							g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%s[FeD] \x3 %s\x1 wrote\x3 %s", clr, playerName, chatMessage).c_str());
							break;
						}
					}
				}
			}

			msgData.Seek(0);
			break;
		}
	case TextMsg:
		{
			if (Vars::Misc::AntiAutobal.m_Var && msgData.GetNumBitsLeft() > 35)
			{
				INetChannel* server = g_Interfaces.Engine->GetNetChannelInfo();

				std::string data(bufData);

				if (data.find("TeamChangeP") != data.npos && g_EntityCache.m_pLocal)
				{
					std::string server_name(server->GetAddress());
					if (server_name != previous_name)
					{
						previous_name = server_name;
						anti_balance_attempts = 0;
					}
					if (anti_balance_attempts < 2)
					{
						g_Interfaces.Engine->ClientCmd_Unrestricted("retry");
					}
					else
					{
						std::string autobalance_msg = "\"";

						g_Interfaces.Engine->ClientCmd_Unrestricted(
							"tf_party_chat \"I will be autobalanced in 3 seconds\"");
					}
					anti_balance_attempts++;
				}
				msgData.Seek(0);
			}
			break;
		}

	case VGUIMenu:
		{
			if (Vars::Visuals::RemoveMOTD.m_Var || Vars::Misc::AutoJoin.m_Var)
			{
				if (strcmp(reinterpret_cast<char*>(msgData.m_pData), "info") == 0)
				{
					g_Interfaces.Engine->ClientCmd_Unrestricted("closedwelcomemenu");
					return true;
				}
			}

			if(Vars::Misc::AutoJoin.m_Var)
			{
				if (strcmp(reinterpret_cast<char*>(msgData.m_pData), "team") == 0)
				{
					g_Interfaces.Engine->ClientCmd_Unrestricted("autoteam");
					return true;
				}

				if (strncmp(reinterpret_cast<char*>(msgData.m_pData), "class_", 6) == 0)
				{
					static std::string classNames[] = { "scout", "soldier", "pyro", "demoman", "heavyweapons", "engineer", "medic", "sniper", "spy" };
					g_Interfaces.Engine->ClientCmd_Unrestricted(std::string("join_class").append(" ").append(classNames[Vars::Misc::AutoJoin.m_Var - 1]).c_str());
					return true;
				}
			}

			break;
		}

	case VoteStart:
		{
			int team = msgData.ReadByte(), caller = msgData.ReadByte();
			char reason[64], vote_target[64];
			msgData.ReadString(reason, 64);
			msgData.ReadString(vote_target, 64);
			int target = static_cast<unsigned char>(msgData.ReadByte()) >> 1;

			PlayerInfo_t info_target{}, info_caller{};
			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				if (target && caller && g_Interfaces.Engine->GetPlayerInfo(target, &info_target) && g_Interfaces.Engine->GetPlayerInfo(caller, &info_caller))
				{
					bool bSameTeam = team == pLocal->GetTeamNum();

					auto bluntLine = tfm::format("%s %s called a vote on %s", bSameTeam ? "" : "(Enemy)", info_caller.name, info_target.name);
					auto verboseLine = tfm::format("%s %s [U:1:%s] called a vote on %s [U:1:%s]", bSameTeam ? "" : "(Enemy)", info_caller.name, info_caller.friendsID, info_target.name, info_target.friendsID);

					int votingOptions = Vars::Misc::VotingOptions.m_Var; bool verboseVoting = votingOptions & (1 << 5);
					
					const auto chosenLine = verboseVoting ?  verboseLine.c_str() : bluntLine.c_str();

					if (votingOptions & (1 << 0)) // text
					{ g_Notifications.Add(chosenLine); }
					if (votingOptions & (1<<1)) // console
					{ g_Interfaces.CVars->ConsoleColorPrintf({ 133, 255, 66, 255 }, tfm::format("%s \n", chosenLine).c_str()); }
					if (votingOptions & (1<<2)) // chat
					{ g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(pLocal->GetIndex(), chosenLine); }
					if (votingOptions & (1<<3)) // party
					{ g_Interfaces.Engine->ClientCmd_Unrestricted(tfm::format("tf_party_chat \"%s\"", chosenLine).c_str()); }
					if (votingOptions & (1 << 4) && bSameTeam && target != g_Interfaces.Engine->GetLocalPlayer()) // auto-vote
					{
						if (g_GlobalInfo.ignoredPlayers.find(info_target.friendsID) != g_GlobalInfo.ignoredPlayers.end() ||
							(target > 0 && target <= 128 && g_EntityCache.Friends[target])) {
							g_Interfaces.Engine->ClientCmd_Unrestricted("vote option2"); //f2 on ignored and steam friends
						}
						else {
							g_Interfaces.Engine->ClientCmd_Unrestricted("vote option1"); //f1 on everyone else
						}
					}
				}
			}
			msgData.Seek(0);
			break;
		}
	case ForcePlayerViewAngles:
	{
		return Vars::Visuals::PreventForcedAngles.m_Var ? true : Table.Original<fn>(index)(g_Interfaces.Client, type, msgData);
	}
	case SpawnFlyingBird:
	case PlayerGodRayEffect:
	case PlayerTauntSoundLoopStart:
	case PlayerTauntSoundLoopEnd:
	{
		return Vars::Visuals::RemoveTaunts.m_Var ? true : Table.Original<fn>(index)(g_Interfaces.Client, type, msgData);
	}
	case Shake:
	case Fade:
	case Rumble:
	{
		return Vars::Visuals::RemoveScreenEffects.m_Var ? true : Table.Original<fn>(index)(g_Interfaces.Client, type, msgData);
	}
	}

	return Table.Original<fn>(index)(g_Interfaces.Client, type, msgData);
}

void __fastcall ClientHook::DoPrecipitation::Hook(void* ecx, void* edx)
{
}

void __fastcall ClientHook::CHud__FindElement::Hook(void* ecx, void* edx, const char* String2)
{
}
