#include "Menu.h"

#include "../Vars.h"
#include "../Camera/CameraWindow.h"
#include "../AttributeChanger/AttributeChanger.h"
#include "../Misc/Misc.h"
#include "Playerlist/Playerlist.h"

#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_stdlib.h"
#include "Fonts/IconsMaterialDesign.h"
#include "MaterialEditor/MaterialEditor.h"

#include "Components.hpp"
#include "ConfigManager/ConfigManager.h"
#include "Blur/Blur.h"

constexpr int MENU_KEY = VK_INSERT;

int unuPrimary = 0;
int unuSecondary = 0;

/* The main menu */
void CMenu::DrawMenu()
{
	ImGui::GetStyle().WindowMinSize = ImVec2(700, 500);

	ImGui::SetNextWindowSize(ImVec2(700, 700), ImGuiCond_FirstUseEver);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	if (ImGui::Begin("Fedoraware", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
	{
		const auto drawList = ImGui::GetWindowDrawList();
		const auto windowSize = ImGui::GetWindowSize();
		const auto windowPos = ImGui::GetWindowPos();

		// Gradient line
		ImGui::GradientRect(&TitleGradient, { windowPos.x, windowPos.y }, windowSize.x, 3.f);
		ImGui::Dummy({ 0, 2 });

		// Title Text
		{
			ImGui::PushFont(TitleFont);
			const auto titleWidth = ImGui::CalcTextSize("Fedoraware").x;
			drawList->AddText(TitleFont, TitleFont->FontSize, { windowPos.x + (windowSize.x / 2) - (titleWidth / 2), windowPos.y }, Accent, "Fedoraware");
			ImGui::PopFont();
		}

		// Icons
		{
			float currentX = windowSize.x;

			// Settings Icon
			ImGui::SetCursorPos({ currentX -= 25, 0 });
			if (ImGui::IconButton(ICON_MD_SETTINGS))
			{
				ShowSettings = !ShowSettings;
			}
			ImGui::HelpMarker("Settings");

			// Playerlist Icon
			ImGui::SetCursorPos({ currentX -= 25, 0 });
			if (ImGui::IconButton(ICON_MD_PEOPLE))
			{
				Vars::Menu::ShowPlayerlist = !Vars::Menu::ShowPlayerlist;
			}
			ImGui::HelpMarker("Playerlist");

			// Material Editor Icon
			ImGui::SetCursorPos({ currentX -= 25, 0 });
			if (ImGui::IconButton(ICON_MD_BRUSH))
			{
				g_MaterialEditor.IsOpen = !g_MaterialEditor.IsOpen;
			}
			ImGui::HelpMarker("Material Editor");

			#ifdef _DEBUG
			// Debug Menu
			ImGui::SetCursorPos({ currentX -= 25, 0 });
			if (ImGui::IconButton(ICON_MD_CODE))
			{
				ShowDebugMenu = !ShowDebugMenu;
			}
			ImGui::HelpMarker("Debug Menu");
			#endif
		}

		// Tabbar
		ImGui::SetCursorPos({ 0, TitleHeight });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, BackgroundLight.Value);
		if (ImGui::BeginChild("Tabbar", { windowSize.x + 5, TabHeight + SubTabHeight }, false, ImGuiWindowFlags_NoScrollWithMouse))
		{
			DrawTabbar();
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();

		// Main content
		ImGui::SetCursorPos({ 0, TitleHeight + TabHeight + SubTabHeight });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8.f, 10.f });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, BackgroundDark.Value);
		if (ImGui::BeginChild("Content", { windowSize.x, windowSize.y - (TitleHeight + TabHeight + SubTabHeight) }, false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar))
		{
			ImGui::PushFont(Verdana);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 3.f, 2.f });

			switch (CurrentTab)
			{
			case MenuTab::Aimbot: { MenuAimbot(); break; }
			case MenuTab::Trigger: { MenuTrigger(); break; }
			case MenuTab::Visuals: { MenuVisuals(); break; }
			case MenuTab::HvH: { MenuHvH(); break; }
			case MenuTab::Misc: { MenuMisc(); break; }
			}

			ImGui::PopStyleVar();
			ImGui::PopFont();
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		// Footer
		{
			if (!Vars::Menu::ModernDesign)
			{
				const auto hintHeight = ImGui::CalcTextSize(FeatureHint.c_str()).y;
				drawList->AddText(Verdana, Verdana->FontSize, { windowPos.x + 10, windowPos.y + windowSize.y - (hintHeight + ImGui::GetStyle().ItemInnerSpacing.y) }, TextLight, FeatureHint.c_str());
			}
		}

		// End
		ImGui::End();
	}
	ImGui::PopStyleVar();
}

void CMenu::DrawTabbar()
{
	ImGui::PushFont(SectionFont);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

	if (ImGui::BeginTable("TabbarTable", 5))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, BackgroundLight.Value);
		ImGui::PushStyleColor(ImGuiCol_Text, TextLight.Value);
		if (ImGui::TabButton("Aimbot", CurrentTab == MenuTab::Aimbot))
		{
			CurrentTab = MenuTab::Aimbot;
		}

		if (ImGui::TabButton("Triggerbot", CurrentTab == MenuTab::Trigger))
		{
			CurrentTab = MenuTab::Trigger;
		}

		if (ImGui::TabButton("Visuals", CurrentTab == MenuTab::Visuals))
		{
			CurrentTab = MenuTab::Visuals;
		}

		if (ImGui::TabButton("HvH", CurrentTab == MenuTab::HvH))
		{
			CurrentTab = MenuTab::HvH;
		}

		if (ImGui::TabButton("Misc", CurrentTab == MenuTab::Misc))
		{
			CurrentTab = MenuTab::Misc;
		}

		ImGui::PopStyleColor(2);
		ImGui::EndTable();
	}

	ImGui::SetCursorPosY(TabHeight);
	if (CurrentTab == MenuTab::Visuals)
	{
		SubTabHeight = 30.f;

		if (ImGui::BeginTable("SubbarTable", 6))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, BackgroundLight.Value);
			ImGui::PushStyleColor(ImGuiCol_Text, TextLight.Value);
			if (ImGui::TabButton("Players", CurrentVisualsTab == VisualsTab::Players))
			{
				CurrentVisualsTab = VisualsTab::Players;
			}

			if (ImGui::TabButton("Buildings", CurrentVisualsTab == VisualsTab::Buildings))
			{
				CurrentVisualsTab = VisualsTab::Buildings;
			}

			if (ImGui::TabButton("World", CurrentVisualsTab == VisualsTab::World))
			{
				CurrentVisualsTab = VisualsTab::World;
			}

			if (ImGui::TabButton("Fonts", CurrentVisualsTab == VisualsTab::Font))
			{
				CurrentVisualsTab = VisualsTab::Font;
			}

			if (ImGui::TabButton("Misc", CurrentVisualsTab == VisualsTab::Misc))
			{
				CurrentVisualsTab = VisualsTab::Misc;
			}

			if (ImGui::TabButton("Radar", CurrentVisualsTab == VisualsTab::Radar))
			{
				CurrentVisualsTab = VisualsTab::Radar;
			}

			ImGui::PopStyleColor(2);
			ImGui::EndTable();
		}
	} else
	{
		SubTabHeight = 0.f;
	}
	
	ImGui::PopStyleVar(3);
	ImGui::PopFont();
}

