#pragma once

#include <string>

namespace ICONS
{
	inline constexpr int WIDHT = 64;
	inline constexpr int HEIGHT = 64;
	inline constexpr int UNDEFINED = -1;

	inline std::string TEXTURE[] =
	{
		_("hud/leaderboard_class_teleporter.vtf"),
		_("hud/leaderboard_class_scout.vtf"),
		_("hud/leaderboard_class_sniper.vtf"),
		_("hud/leaderboard_class_soldier.vtf"),
		_("hud/leaderboard_class_demo.vtf"),
		_("hud/leaderboard_class_medic.vtf"),
		_("hud/leaderboard_class_heavy.vtf"),
		_("hud/leaderboard_class_pyro.vtf"),
		_("hud/leaderboard_class_spy.vtf"),
		_("hud/leaderboard_class_engineer.vtf"),
		_("hud/eng_build_sentry.vtf"),
		_("vgui/class_portraits/scout_alpha.vtf"),
		_("vgui/class_portraits/sniper_alpha.vtf"),
		_("vgui/class_portraits/soldier_alpha.vtf"),
		_("vgui/class_portraits/demoman_alpha.vtf"),
		_("vgui/class_portraits/medic_alpha.vtf"),
		_("vgui/class_portraits/heavy_alpha.vtf"),
		_("vgui/class_portraits/pyro_alpha.vtf"),
		_("vgui/class_portraits/spy_alpha.vtf"),
		_("vgui/class_portraits/engineer_alpha.vtf"),
		_("vgui/class_portraits/spy_blue_grey_alpha.vtf"),
		_("vgui/class_portraits/scout_blue_alpha.vtf"),
		_("vgui/class_portraits/sniper_blue_alpha.vtf"),
		_("vgui/class_portraits/soldier_blue_alpha.vtf"),
		_("vgui/class_portraits/demoman_blue_alpha.vtf"),
		_("vgui/class_portraits/medic_blue_alpha.vtf"),
		_("vgui/class_portraits/heavy_blue_alpha.vtf"),
		_("vgui/class_portraits/pyro_blue_alpha.vtf"),
		_("vgui/class_portraits/spy_blue_alpha.vtf"),
		_("vgui/class_portraits/engineer_blue_alpha.vtf"),
		_("vgui/tf_box.vtf"),
		_("vgui/spellbook_book"),
		_("vgui/spellbook_book"), //EMPTY
		_("vgui/viewmode_spooky.vtf"),
		_("vgui/workshop_sandvich_top.vtf"),
		_("hud/ico_stickybomb_blue.vtf"),
		_("hud/ico_stickybomb.vtf"),
		_("hud/leaderboard_class_medic_uber.vtf"),
		_("hud/objectives_flagpanel_carried_blue.vtf"),
		_("hud/objectives_flagpanel_carried_red.vtf"),
		_("vgui/achievements/tf_halloween_pumpkin_kill.vtf"),
		_("hud/hud_obj_status_sentry_1.vtf"),
		_("hud/hud_obj_status_sentry_2.vtf"),
		_("hud/hud_obj_status_sentry_3.vtf"),
		_("hud/hud_obj_status_dispenser.vtf"),
		_("hud/hud_obj_status_tele_entrance.vtf"),
		_("hud/hud_obj_status_tele_exit.vtf"),
		_("console/title_war.vtf"),
		_("hud/scoreboard_ping_bot_red.vtf"),
		_("hud/scoreboard_ping_bot_blue.vtf"),
		_("sprites/healbeam.vtf"),
		_("hud/ico_spy"),
		_("vgui/../hud/panel_scalable_red"),
		_("vgui/../hud/panel_scalable_blue"),
		_("hud/leaderboard_dead"),
		_("vgui/../hud/hud_obj_status_ammo_64"),
		_("vgui/hud/xbox_reticle"),
		_("hud/hud_obj_weapon_pickup"),
		_("vgui/animated/tf2_logo_hourglass"),
		_("console/characters/pauling_gun_mettle_title"),
		_("console/characters/pauling_tough_break_title"),
		_("console/characters/pauling_tough_break_title_xmas2015"),
	};

	inline constexpr int TEXTURE_AMOUNT = (sizeof(TEXTURE) / sizeof(TEXTURE[0]));

	//C++ wont let me initialize them all with -1 unless I manually do so???
	//Thats why they're 0 now, set to -1 on intialize, 0 can be a valid ID.
	inline int ID[TEXTURE_AMOUNT] = { 0 };
}