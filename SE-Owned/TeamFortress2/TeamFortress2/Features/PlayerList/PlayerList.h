#pragma once
#include "../../SDK/SDK.h"
#include "../Menu/Menu.h"
#include <fstream>
#include <filesystem>

// base class for any custom windows.
class Window {
public:
	// ctors.
	Window() : m_pos(Vector2D(100, 100)), m_size(Vector2D(150, 150)), m_name("uh oh..") { }
	Window(const Vector2D& pos, const Vector2D& size, const std::string& name) : m_pos(pos), m_size(size), m_name(name) { }

	// should the shits run?
	__forceinline bool ShouldRun() { return !g_Interfaces.EngineVGui->IsGameUIVisible(); }

	// pure virtual.
	virtual void Think() = 0;

	// basic virtual.
	virtual void Dragging() {
		static Vector2D drag = {};
		static bool dragging = false;
		bool click = GetAsyncKeyState(VK_LBUTTON);

		int x, y;
		g_Interfaces.Surface->GetCursorPos(x, y);

		if (dragging && !click) dragging = false;
		if (dragging && click)
		{
			m_pos.x = x - drag.x;
			m_pos.y = y - drag.y;
		}

		if (x > m_pos.x && y > m_pos.y && x < m_pos.x + m_size.x && y < m_pos.y + 20) {
			dragging = true;
			drag.x = x - m_pos.x;
			drag.y = y - m_pos.y;
			m_pos.x = std::clamp((float)m_pos.x, 0.0f, g_ScreenSize.w - m_size.x - 1.0f);
			m_pos.y = std::clamp((float)m_pos.y, 0.0f, g_ScreenSize.h - m_size.y - 1.0f);
		}

		m_dragging = dragging;
	}

	// overrides for protected members.
	__forceinline void SetPosition(Vector2D pos) { m_pos = pos; }
	__forceinline void SetSize(Vector2D size) { m_size = size; }
	__forceinline void SetName(const std::string& name) { m_name = name; }

	// getters.
	__forceinline Vector2D GetPosition() { return m_pos; }
	__forceinline Vector2D GetSize() { return m_size; }
	__forceinline std::string GetName() { return m_name; }

protected:
	Vector2D m_pos, m_size;
	std::string m_name;
	bool m_dragging;
};

// :(
class PlayerList : public Window {
public:
	struct PlayerListData_t {
		int index;
		int server_id;
		std::string name;
		uint32_t friendsID;
		int team_num;
		int class_num;
	};

	PlayerList() : Window({ 200, 200 }, { 300, 20 }, "PlayerList") { };

	virtual void Think() override {
		if (g_Menu.m_bOpen || g_Menu.m_flFadeElapsed < g_Menu.m_flFadeDuration) {
			Dragging();

			{ // backboard and titlebar.
				g_Draw.Rect(m_pos.x, m_pos.y, m_size.x, 20, Vars::Menu::Colors::TitleBar);
				g_Draw.String(FONT_MENU, m_pos.x + (m_size.x * 0.5f), m_pos.y + 10, { 255, 255, 255, 255 }, ALIGN_CENTER, m_name.c_str());

				Color_t clrBack = Vars::Menu::Colors::WindowBackground;

				g_Draw.Rect(m_pos.x, m_pos.y + 20, m_size.x, m_size.y, clrBack);
				g_Draw.OutlinedRect(m_pos.x, m_pos.y + 20, m_size.x, m_size.y, clrBack);
			}

			SetSize({ m_size.x, 0.0f });

			int x, y;
			g_Interfaces.Surface->GetCursorPos(x, y);

			PlayerInfo_t pi;
			int i = 1;
			for (; i < g_Interfaces.GlobalVars->maxclients; i++) {
				CBaseEntity* pEntity = g_Interfaces.EntityList->GetClientEntity(i);
				if (!pEntity)
					continue;

				if (!g_Interfaces.Engine->GetPlayerInfo(i, &pi))
					continue;

				const int team_num = pEntity->GetTeamNum();
				const int class_num = pEntity->GetClassNum();

				const Color_t desired_color = g_EntityCache.Friends[i] ? Colors::Friend : team_num == 2 ? Colors::TeamRed : team_num == 3 ? Colors::TeamBlu : Colors::White;
				
				// avatar. :(
				//if (!pi.fakeplayer)
				//	g_Draw.Avatar(m_pos.x, m_pos.y + 22 + m_size.y, 16, 16, pi.friendsID);

				g_Draw.Texture(m_pos.x, m_pos.y + 22 + m_size.y, 16, 16, Colors::White, class_num);
				
				// name.
				g_Draw.String(FONT_MENU, m_pos.x + 18, m_pos.y + 22 + m_size.y, desired_color, ALIGN_DEFAULT, pi.name);

				{ // profile link.
					Vector2D pos = { m_pos.x + m_size.x - 84, m_pos.y + 22 + m_size.y };
					g_Draw.OutlinedRect(pos.x, pos.y, 50, 16, Vars::Menu::Colors::OutlineMenu);
					g_Draw.String(FONT_MENU, pos.x + 25, pos.y + 8, Vars::Menu::Colors::Text, ALIGN_CENTER, "Profile");
					if (GetAsyncKeyState(VK_LBUTTON) && (x > pos.x && y > pos.y && x < pos.x + 50 && y < pos.y + 16) && !has_pressed_profile && !m_dragging) {
						g_SteamInterfaces.Friends015->ActivateGameOverlayToUser("steamid", CSteamID((UINT64)(0x0110000100000000ULL + pi.friendsID)));
						g_Draw.OutlinedRect(pos.x, pos.y, 50, 16, { 255, 0, 0, 255 });
						has_pressed_profile = true;
					} else if (!GetAsyncKeyState(VK_LBUTTON) && (x > pos.x && y > pos.y && x < pos.x + 50 && y < pos.y + 16))
						has_pressed_profile = false;
				}

				{ // kick button.
					Vector2D pos = { m_pos.x + m_size.x - 32, m_pos.y + 22 + m_size.y };
					g_Draw.OutlinedRect(pos.x, pos.y, 30, 16, Vars::Menu::Colors::OutlineMenu);
					g_Draw.String(FONT_MENU, pos.x + 15, pos.y + 8, Vars::Menu::Colors::Text, ALIGN_CENTER, "Kick");
					if ((GetAsyncKeyState(VK_LBUTTON)) && (x > pos.x && y > pos.y && x < pos.x + 30 && y < pos.y + 16) && !m_dragging) {
						g_Interfaces.Engine->ClientCmd_Unrestricted(tfm::format("callvote kick %i", pi.userID).c_str());
						g_Draw.OutlinedRect(pos.x, pos.y, 50, 16, { 255, 0, 0, 255 });
					}
				}

				SetSize({ m_size.x, m_size.y + 20 });
			}
		}
	}

private:
	bool has_pressed_profile = false;
};

inline PlayerList g_playerlist;