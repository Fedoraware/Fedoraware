#include "ClientHook.h"

#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AttributeChanger/AttributeChanger.h"
#include "../../Features/Resolver/Resolver.h"
#include "../../Features/Menu/Playerlist/Playerlist.h"

const static std::string clear("?\nServer:\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

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
static std::string previous_name = "";

bool __stdcall ClientHook::DispatchUserMessage::Hook(int type, bf_read& msg_data)
{
	auto buf_data = reinterpret_cast<const char*>(msg_data.m_pData);

	switch (type)
	{
	case 4:
		{
			// Received chat message
			int nbl = msg_data.GetNumBytesLeft();
			if (nbl >= 256)
			{
				break;
			}

			std::string data;
			for (int i = 0; i < nbl; i++)
			{
				data.push_back(buf_data[i]);
			}

			const char* p = data.c_str() + 2;
			std::string event(p), name((p += event.size() + 1)), message(p + name.size() + 1);
			int ent_idx = data[0];

			if (Vars::Misc::ChatCensor.m_Var)
			{
				std::vector<std::string> badWords{"cheat", "hack", "bot", "aim", "esp", "kick", "hax"};
				bool bwFound = false;
				for (std::string word : badWords)
				{
					if (strstr(message.c_str(), word.c_str()))
					{
						bwFound = true;
						break;
					}
				}

				if (bwFound)
				{
					std::string cmd = "say \"" + clear + "\"";
					g_Interfaces.Engine->ServerCmd(cmd.c_str(), true);
					g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(
						0, tfm::format("%s[FeD] \x3 %s\x1 wrote\x3 %s", clr, name, message).c_str());
				}
			}
			msg_data.Seek(0);
			break;
		}
	case 5:
		{
			if (Vars::Misc::AntiAutobal.m_Var && msg_data.GetNumBitsLeft() > 35)
			{
				INetChannel* server = g_Interfaces.Engine->GetNetChannelInfo();

				std::string data(buf_data);

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
				msg_data.Seek(0);
			}
			break;
		}
	case 46:
		{
			int team = msg_data.ReadByte(), caller = msg_data.ReadByte();
			char reason[64], vote_target[64];
			msg_data.ReadString(reason, 64);
			msg_data.ReadString(vote_target, 64);
			int target = static_cast<unsigned char>(msg_data.ReadByte()) >> 1;

			PlayerInfo_t info_target{}, info_caller{};
			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				if (target > 0 && g_Interfaces.Engine->GetPlayerInfo(target, &info_target) && caller > 0 && g_Interfaces.Engine->GetPlayerInfo(caller, &info_caller))
				{
					bool bSameTeam = team == pLocal->GetTeamNum();

					if (Vars::Misc::AnnounceVotesConsole.m_Var)
					{
						if (Vars::Misc::AnnounceVotes.m_Var == 0) {
							g_Interfaces.CVars->ConsoleColorPrintf({ 133, 255, 66, 255 }, tfm::format("%s %s called a vote on %s", bSameTeam ? "" : "(Enemy)", info_caller.name, info_target.name).c_str());
						}
						else {
							g_Interfaces.CVars->ConsoleColorPrintf({ 133, 255, 66, 255 }, tfm::format("%s %s [U:1:%s] called a vote on %s [U:1:%s]", bSameTeam ? "" : "(Enemy)", info_caller.name, info_caller.friendsID, info_target.name, info_target.friendsID).c_str());
						}
					}
					if (Vars::Misc::AnnounceVotesText.m_Var)
					{
						if (Vars::Misc::AnnounceVotes.m_Var == 0) {
							g_Notifications.Add(tfm::format("%s %s called a vote on %s", bSameTeam ? "" : "(Enemy)", info_caller.name, info_target.name));
						}
						else {
							g_Notifications.Add(tfm::format("%s %s [U:1:%s] called a vote on %s [U:1:%s]", bSameTeam ? "" : "(Enemy)", info_caller.name, info_caller.friendsID, info_target.name, info_target.friendsID));
						}
					}
					if (Vars::Misc::AnnounceVotesChat.m_Var)
					{
						if (Vars::Misc::AnnounceVotes.m_Var == 0) {
							g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(pLocal->GetIndex(), tfm::format("%s[FeD] \x3%s %s %s %scalled a vote on %s%s", clr, green, bSameTeam ? "" : "(Enemy)", info_caller.name, white, green, info_target.name).c_str());
						}
						else {
							g_Interfaces.ClientMode->m_pChatElement->ChatPrintf(pLocal->GetIndex(), tfm::format("%s[FeD] \x3%s %s %s %s[U:1:%s] %scalled a vote on %s%s %s[U:1:%s]", clr, green, bSameTeam ? "" : "(Enemy)", info_caller.name, yellow, info_caller.friendsID, white, green, info_target.name, yellow, info_target.friendsID).c_str());
						}
					}
					if (Vars::Misc::AnnounceVotesParty.m_Var)
					{
						if (Vars::Misc::AnnounceVotes.m_Var == 0) {
							g_Interfaces.Engine->ClientCmd_Unrestricted(
								tfm::format("say_party \"%s %s called a vote on %s\"", bSameTeam ? "" : "(Enemy)", info_caller.name, info_target.name).c_str());
						}
						else {
							g_Interfaces.Engine->ClientCmd_Unrestricted(
								tfm::format("say_party \"%s %s [U:1:%s] called a vote on %s [U:1:%s]\"", bSameTeam ? "" : "(Enemy)", info_caller.name, info_caller.friendsID, info_target.name, info_target.friendsID).c_str());
						}
					}

					if (Vars::Misc::AutoVote.m_Var && bSameTeam && target != g_Interfaces.Engine->GetLocalPlayer())
					{
						if (g_GlobalInfo.ignoredPlayers.find(info_target.friendsID) != g_GlobalInfo.ignoredPlayers.end() ||
							(target > 0 && target <= 129 && g_EntityCache.Friends[target])) {
							g_Interfaces.Engine->ClientCmd_Unrestricted("vote option2"); //f2 on ignored and steam friends
						}
						else {
							g_Interfaces.Engine->ClientCmd_Unrestricted("vote option1"); //f1 on everyone else
						}
					}
				}
			}
			msg_data.Seek(0);
			break;
		}
	}

	return Table.Original<fn>(index)(g_Interfaces.Client, type, msg_data);
}

void __fastcall ClientHook::DoPrecipitation::Hook(void* ecx, void* edx)
{
}

void __fastcall ClientHook::CHud__FindElement::Hook(void* ecx, void* edx, const char* String2)
{
}
