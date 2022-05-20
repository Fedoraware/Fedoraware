#include "Playerlist.h"
#include "../../Vars.h"
#include "../../Resolver/Resolver.h"
#include "../../Menu/Menu.h"
#include "../ImGui/imgui_color_gradient.h"
#include <mutex>
#include "../../PlayerResource/PlayerResource.h"
#include "../ImGui/imgui_internal.h"

struct ImGuiWindow;
const char* resolveListPitch[]{"None", "Up", "Down", "Zero", "Auto"};
const char* resolveListYaw[]{"None", "Forward", "Backward", "Left", "Right", "Invert", "Auto"};
const char* priorityModes[]{ "Friend", "Ignore", "Default", "Rage", "Cheater" };

std::mutex mutex;

void CPlayerList::UpdatePlayers()
{
	std::multimap<int, ListPlayer> listBuffer{};
	for (int i = 1; i < I::GlobalVars->maxclients; i++)
	{
		if (g_PR->isValid(i))
		{
			ListPlayer player{
				g_PR->GetPlayerName(i),
				g_PR->GetUserID(i),
				g_PR->GetAccountID(i),
				g_PR->GetPing(i) == 0,
				Utils::GetTeamColor(g_PR->GetTeam(i), Vars::ESP::Main::EnableTeamEnemyColors.m_Var),
				g_PR->GetHealth(i),
				g_PR->GetMaxHealth(i),
				g_PR->GetClass(i),
				g_PR->IsAlive(i)
			};

			listBuffer.emplace(g_PR->GetTeam(i), player);
		}
	}

	std::lock_guard lock(mutex);
	listBuffer.swap(PlayerCache);
}

void CPlayerList::Render()
{
	if (!Vars::Menu::ShowPlayerlist) { return; }

	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(200, 24));
	if (ImGui::Begin("Playerlist", &Vars::Menu::ShowPlayerlist,
	                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::PushFont(g_Menu.Verdana);
		const auto winSize = ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		auto winPos = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		ImGui::GradientRect(&g_Menu.TitleGradient, {winPos.x, winPos.y}, winSize.x, 3);
		ImGui::Dummy(ImVec2());

		// Check if we are in-game
		if (!I::Engine->IsInGame())
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
			const int columnCount = Vars::AntiHack::Resolver::Resolver.m_Var ? 5 : 4;
			if (ImGui::BeginTable("Playerlist", columnCount, ImGuiTableFlags_Borders))
			{
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Class");
				ImGui::TableSetupColumn("Health");
				ImGui::TableSetupColumn("Priority");
				if (Vars::AntiHack::Resolver::Resolver.m_Var)
				{
					ImGui::TableSetupColumn("Resolver");
				}
				ImGui::TableHeadersRow();

				std::lock_guard lock(mutex);

				for (const auto& [Team, Player] : PlayerCache)
				{
					ImGui::TableNextRow();

					ImGui::PushID(ImGui::GetCurrentTable()->CurrentRow);

					for (int column = 0; column < columnCount; column++)
					{
						ImGui::TableSetColumnIndex(column);

						switch (column)
						{
						case 0:
							{
								/* Name */
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(Player.Color.r, Player.Color.g, Player.Color.b, Player.Alive ? Player.Color.a : 127).Value);
								ImGui::Text("%s", Player.Name);
								ImGui::PopStyleColor();
								break;
							}

						case 1:
							{
								/* Class */
								ImGui::Text("%s", Utils::GetClassByIndex(Player.Class));
								break;
							}

						case 2:
							{
								/* Health */
								ImGui::Text("%i/%i", Player.Health, Player.MaxHealth);
								break;
							}

						case 3:
							{
								/* Priority */
								ImGui::PushItemWidth(70.f);
								ImGui::Combo("###Priority", &g_GlobalInfo.PlayerPriority[Player.FriendsID].Mode, priorityModes, IM_ARRAYSIZE(priorityModes));
								ImGui::PopItemWidth();
								break;
							}

						case 4:
							{
								/* Resolver */
								auto findResolveMode = g_Resolver.ResolvePlayers.find(Player.FriendsID);
								ResolveMode resolveMode;
								if (findResolveMode != g_Resolver.ResolvePlayers.end())
								{
									resolveMode = findResolveMode->second;
								}

								// Pitch resolver
								ImGui::PushItemWidth(90.f);
								if (ImGui::Combo("Pitch", &resolveMode.m_Pitch, resolveListPitch,
								                 IM_ARRAYSIZE(resolveListPitch)))
								{
									g_Resolver.ResolvePlayers[Player.FriendsID].m_Pitch = resolveMode.m_Pitch;
								}
								ImGui::PopItemWidth();
								ImGui::SameLine();

								// Yaw resolver
								ImGui::PushItemWidth(90.f);
								if (ImGui::Combo("Yaw", &resolveMode.m_Yaw, resolveListYaw,
								                 IM_ARRAYSIZE(resolveListYaw)))
								{
									g_Resolver.ResolvePlayers[Player.FriendsID].m_Yaw = resolveMode.m_Yaw;
								}
								ImGui::PopItemWidth();
								break;
							}

						default:
							{
								break;
							}
						}
					}

					ImGui::SameLine();
					ImGui::Selectable("##contextmenu", false, ImGuiSelectableFlags_SpanAllColumns);

					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5, 5 });
					if (!Player.FakePlayer && ImGui::BeginPopupContextItem())
					{
						if (ImGui::Button("Profile"))
						{
							g_SteamInterfaces.Friends015->ActivateGameOverlayToUser(
								"steamid", CSteamID(0x0110000100000000ULL + Player.FriendsID));
						}

						ImGui::SameLine();
						if (ImGui::Button("Votekick"))
						{
							I::Engine->ClientCmd_Unrestricted(tfm::format("callvote kick %i", Player.UserID).c_str());
						}

						ImGui::EndPopup();
					}

					ImGui::PopStyleVar();
					ImGui::PopID();
				}

				ImGui::EndTable();
			}
		}

		ImGui::PopFont();
		ImGui::End();
	}

	ImGui::PopStyleVar();
}
