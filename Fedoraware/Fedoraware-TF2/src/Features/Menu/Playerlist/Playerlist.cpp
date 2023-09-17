#include "Playerlist.h"
#include "../../Vars.h"
#include "../../Resolver/Resolver.h"
#include "../../Menu/Menu.h"
#include "../ConfigManager/ConfigManager.h"

#include <ImGui/imgui_color_gradient.h>
#include <ImGui/imgui_internal.h>

#include <mutex>
#include <ranges>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

const char* resolveListPitch[]{"None", "Up", "Down", "Zero", "Auto"};
const char* resolveListYaw[]{"None", "Forward", "Backward", "Left", "Right", "Invert", "Edge", "Auto"};
const char* priorityModes[]{"Friend", "Ignore", "Default", "Rage", "Cheater"};

struct ListPlayer {
	const char* Name{};
	int UserID{};
	uint32_t FriendsID{};
	Color_t Color{};
	int Health{};
	int MaxHealth{};
	int Class{};
	bool Alive{};
	bool FakePlayer{};
};

std::mutex mutex;
std::multimap<int, ListPlayer> PlayerCache{};

void CPlayerList::UpdatePlayers()
{
	static Timer updateTimer{};
	if (!updateTimer.Run(1000)) { return; }

	// Get info for every player
	std::multimap<int, ListPlayer> listBuffer{};
	for (int i = 1; i <= I::GlobalVars->maxclients; i++)
	{
		const auto& pr = g_EntityCache.GetPR();
		if (!pr || !pr->GetValid(i) || !pr->GetConnected(i)) { continue; }

		PlayerInfo_t info{};
		if (!I::EngineClient->GetPlayerInfo(i, &info)) { continue; }

		ListPlayer player{
			.Name = pr->GetPlayerName(i),
			.UserID = pr->GetUserID(i),
			.FriendsID = pr->GetAccountID(i),
			.Color = Utils::GetTeamColor(pr->GetTeam(i), Vars::ESP::Main::EnableTeamEnemyColors.Value),
			.Health = pr->GetHealth(i),
			.MaxHealth = pr->GetMaxHealth(i),
			.Class = pr->GetClass(i),
			.Alive = pr->IsAlive(i),
			.FakePlayer = info.fakeplayer
		};

		listBuffer.emplace(pr->GetTeam(i), player);
	}

	// Swap the buffers
	std::lock_guard lock(mutex);
	listBuffer.swap(PlayerCache);
}

void CPlayerList::Run()
{
	// Save every 10 seconds, if needed
	static Timer saveTimer{};
	if (saveTimer.Run(10000))
	{
		if (ShouldSave)
		{
			Save();
			ShouldSave = false;
		}

		if (ShouldLoad)
		{
			Load();
			ShouldLoad = false;
		}
	}
}

