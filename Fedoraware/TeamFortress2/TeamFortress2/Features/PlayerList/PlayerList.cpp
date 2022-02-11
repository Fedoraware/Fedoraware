#include "PlayerList.h"
#include "../Menu/ImGui/imgui.h"
#include "../../SDK/SDK.h"
#include <algorithm>
#include "../Menu/ImGui/imgui_color_gradient.h"
#include "../Menu/ImGui/imgui_internal.h"
#include "../Menu/Menu.h"

const char* resolveListPitch[]{"None", "Up", "Down", "Zero", "Auto"};
const char* resolveListYaw[]{"None", "Forward", "Backward", "Left", "Right", "Invert"};

bool sortByTeam(const plistPlayer& a, const plistPlayer& b)
{
	return a.teamNum < b.teamNum;
}

bool findPlayer(int id)
{
	for (const auto& player : g_PlayerList.players)
	{
		if (player.index == id)
		{
			return true;
		}
	}
	return false;
}

constexpr plistPlayer nullPlayer{
	{},
	{0, 0, 0, 0},
	-1,
	-1
};

void CPlayerList::GetPlayers()
{
	if (g_Interfaces.Engine->IsInGame() && !g_Interfaces.Engine->IsDrawingLoadingImage() && g_Interfaces.Engine->
		IsConnected())
	{
		for (size_t i = 1; i < players.size(); i++)
		{
			CBaseEntity* ent = g_Interfaces.EntityList->GetClientEntity(i);

			if (!ent || !ent->IsPlayer() || ent == g_EntityCache.m_pLocal)
			{
				players[i] = nullPlayer;
				continue;
			}

			plistPlayer player;

			if (g_Interfaces.Engine->GetPlayerInfo(i, &player.info))
			{
				const int teamNum = ent->GetTeamNum();
				player.teamNum = teamNum;
				player.color = Utils::GetTeamColor(teamNum, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
				player.index = i;

				players[i] = player;
			}
			else
			{
				players[i] = nullPlayer;
			}
		}
		if (!g_PlayerList.players.empty())
		{
			std::sort(players.begin(), players.end(), sortByTeam);
		}
	}
}

void CPlayerList::Render()
{
	auto accent = ImColor(Color::TOFLOAT(Vars::Menu::Colors::MenuAccent.r),
	                      Color::TOFLOAT(Vars::Menu::Colors::MenuAccent.g),
	                      Color::TOFLOAT(Vars::Menu::Colors::MenuAccent.b));
	static ImGradient titlegradient;
	{
		ImColor titlebg = ImGui::GetStyle().Colors[ImGuiCol_TitleBg];
		titlegradient.m_marks.clear();
		titlegradient.addMark(0.0f, titlebg);
		titlegradient.addMark(0.32f, titlebg);
		titlegradient.addMark(0.5f, accent);
		titlegradient.addMark(0.68f, titlebg);
		titlegradient.addMark(0.9999998f, titlebg); // literally why
	}

	ImGui::PushFont(g_Menu.VerdanaBold);
	ImGui::GetStyle().WindowMinSize = ImVec2(200, 24);
	ImGui::PushStyleColor(ImGuiCol_Text, accent.Value);
	if (ImGui::Begin("Playerlist", &g_PlayerList.showWindow,
	                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::PushFont(g_Menu.VerdanaNormal);
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		auto winSize = ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		auto winPos = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		const auto foregroundDrawList = ImGui::GetForegroundDrawList();
		ImGui::GradientRect(foregroundDrawList, &titlegradient, {winPos.x, winPos.y}, winSize.x, 3);
		ImGui::Dummy(ImVec2(0, 7));
		if (!g_Interfaces.Engine->IsInGame())
		{
			ImGui::Dummy(ImVec2(0, 20));
			static ImVec2 fontSize = ImGui::CalcTextSize("You're not in game, noob!");
			ImGui::SameLine(
				ImGui::GetWindowSize().x / 2 -
				fontSize.x + (fontSize.x / 2)
			);
			ImGui::Text("You're not in game, noob!");
		}
		else if (g_Interfaces.Engine->IsInGame() && !g_PlayerList.players.empty())
		{
			if (ImGui::BeginTable("playerlist", 2,
			                      ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter
			                      | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersH))
			{
				ImGui::TableSetupColumn("Player");
				ImGui::TableSetupColumn("Actions");
				//ImGui::TableHeadersRow();

				for (const auto& player : players) // Print players
				{
					if (player.index == -1)
						continue;

					bool ignored = (g_GlobalInfo.ignoredPlayers.find(player.info.friendsID) != g_GlobalInfo.
						ignoredPlayers.end());
					auto findResolveMode = g_GlobalInfo.resolvePlayers.find(player.info.friendsID);
					ResolveMode resolveMode;
					if (findResolveMode != g_GlobalInfo.resolvePlayers.end())
					{
						resolveMode = findResolveMode->second;
					}

					ImGui::TableNextRow();

					for (int column = 0; column < 2; column++)
					{
						ImGui::TableSetColumnIndex(column);
						if (column == 0)
						{
							ImGui::PushStyleColor(ImGuiCol_Text,
							                      IM_COL32(player.color.r, player.color.g, player.color.b,
							                               player.color.a));
							ImGui::Text("%s", player.info.name);
							ImGui::PopStyleColor();
						}
						if (column == 1)
						{
							if (player.info.fakeplayer)
								break;

							ImGui::PushID(player.info.userID);

							if (ImGui::Button("Profile"))
								g_SteamInterfaces.Friends015->ActivateGameOverlayToUser(
									"steamid", CSteamID(0x0110000100000000ULL + player.info.friendsID));

							ImGui::SameLine();

							if (ImGui::Button("Kick"))
								g_Interfaces.Engine->ClientCmd_Unrestricted(
									tfm::format("callvote kick %i", player.info.userID).c_str());

							ImGui::SameLine();

							if (Vars::AntiHack::Resolver::Resolver.m_Var)
							{
								// Pitch resolver
								ImGui::PushItemWidth(90.f);
								if (ImGui::Combo("##pitch", &resolveMode.m_Pitch, resolveListPitch,
								                 IM_ARRAYSIZE(resolveListPitch)))
								{
									g_GlobalInfo.resolvePlayers[player.info.friendsID].m_Pitch = resolveMode.m_Pitch;
								}
								ImGui::PopItemWidth();
								ImGui::SameLine();

								// Yaw resolver
								ImGui::PushItemWidth(90.f);
								if (ImGui::Combo("##yaw", &resolveMode.m_Yaw, resolveListYaw,
								                 IM_ARRAYSIZE(resolveListYaw)))
								{
									g_GlobalInfo.resolvePlayers[player.info.friendsID].m_Yaw = resolveMode.m_Yaw;
								}
								ImGui::PopItemWidth();
								ImGui::SameLine();
							}

							if (ImGui::Checkbox("Ignore", &ignored))
								if (g_GlobalInfo.ignoredPlayers.find(player.info.friendsID) == g_GlobalInfo.
									ignoredPlayers.end())
									g_GlobalInfo.ignoredPlayers[player.info.friendsID] = true;
								else if (g_GlobalInfo.ignoredPlayers.find(player.info.friendsID) != g_GlobalInfo.
									ignoredPlayers.end())
									g_GlobalInfo.ignoredPlayers.erase(player.info.friendsID);

							ImGui::PopID();
						}
					}
				}
				ImGui::EndTable();
			}
		}
		ImGui::PopFont();
		ImGui::End();
	}
}
