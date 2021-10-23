#include "PlayerList.h"
#include "../What/ImGui/imgui.h"
#include "../../SDK/SDK.h"

void CPlayerList::Render()
{
  std::vector<std::pair<int, CBaseEntity*>> players;
  if (ImGui::Begin("Playerlist", &g_PlayerList.showWindow, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
  {
    if (!g_Interfaces.Engine->IsInGame())
      ImGui::TextUnformatted("You're not in game, noob!");
    else if (g_Interfaces.Engine->IsInGame())
    {
      if (ImGui::BeginTable("playerlist", 2, ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersH))
      {
        ImGui::TableSetupColumn("Player");
        ImGui::TableSetupColumn("Actions");
        ImGui::TableHeadersRow();

        for (int i = 1; i <= g_Interfaces.Engine->GetMaxClients(); i++) // Get players
        {
          CBaseEntity* ent = g_Interfaces.EntityList->GetClientEntity(i);

          if (!ent || !ent->IsPlayer())
            continue;

          int teamNum = ent->GetTeamNum();
          players.push_back(std::make_pair(teamNum, ent));
        }

        std::sort(players.rbegin(), players.rend()); // Sort players by team number

        for (auto& player : players) // Print players 
        {
          PlayerInfo_t info;

          if (!g_Interfaces.Engine->GetPlayerInfo(player.second->GetIndex(), &info))
            continue;

          Color_t color = Utils::GetTeamColor(player.first, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
          bool ignored = (g_GlobalInfo.ignoredPlayers.find(info.friendsID) != g_GlobalInfo.ignoredPlayers.end());
          CBaseEntity* localPlayer = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer());

          ImGui::TableNextRow();

          for (int column = 0; column < 2; column++)
          {
            ImGui::TableSetColumnIndex(column);
            if (column == 0)
            {
              ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(color.r, color.g, color.b, color.a));
              ImGui::Text("%s", info.name);
              ImGui::PopStyleColor();
            }
            if (column == 1)
            {
              if (info.fakeplayer || player.second == localPlayer)
                break;

              ImGui::PushID(info.userID);

              if (ImGui::Button("Profile"))
                g_SteamInterfaces.Friends015->ActivateGameOverlayToUser("steamid", CSteamID((UINT64)(0x0110000100000000ULL + info.friendsID)));

              ImGui::SameLine();

              if (ImGui::Button("Kick"))
                g_Interfaces.Engine->ClientCmd_Unrestricted(tfm::format("callvote kick %i", info.userID).c_str());

              ImGui::SameLine();

              if (ImGui::Checkbox("Ignore", &ignored))
                if (g_GlobalInfo.ignoredPlayers.find(info.friendsID) == g_GlobalInfo.ignoredPlayers.end())
                  g_GlobalInfo.ignoredPlayers[info.friendsID] = true;
                else if (g_GlobalInfo.ignoredPlayers.find(info.friendsID) != g_GlobalInfo.ignoredPlayers.end())
                  g_GlobalInfo.ignoredPlayers.erase(info.friendsID);

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
