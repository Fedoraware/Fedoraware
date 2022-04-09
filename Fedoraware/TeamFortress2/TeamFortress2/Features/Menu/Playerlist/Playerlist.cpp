#include "Playerlist.h"
#include "../../Vars.h"
#include "../../Resolver/Resolver.h"
#include "../../Menu/Menu.h"
#include "../ImGui/imgui_color_gradient.h"

struct ImGuiWindow;
const char* resolveListPitch[]{ "None", "Up", "Down", "Zero", "Auto" };
const char* resolveListYaw[]{ "None", "Forward", "Backward", "Left", "Right", "Invert", "Auto" };

bool CPlayerList::SortByTeam(const ListPlayer& a, const ListPlayer& b)
{
	return a.Team < b.Team;
}

void CPlayerList::UpdatePlayers()
{
	// TODO: Improve this
	if (g_Interfaces.Engine->IsInGame() && !g_Interfaces.Engine->IsDrawingLoadingImage() && g_Interfaces.Engine->IsConnected())
	{
		for (int i = 1; i < PlayerCache.size(); i++)
		{
			CBaseEntity* ent = g_Interfaces.EntityList->GetClientEntity(i);

			if (!ent || !ent->IsPlayer() || ent == g_EntityCache.m_pLocal)
			{
				PlayerCache[i] = ListPlayer{ };
				continue;
			}

			ListPlayer player;
			if (g_Interfaces.Engine->GetPlayerInfo(i, &player.Info))
			{
				const int teamNum = ent->GetTeamNum();
				player.Team = teamNum;
				player.Color = Utils::GetTeamColor(teamNum, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
				player.Index = i;

				PlayerCache[i] = player;
			}
			else
			{
				PlayerCache[i] = ListPlayer{ };
			}
		}

		if (!PlayerCache.empty())
		{
			std::sort(PlayerCache.begin(), PlayerCache.end(), [](ListPlayer a, ListPlayer b) { return a.Team < b.Team; });
		}
	}
}

void CPlayerList::Render()
{
	if (!Vars::Menu::ShowPlayerlist) { return; }

	const auto accent = ImColor(Color::TOFLOAT(Vars::Menu::Colors::MenuAccent.r),
	                            Color::TOFLOAT(Vars::Menu::Colors::MenuAccent.g),
	                            Color::TOFLOAT(Vars::Menu::Colors::MenuAccent.b));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(200, 24));
	if (ImGui::Begin("Playerlist", &Vars::Menu::ShowPlayerlist,
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::PushFont(g_Menu.Verdana);
		const auto winSize = ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		auto winPos = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		const auto foregroundDrawList = ImGui::GetForegroundDrawList();
		ImGui::GradientRect(&g_Menu.TitleGradient, { winPos.x, winPos.y }, winSize.x, 3);
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

		else if (g_Interfaces.Engine->IsInGame() && !g_PlayerList.PlayerCache.empty())
		{
			if (ImGui::BeginTable("playerlist", 2,
				ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter
				| ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersH))
			{
				ImGui::TableSetupColumn("Player");
				ImGui::TableSetupColumn("Actions");

				for (const auto& player : PlayerCache) // Print players
				{
					if (player.Index == -1)
					{
						continue;
					}

					bool ignored = (g_GlobalInfo.ignoredPlayers.find(player.Info.friendsID) != g_GlobalInfo.ignoredPlayers.end());
					auto findResolveMode = g_Resolver.ResolvePlayers.find(player.Info.friendsID);
					ResolveMode resolveMode;
					if (findResolveMode != g_Resolver.ResolvePlayers.end())
					{
						resolveMode = findResolveMode->second;
					}

					ImGui::TableNextRow();

					for (int column = 0; column < 2; column++)
					{
						ImGui::TableSetColumnIndex(column);
						if (column == 0)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, ImColor(player.Color.r, player.Color.g, player.Color.b, player.Color.a).Value);
							ImGui::Text("%s", player.Info.name);
							ImGui::PopStyleColor();
						}
						if (column == 1)
						{
							if (player.Info.fakeplayer)
							{
								break;
							}

							ImGui::PushID(player.Info.userID);

							if (ImGui::Button("Profile"))
							{
								g_SteamInterfaces.Friends015->ActivateGameOverlayToUser(
									"steamid", CSteamID(0x0110000100000000ULL + player.Info.friendsID));
							}

							ImGui::SameLine();

							if (ImGui::Button("Kick"))
							{
								g_Interfaces.Engine->ClientCmd_Unrestricted(
									tfm::format("callvote kick %i", player.Info.userID).c_str());
							}

							ImGui::SameLine();

							if (Vars::AntiHack::Resolver::Resolver.m_Var)
							{
								// Pitch resolver
								ImGui::PushItemWidth(90.f);
								if (ImGui::Combo("##pitch", &resolveMode.m_Pitch, resolveListPitch,
									IM_ARRAYSIZE(resolveListPitch)))
								{
									g_Resolver.ResolvePlayers[player.Info.friendsID].m_Pitch = resolveMode.m_Pitch;
								}
								ImGui::PopItemWidth();
								ImGui::SameLine();

								// Yaw resolver
								ImGui::PushItemWidth(90.f);
								if (ImGui::Combo("##yaw", &resolveMode.m_Yaw, resolveListYaw,
									IM_ARRAYSIZE(resolveListYaw)))
								{
									g_Resolver.ResolvePlayers[player.Info.friendsID].m_Yaw = resolveMode.m_Yaw;
								}
								ImGui::PopItemWidth();
								ImGui::SameLine();
							}

							if (ImGui::Checkbox("Ignore", &ignored))
							{
								if (g_GlobalInfo.ignoredPlayers.find(player.Info.friendsID) == g_GlobalInfo.
									ignoredPlayers.end())
								{
									g_GlobalInfo.ignoredPlayers[player.Info.friendsID] = true;
								}
								else
								{
									g_GlobalInfo.ignoredPlayers.erase(player.Info.friendsID);
								}
							}

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

	ImGui::PopStyleVar();
}