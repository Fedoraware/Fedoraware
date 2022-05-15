#include "ClientHook.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "../../Features/Misc/Misc.h"
#include "../../Features/ChatInfo/ChatInfo.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AttributeChanger/AttributeChanger.h"
#include "../../Features/Resolver/Resolver.h"

const static std::string CLEAR_MSG("?\nServer:\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

const static std::vector<std::string> BAD_WORDS{_("cheat"), _("hack"), _("bot"), _("aim"), _("esp"), _("kick"), _("hax"), _("script")};

static std::string clr({'\x7', '0', 'D', '9', '2', 'F', 'F'});
static std::string yellow({'\x7', 'C', '8', 'A', '9', '0', '0'}); //C8A900
static std::string white({'\x7', 'F', 'F', 'F', 'F', 'F', 'F'}); //FFFFFF
static std::string green({'\x7', '3', 'A', 'F', 'F', '4', 'D'}); //3AFF4D

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


void __stdcall ClientHook::FrameStageNotify::Hook(EClientFrameStage frameStage)
{
	switch (frameStage)
	{
	case EClientFrameStage::FRAME_RENDER_START:
		{
			g_GlobalInfo.m_vPunchAngles = Vec3();

			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				// Handle freecam position
				if (g_GlobalInfo.m_bFreecamActive && Vars::Visuals::FreecamKey.m_Var && GetAsyncKeyState(Vars::Visuals::FreecamKey.m_Var) & 0x8000)
				{
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
	}

	Table.Original<fn>(index)(g_Interfaces.Client, frameStage);

	switch (frameStage)
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
			g_GlobalInfo.m_bLocalSpectated = false;

			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				for (const auto& teammate : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
				{
					if (teammate->IsAlive() || g_EntityCache.Friends[teammate->GetIndex()])
					{
						continue;
					}

					const CBaseEntity* pObservedPlayer = g_Interfaces.EntityList->GetClientEntityFromHandle(teammate->GetObserverTarget());

					if (pObservedPlayer == pLocal)
					{
						g_GlobalInfo.m_bLocalSpectated = true;
						break;
					}
				}
			}

			for (int i = 0; i < g_Interfaces.Engine->GetMaxClients(); i++)
			{
				if (const auto& player = g_Interfaces.EntityList->GetClientEntity(i))
				{
					const VelFixRecord record = {player->m_vecOrigin(), player->m_fFlags(), player->GetSimulationTime()};
					g_GlobalInfo.velFixRecord[player] = record;
				}
			}

			break;
		}

	case EClientFrameStage::FRAME_RENDER_START:
		{
			if (!g_GlobalInfo.unloadWndProcHook)
			{
				if (Vars::Visuals::Rain.m_Var > 0)
				{
					g_Visuals.rain.Run();
				}

				// genius method i swear
				static bool modded = false;
				if (Vars::Visuals::SkyModulation.m_Var || Vars::Visuals::WorldModulation.m_Var)
				{
					g_Visuals.ModulateWorld();
					modded = true;
				}
				else if (modded)
				{
					modded = false;
					g_Visuals.ModulateWorld();
				}
			}
			break;
		}
	}
}

static int anti_balance_attempts = 0;
static std::string previous_name;

bool __stdcall ClientHook::DispatchUserMessage::Hook(UserMessageType type, bf_read& msgData)
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
				if (g_Interfaces.Engine->GetPlayerInfo(entIdx, &senderInfo))
				{
					if (entIdx == g_Interfaces.Engine->GetLocalPlayer()) { break; }
					if (g_GlobalInfo.ignoredPlayers.find(senderInfo.friendsID) != g_GlobalInfo.ignoredPlayers.end()
						|| (entIdx > 0 && entIdx <= 128 && g_EntityCache.Friends[entIdx]))
					{
						break;
					}

					const std::vector<std::string> toReplace = {" ", "4", "3", "0", "6", "5", "7", "@", ".", ",", "-", "!"};
					const std::vector<std::string> replaceWith = {"", "a", "e", "o", "g", "s", "t", "a", "", "", "", "i"};

					for (std::vector<int>::size_type i = 0; i != toReplace.size(); i++)
					{
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

			break;
		}

	case TextMsg:
		{
			if (Vars::Misc::AntiAutobal.m_Var && msgData.GetNumBitsLeft() > 35)
			{
				const INetChannel* server = g_Interfaces.Engine->GetNetChannelInfo();
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
						g_Interfaces.Engine->ClientCmd_Unrestricted("retry");
					}
					else
					{
						g_Interfaces.Engine->ClientCmd_Unrestricted(
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
					g_Interfaces.Engine->ClientCmd_Unrestricted("closedwelcomemenu");
					return true;
				}
			}

			// Autojoin team / class
			if (Vars::Misc::AutoJoin.m_Var)
			{
				if (strcmp(reinterpret_cast<char*>(msgData.m_pData), "team") == 0)
				{
					g_Interfaces.Engine->ClientCmd_Unrestricted("autoteam");
					return true;
				}

				if (strncmp(reinterpret_cast<char*>(msgData.m_pData), "class_", 6) == 0)
				{
					static std::string classNames[] = {"scout", "soldier", "pyro", "demoman", "heavyweapons", "engineer", "medic", "sniper", "spy"};
					g_Interfaces.Engine->ClientCmd_Unrestricted(std::string("join_class").append(" ").append(classNames[Vars::Misc::AutoJoin.m_Var - 1]).c_str());
					return true;
				}
			}

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

	msgData.Seek(0);
	return Table.Original<fn>(index)(g_Interfaces.Client, type, msgData);
}

void __fastcall ClientHook::DoPrecipitation::Hook(void* ecx, void* edx) {}

void __fastcall ClientHook::CHud__FindElement::Hook(void* ecx, void* edx, const char* String2) {}