#pragma region Tabs
/* Tab: Aimbot */
void CMenu::MenuAimbot()
{
	using namespace ImGui;

	if (BeginTable("AimbotTable", 3))
	{
		/* Column 1 */
		if (TableColumnChild("AimbotCol1"))
		{
			SectionTitle("Global");

			WToggle("Aimbot", &Vars::Aimbot::Global::Active.m_Var); HelpMarker("Aimbot master switch");
			ColorPickerL("Target", Colors::Target);
			InputKeybind("Aimbot key", Vars::Aimbot::Global::AimKey); HelpMarker("The key to enable aimbot");
			WSlider("Aimbot FoV####AimbotFoV", &Vars::Aimbot::Global::AimFOV.m_Var, 0.f, 180.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
			ColorPickerL("Aimbot FOV circle", Colors::FOVCircle);
			WToggle("Autoshoot###AimbotAutoshoot", &Vars::Aimbot::Global::AutoShoot.m_Var); HelpMarker("Automatically shoot when a target is found");
			MultiCombo({ "Players", "Buildings", "Stickies"}, {&Vars::Aimbot::Global::AimPlayers.m_Var, &Vars::Aimbot::Global::AimBuildings.m_Var, &Vars::Aimbot::Global::AimStickies.m_Var}, "Aim targets");
			HelpMarker("Choose which targets the Aimbot should aim at");
			MultiCombo({ "Invulnerable", "Cloaked", "Friends", "Taunting" }, { &Vars::Aimbot::Global::IgnoreInvlunerable.m_Var, &Vars::Aimbot::Global::IgnoreCloaked.m_Var, &Vars::Aimbot::Global::IgnoreFriends.m_Var, &Vars::Aimbot::Global::IgnoreTaunting.m_Var }, "Ignored targets###HitscanIgnoredTargets");
			HelpMarker("Choose which targets should be ignored");
			ColorPickerL("Invulnerable colour", Colors::Invuln);

			SectionTitle("Crits");
			WToggle("Crit hack", &Vars::CritHack::Active.m_Var);  HelpMarker("Enables the crit hack (BETA)");
			MultiCombo({ "Indicators", "Avoid Random", "Always Melee" }, {&Vars::CritHack::indicators.m_Var, &Vars::CritHack::avoidrandom.m_Var, &Vars::CritHack::AlwaysMelee.m_Var }, "Misc###CrithackMiscOptions");
			HelpMarker("Misc options for crithack");
			InputKeybind("Crit key", Vars::CritHack::CritKey); HelpMarker("Will try to force crits when the key is held");

			SectionTitle("Backtrack");
			WToggle("Active", &Vars::Backtrack::Enabled.m_Var); HelpMarker("If you shoot at the backtrack manually it will attempt to hit it");
			WToggle("Aimbot aims last tick", &Vars::Backtrack::Aim.m_Var); HelpMarker("Aimbot aims at the last tick if visible");
		} EndChild();

		/* Column 2 */
		if (TableColumnChild("AimbotCol2"))
		{
			SectionTitle("Hitscan");
			WCombo("Sort method###HitscanSortMethod", &Vars::Aimbot::Hitscan::SortMethod.m_Var, { "FOV", "Distance" }); HelpMarker("Which method the aimbot uses to decide which target to aim at");
			WCombo("Aim method###HitscanAimMethod", &Vars::Aimbot::Hitscan::AimMethod.m_Var, { "Plain", "Smooth", "Silent" }); HelpMarker("Which method the aimbot uses to aim at the target");
			WCombo("Preferred Hitbox###HitscanHitbox", &Vars::Aimbot::Hitscan::AimHitbox.m_Var, { "Head", "Body", "Auto"}); // this could probably be removed entirely since it actually does nothing.
			WCombo("Tapfire###HitscanTapfire", &Vars::Aimbot::Hitscan::TapFire.m_Var, { "Off", "Distance", "Always" }); HelpMarker("How/If the aimbot chooses to tapfire enemies.");
			WSlider("Smooth factor###HitscanSmoothing", &Vars::Aimbot::Hitscan::SmoothingAmount.m_Var, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Changes how smooth the aimbot will aim at the target");
			{
				static std::vector flagNames{ "Head", "Body", "Pelvis", "Arms", "Legs"};
				static std::vector flagValues{ 0x00000001, 0x00000004, 0x00000002, 0x00000008, 0x00000010}; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Hitscan::ScanHitboxes.m_Var, "Scan Hitboxes###AimbotHitboxScanning");
			}
			{
				static std::vector flagNames{ "Head", "Body", "Pelvis", "Arms", "Legs" };
				static std::vector flagValues{ 0x00000001, 0x00000004, 0x00000002, 0x00000008, 0x00000010 }; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Hitscan::MultiHitboxes.m_Var, "Multipoint Hitboxes###AimbotMultipointScanning");
			}
			WToggle("Buildings Multipoint", &Vars::Aimbot::Hitscan::ScanBuildings.m_Var); HelpMarker("Scans the building hitbox to improve hitchance");
			WToggle("Wait for headshot", &Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var); HelpMarker("The aimbot will wait until it can headshot (if applicable)");
			WToggle("Wait for charge", &Vars::Aimbot::Hitscan::WaitForCharge.m_Var); HelpMarker("The aimbot will wait until the rifle has charged long enough to kill in one shot");
			WToggle("Smooth if spectated", &Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var); HelpMarker("The aimbot will switch to the smooth method if being spectated");
			WToggle("Scoped only", &Vars::Aimbot::Hitscan::ScopedOnly.m_Var); HelpMarker("The aimbot will only shoot if scoped");
			WToggle("Auto scope", &Vars::Aimbot::Hitscan::AutoScope.m_Var); HelpMarker("The aimbot will automatically scope in to shoot");
			WToggle("Auto rev minigun", &Vars::Aimbot::Hitscan::AutoRev.m_Var); HelpMarker("Will rev heavy's minigun regardless of if aimbot has a target");
			WToggle("Bodyaim if lethal", &Vars::Aimbot::Global::BAimLethal.m_Var); HelpMarker("The aimbot will aim for body when damage is lethal to it");
		} EndChild();

		/* Column 3 */
		if (TableColumnChild("AimbotCol3"))
		{
			SectionTitle("Projectile");
			WToggle("Performance mode", &Vars::Aimbot::Projectile::PerformanceMode.m_Var); HelpMarker("Only target enemy closest to the crosshair");
			WToggle("Movement simulation", &Vars::Aimbot::Projectile::MovementSimulation.m_Var); HelpMarker("Uses game functions to predict where the player will be");
			ColorPickerL("Prediction Line Color", Vars::Aimbot::Projectile::PredictionColor);
			if (Vars::Aimbot::Projectile::MovementSimulation.m_Var)
			{
				WSlider("Prediction Time", &Vars::Aimbot::Projectile::predTime.m_Var, 0.1f, 10.f, "%.1f");
			}
			{
				WCombo("Sort method###ProjectileSortMethod", &Vars::Aimbot::Projectile::SortMethod.m_Var, { "FOV", "Distance" });
				WCombo("Aim method###ProjectileAimMethod", &Vars::Aimbot::Projectile::AimMethod.m_Var, { "Plain", "Silent" });
				WCombo("Hitbox###ProjectileHitbox", &Vars::Aimbot::Projectile::AimPosition.m_Var, { "Head", "Body", "Feet", "Auto"});
			}
			WToggle("Feet aim on ground", &Vars::Aimbot::Projectile::FeetAimIfOnGround.m_Var); HelpMarker("Will aim at feet if target is on the ground");
			//WToggle("Custom huntsman Z-Adjust", &Vars::Aimbot::Projectile::ManualZAdjust.m_Var); HelpMarker("Enables the ability to adjust the Z-Position for huntsman");
			//if (Vars::Aimbot::Projectile::ManualZAdjust.m_Var)
			//{
			//	WSlider("Z-Value###ZAdjustValue", &Vars::Aimbot::Projectile::ZAdjustAmount.m_Var, 0.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Manual Z-Adjust for projectiles");
			//}

			SectionTitle("Melee");
			{
				WCombo("Sort method###MeleeSortMethod", &Vars::Aimbot::Melee::SortMethod.m_Var, { "FOV", "Distance", }); HelpMarker("Which method the aimbot uses to decide which target to aim at");
				WCombo("Aim method###MeleeAimMethod", &Vars::Aimbot::Melee::AimMethod.m_Var, { "Plain", "Smooth", "Silent" }); HelpMarker("Which method the aimbot uses to aim at the target");
			}
			WSlider("Smooth factor###MeleeSmoothing", &Vars::Aimbot::Melee::SmoothingAmount.m_Var, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How smooth the aimbot should be");
			WToggle("Range check", &Vars::Aimbot::Melee::RangeCheck.m_Var); HelpMarker("Only aim at target if within melee range");
			WToggle("Swing prediction", &Vars::Aimbot::Melee::PredictSwing.m_Var); HelpMarker("Aimbot will attack preemptively, predicting you will be in range of the target");
			WToggle("Whip teammates", &Vars::Aimbot::Melee::WhipTeam.m_Var); HelpMarker("Aimbot will target teammates if holding the Disciplinary Action");
			WToggle("Wait for hit", &Vars::Aimbot::Projectile::WaitForHit.m_Var); HelpMarker("Will avoid shooting until the last shot hits");

		} EndChild();

		/* End */
		EndTable();
	}
}

/* Tab: Trigger */
void CMenu::MenuTrigger()
{
	using namespace ImGui;
	if (BeginTable("TriggerTable", 3))
	{
		/* Column 1 */
		if (TableColumnChild("TriggerCol1"))
		{
			SectionTitle("Global");
			WToggle("Triggerbot", &Vars::Triggerbot::Global::Active.m_Var); HelpMarker("Global triggerbot master switch");
			InputKeybind("Trigger key", Vars::Triggerbot::Global::TriggerKey); HelpMarker("The key which activates the triggerbot");
			MultiCombo({ "Invulnerable", "Cloaked", "Friends" }, { &Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var, &Vars::Triggerbot::Global::IgnoreCloaked.m_Var, &Vars::Triggerbot::Global::IgnoreFriends.m_Var }, "Ignored targets###TriggerIgnoredTargets");
			HelpMarker("Choose which targets should be ignored");

			SectionTitle("Autoshoot");
			WToggle("Autoshoot###AutoshootTrigger", &Vars::Triggerbot::Shoot::Active.m_Var); HelpMarker("Shoots if mouse is over a target");
			MultiCombo({ "Players", "Buildings" }, { &Vars::Triggerbot::Shoot::TriggerPlayers.m_Var, &Vars::Triggerbot::Shoot::TriggerBuildings.m_Var }, "Trigger targets");
			HelpMarker("Choose which target the triggerbot should shoot at");
			WToggle("Head only###TriggerHeadOnly", &Vars::Triggerbot::Shoot::HeadOnly.m_Var); HelpMarker("Auto shoot will only shoot if you are aiming at the head");
			WToggle("Wait for charge###TriggerbotWaitForCharge", &Vars::Triggerbot::Shoot::WaitForCharge.m_Var); HelpMarker("Auto shoot will only shoot if the sniper is charged enough to kill in one hit / is fully charged");
			WSlider("Head scale###TriggerHeadScale", &Vars::Triggerbot::Shoot::HeadScale.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The scale at which the auto shoot will try to shoot the targets head");
		} EndChild();

		/* Column 2 */
		if (TableColumnChild("TriggerCol2"))
		{
			SectionTitle("Autostab");
			WToggle("Auto backstab###TriggerAutostab", &Vars::Triggerbot::Stab::Active.m_Var); HelpMarker("Auto backstab will attempt to backstab the target if possible");
			WToggle("Rage mode", &Vars::Triggerbot::Stab::RageMode.m_Var); HelpMarker("Stabs whenever possible by aiming toward the back");
			WToggle("Silent", &Vars::Triggerbot::Stab::Silent.m_Var); HelpMarker("Aim changes made by the rage mode setting aren't visible");
			WToggle("Disguise on kill", &Vars::Triggerbot::Stab::Disguise.m_Var); HelpMarker("Will apply the previous disguise after stabbing");
			WToggle("Ignore razorback", &Vars::Triggerbot::Stab::IgnRazor.m_Var); HelpMarker("Will not attempt to backstab snipers wearing the razorback");
			WSlider("Stab range###StabRange", &Vars::Triggerbot::Stab::Range.m_Var, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The range at which auto backstab will attempt to stab");

			SectionTitle("Auto Detonate");
			WToggle("Autodetonate###TriggerDet", &Vars::Triggerbot::Detonate::Active.m_Var);
			WToggle("Explode stickies###TriggerSticky", &Vars::Triggerbot::Detonate::Stickies.m_Var); HelpMarker("Detonate sticky bombs when a player is in range");
			WToggle("Detonate flares###TriggerFlares", &Vars::Triggerbot::Detonate::Flares.m_Var); HelpMarker("Detonate detonator flares when a player is in range");
			WSlider("Detonation radius###TriggerDetRadius", &Vars::Triggerbot::Detonate::RadiusScale.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The radius around the projectile that it will detonate if a player is in");
		} EndChild();

		/* Column 3 */
		if (TableColumnChild("TriggerCol3"))
		{
			SectionTitle("Autoblast");
			WToggle("Autoblast###Triggreairblast", &Vars::Triggerbot::Blast::Active.m_Var); HelpMarker("Auto airblast master switch");
			WToggle("Rage airblast###TriggerAirRage", &Vars::Triggerbot::Blast::Rage.m_Var); HelpMarker("Will airblast whenever possible, regardless of FoV");
			WToggle("Silent###triggerblastsilent", &Vars::Triggerbot::Blast::Silent.m_Var); HelpMarker("Aim changes made by the rage mode setting aren't visible");

			SectionTitle("Autouber");
			WToggle("Autouber###Triggeruber", &Vars::Triggerbot::Uber::Active.m_Var); HelpMarker("Auto uber master switch");
			WToggle("Only uber friends", &Vars::Triggerbot::Uber::OnlyFriends.m_Var); HelpMarker("Auto uber will only activate if healing steam friends");
			WToggle("Preserve self", &Vars::Triggerbot::Uber::PopLocal.m_Var); HelpMarker("Auto uber will activate if local player's health falls below the percentage");
			WToggle("Vaccinator resistances", &Vars::Triggerbot::Uber::AutoVacc.m_Var); HelpMarker("Auto uber will automatically find the best resistance and pop when needed (This doesn't work properly)");
			WSlider("Health left (%)###TriggerUberHealthLeft", &Vars::Triggerbot::Uber::HealthLeft.m_Var, 1.f, 99.f, "%.0f%%", 1.0f); HelpMarker("The amount of health the heal target must be below to actiavte");
		} EndChild();

		EndTable();
	}
}

/* Tab: Visuals */
void CMenu::MenuVisuals()
{
	using namespace ImGui;

	switch (CurrentVisualsTab)
	{
	// Visuals: Players
	case VisualsTab::Players:
	{
		if (BeginTable("VisualsPlayersTable", 3))
		{
			/* Column 1 */
			if (TableColumnChild("VisualsPlayersCol1"))
			{
				SectionTitle("ESP Main");
				WToggle("ESP###EnableESP", &Vars::ESP::Main::Active.m_Var); HelpMarker("Global ESP master switch");
				WToggle("Outlined health bars", &Vars::ESP::Main::Outlinedbar.m_Var); HelpMarker("Will outline the health bars");
				WToggle("Relative colours", &Vars::ESP::Main::EnableTeamEnemyColors.m_Var); HelpMarker("Chooses colors relative to your team (team/enemy)");
				if (Vars::ESP::Main::EnableTeamEnemyColors.m_Var)
				{
					ColorPickerL("Enemy color", Colors::Enemy);
					ColorPickerL("Team color", Colors::rTeam, 1);
				}
				else
				{
					ColorPickerL("RED Team color", Colors::TeamRed);
					ColorPickerL("BLU Team color", Colors::TeamBlu, 1);
				}

				SectionTitle("Player ESP");
				WToggle("Player ESP###EnablePlayerESP", &Vars::ESP::Players::Active.m_Var); HelpMarker("Will draw useful information/indicators on players");
				WToggle("Name ESP###PlayerNameESP", &Vars::ESP::Players::Name.m_Var); HelpMarker("Will draw the players name");
				WToggle("Custom Name Color", &Vars::ESP::Players::NameCustom.m_Var); HelpMarker("Custom color for name esp");
				if (Vars::ESP::Players::NameCustom.m_Var)
				{
					ColorPickerL("Name ESP Color", Vars::ESP::Players::NameColor);
				}
				WToggle("Name ESP box###PlayerNameESPBox", &Vars::ESP::Players::NameBox.m_Var); HelpMarker("Will draw a box around players name to make it stand out");
				WToggle("Self ESP###SelfESP", &Vars::ESP::Players::ShowLocal.m_Var); HelpMarker("Will draw ESP on local player (thirdperson)");
				ColorPickerL("Local colour", Colors::Local);
				WCombo("Ignore team###IgnoreTeamESPp", &Vars::ESP::Players::IgnoreTeammates.m_Var, { "Off", "All", "Only friends" }); HelpMarker("Which teammates the ESP will ignore drawing on");
				ColorPickerL("Friend colour", Colors::Friend);
				WCombo("Ignore cloaked###IgnoreCloakESPp", &Vars::ESP::Players::IgnoreCloaked.m_Var, { "Off", "All", "Only enemies" }); HelpMarker("Which cloaked spies the ESP will ignore drawing on");
				ColorPickerL("Cloaked colour", Colors::Cloak);
				WCombo("Ubercharge###PlayerUber", &Vars::ESP::Players::Uber.m_Var, { "Off", "Text", "Bar" }); HelpMarker("Will draw how much ubercharge a medic has");
				ColorPickerL("Ubercharge colour", Colors::UberColor);
				WCombo("Class###PlayerIconClass", &Vars::ESP::Players::Class.m_Var, { "Off", "Icon", "Text", "Both" }); HelpMarker("Will draw the class the player is");
				WToggle("Weapon icons", &Vars::ESP::Players::WeaponIcon.m_Var); HelpMarker("Shows an icon for the weapon that the player has currently equipped");
				ColorPickerL("Invulnerable colour", Colors::WeaponIcon);
				WToggle("Health bar###ESPPlayerHealthBar", &Vars::ESP::Players::HealthBar.m_Var); HelpMarker("Will draw a bar visualizing how much health the player has");
				ColorPickerL("Health Bar Top", Colors::GradientHealthBar.startColour);
				ColorPickerL("Health Bar Bottom", Colors::GradientHealthBar.endColour, 1);
				WToggle("Health text###ESPPlayerHealthText", &Vars::ESP::Players::Health.m_Var); HelpMarker("Will draw the players health, as well as their max health");
				WToggle("Condition", &Vars::ESP::Players::Cond.m_Var); HelpMarker("Will draw what conditions the player is under");
				ColorPickerL("Condition colour", Colors::Cond);
				WToggle("GUID", &Vars::ESP::Players::GUID.m_Var); HelpMarker("Show's the players Steam ID");
				WToggle("Choked Packets", &Vars::ESP::Players::Choked.m_Var); HelpMarker("Shows how many packets the player has choked");
				ColorPickerL("Choked Bar Top", Colors::ChokedBar.startColour);
				ColorPickerL("Choked Bar Bottom", Colors::ChokedBar.endColour, 1);
				WToggle("Cheater Detection", &Vars::ESP::Players::CheaterDetection.m_Var); HelpMarker("Attempts to automatically mark cheaters.");
				WCombo("Box###PlayerBoxESP", &Vars::ESP::Players::Box.m_Var, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on players");
				WCombo("Skeleton###PlayerSkellington", &Vars::ESP::Players::Bones.m_Var, { "Off", "Custom colour", "Health" }); HelpMarker("Will draw the bone structure of the player");
				ColorPickerL("Skellington colour", Colors::Bones);
				WToggle("Lines###Playerlines", &Vars::ESP::Players::Lines.m_Var); HelpMarker("Draws lines from the local players position to enemies position");
				WToggle("Dlights###PlayerDlights", &Vars::ESP::Players::Dlights.m_Var); HelpMarker("Will make players emit a dynamic light around them");
				WSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Players::DlightRadius.m_Var, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
				WSlider("ESP alpha###PlayerESPAlpha", &Vars::ESP::Players::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsPlayersCol2"))
			{
				SectionTitle("Chams Main");
				WToggle("Chams###ChamsMasterSwitch", &Vars::Chams::Main::Active.m_Var); HelpMarker("Chams master switch");

				static std::vector chamOptions{
					"Local",
					"Friends",
					"Enemies",
					"Teammates",
					"Target"
				};

				static int currentSelected = 0; // 0 - local, 1 - friends, 2 - enemy, 3 - team
				static std::vector pchamsMaterials{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel", "Plastic", "Custom" };

				SectionTitle("Player Chams");
				WToggle("Player chams###PlayerChamsBox", &Vars::Chams::Players::Active.m_Var); HelpMarker("Player chams master switch");
				MultiCombo({ "Render Wearable", "Render Weapon", "Fadeout Own Team"}, {&Vars::Chams::Players::Wearables.m_Var, &Vars::Chams::Players::Weapons.m_Var, &Vars::Chams::Players::FadeoutTeammates.m_Var}, "Flags");
				HelpMarker("Customize Chams");
				WCombo("Config", &currentSelected, chamOptions);

				switch (currentSelected) // please find a better way to do this, i have tried so many things and i cant get it to work properly
				{
				case 0:
				{
					// Local
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Players::Local.chamsActive, &Vars::Chams::Players::Local.showObstructed }, "Options");
					WCombo("Material", &Vars::Chams::Players::Local.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
					ColorPickerL("Fresnel base colour", Vars::Chams::Players::Local.fresnelBase);
					if (Vars::Chams::Players::Local.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::Players::Local.customMaterial);
					}
					break;
				}
				case 1:
				{
					// Friends
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Players::Friend.chamsActive, &Vars::Chams::Players::Friend.showObstructed }, "Options");
					WCombo("Material", &Vars::Chams::Players::Friend.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
					ColorPickerL("Fresnel base colour", Vars::Chams::Players::Friend.fresnelBase);
					if (Vars::Chams::Players::Friend.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::Players::Friend.customMaterial);
					}
					break;
				}
				case 2:
				{
					// Enemies
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Players::Enemy.chamsActive, &Vars::Chams::Players::Enemy.showObstructed }, "Options");
					WCombo("Material", &Vars::Chams::Players::Enemy.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
					ColorPickerL("Fresnel base colour", Vars::Chams::Players::Enemy.fresnelBase);
					if (Vars::Chams::Players::Enemy.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::Players::Enemy.customMaterial);
					}
					break;
				}
				case 3:
				{
					// Teammates
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Players::Team.chamsActive, &Vars::Chams::Players::Team.showObstructed, }, "Options");
					WCombo("Material", &Vars::Chams::Players::Team.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
					ColorPickerL("Fresnel base colour", Vars::Chams::Players::Team.fresnelBase);
					if (Vars::Chams::Players::Team.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::Players::Team.customMaterial);
					}
					break;
				}
				case 4:
				{
					// Target
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Players::Target.chamsActive, &Vars::Chams::Players::Target.showObstructed, }, "Options");
					WCombo("Material", &Vars::Chams::Players::Target.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
					ColorPickerL("Fresnel base colour", Vars::Chams::Players::Target.fresnelBase);
					if (Vars::Chams::Players::Target.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::Players::Target.customMaterial);
					}
					break;
				}
				}

				SectionTitle("DME Chams");
				WToggle("DME chams###dmeactive", &Vars::Chams::DME::Active.m_Var); HelpMarker("DME chams master switch");
				ColorPickerL("Weapon colour", Colors::Weapon);
				ColorPickerL("Hand colour", Colors::Hands, 1);

				static std::vector handsMaterial{
					"Original",
					"Shaded",
					"Shiny",
					"Flat",
					"Wireframe shaded",
					"Wireframe shiny",
					"Wireframe flat",
					"Fresnel",
					"Brick",
					"What",
					"Wacky"
				};
				WCombo("Hand material", &Vars::Chams::DME::Hands.m_Var, handsMaterial);
				ColorPickerL("Fresnel Hands Base", Colors::FresnelBaseHands); HelpMarker("What material to put on your viewmodels arms/hands");

				static std::vector handsProxyMaterial{
					"None",
					"Spectrum splattered",
					"Electro skulls",
					"Jazzy",
					"Frozen aurora",
					"Hana",
					"IDK",
					"Ghost thing",
					"Flames",
					"Spook wood",
					"Edgy",
					"Starlight serenity",
					"Fade"
				};
				WCombo("Hand proxy material", &Vars::Chams::DME::HandsProxySkin.m_Var, handsProxyMaterial); HelpMarker("Puts a cool looking animated skin on your hands");

				static std::vector dmeGlowMaterial{
					"None",
					"Fresnel Glow",
					"Wireframe Glow"
				};
				WCombo("Hand Glow", &Vars::Chams::DME::HandsGlowOverlay.m_Var, dmeGlowMaterial);
				ColorPickerL("Hand glow colour", Colors::HandsOverlay);

				static std::vector weaponMaterial{
					"Original",
					"Shaded",
					"Shiny",
					"Flat",
					"Wireframe shaded",
					"Wireframe shiny",
					"Wireframe flat",
					"Fresnel",
					"Brick",
					"What",
					"Wacky"
				};
				WCombo("Weapon material", &Vars::Chams::DME::Weapon.m_Var, weaponMaterial);
				ColorPickerL("Fresnel Weapons Base", Colors::FresnelBaseWeps); HelpMarker("What material to put on your viewmodels weapon");

				static std::vector weaponProxyMaterial{
					"None",
					"Spectrum splattered",
					"Electro skulls",
					"Jazzy",
					"Frozen aurora",
					"Hana",
					"IDK",
					"Ghost thing",
					"Flames",
					"Spook wood",
					"Edgy",
					"Starlight serenity",
					"Fade"
				};
				WCombo("Weapon proxy material", &Vars::Chams::DME::WeaponsProxySkin.m_Var, weaponProxyMaterial); HelpMarker("Puts a cool looking animated skin on your weapons");
				WCombo("Weapon Glow", &Vars::Chams::DME::WeaponGlowOverlay.m_Var, dmeGlowMaterial);
				ColorPickerL("Weapon glow colour", Colors::WeaponOverlay);
				MultiCombo({ "Hands", "Hands overlay", "Weapon", "Weapon overlay" }, { &Vars::Chams::DME::HandsRainbow.m_Var, &Vars::Chams::DME::HandsOverlayRainbow.m_Var, &Vars::Chams::DME::WeaponRainbow.m_Var, &Vars::Chams::DME::WeaponOverlayRainbow.m_Var }, "Rainbow DME###RainbowDMEChams");
				MultiCombo({ "Hand Overlay", "Weapon Overlay" }, { &Vars::Chams::DME::HandsOverlayPulse.m_Var, &Vars::Chams::DME::WeaponOverlayPulse.m_Var }, "Pulse Overlay###PulseDMEOverlay");
				HelpMarker("Rainbow DME chams");
				WSlider("Hands glow amount", &Vars::Chams::DME::HandsGlowAmount.m_Var, 150, 1, "%.0f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_ClampOnInput);
				WSlider("Weapon glow amount", &Vars::Chams::DME::WeaponGlowAmount.m_Var, 150, 1, "%.0f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_ClampOnInput);

				SectionTitle("Backtrack chams");
				WToggle("Backtrack chams", &Vars::Backtrack::BtChams::Enabled.m_Var); HelpMarker("Draws chams to show where a player is");
				ColorPickerL("Backtrack colour", Vars::Backtrack::BtChams::BacktrackColor);
				WToggle("Only draw last tick", &Vars::Backtrack::BtChams::LastOnly.m_Var); HelpMarker("Only draws the last tick (can save FPS)");
				WToggle("Enemy only", &Vars::Backtrack::BtChams::EnemyOnly.m_Var); HelpMarker("You CAN backtrack your teammates. (Whip, medigun)");

				static std::vector backtrackMaterial{
					"Shaded",
					"Shiny",
					"Flat",
					"Wireframe shaded",
					"Wireframe shiny",
					"Wireframe flat",
					"Fresnel",
					"Brick"
				};
				WCombo("Backtrack material", &Vars::Backtrack::BtChams::Material.m_Var, backtrackMaterial);
				WToggle("Ignore Z###BtIgnoreZ", &Vars::Backtrack::BtChams::IgnoreZ.m_Var); HelpMarker("Draws them through walls");

				SectionTitle("Fakelag chams");
				WToggle("Fakelag chams", &Vars::Misc::CL_Move::FakelagIndicator.m_Var); HelpMarker("Draws chams to show your fakelag position");
				ColorPickerL("Fakelag colour", Vars::Misc::CL_Move::FLGChams::FakelagColor);
				WCombo("Fakelag material", &Vars::Misc::CL_Move::FLGChams::Material.m_Var, backtrackMaterial);
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsPlayersCol3"))
			{
				SectionTitle("Glow Main");
				WToggle("Glow", &Vars::Glow::Main::Active.m_Var);
				WToggle("Stencil glow", &Vars::Glow::Main::Stencil.m_Var);
				if (!Vars::Glow::Main::Stencil.m_Var) { WSlider("Glow scale", &Vars::Glow::Main::Scale.m_Var, 1, 10, "%d", ImGuiSliderFlags_AlwaysClamp); }

				SectionTitle("Player Glow");
				WToggle("Player glow###PlayerGlowButton", &Vars::Glow::Players::Active.m_Var); HelpMarker("Player glow master switch");
				WToggle("Self glow###SelfGlow", &Vars::Glow::Players::ShowLocal.m_Var); HelpMarker("Draw glow on the local player");
				WToggle("Self rainbow glow###SelfGlowRainbow", &Vars::Glow::Players::LocalRainbow.m_Var); HelpMarker("Homosapien");
				WCombo("Ignore team###IgnoreTeamGlowp", &Vars::Glow::Players::IgnoreTeammates.m_Var, { "Off", "All", "Only friends" }); HelpMarker("Which teammates the glow will ignore drawing on");
				WToggle("Wearable glow###PlayerWearableGlow", &Vars::Glow::Players::Wearables.m_Var); HelpMarker("Will draw glow on player cosmetics");
				WToggle("Weapon glow###PlayerWeaponGlow", &Vars::Glow::Players::Weapons.m_Var); HelpMarker("Will draw glow on player weapons");
				WSlider("Glow alpha###PlayerGlowAlpha", &Vars::Glow::Players::Alpha.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
				WCombo("Glow colour###GlowColour", &Vars::Glow::Players::Color.m_Var, { "Team", "Health" }); HelpMarker("Which colour the glow will draw");
			} EndChild();

			EndTable();
		}
		break;
	}

	// Visuals: Building
	case VisualsTab::Buildings:
	{
		if (BeginTable("VisualsBuildingsTable", 3))
		{
			/* Column 1 */
			if (TableColumnChild("VisualsBuildingsCol1"))
			{
				SectionTitle("Building ESP");
				WToggle("Building ESP###BuildinGESPSwioifas", &Vars::ESP::Buildings::Active.m_Var); HelpMarker("Will draw useful information/indicators on buildings");
				WToggle("Ignore team buildings###BuildingESPIgnoreTeammates", &Vars::ESP::Buildings::IgnoreTeammates.m_Var); HelpMarker("Whether or not to draw ESP on your teams buildings");
				WToggle("Name ESP###BuildingNameESP", &Vars::ESP::Buildings::Name.m_Var); HelpMarker("Will draw the players name");
				WToggle("Custom Name Color", &Vars::ESP::Buildings::NameCustom.m_Var); HelpMarker("Custom color for name esp");
				if (Vars::ESP::Buildings::NameCustom.m_Var)
				{
					ColorPickerL("Name ESP Color", Vars::ESP::Buildings::NameColor);
				}
				WToggle("Name ESP box###BuildingNameESPBox", &Vars::ESP::Buildings::NameBox.m_Var); HelpMarker("Will draw a box around the buildings name to make it stand out");
				WToggle("Health bar###Buildinghelathbar", &Vars::ESP::Buildings::HealthBar.m_Var); HelpMarker("Will draw a bar visualizing how much health the building has");
				WToggle("Health text###buildinghealth", &Vars::ESP::Buildings::Health.m_Var); HelpMarker("Will draw the building's health, as well as its max health");
				WToggle("Building owner###Buildingowner", &Vars::ESP::Buildings::Owner.m_Var); HelpMarker("Shows who built the building");
				WToggle("Building level###Buildinglevel", &Vars::ESP::Buildings::Level.m_Var); HelpMarker("Will draw what level the building is");
				WToggle("Building condition###Buildingconditions", &Vars::ESP::Buildings::Cond.m_Var); HelpMarker("Will draw what conditions the building is under");
				WToggle("Teleporter exit direction###Buildingteleexitdir", &Vars::ESP::Buildings::TeleExitDir.m_Var); HelpMarker("Show teleporter exit direction arrow");
				ColorPickerL("Teleporter exit direction arrow color", Vars::ESP::Buildings::TeleExitDirColor);
				WToggle("Lines###buildinglines", &Vars::ESP::Buildings::Lines.m_Var); HelpMarker("Draws lines from the local players position to the buildings position");
				WCombo("Box###PBuildingBoxESP", &Vars::ESP::Buildings::Box.m_Var, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on buildings");
				WToggle("Dlights###PlayerDlights", &Vars::ESP::Buildings::Dlights.m_Var); HelpMarker("Will make buildings emit a dynamic light around them, although buildings can't move some I'm not sure that the lights are actually dynamic here...");
				WSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Buildings::DlightRadius.m_Var, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
				WSlider("ESP alpha###BuildingESPAlpha", &Vars::ESP::Buildings::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the ESP should be");
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsBuildingsCol2"))
			{
				static std::vector chamOptions{
						"Local",
						"Friends",
						"Enemies",
						"Teammates",
						"Target"
				};

				static int currentSelected = 0; // 0 - local, 1 - friends, 2 - enemy, 3 - team
				static std::vector pchamsMaterials{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel", "Plastic", "Custom" };

				SectionTitle("Building Chams");
				WToggle("Building chams###BuildingChamsBox", &Vars::Chams::Buildings::Active.m_Var); HelpMarker("Building chams master switch");

				WCombo("Config", &currentSelected, chamOptions);
				switch (currentSelected) // please find a better way to do this, i have tried so many things and i cant get it to work properly
				{
				case 0:
				{
					// Local
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Buildings::Local.chamsActive, &Vars::Chams::Buildings::Local.showObstructed }, "Options");
					WCombo("Material", &Vars::Chams::Buildings::Local.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the building");
					ColorPickerL("Fresnel base colour", Vars::Chams::Buildings::Local.fresnelBase);
					if (Vars::Chams::Buildings::Local.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::Buildings::Local.customMaterial);
					}
					break;
				}
				case 1:
				{
					// Friends
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Buildings::Friend.chamsActive, &Vars::Chams::Buildings::Friend.showObstructed }, "Options");
					WCombo("Material", &Vars::Chams::Buildings::Friend.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the building");
					ColorPickerL("Fresnel base colour", Vars::Chams::Buildings::Friend.fresnelBase);
					if (Vars::Chams::Buildings::Friend.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::Buildings::Friend.customMaterial);
					}
					break;
				}
				case 2:
				{
					// Enemy
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Buildings::Enemy.chamsActive, &Vars::Chams::Buildings::Enemy.showObstructed }, "Options");
					WCombo("Material", &Vars::Chams::Buildings::Enemy.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the building");
					ColorPickerL("Fresnel base colour", Vars::Chams::Buildings::Enemy.fresnelBase);
					if (Vars::Chams::Buildings::Enemy.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::Buildings::Enemy.customMaterial);
					}
					break;
				}
				case 3:
				{
					// Team
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Buildings::Team.chamsActive, &Vars::Chams::Buildings::Team.showObstructed, }, "Options");
					WCombo("Material", &Vars::Chams::Buildings::Team.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the building");
					ColorPickerL("Fresnel base colour", Vars::Chams::Buildings::Team.fresnelBase);
					if (Vars::Chams::Buildings::Team.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::Buildings::Team.customMaterial);
					}
					break;
				}
				case 4:
				{
					// Target
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Buildings::Target.chamsActive, &Vars::Chams::Buildings::Target.showObstructed, }, "Options");
					WCombo("Material", &Vars::Chams::Buildings::Target.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the building");
					ColorPickerL("Fresnel base colour", Vars::Chams::Buildings::Target.fresnelBase);
					if (Vars::Chams::Buildings::Target.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::Buildings::Target.customMaterial);
					}
					break;
				}
				}
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsBuildingsCol3"))
			{
				SectionTitle("Building Glow");
				WToggle("Building glow###BuildiongGlowButton", &Vars::Glow::Buildings::Active.m_Var);
				WToggle("Ignore team buildings###buildingglowignoreteams", &Vars::Glow::Buildings::IgnoreTeammates.m_Var);
				WSlider("Glow alpha###BuildingGlowAlpha", &Vars::Glow::Buildings::Alpha.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
				WCombo("Glow colour###GlowColourBuildings", &Vars::Glow::Buildings::Color.m_Var, { "Team", "Health" });
			} EndChild();

			EndTable();
		}
		break;
	}

	// Visuals: World
	case VisualsTab::World:
	{
		if (BeginTable("VisualsWorldTable", 3))
		{
			/* Column 1 */
			if (TableColumnChild("VisualsWorldCol1"))
			{
				SectionTitle("World ESP");
				WToggle("World ESP###WorldESPActive", &Vars::ESP::World::Active.m_Var); HelpMarker("World ESP master switch");
				WToggle("Healthpacks###WorldESPHealthPacks", &Vars::ESP::World::HealthText.m_Var); HelpMarker("Will draw ESP on healthpacks");
				ColorPickerL("Healthpack colour", Colors::Health); HelpMarker("Color for healthpack ESP");
				WToggle("Ammopacks###WorldESPAmmoPacks", &Vars::ESP::World::AmmoText.m_Var); HelpMarker("Will draw ESP on ammopacks");
				ColorPickerL("Ammopack colour", Colors::Ammo); HelpMarker("Color for ammopack ESP");
				WSlider("ESP alpha###WordlESPAlpha", &Vars::ESP::World::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the world ESP should be");
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsWorldCol2"))
			{
				SectionTitle("World Chams");
				WToggle("World chams###woldchamsbut", &Vars::Chams::World::Active.m_Var);

				static std::vector chamOptions{
					"Healthpacks",
					"Ammopacks",
					"Projectiles"
				};

				static int currentSelected = 0;
				static std::vector pchamsMaterials{ "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel", "Plastic", "Custom" };

				WCombo("Config", &currentSelected, chamOptions);
				switch (currentSelected) // please find a better way to do this, i have tried so many things and i cant get it to work properly
				{
				case 0:
				{
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::World::Health.chamsActive, &Vars::Chams::World::Health.showObstructed }, "Options");
					WCombo("Material", &Vars::Chams::World::Health.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to healthpacks");
					ColorPickerL("Fresnel base colour", Vars::Chams::World::Health.fresnelBase);
					if (Vars::Chams::World::Health.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::World::Health.customMaterial);
					}
					break;
				}
				case 1:
				{
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::World::Ammo.chamsActive, &Vars::Chams::World::Ammo.showObstructed }, "Options");
					WCombo("Material", &Vars::Chams::World::Ammo.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to ammopacks");
					ColorPickerL("Fresnel base colour", Vars::Chams::World::Ammo.fresnelBase);
					if (Vars::Chams::World::Ammo.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::World::Ammo.customMaterial);
					}
					break;
				}
				case 2:
				{
					MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::World::Projectiles.chamsActive, &Vars::Chams::World::Projectiles.showObstructed }, "Options");
					WCombo("Material", &Vars::Chams::World::Projectiles.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to projectiles");
					ColorPickerL("Fresnel base colour", Vars::Chams::World::Projectiles.fresnelBase);
					if (Vars::Chams::World::Projectiles.drawMaterial == 8)
					{
						MaterialCombo("Custom Material", &Vars::Chams::World::Projectiles.customMaterial);
					}
					WCombo("Team###WorldChamsProjectiles", &Vars::Chams::World::Projectilez.m_Var, { "All", "Enemy only" });
					break;

				}
				}
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsWorldCol3"))
			{
				SectionTitle("World Glow");
				WToggle("World glow###Worldglowbutton", &Vars::Glow::World::Active.m_Var);
				WToggle("Healthpacks###worldhealthpackglow", &Vars::Glow::World::Health.m_Var);
				WToggle("Ammopacks###worldammopackglow", &Vars::Glow::World::Ammo.m_Var);
				WCombo("Projectile glow###teamprojectileglow", &Vars::Glow::World::Projectiles.m_Var, { "Off", "All", "Only enemies" });
				WSlider("Glow alpha###WorldGlowAlpha", &Vars::Glow::World::Alpha.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			} EndChild();

			EndTable();
		}
		break;
	}

	// Visuals: Font
	case VisualsTab::Font:
	{
		if (BeginTable("VisualsFontTable", 3))
		{
			static std::vector fontFlagNames { "Italic", "Underline", "Strikeout", "Symbol", "Antialias", "Gaussian", "Rotary", "Dropshadow", "Additive", "Outline", "Custom" };
			static std::vector fontFlagValues {0x001, 0x002, 0x004, 0x008, 0x010, 0x020, 0x040, 0x080, 0x100, 0x200, 0x400 };

			/* Column 1 */
			if (TableColumnChild("VisualsFontCol1"))
			{
				SectionTitle("ESP Font");
				WInputText("Font name###espfontname", &Vars::Fonts::FONT_ESP::szName);
				WInputInt("Font height###espfontheight", &Vars::Fonts::FONT_ESP::nTall.m_Var);
				WInputInt("Font weight###espfontweight", &Vars::Fonts::FONT_ESP::nWeight.m_Var);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP::nFlags.m_Var, "Font flags###FONT_ESP");

				SectionTitle("Name Font");
				WInputText("Font name###espfontnamename", &Vars::Fonts::FONT_ESP_NAME::szName);
				WInputInt("Font height###espfontnameheight", &Vars::Fonts::FONT_ESP_NAME::nTall.m_Var);
				WInputInt("Font weight###espfontnameweight", &Vars::Fonts::FONT_ESP_NAME::nWeight.m_Var);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var, "Font flags###FONT_ESP_NAME");
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsFontCol2"))
			{
				SectionTitle("Condition Font");
				WInputText("Font name###espfontcondname", &Vars::Fonts::FONT_ESP_COND::szName);
				WInputInt("Font height###espfontcondheight", &Vars::Fonts::FONT_ESP_COND::nTall.m_Var);
				WInputInt("Font weight###espfontcondweight", &Vars::Fonts::FONT_ESP_COND::nWeight.m_Var);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP_COND::nFlags.m_Var, "Font flags###FONT_ESP_COND");

				SectionTitle("Pickup Font");
				WInputText("Font name###espfontpickupsname", &Vars::Fonts::FONT_ESP_PICKUPS::szName);
				WInputInt("Font height###espfontpickupsheight", &Vars::Fonts::FONT_ESP_PICKUPS::nTall.m_Var);
				WInputInt("Font weight###espfontpickupsweight", &Vars::Fonts::FONT_ESP_PICKUPS::nWeight.m_Var);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var, "Font flags###FONT_ESP_PICKUPS");
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsFontCol3"))
			{
				SectionTitle("Menu Font");
				WInputText("Font name###espfontnamenameneby", &Vars::Fonts::FONT_MENU::szName);
				WInputInt("Font height###espfontnameheightafsdfads", &Vars::Fonts::FONT_MENU::nTall.m_Var);
				WInputInt("Font weight###espfontnameweightasfdafsd", &Vars::Fonts::FONT_MENU::nWeight.m_Var);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_MENU::nFlags.m_Var, "Font flags###FONT_MENU");

				SectionTitle("Indicator Font");
				WInputText("Font name###espfontindicatorname", &Vars::Fonts::FONT_INDICATORS::szName);
				WInputInt("Font height###espfontindicatorheight", &Vars::Fonts::FONT_INDICATORS::nTall.m_Var);
				WInputInt("Font weight###espfontindicatorweight", &Vars::Fonts::FONT_INDICATORS::nWeight.m_Var);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_INDICATORS::nFlags.m_Var, "Font flags###FONT_INDICATORS");

				if (Button("Apply settings###fontapply"))
				{
					const Font_t fontEsp = {
						0x0,
						Vars::Fonts::FONT_ESP::szName.c_str(),
						Vars::Fonts::FONT_ESP::nTall.m_Var,
						Vars::Fonts::FONT_ESP::nWeight.m_Var,
						Vars::Fonts::FONT_ESP::nFlags.m_Var
					};
					const Font_t fontEspName = {
						0x0,
						Vars::Fonts::FONT_ESP_NAME::szName.c_str(),
						Vars::Fonts::FONT_ESP_NAME::nTall.m_Var,
						Vars::Fonts::FONT_ESP_NAME::nWeight.m_Var,
						Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var
					};
					const Font_t fontEspCond = {
						0x0,
						Vars::Fonts::FONT_ESP_COND::szName.c_str(),
						Vars::Fonts::FONT_ESP_COND::nTall.m_Var,
						Vars::Fonts::FONT_ESP_COND::nWeight.m_Var,
						Vars::Fonts::FONT_ESP_COND::nFlags.m_Var
					};
					const Font_t fontIndicator = {
						0x0,
						Vars::Fonts::FONT_INDICATORS::szName.c_str(),
						Vars::Fonts::FONT_INDICATORS::nTall.m_Var,
						Vars::Fonts::FONT_INDICATORS::nWeight.m_Var,
						Vars::Fonts::FONT_INDICATORS::nFlags.m_Var
					};
					const Font_t fontEspPickups = {
						0x0,
						Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(),
						Vars::Fonts::FONT_ESP_PICKUPS::nTall.m_Var,
						Vars::Fonts::FONT_ESP_PICKUPS::nWeight.m_Var,
						Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var
					};
					const Font_t fontMenu = {
						0x0,
						Vars::Fonts::FONT_MENU::szName.c_str(),
						Vars::Fonts::FONT_MENU::nTall.m_Var,
						Vars::Fonts::FONT_MENU::nWeight.m_Var,
						Vars::Fonts::FONT_MENU::nFlags.m_Var
					};

					const std::vector <Font_t> fonts = {
						fontEsp,
						fontEspName,
						fontEspCond,
						fontEspPickups,
						fontMenu,
						fontIndicator,
						{ 0x0, "Verdana", 18, 800, FONTFLAG_ANTIALIAS},
					};

					g_Draw.RemakeFonts(fonts);
				}
			} EndChild();

			EndTable();
		}
		break;
	}

		// Visuals: Misc
	case VisualsTab::Misc:
	{
		if (BeginTable("VisualsMiscTable", 2))
		{
			/* Column 1 */
			if (TableColumnChild("VisualsMiscCol1"))
			{
				SectionTitle("World & UI");
				WSlider("Field of view", &Vars::Visuals::FieldOfView.m_Var, 70, 150, "%d"); HelpMarker("How many degrees of field of vision you would like");
				WCombo("Vision modifiers", &Vars::Visuals::Vision.m_Var, { "Off", "Pyrovision", "Halloween", "Romevision" }); HelpMarker("Vision modifiers");
				MultiCombo({ "World", "Sky", "Prop Wireframe" }, { &Vars::Visuals::WorldModulation.m_Var, &Vars::Visuals::SkyModulation.m_Var, &Vars::Visuals::PropWireframe.m_Var }, "Modulations");
				HelpMarker("Select which types of modulation you want to enable");
				ColorPickerL("World modulation colour", Colors::WorldModulation);
				ColorPickerL("Sky modulation colour", Colors::SkyModulation, 1);
				ColorPickerL("Prop modulation colour", Colors::StaticPropModulation, 2);
				MultiCombo({ "Scope", "Zoom", "Disguises", "Taunts", "Interpolation", "View Punch", "MOTD", "Screen Effects", "Angle Forcing"}, {&Vars::Visuals::RemoveScope.m_Var, &Vars::Visuals::RemoveZoom.m_Var, &Vars::Visuals::RemoveDisguises.m_Var, &Vars::Visuals::RemoveTaunts.m_Var, &Vars::Misc::DisableInterpolation.m_Var, &Vars::Visuals::RemovePunch.m_Var, &Vars::Visuals::RemoveMOTD.m_Var, &Vars::Visuals::RemoveScreenEffects.m_Var, &Vars::Visuals::PreventForcedAngles.m_Var }, "Removals");
				HelpMarker("Select what you want to remove");
				MultiCombo({ "Aimbot Crosshair", "Render Proj Line", "Bullet Tracers", "Viewmodel Aimbot", "Weapon Sway", "Move sim line" }, { &Vars::Visuals::CrosshairAimPos.m_Var, &Vars::Visuals::AimPosSquare.m_Var, &Vars::Visuals::BulletTracer.m_Var, &Vars::Visuals::AimbotViewmodel.m_Var, &Vars::Visuals::ViewmodelSway.m_Var, &Vars::Visuals::MoveSimLine.m_Var }, "Misc");
				HelpMarker("What misc visual features should be run");
				ColorPickerL("Bullet tracer colour", Colors::BulletTracer);
				{
					static std::vector flagNames{ "Text", "Console", "Chat", "Party", "Auto-Cast", "Verbose"};
					static std::vector flagValues{ 1, 2, 4, 8, 16, 32 };
					MultiFlags(flagNames, flagValues, &Vars::Misc::VotingOptions.m_Var, "Vote Logger###VoteLoggingOptions");
				}
				MultiCombo({ "Damage Logs (Console)", "Damage Logs (Text)", "Damage Logs (Chat)", "Class Changes (Text)", "Class Changes (Chat)" }, { &Vars::Visuals::damageLoggerConsole.m_Var, &Vars::Visuals::damageLoggerText.m_Var, &Vars::Visuals::damageLoggerChat.m_Var, &Vars::Visuals::ChatInfoText.m_Var, &Vars::Visuals::ChatInfoChat.m_Var }, "Event Logging");
				HelpMarker("What & How should events be logged");
				ColorPickerL("GUI Notif Background", Colors::NotifBG);
				ColorPickerL("GUI Notif Outline", Colors::NotifOutline, 1);
				ColorPickerL("GUI Notif Colour", Colors::NotifText, 2);
				WSlider("GUI Notif Time", &Vars::Visuals::despawnTime.m_Var, 0.5f, 3.f, "%.1f");
				WCombo("Particle tracer", &Vars::Visuals::ParticleTracer.m_Var, { "Off", "Machina", "C.A.P.P.E.R", "Short Circuit", "Merasmus ZAP", "Merasmus ZAP Beam 2", "Big Nasty", "Distortion Trail", "Black Ink", "Custom" });
				if (Vars::Visuals::ParticleTracer.m_Var == 9)
				{
					WInputText("Custom Tracer", &Vars::Visuals::ParticleName); HelpMarker("If you want to use a custom particle tracer");
				}
				if (Vars::Visuals::BulletTracer.m_Var)
				{
					WToggle("Rainbow tracers", &Vars::Visuals::BulletTracerRainbow.m_Var); HelpMarker("Bullet tracer color will be dictated by a changing color");
				}
				if (Vars::Visuals::RemoveScope.m_Var)
				{
					WToggle("Noscope lines", &Vars::Visuals::ScopeLines.m_Var); HelpMarker("Will draw a custom overlay");
					ColorPickerL("Inner line color", Colors::NoscopeLines1);
					ColorPickerL("Outer line color", Colors::NoscopeLines2, 1);
				}
				WToggle("Pickup Timers", &Vars::Visuals::PickupTimers.m_Var); HelpMarker("Displays the respawn time of health and ammopacks");
				WToggle("Draw Hitboxes", &Vars::Aimbot::Global::showHitboxes.m_Var); HelpMarker("Shows client hitboxes for enemies once they are attacked (not bbox)");
				ColorPickerL("Hitbox matrix face colour", Colors::HitboxFace);
				ColorPickerL("Hitbox matrix edge colour", Colors::HitboxEdge, 1);
				WToggle("Clear Hitboxes", &Vars::Aimbot::Global::clearPreviousHitbox.m_Var); HelpMarker("Removes previous drawn hitboxes to mitigate clutter");
				WSlider("Hitbox Draw Time", &Vars::Aimbot::Global::hitboxTime.m_Var, 1, 5); HelpMarker("Removes previous drawn hitboxes after n seconds");
				WCombo("Spectator list", &Vars::Visuals::SpectatorList.m_Var, { "Off", "Draggable", "Static", "Static + Avatars" });
				WToggle("Killstreak weapon", &Vars::Misc::KillstreakWeapon.m_Var); HelpMarker("Enables the killstreak counter on any weapon");

				SectionTitle("Beams");
				{
					using namespace Vars::Visuals;

					WToggle("Enable beams", &Beans::Active.m_Var); HelpMarker("he loves beans?");
					WToggle("Rainbow beams", &Beans::Rainbow.m_Var);
					ColorPickerL("Beam colour", Beans::BeamColour);
					WToggle("Custom model", &Beans::UseCustomModel.m_Var);
					if (Beans::UseCustomModel.m_Var)
					{
						WInputText("Model", &Beans::Model);
					}
					WSlider("Beam lifespan", &Beans::Life.m_Var, 0.0f, 10.f);
					WSlider("Beam width", &Beans::Width.m_Var, 0.0f, 10.f);
					WSlider("Beam end width", &Beans::EndWidth.m_Var, 0.0f, 10.f);
					WSlider("Beam fade length", &Beans::FadeLength.m_Var, 0.0f, 30.f);
					WSlider("Beam amplitude", &Beans::Amplitude.m_Var, 0.0f, 10.f);
					WSlider("Beam brightness", &Beans::Brightness.m_Var, 0.0f, 255.f);
					WSlider("Beam speed", &Beans::Speed.m_Var, 0.0f, 5.f);
					WSlider("Segments", &Beans::segments.m_Var, 1, 10); //what are good values for this

					// TODO: Reward this ugly code
					{
						static std::vector flagNames { "STARTENTITY", "ENDENTITY","FADEIN","FADEOUT","SINENOISE","SOLID","SHADEIN","SHADEOUT","ONLYNOISEONCE","NOTILE","USE_HITBOXES","STARTVISIBLE","ENDVISIBLE","ISACTIVE","FOREVER","HALOBEAM","REVERSED", };
						static std::vector flagValues { 0x00000001, 0x00000002,0x00000004,0x00000008,0x00000010,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,0x00000400,0x00000800,0x00001000,0x00002000,0x00004000,0x00008000,0x00010000 };
						MultiFlags(flagNames, flagValues, &Beans::Flags.m_Var, "Beam Flags###BeamFlags");
					}
				}

				SectionTitle("Viewmodel Offset");
				WSlider("VM Off X", &Vars::Visuals::VMOffsets.x, -45.f, 45.f);
				WSlider("VM Off Y", &Vars::Visuals::VMOffsets.y, -45.f, 45.f);
				WSlider("VM Off Z", &Vars::Visuals::VMOffsets.z, -45.f, 45.f);
				WSlider("VM Roll", &Vars::Visuals::VMRoll.m_Var, -180, 180);

				SectionTitle("DT Indicator");
				WCombo("DT indicator style", &Vars::Misc::CL_Move::DTBarStyle.m_Var, { "Off", "Default", "Nitro", "Rijin" }); HelpMarker("Which style to do the bar style");
				ColorPickerL("DT charging right", Colors::DTBarIndicatorsCharging.endColour);
				ColorPickerL("DT charging left", Colors::DTBarIndicatorsCharging.startColour, 1);
				if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 3)
				{
					WSlider("DT Bar height###dtBHeightNitro", &Vars::Misc::CL_Move::DTBarScaleY.m_Var, 1, 25);
					ColorPickerL("DT charged right", Colors::DTBarIndicatorsCharged.endColour);
					ColorPickerL("DT charged left", Colors::DTBarIndicatorsCharged.startColour, 1);
					WSlider("DT Bar width###dtBWidthNitro", &Vars::Misc::CL_Move::DTBarScaleX.m_Var, 100, 1000);
				}
				else
				{
					WSlider("DT Bar height###dtBHeight", &Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var, 1, 30);
					ColorPickerL("DT charged right", Colors::DTBarIndicatorsCharged.endColour);
					ColorPickerL("DT charged left", Colors::DTBarIndicatorsCharged.startColour, 1);
					WSlider("DT Bar width###dtBWidth", &Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var, 1, 30);
				}

				SectionTitle("Attribute Changer");

				static std::vector unuEffects{
					"None",
					"Hot",
					"Isotope",
					"Cool",
					"Energy orb"
				};
				static std::vector unuEffects2{
					"None",
					"Hot",
					"Isotope",
					"Cool",
					"Energy orb"
				};

				if (WCombo("Unusual effect 1", &unuPrimary, unuEffects))
				{
					switch (unuPrimary)
					{
					case 0:
						Vars::Visuals::Skins::Particle.m_Var = 0;
						break;
					case 1:
						Vars::Visuals::Skins::Particle.m_Var = 701;
						break;
					case 2:
						Vars::Visuals::Skins::Particle.m_Var = 702;
						break;
					case 3:
						Vars::Visuals::Skins::Particle.m_Var = 703;
						break;
					case 4:
						Vars::Visuals::Skins::Particle.m_Var = 704;
						break;
					default:
						break;
					}
				}
				HelpMarker("The first unusual effect to be applied to the weapon");

				if (WCombo("Unusual effect 2", &unuSecondary, unuEffects2))
				{
					switch (unuSecondary)
					{
					case 0:
						Vars::Visuals::Skins::Effect.m_Var = 0;
						break;
					case 1:
						Vars::Visuals::Skins::Effect.m_Var = 701;
						break;
					case 2:
						Vars::Visuals::Skins::Effect.m_Var = 702;
						break;
					case 3:
						Vars::Visuals::Skins::Effect.m_Var = 703;
						break;
					case 4:
						Vars::Visuals::Skins::Effect.m_Var = 704;
						break;
					default:
						break;
					}
				}
				HelpMarker("The second unusual effect to be applied to the weapon");

				static std::vector sheens{
					"None",
					"Team shine",
					"Deadly daffodil",
					"Manndarin",
					"Mean green",
					"Agonizing emerald",
					"Villainous violet",
					"Hot rod"
				};
				WCombo("Sheen", &Vars::Visuals::Skins::Sheen.m_Var, sheens); HelpMarker("Which sheen to apply to the weapon");
				WToggle("Style override", &Vars::Visuals::Skins::Override.m_Var);

				if (Button("Apply", ImVec2(45, 20)))
				{
					g_AttributeChanger.ShouldSet = true;
				}
				SameLine();
				if (Button("Save", ImVec2(45, 20)))
				{
					g_AttributeChanger.ShouldSave = true;
				}
				SameLine();
				if (Button("Load", ImVec2(44, 20)))
				{
					g_AttributeChanger.ShouldLoad = true;
				}

				SectionTitle("Ragdoll effects");
				WToggle("Enemy only###RagdollEnemyOnly", &Vars::Visuals::RagdollEffects::EnemyOnly.m_Var); HelpMarker("Only runs it on enemies");
				MultiCombo({ "Burning", "Electrocuted", "Become ash", "Dissolve" }, { &Vars::Visuals::RagdollEffects::Burning.m_Var, &Vars::Visuals::RagdollEffects::Electrocuted.m_Var, &Vars::Visuals::RagdollEffects::BecomeAsh.m_Var, &Vars::Visuals::RagdollEffects::Dissolve.m_Var }, "Effects###RagdollEffects");
				HelpMarker("Ragdoll particle effects");
				WCombo("Ragdoll model", &Vars::Visuals::RagdollEffects::RagdollType.m_Var, { "None", "Gold", "Ice" }); HelpMarker("Which ragdoll model should be used");
				HelpMarker("Will make their ragdoll ice");

				SectionTitle("Freecam");
				InputKeybind("Freecam Key", Vars::Visuals::FreecamKey);  HelpMarker("Allows you to freely move your camera when holding the key");
				WSlider("Freecam Speed", &Vars::Visuals::FreecamSpeed.m_Var, 1.f, 20.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Movement speed of freecam");

				SectionTitle("Camera");
				WCombo("Camera mode", &Vars::Visuals::CameraMode.m_Var, { "Off", "Mirror", "Spy", "Teleporter", "Teleporter (Portal)"}); HelpMarker("What the camera should display");
				WSlider("Camera FOV", &Vars::Visuals::CameraFOV.m_Var, 40.f, 130.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("FOV of the camera window");
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsMiscCol2"))
			{
				SectionTitle("Skybox & Textures");
				static std::vector skyNames{
					"Custom",
					"sky_tf2_04",
					"sky_upward",
					"sky_dustbowl_01",
					"sky_goldrush_01",
					"sky_granary_01",
					"sky_well_01",
					"sky_gravel_01",
					"sky_badlands_01",
					"sky_hydro_01",
					"sky_night_01",
					"sky_nightfall_01",
					"sky_trainyard_01",
					"sky_stormfront_01",
					"sky_morningsnow_01",
					"sky_alpinestorm_01",
					"sky_harvest_01",
					"sky_harvest_night_01",
					"sky_halloween",
					"sky_halloween_night_01",
					"sky_halloween_night2014_01",
					"sky_island_01",
					"sky_rainbow_01"
				};
				WToggle("Skybox changer", &Vars::Visuals::SkyboxChanger.m_Var); HelpMarker("Will change the skybox, either to a base TF2 one or a custom one");
				WCombo("Skybox", &Vars::Skybox::SkyboxNum, skyNames);
				if (Vars::Skybox::SkyboxNum == 0)
				{
					WInputText("Custom skybox name", &Vars::Skybox::SkyboxName); HelpMarker("Name of the skybox you want to you (tf/materials/skybox)");
				}
				WToggle("World Textures Override", &Vars::Visuals::OverrideWorldTextures.m_Var); HelpMarker("Turn this off when in-game so you don't drop fps :p");
				WToggle("Bypass sv_pure", &Vars::Misc::BypassPure.m_Var); HelpMarker("Allows you to load any custom files, even if disallowed by the sv_pure setting");
				WToggle("Medal flip", &Vars::Misc::MedalFlip.m_Var); HelpMarker("Medal go spinny spinny weeeeeee");
				WCombo("Precipitation", &Vars::Visuals::Rain.m_Var, { "Off", "Rain", "Snow" });

				SectionTitle("Custom fog");
				if (WToggle("Custom fog", &Vars::Visuals::Fog::CustomFog.m_Var))
				{
					if (static auto fog_enable = I::CVars->FindVar("fog_enable"); fog_enable)
					{
						fog_enable->SetValue(Vars::Visuals::Fog::CustomFog.m_Var);
					}
					if (static auto fog_enableskybox = I::CVars->FindVar("fog_enableskybox"); fog_enableskybox)
					{
						fog_enableskybox->SetValue(Vars::Visuals::Fog::CustomFog.m_Var);
					}
					if (static auto fog_override = I::CVars->FindVar("fog_override"); fog_override)
					{
						fog_override->SetValue(Vars::Visuals::Fog::CustomFog.m_Var);
					}
				}

				if (WSlider("Fog density", &Vars::Visuals::Fog::FogDensity.m_Var, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
				{
					if (static auto fog_density = I::CVars->FindVar("fog_maxdensity"); fog_density)
					{
						fog_density->SetValue(Vars::Visuals::Fog::FogDensity.m_Var);
					}

				}
				if (ColorPickerL("Fog colour", Vars::Visuals::Fog::FogColor))
				{
					if (static auto fog_color = I::CVars->FindVar("fog_color"); fog_color)
					{
						fog_color->SetValue(std::string("").
							append(std::to_string(Vars::Visuals::Fog::FogColor.r)).
							append(" ").
							append(std::to_string(Vars::Visuals::Fog::FogColor.g)).
							append(" ").
							append(std::to_string(Vars::Visuals::Fog::FogColor.b)).
							append(" ").c_str());
					}
				}

				if (WSlider("Fog start", &Vars::Visuals::Fog::FogStart.m_Var, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None))
				{
					if (static auto fog_start = I::CVars->FindVar("fog_start"); fog_start)
					{
						fog_start->SetValue(Vars::Visuals::Fog::FogStart.m_Var);
					}
				}
				if (WSlider("Fog end", &Vars::Visuals::Fog::FogEnd.m_Var, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None))
				{
					if (static auto fog_end = I::CVars->FindVar("fog_end"); fog_end)
					{
						fog_end->SetValue(Vars::Visuals::Fog::FogEnd.m_Var);
					}
				}
				if (WSlider("Skybox fog density", &Vars::Visuals::Fog::FogDensitySkybox.m_Var, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
				{
					if (static auto fog_density = I::CVars->FindVar("fog_maxdensityskybox"); fog_density)
					{
						fog_density->SetValue(Vars::Visuals::Fog::FogDensitySkybox.m_Var);
					}
				}

				if (ColorPickerL("Skybox fog colour", Vars::Visuals::Fog::FogColorSkybox))
				{
					if (static auto fog_colorskybox = I::CVars->FindVar("fog_colorskybox"); fog_colorskybox)
					{
						fog_colorskybox->SetValue(std::string("").
							append(std::to_string(Vars::Visuals::Fog::FogColorSkybox.r)).
							append(" ").
							append(std::to_string(Vars::Visuals::Fog::FogColorSkybox.g)).
							append(" ").
							append(std::to_string(Vars::Visuals::Fog::FogColorSkybox.b)).
							append(" ").c_str());
					}
				}

				if (WSlider("Skybox fog start", &Vars::Visuals::Fog::FogStart.m_Var, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None))
				{
					if (static auto fog_start = I::CVars->FindVar("fog_startskybox"); fog_start)
					{
						fog_start->SetValue(Vars::Visuals::Fog::FogStartSkybox.m_Var);
					}
				}
				if (WSlider("Skybox fog end", &Vars::Visuals::Fog::FogEndSkybox.m_Var, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None))
				{
					if (static auto fog_end = I::CVars->FindVar("fog_endskybox"); fog_end)
					{
						fog_end->SetValue(Vars::Visuals::Fog::FogEndSkybox.m_Var);
					}
				}

				SectionTitle("Thirdperson");
				WToggle("Thirdperson", &Vars::Visuals::ThirdPerson.m_Var); HelpMarker("Will move your camera to be in a thirdperson view");
				InputKeybind("Thirdperson key", Vars::Visuals::ThirdPersonKey); HelpMarker("What key to toggle thirdperson, press ESC if no bind is desired");
				WToggle("Show real angles###tpRealAngles", &Vars::Visuals::ThirdPersonSilentAngles.m_Var); HelpMarker("Will show your real angles on thirdperson (not what others see)");
				WToggle("Instant yaw###tpInstantYaw", &Vars::Visuals::ThirdPersonInstantYaw.m_Var); HelpMarker("Will set your yaw instantly in thirdperson, showing your actual angle, instead of what others see");
				WToggle("Show server hitboxes (localhost only)###tpShowServer", &Vars::Visuals::ThirdPersonServerHitbox.m_Var); HelpMarker("Will show the server angles in thirdperson");

				WToggle("Thirdperson offsets", &Vars::Visuals::ThirdpersonOffset.m_Var); HelpMarker("These will mess you up if you use a small FoV");
				WSlider("Thirdperson distance", &Vars::Visuals::ThirdpersonDist.m_Var, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
				WSlider("Thirdperson right", &Vars::Visuals::ThirdpersonRight.m_Var, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
				WSlider("Thirdperson up", &Vars::Visuals::ThirdpersonUp.m_Var, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
				WToggle("Thirdperson crosshair", &Vars::Visuals::ThirdpersonCrosshair.m_Var);
				WToggle("Offset with arrow keys", &Vars::Visuals::ThirdpersonOffsetWithArrows.m_Var);
				InputKeybind("Move offset key", Vars::Visuals::ThirdpersonArrowOffsetKey, false);

				SectionTitle("Out of FOV arrows");
				WToggle("Active###fovar", &Vars::Visuals::OutOfFOVArrows.m_Var); HelpMarker("Will draw arrows to players who are outside of the range of your FoV");
				WToggle("Outline arrows###OutlinedArrows", &Vars::Visuals::OutOfFOVArrowsOutline.m_Var); HelpMarker("16 missed calls");
				WSlider("Arrow length", &Vars::Visuals::ArrowLength.m_Var, 5.f, 50.f, "%.2f"); HelpMarker("How long the arrows are");
				WSlider("Arrow angle", &Vars::Visuals::ArrowAngle.m_Var, 5.f, 180.f, "%.2f"); HelpMarker("The angle of the arrow");
				WSlider("Distance from center", &Vars::Visuals::FovArrowsDist.m_Var, 0.01f, 0.2f, "%.3f"); HelpMarker("How far from the center of the screen the arrows will draw");
				WSlider("Max distance", &Vars::Visuals::MaxDist.m_Var, 0.f, 4000.f, "%.2f"); HelpMarker("How far until the arrows will not show");
				WSlider("Min distance", &Vars::Visuals::MinDist.m_Var, 0.f, 1000.f, "%.2f"); HelpMarker("How close until the arrows will be fully opaque");

				SectionTitle("Spy Warning");
				WToggle("Active###spywarn", &Vars::Visuals::SpyWarning.m_Var); HelpMarker("Will alert you when spies with their knife out may attempt to backstab you");
				WToggle("Voice command###spywarn1", &Vars::Visuals::SpyWarningAnnounce.m_Var); HelpMarker("Will make your character say \"Spy!\" when a spy is detected");
				WToggle("Visible only###spywarn2", &Vars::Visuals::SpyWarningVisibleOnly.m_Var); HelpMarker("Will only alert you to visible spies");
				WToggle("Ignore friends###spywarn3", &Vars::Visuals::SpyWarningIgnoreFriends.m_Var); HelpMarker("Will ignore spies who are on your friends list");
				WCombo("Warning style", &Vars::Visuals::SpyWarningStyle.m_Var, { "Arrow", "Flash" }); HelpMarker("Choose the style of the spy indicator");
			} EndChild();

			EndTable();
		}
		break;
	}

	case VisualsTab::Radar:
	{
		if (BeginTable("VisualsRadarTable", 3))
		{
			/* Column 1 */
			if (TableColumnChild("VisualsRadarCol1"))
			{
				SectionTitle("Main");
				WToggle("Enable Radar###RadarActive", &Vars::Radar::Main::Active.m_Var); HelpMarker("Will show nearby things relative to your player");
				WSlider("Size###RadarSize", &Vars::Radar::Main::Size.m_Var, 20, 200); HelpMarker("The size of the radar window");
				WSlider("Range###RadarRange", &Vars::Radar::Main::Range.m_Var, 50, 3000, "%d"); HelpMarker("The range of the radar");
				WSlider("Background alpha###RadarBGA", &Vars::Radar::Main::BackAlpha.m_Var, 0, 255, "%d"); HelpMarker("The background alpha of the radar");
				WSlider("Line alpha###RadarLineA", &Vars::Radar::Main::LineAlpha.m_Var, 0, 255, "%d"); HelpMarker("The line alpha of the radar");

				SectionTitle("Players");
				WCombo("Icon###radari", &Vars::Radar::Players::IconType.m_Var, { "Scoreboard", "Portraits", "Avatar" }); HelpMarker("What sort of icon to represent players with");
				WCombo("Background###radarb", &Vars::Radar::Players::BackGroundType.m_Var, { "Off", "Rectangle", "Texture" }); HelpMarker("What sort of background to put on players on the radar");
				WToggle("Outline###radaro", &Vars::Radar::Players::Outline.m_Var); HelpMarker("Will put an outline on players on the radar");
				WCombo("Ignore teammates###radarplayersteam", &Vars::Radar::Players::IgnoreTeam.m_Var, { "Off", "All", "Keep friends" }); HelpMarker("Which teammates the radar will ignore drawing on");
				WCombo("Ignore cloaked###radarplayerscloaked", &Vars::Radar::Players::IgnoreCloaked.m_Var, { "Off", "All", "Keep friends" }); HelpMarker("Which cloaked players the radar will ignore drawing on");
				WToggle("Health bar###radarhealt", &Vars::Radar::Players::Health.m_Var); HelpMarker("Will show players health on the radar");
				WSlider("Icon size###playersizeiconradar", &Vars::Radar::Players::IconSize.m_Var, 12, 30, "%d"); HelpMarker("The icon size of players on the radar");
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsRadarCol2"))
			{
				SectionTitle("Building");
				WToggle("Show buildings###radarbuildingsa", &Vars::Radar::Buildings::Active.m_Var);
				WToggle("Outline###radarbuildingsao", &Vars::Radar::Buildings::Outline.m_Var);
				WToggle("Ignore team###radarbuildingsb", &Vars::Radar::Buildings::IgnoreTeam.m_Var);
				WToggle("Health bar###radarbuildingsc", &Vars::Radar::Buildings::Health.m_Var);
				WSlider("Icon size###buildingsizeiconradar", &Vars::Radar::Buildings::IconSize.m_Var, 12, 30, "%d");
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsRadarCol3"))
			{
				SectionTitle("World");
				WToggle("Active###radarworldd", &Vars::Radar::World::Active.m_Var);
				WToggle("Healthpack###radarworldda", &Vars::Radar::World::Health.m_Var);
				WToggle("Ammopack###radarworlddb", &Vars::Radar::World::Ammo.m_Var);
				WSlider("Icon size###worldsizeiconradar", &Vars::Radar::World::IconSize.m_Var, 12, 30, "%d");
			} EndChild();

			EndTable();
		}
		break;
	}
	}
}

/* Tab: HvH */
void CMenu::MenuHvH()
{
	using namespace ImGui;
	if (BeginTable("HvHTable", 2))
	{
		/* Column 1 */
		if (TableColumnChild("HvHCol1"))
		{
			/* Section: Tickbase Exploits */
			SectionTitle("Tickbase Exploits");
			WToggle("Doubletap", &Vars::Misc::CL_Move::Enabled.m_Var); HelpMarker("Shifts ticks when shooting for a rapid-fire effect");
			ColorPickerL("DT bar outline colour", Colors::DtOutline);
			InputKeybind("Recharge key", Vars::Misc::CL_Move::RechargeKey); HelpMarker("Recharges ticks for shifting");
			InputKeybind("Teleport key", Vars::Misc::CL_Move::TeleportKey); HelpMarker("Shifts ticks to warp");
			if (Vars::Misc::CL_Move::DTMode.m_Var == 0 || Vars::Misc::CL_Move::DTMode.m_Var == 2)
			{
				InputKeybind("Doubletap key", Vars::Misc::CL_Move::DoubletapKey); HelpMarker("Only doubletap when the key is pressed. Leave as (None) for always active.");
			}

			WCombo("Teleport Mode", &Vars::Misc::CL_Move::TeleportMode.m_Var, { "Plain", "Smooth" }); HelpMarker("How the teleport should be done");
			MultiCombo({ "Recharge While Dead", "Auto Recharge", "Wait for DT", "Anti-warp", "Avoid airborne" }, { &Vars::Misc::CL_Move::RechargeWhileDead.m_Var, &Vars::Misc::CL_Move::AutoRecharge.m_Var, &Vars::Misc::CL_Move::WaitForDT.m_Var, &Vars::Misc::CL_Move::AntiWarp.m_Var, &Vars::Misc::CL_Move::NotInAir.m_Var }, "Options");
			HelpMarker("Enable various features regarding tickbase exploits");
			WCombo("DT Mode", &Vars::Misc::CL_Move::DTMode.m_Var, { "On key", "Always", "Disable on key", "Disabled" }); HelpMarker("How should DT behave");
			WSlider("Ticks to shift", &Vars::Misc::CL_Move::DTTicks.m_Var, 1, 24, "%d"); HelpMarker("How many ticks to shift");
			WToggle("SpeedHack", &Vars::Misc::CL_Move::SEnabled.m_Var); HelpMarker("Speedhack Master Switch");
			if (Vars::Misc::CL_Move::SEnabled.m_Var)
			{
				WSlider("SpeedHack Factor", &Vars::Misc::CL_Move::SFactor.m_Var, 1, 66, "%d");
			}
			HelpMarker("High values are not recommended");

			/* Section: Fakelag */
			SectionTitle("Fakelag");
			WToggle("Enable Fakelag", &Vars::Misc::CL_Move::Fakelag.m_Var);
			WCombo("Fakelag Mode###FLmode", &Vars::Misc::CL_Move::FakelagMode.m_Var, { "Plain", "Random", "Velocity Based" }); HelpMarker("Controls how fakelag will be controlled.");

			if (Vars::Misc::CL_Move::FakelagMode.m_Var == 0 || Vars::Misc::CL_Move::FakelagMode.m_Var == 2)
			{
				WSlider("Fakelag value", &Vars::Misc::CL_Move::FakelagValue.m_Var, 1, 22, "%d"); HelpMarker("How much lag you should fake(?)");
				if (Vars::Misc::CL_Move::FakelagMode.m_Var == 0)
				{
					WToggle("Fakelag on key", &Vars::Misc::CL_Move::FakelagOnKey.m_Var); HelpMarker("Fakelag will only activate when an assigned key is held");
					if (Vars::Misc::CL_Move::FakelagOnKey.m_Var)
					{
						InputKeybind("Fakelag key", Vars::Misc::CL_Move::FakelagKey); HelpMarker("The key to activate fakelag as long as it's held");
					}
				}
			}
			if (Vars::Misc::CL_Move::FakelagMode.m_Var == 1)
			{
				WSlider("Random max###flRandMax", &Vars::Misc::CL_Move::FakelagMax.m_Var, Vars::Misc::CL_Move::FakelagMin.m_Var + 1, 22, "%d"); HelpMarker("Maximum random fakelag value");
				WSlider("Random min###flRandMin", &Vars::Misc::CL_Move::FakelagMin.m_Var, 1, Vars::Misc::CL_Move::FakelagMax.m_Var - 1, "%d"); HelpMarker("Minimum random fakelag value");
			}
		} EndChild();

		/* Column 2 */
		if (TableColumnChild("HvHCol2"))
		{
			/* Section: Anti Aim */
			SectionTitle("Anti Aim");
			WToggle("Enable Anti-aim", &Vars::AntiHack::AntiAim::Active.m_Var);
			InputKeybind("Anti-aim Key", Vars::AntiHack::AntiAim::ToggleKey); HelpMarker("The key to toggle anti aim");
			WCombo("Pitch", &Vars::AntiHack::AntiAim::Pitch.m_Var, { "None", "Zero", "Up", "Down", "Fake up", "Fake down", "Random" }); HelpMarker("Which way to look up/down");
			WCombo("Real yaw", &Vars::AntiHack::AntiAim::YawReal.m_Var, { "None", "Forward", "Left", "Right", "Backwards", "Random", "Spin", "Edge", "On Hurt" }); HelpMarker("Which way to look horizontally");
			WCombo("Fake yaw", &Vars::AntiHack::AntiAim::YawFake.m_Var, { "None", "Forward", "Left", "Right", "Backwards", "Random", "Spin", "Edge", "On Hurt" }); HelpMarker("Which way to appear to look horizontally");
			if (Vars::AntiHack::AntiAim::YawFake.m_Var == 6 || Vars::AntiHack::AntiAim::YawReal.m_Var == 6)
			{
				WSlider("Spin Speed", &Vars::AntiHack::AntiAim::SpinSpeed.m_Var, -30.f, 30.f, "%.1f", 0); HelpMarker("You spin me right 'round, baby, right 'round");
			}
			if (Vars::AntiHack::AntiAim::Pitch.m_Var == 6 || Vars::AntiHack::AntiAim::YawFake.m_Var == 5 || Vars::AntiHack::AntiAim::YawReal.m_Var == 5)
			{
				WSlider("Random Interval", &Vars::AntiHack::AntiAim::RandInterval.m_Var, 0, 100, "%d"); HelpMarker("How often the random Anti-Aim should update");
			}
			WToggle("Resolver", &Vars::AntiHack::Resolver::Resolver.m_Var); HelpMarker("Enables Anti-aim resolver in the playerlist");
			MultiCombo({ "AntiOverlap", "Jitter Legs", "HidePitchOnShot", "Anti-Backstab"}, { &Vars::AntiHack::AntiAim::AntiOverlap.m_Var, &Vars::AntiHack::AntiAim::legjitter.m_Var, &Vars::AntiHack::AntiAim::invalidshootpitch.m_Var, &Vars::AntiHack::AntiAim::AntiBackstab.m_Var }, "Misc.");

			/* Section: Auto Peek */
			SectionTitle("Auto Peek");
			InputKeybind("Autopeek Key", Vars::Misc::CL_Move::AutoPeekKey); HelpMarker("Hold this key while peeking and use A/D to set the peek direction");
			WSlider("Max Distance", &Vars::Misc::CL_Move::AutoPeekDistance.m_Var, 50.f, 400.f, "%.0f"); HelpMarker("Maximum distance that auto peek can walk");
			WToggle("Free move", &Vars::Misc::CL_Move::AutoPeekFree.m_Var); HelpMarker("Allows you to move freely while peeking");
		} EndChild();

		EndTable();
	}
}

/* Tab: Misc */
void CMenu::MenuMisc()
{
	using namespace ImGui;
	if (BeginTable("MiscTable", 3))
	{
		/* Column 1 */
		if (TableColumnChild("MiscCol1"))
		{
			SectionTitle("Automation");
			WToggle("No push", &Vars::Misc::NoPush.m_Var); HelpMarker("Will make teammates unable to push you around");
			WToggle("Quick stop", &Vars::Misc::AccurateMovement.m_Var); HelpMarker("Will stop you from sliding once you stop pressing movement buttons");
			WToggle("Duck Jump", &Vars::Misc::DuckJump.m_Var); HelpMarker("Will duck when bhopping");
			WToggle("Bunnyhop", &Vars::Misc::AutoJump.m_Var); HelpMarker("Will jump as soon as you touch the ground again, keeping speed between jumps");
			if (Vars::Misc::AutoJump.m_Var)
			{
				WCombo("Autostrafe", &Vars::Misc::AutoStrafe.m_Var, { "Off", "Legit", "Directional" }); HelpMarker("Will strafe for you in air automatically so that you gain speed");
			}
			WToggle("Edge jump", &Vars::Misc::EdgeJump.m_Var); HelpMarker("Will jump at the very end of whatever platform you're on, allowing you to perfectly make longer jumps.");
			if (Vars::Misc::EdgeJump.m_Var)
			{
				InputKeybind("Edge jump key", Vars::Misc::EdgeJumpKey, true);  HelpMarker("Edge jump bind, leave as None for always on");
			}
			WToggle("Auto rocket jump", &Vars::Misc::AutoRocketJump.m_Var); HelpMarker("Will rocket jump at the angle you're looking at when you press mouse2 with a rocket launcher");
			WToggle("Anti-AFK", &Vars::Misc::AntiAFK.m_Var); HelpMarker("Will make you jump every now and then so you don't get kicked for idling");
			WToggle("Taunt slide", &Vars::Misc::TauntSlide.m_Var); HelpMarker("Allows you to input in taunts");
			WToggle("Taunt control", &Vars::Misc::TauntControl.m_Var); HelpMarker("Gives full control if enabled with taunt slide");
			WCombo("Crouch speed", &Vars::Misc::Roll.m_Var, { "Off", "Backwards", "Fake forward" }); HelpMarker("Allows you to go at normal walking speed when crouching (affects many things, use with caution)");
			WCombo("Pick Class", &Vars::Misc::AutoJoin.m_Var, { "Off", "Scout", "Soldier", "Pyro", "Demoman", "Heavy", "Engineer", "Medic", "Sniper", "Spy" }); HelpMarker("Automatically joins the given class");
			WToggle("Rage retry", &Vars::Misc::RageRetry.m_Var); HelpMarker("Will automatically reconnect when your health is low");
			if (Vars::Misc::RageRetry.m_Var)
			{
				WSlider("Rage Retry health", &Vars::Misc::RageRetryHealth.m_Var, 1, 99, "%d%%"); HelpMarker("Minimum health percentage that will cause a retry");
			}
			WToggle("Pseudo Spectator", &Vars::Misc::ExtendFreeze.m_Var); HelpMarker("Causes an infinite respawn/spectator time");

		} EndChild();

		/* Column 2 */
		if (TableColumnChild("MiscCol2"))
		{
			SectionTitle("Chat");
			WToggle("Chat Censor", &Vars::Misc::ChatCensor.m_Var); HelpMarker("Clears the chat when someone accuses your");
			WToggle("Allow Newlines", &Vars::Misc::ChatNL.m_Var); HelpMarker("Allows you to use \\n in the chat");
			WCombo("Chat spam", &Vars::Misc::ChatSpam.m_Var, { "Off", "Fedoraware", "Lmaobox", "Cathook" });

			SectionTitle("Exploits");
			WToggle("Anti Autobalance", &Vars::Misc::AntiAutobal.m_Var); HelpMarker("Prevents auto balance by reconnecting to the server");
			WToggle("sv_cheats Bypass", &Vars::Misc::CheatsBypass.m_Var); HelpMarker("Allows you to use some sv_cheats commands (clientside)");
			WToggle("Noisemaker Spam", &Vars::Misc::NoisemakerSpam.m_Var); HelpMarker("Spams the noisemaker without reducing it's charges");
			//WToggle("Cat identify", &Vars::Misc::BeCat.m_Var); HelpMarker("Will mark you as a cathook instance to other cathook instances (basically catbots)");
			WToggle("Ping reducer", &Vars::Misc::PingReducer.m_Var); HelpMarker("Reduces your ping on the scoreboard");
			if (Vars::Misc::PingReducer.m_Var)
			{
				WSlider("Target ping", &Vars::Misc::PingTarget.m_Var, 0, 200); HelpMarker("Target ping that should be reached");
			}

			SectionTitle("Party Networking");
			WToggle("Enable###PartyNetEnable", &Vars::Misc::PartyNetworking.m_Var); HelpMarker("Enables party networking between Fedoraware users");
			WToggle("Party crasher###PartyNetCrash", &Vars::Misc::PartyCrasher.m_Var); HelpMarker("Annoy your friends by crashing their game");
			InputKeybind("Party marker", Vars::Misc::PartyMarker, true);  HelpMarker("Sends a marker to other Fedoraware users in your party");
			WToggle("Party ESP###PartyNetESP", &Vars::Misc::PartyESP.m_Var); HelpMarker("Sends player locations to your party members");

			SectionTitle("Followbot");
			WToggle("Enable Followbot###FollowbotEnable", &Vars::Misc::Followbot::Enabled.m_Var); HelpMarker("Follows a player around.");
			WToggle("Friends only###FollowbotFriends", &Vars::Misc::Followbot::FriendsOnly.m_Var); HelpMarker("Only follow friends");
			WSlider("Follow Distance###FollowbotDistance", &Vars::Misc::Followbot::Distance.m_Var, 50.f, 400.f, "%.0f"); HelpMarker("How close we should follow the target");
		} EndChild();

		/* Column 3 */
		if (TableColumnChild("MiscCol3"))
		{
			SectionTitle("Discord RPC");
			WToggle("Discord RPC", &Vars::Misc::Discord::EnableRPC.m_Var); HelpMarker("Enable Discord Rich Presence");
			WToggle("Include map", &Vars::Misc::Discord::IncludeMap.m_Var); HelpMarker("Should Discord Rich Presence contain current map name?");
			WToggle("Include class", &Vars::Misc::Discord::IncludeClass.m_Var); HelpMarker("Should Discord Rich Presence contain current class?");
			WToggle("Include timestamp", &Vars::Misc::Discord::IncludeTimestamp.m_Var); HelpMarker("Should time since you started playing TF2 be included?");
			WCombo("Image Options", &Vars::Misc::Discord::WhatImagesShouldBeUsed.m_Var, { "Big fedora + Small TF2", "Big TF2 + Small fedora" });

			SectionTitle("Steam RPC");
			WToggle("Steam RPC", &Vars::Misc::Steam::EnableRPC.m_Var); HelpMarker("Enable Steam Rich Presence"); HelpMarker("Enable Steam Rich Presence");
			WCombo("Match group", &Vars::Misc::Steam::MatchGroup.m_Var, { "Special Event", "MvM Mann Up", "Competitive", "Casual", "MvM Boot Camp" }); HelpMarker("Which match group should be used?");
			WToggle("Override in menu", &Vars::Misc::Steam::OverrideMenu.m_Var); HelpMarker("Override match group to \"Main Menu\" when in main menu");
			WCombo("Map text", &Vars::Misc::Steam::MapText.m_Var, { "Custom", "Fedoraware", "Figoraware", "Meowhook.club", "Rathook.cc", "Nitro.tf" }); HelpMarker("Which map text should be used?");
			if (Vars::Misc::Steam::MapText.m_Var == 0)
			{
				WInputText("Custom map text", &Vars::Misc::Steam::CustomText.m_Var); HelpMarker(R"(For when "Custom" is selcted in "Map text". Sets custom map text.)");
			}
			WInputInt("Group size", &Vars::Misc::Steam::GroupSize.m_Var); HelpMarker("Sets party size");

			SectionTitle("Utilities");
			const auto btnWidth = GetWindowSize().x - 2 * GetStyle().WindowPadding.x;
			if (Button("Full update", ImVec2(btnWidth, 20)))
				I::Engine->ClientCmd_Unrestricted("cl_fullupdate");
			if (Button("Reload HUD", ImVec2(btnWidth, 20)))
				I::Engine->ClientCmd_Unrestricted("hud_reloadscheme");
			if (Button("Restart sound", ImVec2(btnWidth, 20)))
				I::Engine->ClientCmd_Unrestricted("snd_restart");
			if (Button("Stop sound", ImVec2(btnWidth, 20)))
				I::Engine->ClientCmd_Unrestricted("stopsound");
			if (Button("Status", ImVec2(btnWidth, 20)))
				I::Engine->ClientCmd_Unrestricted("status");
			if (Button("Ping", ImVec2(btnWidth, 20)))
				I::Engine->ClientCmd_Unrestricted("ping");
			if (Button("Retry", ImVec2(btnWidth, 20)))
				I::Engine->ClientCmd_Unrestricted("retry");
			if (Button("Exit", ImVec2(btnWidth, 20)))
				I::Engine->ClientCmd_Unrestricted("exit");
			if (Button("Console", ImVec2(btnWidth, 20)))
				I::Engine->ClientCmd_Unrestricted("showconsole");
			if (Button("Demo playback", ImVec2(btnWidth, 20)))
				I::Engine->ClientCmd_Unrestricted("demoui");
			if (Button("Demo trackbar", ImVec2(btnWidth, 20)))
				I::Engine->ClientCmd_Unrestricted("demoui2");
			if (Button("Itemtest", ImVec2(btnWidth, 20)))
				I::Engine->ClientCmd_Unrestricted("itemtest");

			if (Button("Unlock all achievements", ImVec2(btnWidth, 20)))
			{
				g_Misc.UnlockAchievements();
			}
			if (Button("Lock all achievements", ImVec2(btnWidth, 20)))
			{
				g_Misc.LockAchievements();
			}
		} EndChild();

		EndTable();
	}
}
#pragma endregion

/* Settings Window */
void CMenu::SettingsWindow()
{
	using namespace ImGui;
	if (!ShowSettings) { return; }

	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
	PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(10, 10));

	if (Begin("Settings", &ShowSettings, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
	{
		if (ColorPicker("Menu accent", Vars::Menu::Colors::MenuAccent)) { LoadStyle(); } SameLine(); Text("Menu accent");
		if (Checkbox("Alternative Design", &Vars::Menu::ModernDesign)) { LoadStyle(); }
		Checkbox("Blur background", &Vars::Menu::BlurBackground);

		Dummy({ 0, 5 });
		static std::string selected;
		int nConfig = 0;

		// Load config files
		for (const auto& entry : std::filesystem::directory_iterator(g_CFG.GetConfigPath()))
		{
			if (std::string(std::filesystem::path(entry).filename().string()).find(g_CFG.ConfigExtension) == std::string_view::npos)
			{
				continue;
			}
			nConfig++;
		}

		// Current config
		const std::string cfgText = "Loaded: " + g_CFG.GetCurrentConfig();
		Text(cfgText.c_str());

		// Config name field
		if (nConfig < 100)
		{
			std::string newConfigName = {};

			PushItemWidth(200);
			if (InputTextWithHint("###configname", "New config name", &newConfigName, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (!std::filesystem::exists(g_CFG.GetConfigPath() + "\\" + newConfigName))
				{
					g_CFG.SaveConfig(newConfigName);
				}
			}
			PopItemWidth();
		}

		// Config list
		for (const auto& entry : std::filesystem::directory_iterator(g_CFG.GetConfigPath()))
		{
			if (std::string(std::filesystem::path(entry).filename().string()).find(g_CFG.ConfigExtension) == std::string_view::npos)
			{
				continue;
			}

			std::string configName = entry.path().filename().string();
			configName.erase(configName.end() - g_CFG.ConfigExtension.size(), configName.end());

			if (configName == selected)
			{
				const ImGuiStyle* style2 = &GetStyle();
				const ImVec4* colors2 = style2->Colors;
				ImVec4 buttonColor = colors2[ImGuiCol_Button];
				buttonColor.w *= .5f;
				PushStyleColor(ImGuiCol_Button, buttonColor);

				// Config name button
				if (Button(configName.c_str(), ImVec2(200, 20)))
				{
					selected = configName;
				}
				PopStyleColor();

				// Save config button
				if (Button("Save", ImVec2(61, 20)))
				{
					if (configName != g_CFG.GetCurrentConfig())
					{
						OpenPopup("Save config?");
					} else
					{
						g_CFG.SaveConfig(selected);
						selected.clear();
					}
				}

				// Load config button
				SameLine();
				if (Button("Load", ImVec2(61, 20)))
				{
					g_CFG.LoadConfig(selected);
					selected.clear();
					LoadStyle();
				}

				// Remove config button
				SameLine();
				if (Button("Remove", ImVec2(62, 20)))
				{
					OpenPopup("Remove config?");
				}

				// Save config dialog
				if (BeginPopupModal("Save config?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
				{
					Text("Do you really want to override this config?");

					Separator();
					if (Button("Yes, override!", ImVec2(150, 0)))
					{
						g_CFG.SaveConfig(selected);
						selected.clear();
						CloseCurrentPopup();
					}

					SameLine();
					if (Button("No", ImVec2(120, 0)))
					{
						CloseCurrentPopup();
					}
					EndPopup();
				}

				// Delete config dialog
				if (BeginPopupModal("Remove config?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
				{
					Text("Do you really want to delete this config?");

					Separator();
					if (Button("Yes, remove!", ImVec2(150, 0)))
					{
						g_CFG.RemoveConfig(selected);
						selected.clear();
						CloseCurrentPopup();
					}
					SameLine();
					if (Button("No", ImVec2(150, 0)))
					{
						CloseCurrentPopup();
					}
					EndPopup();
				}
			}
			else if (configName == g_CFG.GetCurrentConfig())
			{
				PushStyleColor(ImGuiCol_Button, GetStyle().Colors[ImGuiCol_ButtonActive]);
				std::string buttonText = "> " + configName + " <";
				if (Button(buttonText.c_str(), ImVec2(200, 20)))
				{
					selected = configName;
				}
				PopStyleColor();
			}
			else
			{
				if (Button(configName.c_str(), ImVec2(200, 20)))
				{
					selected = configName;
				}
			}
		}

		End();
	}

	PopStyleVar(2);
}

/* Debug Menu */
void CMenu::DebugMenu()
{
	#ifdef _DEBUG
	using namespace ImGui;
	if (!ShowDebugMenu) { return; }

	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
	PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(200, 200));

	if (Begin("Debug", &ShowDebugMenu, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
	{
		const auto& pLocal = g_EntityCache.m_pLocal;

		Checkbox("Show Debug info", &Vars::Visuals::DebugInfo.m_Var);

		// Particle tester
		if (CollapsingHeader("Particles"))
		{
			static std::string particleName = "ping_circle";

			InputText("Particle name", &particleName);
			if (Button("Dispatch") && pLocal != nullptr)
			{
				Particles::DispatchParticleEffect(particleName.c_str(), pLocal->GetAbsOrigin(), { });
			}
		}

		// Debug options
		if (CollapsingHeader("Debug options"))
		{
			Checkbox("Debug Bool", &Vars::Debug::DebugBool.m_Var);
		}

		End();
	}

	PopStyleVar(2);
	#endif
}

/* Window for the camera feature */
void CMenu::DrawCameraWindow()
{
	if (I::Engine->IsInGame() && Vars::Visuals::CameraMode.m_Var != 0)
	{
		int windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;
		if (Vars::Visuals::CameraMode.m_Var <= 1 || g_CameraWindow.CanDraw) {
			windowFlags |= ImGuiWindowFlags_NoBackground;
		}

		// Draw the camera window
		ImGui::SetNextWindowSize({ static_cast<float>(g_CameraWindow.ViewRect.w), static_cast<float>(g_CameraWindow.ViewRect.h) }, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos({ static_cast<float>(g_CameraWindow.ViewRect.x), static_cast<float>(g_CameraWindow.ViewRect.y) }, ImGuiCond_FirstUseEver);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 60.f, 60.f });
		if (ImGui::Begin("Camera", nullptr, windowFlags))
		{
			const ImVec2 winPos = ImGui::GetWindowPos();
			const ImVec2 winSize = ImGui::GetWindowSize();

			g_CameraWindow.ViewRect.x = static_cast<int>(winPos.x);
			g_CameraWindow.ViewRect.y = static_cast<int>(winPos.y);
			g_CameraWindow.ViewRect.w = static_cast<int>(winSize.x);
			g_CameraWindow.ViewRect.h = static_cast<int>(winSize.y);

			ImGui::End();
		}
		ImGui::PopStyleVar();
	}
}

void CMenu::Render(IDirect3DDevice9* pDevice)
{
	static bool initialized = false;
	if (!ConfigLoaded) { return; }

	if (!initialized)
	{
		Init(pDevice);
		initialized = true;
	}

	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

	// Toggle menu
	if (GetAsyncKeyState(MENU_KEY) & 1)
	{
		// TOOD: & 8000?
		g_Menu.IsOpen = !g_Menu.IsOpen;
		I::Surface->SetCursorAlwaysVisible(g_Menu.IsOpen);
	}

	// Begin current frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (g_Menu.IsOpen)
	{
		if (Vars::Menu::BlurBackground)
		{
			g_Blur.DrawBackgroundBlur(ImGui::GetBackgroundDrawList(), pDevice);
		}

		ImGui::PushFont(Verdana);
		DrawMenu();
		DrawCameraWindow();

		// TODO: Draw DT-Bar, Playerlist, Spectator list etc.
		SettingsWindow();
		DebugMenu();
		g_MaterialEditor.Render();
		g_PlayerList.Render();

		ImGui::PopFont();
	}

	// End frame and render
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
}

void CMenu::LoadStyle()
{
	// Style & Colors
	{
		ItemWidth = 120.f;

		// https://raais.github.io/ImStudio/
		Accent = ImGui::ColorToVec(Vars::Menu::Colors::MenuAccent);
		AccentDark = ImColor(Accent.Value.x * 0.8f, Accent.Value.y * 0.8f, Accent.Value.z * 0.8f, Accent.Value.w);

		auto& style = ImGui::GetStyle();
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f); // Center window title
		style.WindowMinSize = ImVec2(700, 700);
		style.WindowPadding = ImVec2(0, 0);
		style.WindowBorderSize = 1.f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.4f); // Center button text
		style.FrameBorderSize = 1.f; // Old menu feeling
		style.FrameRounding = 0.f;
		style.ChildBorderSize = 1.f;
		style.ChildRounding = 0.f;
		style.GrabMinSize = 15.f;
		style.GrabRounding = 0.f;
		style.ScrollbarSize = 4.f;
		style.ScrollbarRounding = 6.f;
		style.ItemSpacing = ImVec2(8.f, 5.f);

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Border] = ImColor(110, 110, 128);
		colors[ImGuiCol_WindowBg] = Background;
		colors[ImGuiCol_TitleBg] = BackgroundDark;
		colors[ImGuiCol_TitleBgActive] = BackgroundLight;
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.4f);
		colors[ImGuiCol_Button] = BackgroundLight;
		colors[ImGuiCol_ButtonHovered] = ImColor(69, 69, 77);
		colors[ImGuiCol_ButtonActive] = ImColor(82, 79, 87);
		colors[ImGuiCol_PopupBg] = BackgroundDark;
		colors[ImGuiCol_FrameBg] = ImColor(50, 50, 50);
		colors[ImGuiCol_FrameBgHovered] = ImColor(60, 60, 60);
		colors[ImGuiCol_FrameBgActive] = ImColor(70, 70, 70);
		colors[ImGuiCol_CheckMark] = Accent;
		colors[ImGuiCol_Text] = TextLight;

		colors[ImGuiCol_SliderGrab] = Accent;
		colors[ImGuiCol_SliderGrabActive] = AccentDark;
		colors[ImGuiCol_ResizeGrip] = Accent;
		colors[ImGuiCol_ResizeGripActive] = Accent;
		colors[ImGuiCol_ResizeGripHovered] = Accent;
		colors[ImGuiCol_Header] = ImColor(70, 70, 70);
		colors[ImGuiCol_HeaderActive] = ImColor(40, 40, 40);
		colors[ImGuiCol_HeaderHovered] = ImColor(60, 60, 60);

		// Alternative Designs
		if (Vars::Menu::ModernDesign)
		{
			ItemWidth = 150.f;

			style.FrameBorderSize = 0.f;
			style.FrameRounding = 2.f;
			style.GrabRounding = 2.f;
		}
	}

	// Misc
	{
		TitleGradient.ClearMarks();
		TitleGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		TitleGradient.AddMark(0.3f, ImColor(0, 0, 0, 0));
		TitleGradient.AddMark(0.5f, Accent);
		TitleGradient.AddMark(0.7f, ImColor(0, 0, 0, 0));
		TitleGradient.AddMark(1.f, ImColor(0, 0, 0, 0));
	}

	{
		MainGradient.ClearMarks();
		MainGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		MainGradient.AddMark(0.2f, ImColor(0, 0, 0, 0));
		MainGradient.AddMark(0.5f, Accent);
		MainGradient.AddMark(0.8f, ImColor(0, 0, 0, 0));
		MainGradient.AddMark(1.f, ImColor(0, 0, 0, 0));
	}

	{
		TabGradient.ClearMarks();
		TabGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		TabGradient.AddMark(0.2f, ImColor(0, 0, 0, 0));
		TabGradient.AddMark(0.5f, ImColor(255, 255, 255));
		TabGradient.AddMark(0.8f, ImColor(0, 0, 0, 0));
		TabGradient.AddMark(1.f, ImColor(0, 0, 0, 0));
	}
}

void CMenu::Init(IDirect3DDevice9* pDevice)
{
	// Initialize ImGui and device
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(FindWindowA(nullptr, "Team Fortress 2"));
	ImGui_ImplDX9_Init(pDevice);

	// Fonts
	{
		const auto& io = ImGui::GetIO();

		auto fontConfig = ImFontConfig();
		fontConfig.OversampleH = 2;

		constexpr ImWchar fontRange[]{ 0x0020, 0x00FF,0x0400, 0x044F, 0 }; // Basic Latin, Latin Supplement and Cyrillic

		VerdanaSmall = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdana.ttf", 12.0f, &fontConfig, fontRange);
		Verdana = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdana.ttf", 14.0f, &fontConfig, fontRange);
		VerdanaBold = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdanab.ttf", 18.0f, &fontConfig, fontRange);

		SectionFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdana.ttf", 16.0f, &fontConfig, fontRange);
		TitleFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\verdanab.ttf", 20.0f, &fontConfig, fontRange);

		constexpr ImWchar iconRange[]{ ICON_MIN_MD, ICON_MAX_MD, 0 };
		ImFontConfig iconConfig;
		iconConfig.MergeMode = true;
		iconConfig.PixelSnapH = true;
		IconFont = io.Fonts->AddFontFromMemoryCompressedTTF(MaterialFont_compressed_data, MaterialFont_compressed_size, 16.f, &iconConfig, iconRange);

		io.Fonts->Build();
	}

	LoadStyle();
	g_MaterialEditor.Init();
}