void CPlayerList::Render()
{
	if (!Vars::Menu::ShowPlayerlist) { return; }

	// Draw the playerlist
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(200, 24));
	if (ImGui::Begin("Playerlist", &Vars::Menu::ShowPlayerlist,
	                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::PushFont(F::Menu.Verdana);
		const auto winSize = ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		auto winPos = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		ImGui::GradientRect(&F::Menu.TitleGradient, {winPos.x, winPos.y}, winSize.x, 3);
		ImGui::Dummy(ImVec2());

		// Check if we are in-game
		if (!I::EngineClient->IsInGame())
		{
			ImGui::Dummy(ImVec2(0, 20));
			static ImVec2 fontSize = ImGui::CalcTextSize("You're not in game, noob!");
			ImGui::SameLine(
				ImGui::GetWindowSize().x / 2 -
				fontSize.x + (fontSize.x / 2)
			);
			ImGui::Text("You're not in game, noob!");
		}
		else
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {5, 5});
			const int columnCount = Vars::AntiHack::Resolver::Resolver.Value ? 6 : 4;
			if (ImGui::BeginTable("Playerlist", columnCount,
			                      ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
			{
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Class");
				ImGui::TableSetupColumn("Health");
				ImGui::TableSetupColumn("Priority");
				if (Vars::AntiHack::Resolver::Resolver.Value)
				{
					ImGui::TableSetupColumn("Pitch");
					ImGui::TableSetupColumn("Yaw");
				}
				ImGui::TableHeadersRow();

				std::lock_guard lock(mutex);

				for (const auto& player : PlayerCache | std::views::values)
				{
					ImGui::TableNextRow();

					ImGui::PushID(ImGui::GetCurrentTable()->CurrentRow);

					for (int column = 0; column < columnCount; column++)
					{
						ImGui::TableSetColumnIndex(column);

						//// don't show priority or resolver for bots
						//if (Player.FakePlayer && column > 2)
						//{
						//	continue;
						//}

						switch (column)
						{
						case 0:
							{
								/* Name */
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(player.Color.r, player.Color.g, player.Color.b, player.Alive ? player.Color.a : 127).Value);
								ImGui::Text("%s", player.Name ? player.Name : "N/A");
								ImGui::PopStyleColor();
								break;
							}

						case 1:
							{
								/* Class */
								ImGui::Text("%s", Utils::GetClassByIndex(player.Class));
								break;
							}

						case 2:
							{
								/* Health */
								ImGui::Text("%i/%i", player.Health, player.MaxHealth);
								break;
							}

						case 3:
							{
								/* Priority */
								if (ImGui::Selectable(priorityModes[G::PlayerPriority[player.FriendsID].Mode]))
								{
									ImGui::OpenPopup("priority_popup");
								}

								if (ImGui::BeginPopup("priority_popup"))
								{
									for (int i = 0; i < IM_ARRAYSIZE(priorityModes); i++)
									{
										if (ImGui::MenuItem(priorityModes[i]))
										{
											G::PlayerPriority[player.FriendsID].Mode = i;
											ShouldSave = true;
										}
									}

									ImGui::EndPopup();
								}
								break;
							}

						case 4:
							{
								// Pitch resolver
								if (ImGui::Selectable(resolveListPitch[F::Resolver.mResolverMode[player.FriendsID].first]))
								{
									ImGui::OpenPopup("pitch_popup");
								}

								if (ImGui::BeginPopup("pitch_popup"))
								{
									for (int i = 0; i < IM_ARRAYSIZE(resolveListPitch); i++)
									{
										if (ImGui::MenuItem(resolveListPitch[i]))
										{
											F::Resolver.mResolverMode[player.FriendsID].first = i;
										}
									}

									ImGui::EndPopup();
								}
								break;
							}

						case 5:
							{
								// Yaw resolver
								ImGui::PushID(1); // in case of m_Yaw = m_Pitch
								if (ImGui::Selectable(resolveListYaw[F::Resolver.mResolverMode[player.FriendsID].second]))
								{
									ImGui::OpenPopup("yaw_popup");
								}

								if (ImGui::BeginPopup("yaw_popup"))
								{
									for (int i = 0; i < IM_ARRAYSIZE(resolveListYaw); i++)
									{
										if (ImGui::MenuItem(resolveListYaw[i]))
										{
											F::Resolver.mResolverMode[player.FriendsID].second = i;
										}
									}

									ImGui::EndPopup();
								}
								ImGui::PopID();

								break;
							}
						}
					}

					ImGui::SameLine();
					ImGui::Selectable("##contextmenu", false, ImGuiSelectableFlags_SpanAllColumns);

					if (!player.FakePlayer && ImGui::BeginPopupContextItem())
					{
						if (ImGui::Selectable("Profile"))
						{
							g_SteamInterfaces.Friends->ActivateGameOverlayToUser(
								"steamid", CSteamID(0x0110000100000000ULL + player.FriendsID));
						}

						if (ImGui::Selectable("Votekick"))
						{
							I::EngineClient->ClientCmd_Unrestricted(std::format("callvote kick {:d}", player.UserID).c_str());
						}

						ImGui::EndPopup();
					}

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		ImGui::PopFont();
		ImGui::End();
	}
	ImGui::PopStyleVar();
}

void CPlayerList::Save()
{
	try
	{
		boost::property_tree::ptree writeTree;

		// Put map entries into ptree
		for (const auto& [user, prio] : G::PlayerPriority)
		{
			boost::property_tree::ptree userTree;
			userTree.put("Mode", prio.Mode);

			writeTree.put_child(std::to_string(user), userTree);
		}

		// Save the file
		write_json(g_CFG.GetConfigPath() + "\\Core\\Playerlist.json", writeTree);
	}
	catch (...) {}
}

void CPlayerList::Load()
{
	try
	{
		boost::property_tree::ptree readTree;
		read_json(g_CFG.GetConfigPath() + "\\Core\\Playerlist.json", readTree);
		G::PlayerPriority.clear();

		for (auto& it : readTree)
		{
			Priority prio;
			uint32_t userID = std::stoi(it.first);

			if (auto getValue = it.second.get_optional<int>("Mode"))
			{
				prio.Mode = std::max(*getValue, 0);
			}

			G::PlayerPriority[userID] = prio;
		}
	}
	catch (...) {}
}
