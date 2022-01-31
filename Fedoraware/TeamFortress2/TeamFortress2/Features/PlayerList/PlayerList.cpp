#include "PlayerList.h"
#include "../Menu/ImGui/imgui.h"
#include "../../SDK/SDK.h"
#include <algorithm>

const char* resolveListPitch[]{ "None", "Up", "Down", "Zero", "Auto" };
const char* resolveListYaw[]{ "None", "Forward", "Backward", "Left", "Right", "Invert" };

bool sortByTeam(const plistPlayer& a, const plistPlayer& b)
{
	return a.teamNum < b.teamNum;
}

bool findPlayer(int id, int teamNum)
{
	for (const auto& player : g_PlayerList.players)
		if (player.info.userID == id && player.teamNum == teamNum)
			return true;
	return false;
}

void CPlayerList::GetPlayers()
{
	if (g_Interfaces.Engine->IsInGame() && !g_Interfaces.Engine->IsDrawingLoadingImage() && g_Interfaces.Engine->IsConnected())
	{
		CBaseEntity* local = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer());
		for (int i = 1; i <= g_Interfaces.Engine->GetMaxClients(); i++)
		{
			CBaseEntity* ent = g_Interfaces.EntityList->GetClientEntity(i);

			if (!ent || !ent->IsPlayer() || ent == g_EntityCache.m_pLocal)
			{
				if (!g_PlayerList.players.empty())
				{
					for (auto it = g_PlayerList.players.begin(); it != g_PlayerList.players.end(); it++)
					{
						if (it->index == i)
						{
							it = g_PlayerList.players.erase(it);
							break;
						}
					}
				}
				continue;
			}

			plistPlayer player;

			if (g_Interfaces.Engine->GetPlayerInfo(i, &player.info))
			{
				int teamNum = ent->GetTeamNum();
				player.teamNum = teamNum;
				player.color = Utils::GetTeamColor(teamNum, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
				player.index = i;

				if (g_PlayerList.players.empty())
					players.push_back(player);
				else if (!g_PlayerList.players.empty() && !findPlayer(player.info.userID, teamNum))
				{
					for (auto i = g_PlayerList.players.begin(); i != g_PlayerList.players.end(); i++)
					{
						if (i->info.userID == player.info.userID)
						{
							i = g_PlayerList.players.erase(i);
							break;
						}
					}
					players.push_back(player);
				}
			}
		}
		if (!g_PlayerList.players.empty())
			std::sort(players.begin(), players.end(), sortByTeam);
	}
}

void CPlayerList::Render()
{
	if (ImGui::Begin("Playerlist", &g_PlayerList.showWindow, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		if (!g_Interfaces.Engine->IsInGame())
			ImGui::TextUnformatted("You're not in game, noob!");
		else if (g_Interfaces.Engine->IsInGame() && g_PlayerList.players.empty())
			ImGui::TextUnformatted("You're all alone, again.");
		else if (g_Interfaces.Engine->IsInGame() && !g_PlayerList.players.empty())
		{
			if (ImGui::BeginTable("playerlist", 2, ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersH))
			{
				ImGui::TableSetupColumn("Player");
				ImGui::TableSetupColumn("Actions");
				ImGui::TableHeadersRow();

				for (const auto& player : players) // Print players
				{
					if (!player.info.userID)
						continue;

					bool ignored = (g_GlobalInfo.ignoredPlayers.find(player.info.friendsID) != g_GlobalInfo.ignoredPlayers.end());
					auto findResolveMode = g_GlobalInfo.resolvePlayers.find(player.info.friendsID);
					ResolveMode resolveMode;
					if (findResolveMode != g_GlobalInfo.resolvePlayers.end()) {
						resolveMode = findResolveMode->second;
					}

					ImGui::TableNextRow();

					for (int column = 0; column < 2; column++)
					{
						ImGui::TableSetColumnIndex(column);
						if (column == 0)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(player.color.r, player.color.g, player.color.b, player.color.a));
							ImGui::Text("%s", player.info.name);
							ImGui::PopStyleColor();
						}
						if (column == 1)
						{
							if (player.info.fakeplayer)
								break;

							ImGui::PushID(player.info.userID);

							if (ImGui::Button("Profile"))
								g_SteamInterfaces.Friends015->ActivateGameOverlayToUser("steamid", CSteamID((UINT64)(0x0110000100000000ULL + player.info.friendsID)));

							ImGui::SameLine();

							if (ImGui::Button("Kick"))
								g_Interfaces.Engine->ClientCmd_Unrestricted(tfm::format("callvote kick %i", player.info.userID).c_str());

							ImGui::SameLine();

							if (Vars::AntiHack::Resolver::Resolver.m_Var) {
								// Pitch resolver
								ImGui::PushItemWidth(90.f);
								if (ImGui::Combo("##pitch", &resolveMode.m_Pitch, resolveListPitch, IM_ARRAYSIZE(resolveListPitch))) {
									g_GlobalInfo.resolvePlayers[player.info.friendsID].m_Pitch = resolveMode.m_Pitch;
								}
								ImGui::PopItemWidth();
								ImGui::SameLine();

								// Yaw resolver
								ImGui::PushItemWidth(90.f);
								if (ImGui::Combo("##yaw", &resolveMode.m_Yaw, resolveListYaw, IM_ARRAYSIZE(resolveListYaw))) {
									g_GlobalInfo.resolvePlayers[player.info.friendsID].m_Yaw = resolveMode.m_Yaw;
								}
								ImGui::PopItemWidth();
								ImGui::SameLine();
							}

							if (ImGui::Checkbox("Ignore", &ignored))
								if (g_GlobalInfo.ignoredPlayers.find(player.info.friendsID) == g_GlobalInfo.ignoredPlayers.end())
									g_GlobalInfo.ignoredPlayers[player.info.friendsID] = true;
								else if (g_GlobalInfo.ignoredPlayers.find(player.info.friendsID) != g_GlobalInfo.ignoredPlayers.end())
									g_GlobalInfo.ignoredPlayers.erase(player.info.friendsID);

							ImGui::PopID();
						}
					}
				}
				ImGui::EndTable();
			}
		}
		ImGui::End();
	}
}
