#pragma once

#include <string>

namespace ICONS
{
	inline constexpr int WIDHT = 64;
	inline constexpr int HEIGHT = 64;
	inline constexpr int UNDEFINED = -1;

	inline std::string TEXTURE[] =
	{
		"hud/leaderboard_class_teleporter.vtf",
		"hud/leaderboard_class_scout.vtf",
		"hud/leaderboard_class_sniper.vtf",
		"hud/leaderboard_class_soldier.vtf",
		"hud/leaderboard_class_demo.vtf",
		"hud/leaderboard_class_medic.vtf",
		"hud/leaderboard_class_heavy.vtf",
		"hud/leaderboard_class_pyro.vtf",
		"hud/leaderboard_class_spy.vtf",
		"hud/leaderboard_class_engineer.vtf",
		"hud/eng_build_sentry.vtf",
		"vgui/class_portraits/scout_alpha.vtf",
		"vgui/class_portraits/sniper_alpha.vtf",
		"vgui/class_portraits/soldier_alpha.vtf",
		"vgui/class_portraits/demoman_alpha.vtf",
		"vgui/class_portraits/medic_alpha.vtf",
		"vgui/class_portraits/heavy_alpha.vtf",
		"vgui/class_portraits/pyro_alpha.vtf",
		"vgui/class_portraits/spy_alpha.vtf",
		"vgui/class_portraits/engineer_alpha.vtf",
		"vgui/class_portraits/spy_blue_grey_alpha.vtf",
		"vgui/class_portraits/scout_blue_alpha.vtf",
		"vgui/class_portraits/sniper_blue_alpha.vtf",
		"vgui/class_portraits/soldier_blue_alpha.vtf",
		"vgui/class_portraits/demoman_blue_alpha.vtf",
		"vgui/class_portraits/medic_blue_alpha.vtf",
		"vgui/class_portraits/heavy_blue_alpha.vtf",
		"vgui/class_portraits/pyro_blue_alpha.vtf",
		"vgui/class_portraits/spy_blue_alpha.vtf",
		"vgui/class_portraits/engineer_blue_alpha.vtf",
		"vgui/tf_box.vtf",
		"vgui/spellbook_book",
		"vgui/spellbook_book", //EMPTY
		"vgui/viewmode_spooky.vtf",
		"vgui/workshop_sandvich_top.vtf",
		"hud/ico_stickybomb_blue.vtf",
		"hud/ico_stickybomb.vtf",
		"hud/leaderboard_class_medic_uber.vtf",
		"hud/objectives_flagpanel_carried_blue.vtf",
		"hud/objectives_flagpanel_carried_red.vtf",
		"vgui/achievements/tf_halloween_pumpkin_kill.vtf",
		"hud/hud_obj_status_sentry_1.vtf",
		"hud/hud_obj_status_sentry_2.vtf",
		"hud/hud_obj_status_sentry_3.vtf",
		"hud/hud_obj_status_dispenser.vtf",
		"hud/hud_obj_status_tele_entrance.vtf",
		"hud/hud_obj_status_tele_exit.vtf",
		"console/title_war.vtf",
		"hud/scoreboard_ping_bot_red.vtf",
		"hud/scoreboard_ping_bot_blue.vtf",
		"sprites/healbeam.vtf",
		"hud/ico_spy",
		"vgui/../hud/panel_scalable_red",
		"vgui/../hud/panel_scalable_blue",
		"hud/leaderboard_dead",
		"vgui/../hud/hud_obj_status_ammo_64",
		"vgui/hud/xbox_reticle",
		"hud/hud_obj_weapon_pickup",
		"vgui/animated/tf2_logo_hourglass",
		"console/characters/pauling_gun_mettle_title",
		"console/characters/pauling_tough_break_title",
		"console/characters/pauling_tough_break_title_xmas2015",
	};

	inline constexpr int TEXTURE_AMOUNT = (sizeof(TEXTURE) / sizeof(TEXTURE[0]));

	//C++ wont let me initialize them all with -1 unless I manually do so???
	//Thats why they're 0 now, set to -1 on intialize, 0 can be a valid ID.
	inline int ID[TEXTURE_AMOUNT] = { 0 };
}