#include "Menu.h"

#include "../Vars.h"
#include "../Camera/CameraWindow.h"
#include "../Items/AttributeChanger/AttributeChanger.h"
#include "../Radar/Radar.h"
#include "../Misc/Misc.h"
#include "../Chams/DMEChams.h"
#include "../Glow/Glow.h"
#include "../Killsay/Killsay.h"
#include "../Discord/Discord.h"
#include "../NoSpread/NoSpread.h"

#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx9.h>
#include <ImGui/imgui_stdlib.h>

#include "Fonts/IconsMaterialDesign.h"
#include "Playerlist/Playerlist.h"
#include "MaterialEditor/MaterialEditor.h"
#include "Pong/Pong.h"

#include "Components.hpp"
#include "ConfigManager/ConfigManager.h"

#include <mutex>

#pragma warning (disable : 4309)

int unuPrimary = 0;
int unuSecondary = 0;

constexpr auto SIZE_FULL_WIDTH = ImVec2(-1, 20);

/* The main menu */
void CMenu::DrawMenu()
{
	ImGui::SetNextWindowSize(ImVec2(700, 700), ImGuiCond_FirstUseEver);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 700, 500 });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorToVec(Vars::Menu::Colors::TitleBg.Value));
	if (ImGui::Begin("MainWindow", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
	{
		const auto drawList = ImGui::GetWindowDrawList();
		const auto windowSize = ImGui::GetWindowSize();
		const auto windowPos = ImGui::GetWindowPos();
		
		// Title gradient setup
		
		ImGui::GradientRect(&TitleGradient, { windowPos.x, windowPos.y }, windowSize.x, 3.f);
		ImGui::Dummy({ 0, 2 });

		// Title Text
		{
			ImGui::PushFont(TitleFont);
			const auto titleWidth = ImGui::CalcTextSize(Vars::Menu::CheatName.Value.c_str()).x;
			drawList->AddText(TitleFont, TitleFont->FontSize, { windowPos.x + (windowSize.x / 2) - (titleWidth / 2), windowPos.y }, ImGui::ColorToImGui(Vars::Menu::Colors::MenuAccent.Value), Vars::Menu::CheatName.Value.c_str());
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
				Vars::Menu::ShowPlayerlist.Value = !Vars::Menu::ShowPlayerlist.Value;
			}
			ImGui::HelpMarker("Playerlist");

			// Keybinds Icon
			ImGui::SetCursorPos({ currentX -= 25, 0 });
			if (ImGui::IconButton(ICON_MD_KEYBOARD))
			{
				Vars::Menu::ShowKeybinds.Value = !Vars::Menu::ShowKeybinds.Value;
			}
			ImGui::HelpMarker("Keybinds");

			// Material Editor Icon
			ImGui::SetCursorPos({ currentX -= 25, 0 });
			if (ImGui::IconButton(ICON_MD_BRUSH))
			{
				F::MaterialEditor.IsOpen = !F::MaterialEditor.IsOpen;
			}
			ImGui::HelpMarker("Material Editor");

// Debug Menu
			ImGui::SetCursorPos({ currentX -= 25, 0 });
			if (ImGui::IconButton(ICON_MD_BUG_REPORT))
			{
				ShowDebugMenu = !ShowDebugMenu;
			}
			ImGui::HelpMarker("Debug Menu");
		}

		// Tabbar
		ImGui::SetCursorPos({ 0, TitleHeight });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorToVec(Vars::Menu::Colors::TabBar.Value));
		if (ImGui::BeginChild("Tabbar", { windowSize.x + 5, TabHeight + SubTabHeight }, false, ImGuiWindowFlags_NoScrollWithMouse))
		{
			DrawTabbar();
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();

		// Main content
		ImGui::SetCursorPos({ 0, TitleHeight + TabHeight + SubTabHeight });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8.f, 10.f });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorToVec(Vars::Menu::Colors::WindowBg.Value));
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
			if (!Vars::Menu::ModernDesign.Value)
			{
				const auto hintHeight = ImGui::CalcTextSize(FeatureHint.c_str()).y;
				drawList->AddText(Verdana, Verdana->FontSize, { windowPos.x + 10, windowPos.y + windowSize.y - (hintHeight + ImGui::GetStyle().ItemInnerSpacing.y) }, ImGui::ColorToImGui(Vars::Menu::Colors::Text.Value), FeatureHint.c_str());
			}
		}

		// End
		ImGui::End();
	}

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor();
}

void CMenu::DrawTabbar()
{
	ImGui::PushFont(SectionFont);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

	if (ImGui::BeginTable("TabbarTable", 5))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::ColorToVec(Vars::Menu::Colors::TabBar.Value));
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorToImGui(Vars::Menu::Colors::Text.Value).Value);
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

		if (ImGui::BeginTable("SubbarTable", 7))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::ColorToVec(Vars::Menu::Colors::TabBar.Value));
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorToImGui(Vars::Menu::Colors::Text.Value).Value);
			if (ImGui::TabButton("ESP", CurrentVisualsTab == VisualsTab::ESP))
			{
				CurrentVisualsTab = VisualsTab::ESP;
			}

			if (ImGui::TabButton("Chams", CurrentVisualsTab == VisualsTab::Chams))
			{
				CurrentVisualsTab = VisualsTab::Chams;
			}

			if (ImGui::TabButton("Glow", CurrentVisualsTab == VisualsTab::Glow))
			{
				CurrentVisualsTab = VisualsTab::Glow;
			}

			if (ImGui::TabButton("World", CurrentVisualsTab == VisualsTab::World))
			{
				CurrentVisualsTab = VisualsTab::World;
			}

			if (ImGui::TabButton("Indicators", CurrentVisualsTab == VisualsTab::Indicators))
			{
				CurrentVisualsTab = VisualsTab::Indicators;
			}

			if (ImGui::TabButton("Radar", CurrentVisualsTab == VisualsTab::Radar))
			{
				CurrentVisualsTab = VisualsTab::Radar;
			}

			if (ImGui::TabButton("Fonts", CurrentVisualsTab == VisualsTab::Font))
			{
				CurrentVisualsTab = VisualsTab::Font;
			}

			ImGui::PopStyleColor(2);
			ImGui::EndTable();
		}
	}
	else
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
			WToggle("Aimbot", &Vars::Aimbot::Global::Active.Value); HelpMarker("Aimbot master switch");
			ColorPickerL("Target", Vars::Colours::Target.Value);
			InputKeybind("Aimbot key", Vars::Aimbot::Global::AimKey); HelpMarker("The key to enable aimbot");
			WSlider("Aimbot FoV####AimbotFoV", &Vars::Aimbot::Global::AimFOV.Value, 0.f, 180.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			ColorPickerL("Aimbot FOV circle", Vars::Colours::FOVCircle.Value);
			WToggle("Autoshoot###AimbotAutoshoot", &Vars::Aimbot::Global::AutoShoot.Value); HelpMarker("Automatically shoot when a target is found");
			MultiCombo({ "Players", "Buildings", "Stickies", "NPCs", "Bombs" }, { &Vars::Aimbot::Global::AimPlayers.Value, &Vars::Aimbot::Global::AimBuildings.Value, &Vars::Aimbot::Global::AimStickies.Value, &Vars::Aimbot::Global::AimNPC.Value, &Vars::Aimbot::Global::AimBombs.Value }, "Aim targets");
			HelpMarker("Choose which targets the Aimbot should aim at");
			{
				static std::vector flagNames{ "Invulnerable", "Cloaked", "Dead Ringer", "Friends", "Taunting", "Vaccinator", "Unsimulated Players", "Disguised" };
				static std::vector flagValues{ 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7 };
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Global::IgnoreOptions.Value, "Ignored targets###AimbotIgnoredTargets");
				HelpMarker("Choose which targets should be ignored");
			}
			ColorPickerL("Invulnerable colour", Vars::Colours::Invuln.Value);

			if (Vars::Aimbot::Global::IgnoreOptions.Value & (1 << 6))
			{
				WSlider("Tick Tolerance###AimbotUnsimulatedTolerance", &Vars::Aimbot::Global::TickTolerance.Value, 0, 21, "%d", ImGuiSliderFlags_AlwaysClamp);
			}

			WToggle("Flick at Enemies", &Vars::Aimbot::Global::FlickatEnemies.Value);


			SectionTitle("Crits");
			WToggle("Crit hack", &Vars::CritHack::Active.Value);  HelpMarker("Enables the crit hack (BETA)");
			MultiCombo({ "Indicators", "Avoid Random", "Always Melee", "Auto Melee Crit" }, { &Vars::CritHack::Indicators.Value, &Vars::CritHack::AvoidRandom.Value, &Vars::CritHack::AlwaysMelee.Value, &Vars::CritHack::AutoMeleeCrit.Value }, "Misc###CrithackMiscOptions");
			HelpMarker("Misc options for crithack");
			InputKeybind("Crit key", Vars::CritHack::CritKey); HelpMarker("Will try to force crits when the key is held");

			SectionTitle("Backtrack");
			WToggle("Active", &Vars::Backtrack::Enabled.Value); HelpMarker("If you shoot at the backtrack manually it will attempt to hit it");
			WToggle("Unchoke Prediction", &Vars::Backtrack::UnchokePrediction.Value);
			WToggle("Allow Forward Tracking", &Vars::Backtrack::AllowForward.Value);
			WCombo("Backtrack Method###HitscanBacktrackMethod", &Vars::Aimbot::Hitscan::BackTrackMethod.Value, { "All", "Last", "Prefer OnShot" });
			WSlider("Amount of latency###BTLatency", &Vars::Backtrack::Latency.Value, 0, 800, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("This won't work on local servers");
		} EndChild();

		/* Column 2 */
		if (TableColumnChild("AimbotCol2"))
		{
			SectionTitle("Hitscan");
			WCombo("Sort method###HitscanSortMethod", &Vars::Aimbot::Hitscan::SortMethod.Value, { "FOV", "Distance" }); HelpMarker("Which method the aimbot uses to decide which target to aim at");
			if (Vars::Aimbot::Hitscan::SortMethod.Value == 1)
			{
				WToggle("Respect FOV", &Vars::Aimbot::Hitscan::RespectFOV.Value); HelpMarker("Respect the Aim FOV set when using distance sorting.");
			}
			WCombo("Aim method###HitscanAimMethod", &Vars::Aimbot::Hitscan::AimMethod.Value, { "Plain", "Smooth", "Silent" }); HelpMarker("Which method the aimbot uses to aim at the target");
			if (Vars::Aimbot::Hitscan::AimMethod.Value == 1) {
				WSlider("Smooth factor###HitscanSmoothing", &Vars::Aimbot::Hitscan::SmoothingAmount.Value, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Changes how smooth the aimbot will aim at the target");
			}
			WCombo("Preferred Hitbox###HitscanHitbox", &Vars::Aimbot::Hitscan::AimHitbox.Value, { "Head", "Body", "Auto" }); // this could probably be removed entirely since it actually does nothing.
			WCombo("Tapfire###HitscanTapfire", &Vars::Aimbot::Hitscan::TapFire.Value, { "Off", "Distance", "Always" }); HelpMarker("How/If the aimbot chooses to tapfire enemies.");
			if (Vars::Aimbot::Hitscan::TapFire.Value == 1) {
				WSlider("Tap Fire Distance###HitscanTapfireDistance", &Vars::Aimbot::Hitscan::TapFireDist.Value, 64.f, 4096.f, "%.0f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The distance at which tapfire will activate.");
			}
			// Leave it out of the brackets since the check runs even if TapFire mode is "Always" 
			WToggle("Check for NoSpread state", &Vars::Aimbot::Hitscan::TapFireCheckForNSS.Value); HelpMarker("Turns off Tapfire if NoSpread is synced");
			{
				static std::vector flagNames{ "Head", "Body", "Pelvis", "Arms", "Legs" };
				static std::vector flagValues{ 0x00000001, 0x00000004, 0x00000002, 0x00000008, 0x00000010 }; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Hitscan::ScanHitboxes.Value, "Scan Hitboxes###AimbotHitboxScanning");
			}
			{
				static std::vector flagNames{ "Head", "Body", "Pelvis", "Arms", "Legs" };
				static std::vector flagValues{ 0x00000001, 0x00000004, 0x00000002, 0x00000008, 0x00000010 }; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Hitscan::MultiHitboxes.Value, "Multipoint Hitboxes###AimbotMultipointScanning");
			}
			{
				static std::vector flagNames{ "Head", "Body", "Pelvis", "Arms", "Legs" };
				static std::vector flagValues{ 0x00000001, 0x00000004, 0x00000002, 0x00000008, 0x00000010 }; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Hitscan::StaticHitboxes.Value, "Static Hitboxes###AimbotStaticOnlyScanning");
			}
			WToggle("Adaptive Multipoint", &Vars::Aimbot::Hitscan::AdaptiveMultiPoint.Value); HelpMarker("Randomly creates points for each hitbox instead of using static points.");
			if (Vars::Aimbot::Hitscan::AdaptiveMultiPoint.Value) {
				WSlider("Point Count###HitscanMultipointCount", &Vars::Aimbot::Hitscan::RandomPoints.Value, 1, 12, "%d", ImGuiSliderFlags_None);
			}
			WSlider("Point Scale###HitscanMultipointScale", &Vars::Aimbot::Hitscan::PointScale.Value, 0.5f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			WToggle("Preserve Target", &Vars::Aimbot::Hitscan::PreserveTarget.Value); HelpMarker("Prioritises the target that you most recently aimed at.");
			if (Vars::Aimbot::Hitscan::PreserveTarget.Value) {
				WToggle("Preserved Target Ignores FoV", &Vars::Aimbot::Hitscan::IgnorePreservedFoV.Value); HelpMarker("Ignores FoV check for preserved target.");
			}
			WToggle("Buildings Multipoint", &Vars::Aimbot::Hitscan::ScanBuildings.Value); HelpMarker("Scans the building hitbox to improve hitchance");
			WToggle("Wait for headshot", &Vars::Aimbot::Hitscan::WaitForHeadshot.Value); HelpMarker("The aimbot will wait until it can headshot (if applicable)");
			WToggle("Wait for charge", &Vars::Aimbot::Hitscan::WaitForCharge.Value); HelpMarker("The aimbot will wait until the rifle has charged long enough to kill in one shot");
			WToggle("Smooth if spectated", &Vars::Aimbot::Hitscan::SpectatedSmooth.Value); HelpMarker("The aimbot will switch to the smooth method if being spectated");
			WToggle("Scoped only", &Vars::Aimbot::Hitscan::ScopedOnly.Value); HelpMarker("The aimbot will only shoot if scoped");
			WToggle("Auto scope", &Vars::Aimbot::Hitscan::AutoScope.Value); HelpMarker("The aimbot will automatically scope in to shoot");
			WToggle("Auto rev minigun", &Vars::Aimbot::Hitscan::AutoRev.Value); HelpMarker("Will rev heavy's minigun regardless of if aimbot has a target");
			WToggle("Bodyaim if lethal", &Vars::Aimbot::Global::BAimLethal.Value); HelpMarker("The aimbot will aim for body when damage is lethal to it");
			WToggle("Piss on Team", &Vars::Aimbot::Hitscan::ExtinguishTeam.Value); HelpMarker("Will aim at burning teammates with The Sydney Sleeper");

			SectionTitle("Melee");
			{
				WToggle("Require Aimbot Bind", &Vars::Aimbot::Melee::RequireBind.Value); HelpMarker("Requires the aimbot keybind to be held in order to run, otherwise it will remain on at all times.");
				WCombo("Sort method###MeleeSortMethod", &Vars::Aimbot::Melee::SortMethod.Value, { "FOV", "Distance", }); HelpMarker("Which method the aimbot uses to decide which target to aim at");
				if (Vars::Aimbot::Melee::SortMethod.Value == 1)
				{
					WToggle("Respect FOV", &Vars::Aimbot::Melee::RespectFOV.Value); HelpMarker("Respect the Aim FOV set when using distance sorting.");
				}
				WCombo("Aim method###MeleeAimMethod", &Vars::Aimbot::Melee::AimMethod.Value, { "Plain", "Smooth", "Silent" }); HelpMarker("Which method the aimbot uses to aim at the target");
				if (Vars::Aimbot::Melee::AimMethod.Value == 1)
				{
					WSlider("Smooth factor###MeleeSmoothing", &Vars::Aimbot::Melee::SmoothingAmount.Value, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How smooth the aimbot should be");
				}
			}
			WToggle("Range check", &Vars::Aimbot::Melee::RangeCheck.Value); HelpMarker("Only aim at target if within melee range");
			WToggle("Swing prediction", &Vars::Aimbot::Melee::PredictSwing.Value); HelpMarker("Aimbot will attack preemptively, predicting you will be in range of the target");
			WToggle("Whip teammates", &Vars::Aimbot::Melee::WhipTeam.Value); HelpMarker("Aimbot will target teammates if holding the Disciplinary Action");
			//WToggle("Wait for hit", &Vars::Aimbot::Projectile::WaitForHit.Value); HelpMarker("Will avoid shooting until the last shot hits");
		} EndChild();

		/* Column 3 */
		if (TableColumnChild("AimbotCol3"))
		{
			SectionTitle("Projectile");
			WSlider("Prediction Time", &Vars::Aimbot::Projectile::PredictionTime.Value, 0.1f, 10.f, "%.1f");

			{
				WCombo("Sort method###ProjectileSortMethod", &Vars::Aimbot::Projectile::SortMethod.Value, { "FOV", "Distance" });
				if (Vars::Aimbot::Projectile::SortMethod.Value == 1)
				{
					WToggle("Respect FOV", &Vars::Aimbot::Projectile::RespectFOV.Value); HelpMarker("Respect the Aim FOV set when using distance sorting.");
				}
				WCombo("Aim method###ProjectileAimMethod", &Vars::Aimbot::Projectile::AimMethod.Value, { "Plain", "Silent" });
				WCombo("Priority Hitbox###ProjectileHitboxPriority", &Vars::Aimbot::Projectile::AimPosition.Value, { "Head", "Body", "Feet", "Auto" });
				{
					static std::vector flagNames{ "Head", "Body", "Feet" };
					static std::vector flagValues{ (1 << 0), (1 << 1), (1 << 2) }; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
					MultiFlags(flagNames, flagValues, &Vars::Aimbot::Projectile::AllowedHitboxes.Value, "Allowed Hitboxes###ProjectileHitboxScanning"); HelpMarker("Controls what hitboxes the cheat is allowed to consider shooting at.");
				}
				WSlider("Point VisTest Limit", &Vars::Aimbot::Projectile::VisTestPoints.Value, 3, 15, "%d", ImGuiSliderFlags_AlwaysClamp);	HelpMarker("Controls how many points the cheat is allowed to consider.");
				WSlider("Point Scan Limit", &Vars::Aimbot::Projectile::ScanPoints.Value, 3, Vars::Aimbot::Projectile::VisTestPoints.Value, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Controls how many visible points the cheat needs to find before it picks one to aim at.");
				WSlider("Point Scale", &Vars::Aimbot::Projectile::ScanScale.Value, 0.7f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Controls the size of the hitbox as it's given to the cheat.");

				SectionTitle("Preferences");
				WToggle("Predict Obscured Enemies", &Vars::Aimbot::Projectile::PredictObscured.Value); HelpMarker("Will predict enemies that cannot yet be targetted because of a wall etc and shoot if they are predicted to peek (FPS)");
				WToggle("Feet aim on ground", &Vars::Aimbot::Projectile::FeetAimIfOnGround.Value); HelpMarker("Will aim at targets feet if they're on the ground in order to launch them into the air");
				InputKeybind("Bounce Target", Vars::Aimbot::Projectile::BounceKey, true, false, "None"); HelpMarker("Forces feet aim to bounce target on keypress");
				WToggle("Viewmodel flipper", &Vars::Misc::ViewmodelFlip.Value); HelpMarker("Will flip your viewmodel if it can hit from that side");
				WToggle("Charge loose cannon", &Vars::Aimbot::Projectile::ChargeLooseCannon.Value); HelpMarker("Will charge your loose cannon in order to double donk");

				SectionTitle("Splash");
				WToggle("Splash prediction", &Vars::Aimbot::Projectile::SplashPrediction.Value); HelpMarker("Will shoot the area near the target to hit them with splash damage");
				if (Vars::Aimbot::Projectile::SplashPrediction.Value)
				{
					WSlider("Minimum splash distance", &Vars::Aimbot::Projectile::MinSplashPredictionDistance.Value, 0.f, Vars::Aimbot::Projectile::MaxSplashPredictionDistance.Value); HelpMarker("The minimum distance to try going for splash damage");
					WSlider("Maximum splash distance", &Vars::Aimbot::Projectile::MaxSplashPredictionDistance.Value, Vars::Aimbot::Projectile::MinSplashPredictionDistance.Value, 10000.f); HelpMarker("The maximum distance to try going for splash damage");
					WToggle("Wait for hit", &Vars::Aimbot::Projectile::WaitForHit.Value); HelpMarker("lol");
				}

				SectionTitle("Strafe Prediction");
				MultiCombo({ "Air", "Ground" }, { &Vars::Aimbot::Projectile::StrafePredictionAir.Value, &Vars::Aimbot::Projectile::StrafePredictionGround.Value }, "Strafe Prediction");
				WSlider("Velocity samples", &Vars::Aimbot::Projectile::StrafePredictionSamples.Value, 1, 20); HelpMarker("How many ticks to keep velocity records of");
				WSlider("Minimum deviation", &Vars::Aimbot::Projectile::StrafePredictionMinDifference.Value, 0, 180); HelpMarker("How big the angle difference of the predicted strafe has to be to apply");
				WSlider("Maximum distance", &Vars::Aimbot::Projectile::StrafePredictionMaxDistance.Value, 100.f, 10000.f); HelpMarker("Max distance to apply strafe prediction (lower is better)");
			}

			SectionTitle("NoSpread");
			{
				WToggle("Hitscan", &Vars::NoSpread::Hitscan.Value); HelpMarker("Enables NoSpread for hitscan weapons, works better on servers with high uptime (100+ hrs)");
				WToggle("Projectile", &Vars::NoSpread::Projectile.Value); HelpMarker("Enables NoSpread for projectile weapons (works independent of the sync state)");
				WToggle("Indicator", &Vars::NoSpread::Indicator.Value); HelpMarker("Shows server uptime, mantissa step size and sync state");
			}
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
			WToggle("Triggerbot", &Vars::Triggerbot::Global::Active.Value); HelpMarker("Global triggerbot master switch");
			InputKeybind("Trigger key", Vars::Triggerbot::Global::TriggerKey); HelpMarker("The key which activates the triggerbot");
			HelpMarker("Choose which targets the Aimbot should aim at");
			{
				static std::vector flagNames{ "Invulnerable", "Cloaked", "Friends", "Taunting", "Unsimulated Players", "Disguised" };
				static std::vector flagValues{ 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5 };
				MultiFlags(flagNames, flagValues, &Vars::Triggerbot::Global::IgnoreOptions.Value, "Ignored targets###TriggerbotIgnoredTargets");
				HelpMarker("Choose which targets should be ignored");
			}

			SectionTitle("Autoshoot");
			WToggle("Autoshoot###AutoshootTrigger", &Vars::Triggerbot::Shoot::Active.Value); HelpMarker("Shoots if mouse is over a target");
			MultiCombo({ "Players", "Buildings" }, { &Vars::Triggerbot::Shoot::TriggerPlayers.Value, &Vars::Triggerbot::Shoot::TriggerBuildings.Value }, "Trigger targets");
			HelpMarker("Choose which target the triggerbot should shoot at");
			WToggle("Head only###TriggerHeadOnly", &Vars::Triggerbot::Shoot::HeadOnly.Value); HelpMarker("Auto shoot will only shoot if you are aiming at the head");
			WToggle("Wait for Headshot###TriggerbotWaitForHeadshot", &Vars::Triggerbot::Shoot::WaitForHeadshot.Value); HelpMarker("Auto shoot will only shoot if the sniper is charged enough to headshot");
			WToggle("Scoped Only###TriggerbotScopedOnly", &Vars::Triggerbot::Shoot::ScopeOnly.Value); HelpMarker("Auto shoot will only shoot while scoped");
			WSlider("Head scale###TriggerHeadScale", &Vars::Triggerbot::Shoot::HeadScale.Value, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The scale at which the auto shoot will try to shoot the targets head");
		} EndChild();

		/* Column 2 */
		if (TableColumnChild("TriggerCol2"))
		{
			SectionTitle("Autostab");
			WToggle("Auto backstab###TriggerAutostab", &Vars::Triggerbot::Stab::Active.Value); HelpMarker("Auto backstab will attempt to backstab the target if possible");
			WToggle("Rage mode", &Vars::Triggerbot::Stab::RageMode.Value); HelpMarker("Stabs whenever possible by aiming toward the back");
			WToggle("Silent", &Vars::Triggerbot::Stab::Silent.Value); HelpMarker("Aim changes made by the rage mode setting aren't visible");
			WToggle("Disguise on kill", &Vars::Triggerbot::Stab::Disguise.Value); HelpMarker("Will apply the previous disguise after stabbing");
			WToggle("Ignore razorback", &Vars::Triggerbot::Stab::IgnRazor.Value); HelpMarker("Will not attempt to backstab snipers wearing the razorback");
			WSlider("Stab range###StabRange", &Vars::Triggerbot::Stab::Range.Value, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The range at which auto backstab will attempt to stab");

			SectionTitle("Auto Detonate");
			WToggle("Autodetonate###TriggerDet", &Vars::Triggerbot::Detonate::Active.Value);
			{
				static std::vector Names{ "Players", "Buildings", "NPCs", "Bombs", "Stickies" };
				static std::vector Values{ 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4 };

				MultiFlags(Names, Values, &Vars::Triggerbot::Detonate::DetonateTargets.Value, "Targets###TriggerbotDetonateTargets");
			}
			WToggle("Explode stickies###TriggerSticky", &Vars::Triggerbot::Detonate::Stickies.Value); HelpMarker("Detonate sticky bombs when a player is in range");
			WToggle("Detonate flares###TriggerFlares", &Vars::Triggerbot::Detonate::Flares.Value); HelpMarker("Detonate detonator flares when a player is in range");
			WSlider("Detonation radius###TriggerDetRadius", &Vars::Triggerbot::Detonate::RadiusScale.Value, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The radius around the projectile that it will detonate if a player is in");
		} EndChild();

		/* Column 3 */
		if (TableColumnChild("TriggerCol3"))
		{
			SectionTitle("Autoblast");
			WToggle("Autoblast###Triggreairblast", &Vars::Triggerbot::Blast::Active.Value); HelpMarker("Auto airblast master switch");
			WToggle("Rage airblast###TriggerAirRage", &Vars::Triggerbot::Blast::Rage.Value); HelpMarker("Will airblast whenever possible, regardless of FoV");
			WToggle("Silent###triggerblastsilent", &Vars::Triggerbot::Blast::Silent.Value); HelpMarker("Aim changes made by the rage mode setting aren't visible");
			WToggle("Extinguish Players###TriggerExtinguishPlayers", &Vars::Triggerbot::Blast::ExtinguishPlayers.Value); HelpMarker("Will automatically extinguish burning players");
			WToggle("Disable on Attack###TriggerDisableOnAttack", &Vars::Triggerbot::Blast::DisableOnAttack.Value); HelpMarker("Will not air blast while attacking");
			WSlider("FOV####AirBlastFov", &Vars::Triggerbot::Blast::Fov.Value, 0, 90, "%d", ImGuiSliderFlags_AlwaysClamp);

			SectionTitle("Autouber");
			WToggle("Autouber###Triggeruber", &Vars::Triggerbot::Uber::Active.Value); HelpMarker("Auto uber master switch");
			WToggle("Only uber friends", &Vars::Triggerbot::Uber::OnlyFriends.Value); HelpMarker("Auto uber will only activate if healing steam friends");
			WToggle("Preserve self", &Vars::Triggerbot::Uber::PopLocal.Value); HelpMarker("Auto uber will activate if local player's health falls below the percentage");
			WToggle("Vaccinator resistances", &Vars::Triggerbot::Uber::AutoVacc.Value); HelpMarker("Auto uber will automatically find the best resistance and pop when needed (This doesn't work properly)");
			if (Vars::Triggerbot::Uber::AutoVacc.Value)
			{
				MultiCombo({ "Bullet", "Blast", "Fire" }, { &Vars::Triggerbot::Uber::BulletRes.Value, &Vars::Triggerbot::Uber::BlastRes.Value, &Vars::Triggerbot::Uber::FireRes.Value }, "Allowed Resistances");
			}
			{
				static std::vector Names{ "Scout", "Soldier", "Pyro", "Heavy", "Engineer", "Sniper", "Spy" };
				static std::vector Values{ 1 << 0, 1 << 1, 1 << 2, 1 << 4, 1 << 5, 1 << 7, 1 << 8 };

				MultiFlags(Names, Values, &Vars::Triggerbot::Uber::ReactClasses.Value, "Hitscan React Classes###TriggerbotAutoVaccClasses");
			}
			WSlider("Health left (%)###TriggerUberHealthLeft", &Vars::Triggerbot::Uber::HealthLeft.Value, 1.f, 99.f, "%.0f%%", 1.0f); HelpMarker("The amount of health the heal target must be below to actiavte");
			WSlider("Reaction FoV###TriggerUberReactFoV", &Vars::Triggerbot::Uber::ReactFoV.Value, 0, 90, "%d", 1); HelpMarker("Checks whether you are within a certain FoV from legit players before auto ubering.");
			WToggle("Activate charge trigger", &Vars::Triggerbot::Uber::VoiceCommand.Value); HelpMarker("Will ubercharge regardless of anything if your target says activate charge");
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
	// Visuals: ESP
		case VisualsTab::ESP:
		{
			if (BeginTable("VisualsESPTable", 3))
			{
				/* Column 1 */
				if (TableColumnChild("VisualsESPCol1"))
				{
					SectionTitle("ESP Main");
					WToggle("ESP###EnableESP", &Vars::ESP::Main::Active.Value); HelpMarker("Global ESP master switch");
					InputKeybind("ESP Key", Vars::ESP::Main::ESPKey, true, false, "None"); HelpMarker("The key to toggle ESP");
					WToggle("Outlined health bars", &Vars::ESP::Main::Outlinedbar.Value); HelpMarker("Will outline the health bars");
					WToggle("Relative colours", &Vars::ESP::Main::EnableTeamEnemyColors.Value); HelpMarker("Chooses colors relative to your team (team/enemy)");
					if (Vars::ESP::Main::EnableTeamEnemyColors.Value)
					{
						ColorPickerL("Enemy color", Vars::Colours::Enemy.Value);
						ColorPickerL("Team color", Vars::Colours::Friendly.Value, 1);
					}
					else
					{
						ColorPickerL("RED Team color", Vars::Colours::TeamRed.Value);
						ColorPickerL("BLU Team color", Vars::Colours::TeamBlu.Value, 1);
					}
					WToggle("Distance2Alpha", &Vars::ESP::Main::DistanceToAlpha.Value); HelpMarker("Will fade out ESP elements as the distance between you and the player increases");
					WToggle("Dormant sound ESP", &Vars::ESP::Main::DormantSoundESP.Value); HelpMarker("Credits: reestart");
					if (Vars::ESP::Main::DormantSoundESP.Value)
					{
						WSlider("Dormant Decay Time###GlobalDormantDecayTime", &Vars::ESP::Main::DormantTime.Value, 0.015f, 5.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
						WSlider("Dormant Max Dist###GlobalDormantMaxDist", &Vars::ESP::Main::DormantDist.Value, 512, 4096, "%d", ImGuiSliderFlags_Logarithmic);
					}
					WSlider("Networked Max Dist###GlobalNetworkedMaxDist", &Vars::ESP::Main::NetworkedDist.Value, 512, 4096, "%d", ImGuiSliderFlags_Logarithmic);

					SectionTitle("Player ESP");
					WToggle("Player ESP###EnablePlayerESP", &Vars::ESP::Players::Active.Value); HelpMarker("Will draw useful information/indicators on players");
					WToggle("Name ESP###PlayerNameESP", &Vars::ESP::Players::Name.Value); HelpMarker("Will draw the players name");
					WToggle("Custom Name Color", &Vars::ESP::Players::NameCustom.Value); HelpMarker("Custom color for name esp");
					if (Vars::ESP::Players::NameCustom.Value)
					{
						ColorPickerL("Name ESP Color", Vars::ESP::Players::NameColor.Value);
					}
					WToggle("Name ESP box###PlayerNameESPBox", &Vars::ESP::Players::NameBox.Value); HelpMarker("Will draw a box around players name to make it stand out");
					WToggle("Self ESP###SelfESP", &Vars::ESP::Players::ShowLocal.Value); HelpMarker("Will draw ESP on local player (thirdperson)");
					ColorPickerL("Local colour", Vars::Colours::Local.Value);
					MultiFlags({ "Friends", "Teammates", "Enemies"},
						{ (1 << 0), (1 << 1), (1 << 2) },
						&Vars::ESP::Players::IgnoreFlags.Value,
						"Ignore Flags###IgnoreFlagsESP"
					); HelpMarker("Which groups the ESP will ignore.");
					ColorPickerL("Friend colour", Vars::Colours::Friend.Value);
					WCombo("Ignore cloaked###IgnoreCloakESPp", &Vars::ESP::Players::IgnoreCloaked.Value, { "Off", "All", "Only enemies" }); HelpMarker("Which cloaked spies the ESP will ignore drawing on");
					ColorPickerL("Cloaked colour", Vars::Colours::Cloak.Value);
					WCombo("Ubercharge###PlayerUber", &Vars::ESP::Players::Uber.Value, { "Off", "Text", "Bar" }); HelpMarker("Will draw how much ubercharge a medic has");
					ColorPickerL("Ubercharge colour", Vars::Colours::UberColor.Value);
					WCombo("Class###PlayerIconClass", &Vars::ESP::Players::Class.Value, { "Off", "Icon", "Text", "Both" }); HelpMarker("Will draw the class the player is");
					WToggle("Weapon text", &Vars::ESP::Players::WeaponText.Value);
					WToggle("Weapon icons", &Vars::ESP::Players::WeaponIcon.Value); HelpMarker("Shows an icon for the weapon that the player has currently equipped");
					ColorPickerL("Weapon icon/text colour", Vars::Colours::WeaponIcon.Value);
					WToggle("Health bar###ESPPlayerHealthBar", &Vars::ESP::Players::HealthBar.Value); HelpMarker("Will draw a bar visualizing how much health the player has");
					if (Vars::ESP::Players::HealthBarStyle.Value == 0)
					{
						ColorPickerL("Health Bar Top", Vars::Colours::GradientHealthBar.Value.startColour);
						ColorPickerL("Health Bar Bottom", Vars::Colours::GradientHealthBar.Value.endColour, 1);
					}

					WCombo("Health bar style", &Vars::ESP::Players::HealthBarStyle.Value, { "Gradient", "Old" }); HelpMarker("How to draw the health bar");
					if (Vars::ESP::Players::HealthBarStyle.Value == 0)
					{
						ColorPickerL("Overheal Bar Top", Vars::Colours::GradientOverhealBar.Value.startColour);
						ColorPickerL("Overheal Bar Bottom", Vars::Colours::GradientOverhealBar.Value.endColour, 1);
					}
					if (Vars::ESP::Players::HealthBarStyle.Value == 1)
					{
						ColorPickerL("Overheal Colour", Vars::Colours::Overheal.Value);
					}
					WCombo("Health Text###ESPPlayerHealthText", &Vars::ESP::Players::HealthText.Value, { "Off", "Default", "Bar" }); HelpMarker("Draws the player health as a text");
					WToggle("Distance", &Vars::ESP::Players::Distance.Value); HelpMarker("Shows the distance from you to the player in meters");
					WToggle("Condition", &Vars::ESP::Players::Cond.Value); HelpMarker("Will draw what conditions the player is under");
					ColorPickerL("Condition colour", Vars::Colours::Cond.Value);
					WToggle("GUID", &Vars::ESP::Players::GUID.Value); HelpMarker("Show's the players Steam ID");
					WToggle("Choked Packets", &Vars::ESP::Players::Choked.Value); HelpMarker("Shows how many packets the player has choked");
					ColorPickerL("Choked Bar Top", Vars::Colours::ChokedBar.Value.startColour);
					ColorPickerL("Choked Bar Bottom", Vars::Colours::ChokedBar.Value.endColour, 1);
					WToggle("Priorities", &Vars::ESP::Players::Priority.Value); HelpMarker("Displays a player's priority.");
					if (Vars::ESP::Players::Priority.Value)
					{
						Text("Friend/Ignore Color");
						ColorPickerL("Frignore Color", Vars::Colours::Friend.Value);
						Text("Rage Color");
						ColorPickerL("Rage Color", Vars::Colours::Rage.Value);
						Text("Cheater Color");
						ColorPickerL("Cheater Color", Vars::Colours::Cheater.Value);
					}
					WCombo("Box###PlayerBoxESP", &Vars::ESP::Players::Box.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on players");
					if (Vars::ESP::Players::Box.Value == 2)
					{
						WSlider("Horizontal Length", &Vars::ESP::Main::CornerHorizLength.Value, 1, 10, "%d"); HelpMarker("Show 1/x of the horizontal box line");
						WSlider("Vertical Length", &Vars::ESP::Main::CornerVertLength.Value, 1, 10, "%d"); HelpMarker("Show 1/x of the vertical box line");
					}
					WCombo("Skeleton###PlayerSkellington", &Vars::ESP::Players::Bones.Value, { "Off", "Custom colour", "Health" }); HelpMarker("Will draw the bone structure of the player");
					ColorPickerL("Skellington colour", Vars::Colours::Bones.Value);
					WToggle("Lines###Playerlines", &Vars::ESP::Players::Lines.Value); HelpMarker("Draws lines from the local players position to enemies position");
					WToggle("Dlights###PlayerDlights", &Vars::ESP::Players::Dlights.Value); HelpMarker("Will make players emit a dynamic light around them");
					if (Vars::ESP::Players::Dlights.Value)
					{
						WSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Players::DlightRadius.Value, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
					}
					WSlider("ESP alpha###PlayerESPAlpha", &Vars::ESP::Players::Alpha.Value, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
					WToggle("Sniper sightlines", &Vars::ESP::Players::SniperSightlines.Value);
				} EndChild();

				/* Column 2 */
				if (TableColumnChild("VisualsESPCol2"))
				{
					SectionTitle("Building ESP");
					WToggle("Building ESP###BuildinGESPSwioifas", &Vars::ESP::Buildings::Active.Value); HelpMarker("Will draw useful information/indicators on buildings");
					WToggle("Ignore team buildings###BuildingESPIgnoreTeammates", &Vars::ESP::Buildings::IgnoreTeammates.Value); HelpMarker("Whether or not to draw ESP on your teams buildings");
					WToggle("Name ESP###BuildingNameESP", &Vars::ESP::Buildings::Name.Value); HelpMarker("Will draw the players name");
					WToggle("Custom Name Color", &Vars::ESP::Buildings::NameCustom.Value); HelpMarker("Custom color for name esp");
					if (Vars::ESP::Buildings::NameCustom.Value)
					{
						ColorPickerL("Name ESP Color", Vars::ESP::Buildings::NameColor.Value);
					}
					WToggle("Name ESP box###BuildingNameESPBox", &Vars::ESP::Buildings::NameBox.Value); HelpMarker("Will draw a box around the buildings name to make it stand out");
					WToggle("Health bar###Buildinghelathbar", &Vars::ESP::Buildings::HealthBar.Value); HelpMarker("Will draw a bar visualizing how much health the building has");
					WToggle("Health text###buildinghealth", &Vars::ESP::Buildings::Health.Value); HelpMarker("Will draw the building's health, as well as its max health");
					WToggle("Distance", &Vars::ESP::Buildings::Distance.Value); HelpMarker("Shows the distance from you to the building in meters");
					WToggle("Building owner###Buildingowner", &Vars::ESP::Buildings::Owner.Value); HelpMarker("Shows who built the building");
					WToggle("Building level###Buildinglevel", &Vars::ESP::Buildings::Level.Value); HelpMarker("Will draw what level the building is");
					WToggle("Building condition###Buildingconditions", &Vars::ESP::Buildings::Cond.Value); HelpMarker("Will draw what conditions the building is under");
					WToggle("Teleporter exit direction###Buildingteleexitdir", &Vars::ESP::Buildings::TeleExitDir.Value); HelpMarker("Show teleporter exit direction arrow");
					ColorPickerL("Teleporter exit direction arrow color", Vars::ESP::Buildings::TeleExitDirColor.Value);
					WToggle("Lines###buildinglines", &Vars::ESP::Buildings::Lines.Value); HelpMarker("Draws lines from the local players position to the buildings position");
					WCombo("Box###PBuildingBoxESP", &Vars::ESP::Buildings::Box.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on buildings");
					WToggle("Dlights###PlayerDlights", &Vars::ESP::Buildings::Dlights.Value); HelpMarker("Will make buildings emit a dynamic light around them, although buildings can't move some I'm not sure that the lights are actually dynamic here...");
					if (Vars::ESP::Buildings::Dlights.Value)
					{
						WSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Buildings::DlightRadius.Value, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
					}
					WSlider("ESP alpha###BuildingESPAlpha", &Vars::ESP::Buildings::Alpha.Value, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the ESP should be");
				} EndChild();

				/* Column 3 */
				if (TableColumnChild("VisualsESPCol3"))
				{
					SectionTitle("World ESP");

					WToggle("World ESP###WorldESPActive", &Vars::ESP::World::Active.Value); HelpMarker("World ESP master switch");
					WToggle("Pickup Timers", &Vars::Visuals::PickupTimers.Value); HelpMarker("Displays the respawn time of health and ammopacks");
					WSlider("ESP alpha###WordlESPAlpha", &Vars::ESP::World::Alpha.Value, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the world ESP should be");

					SectionTitle("Healthpack");
					WToggle("Name###WorldESPHealthpackName", &Vars::ESP::World::HealthName.Value); HelpMarker("Will draw ESP on healthpacks");
					WToggle("Line###WorldESPHealthpackLine", &Vars::ESP::World::HealthLine.Value); HelpMarker("Will draw a line to healthpacks");
					WCombo("Box###WorldESPHealthpackBox", &Vars::ESP::World::HealthBox.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on healthpacks");
					WToggle("Distance###WorldESPHealthpackDistance", &Vars::ESP::World::HealthDistance.Value); HelpMarker("Shows the distance from you to the health pack in meters");
					ColorPickerL("Healthpack colour", Vars::Colours::Health.Value); HelpMarker("Color for healthpack ESP");

					SectionTitle("Ammopack");
					WToggle("Name###WorldESPAmmopackName", &Vars::ESP::World::AmmoName.Value); HelpMarker("Will draw ESP on ammopacks");
					WToggle("Line###WorldESPAmmopackLine", &Vars::ESP::World::AmmoLine.Value); HelpMarker("Will draw a line to ammopacks");
					WCombo("Box###WorldESPAmmopackBox", &Vars::ESP::World::AmmoBox.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on ammopacks");
					WToggle("Distance###WorldESPAmmopackDistance", &Vars::ESP::World::AmmoDistance.Value); HelpMarker("Shows the distance from you to the ammo box in meters");
					ColorPickerL("Ammopack colour", Vars::Colours::Ammo.Value); HelpMarker("Color for ammopack ESP");

					SectionTitle("NPC");
					WToggle("Name###WorldESPNPCName", &Vars::ESP::World::NPCName.Value); HelpMarker("Will draw ESP on NPCs");
					WToggle("Line###WorldESPNPCLine", &Vars::ESP::World::NPCLine.Value); HelpMarker("Will draw a line to NPCs");
					WCombo("Box###WorldESPNPCBox", &Vars::ESP::World::NPCBox.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on NPCs");
					WToggle("Distance###WorldESPNPCDistance", &Vars::ESP::World::NPCDistance.Value); HelpMarker("Shows the distance from you to the NPC in meters");
					ColorPickerL("NPC colour", Vars::Colours::NPC.Value); HelpMarker("Color for NPC ESP");

					SectionTitle("Bombs");
					WToggle("Name###WorldESPBombName", &Vars::ESP::World::BombName.Value); HelpMarker("Will draw ESP on bombs");
					WToggle("Line###WorldESPBombLine", &Vars::ESP::World::BombLine.Value); HelpMarker("Will draw a line to bombs");
					WCombo("Box###WorldESPBombBox", &Vars::ESP::World::BombBox.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on bombs");
					WToggle("Distance###WorldESPBombDistance", &Vars::ESP::World::BombDistance.Value); HelpMarker("Shows the distance from you to the bomb in meters");
					ColorPickerL("Bomb Colour", Vars::Colours::Bomb.Value); HelpMarker("Color for bomb ESP");

					SectionTitle("Spellbooks");
					WToggle("Name###WorldESPSpellbookName", &Vars::ESP::World::SpellbookName.Value); HelpMarker("Will draw ESP on Spellbooks");
					WToggle("Line###WorldESPSpellbookLine", &Vars::ESP::World::SpellbookLine.Value); HelpMarker("Will draw a line to Spellbooks");
					WCombo("Box###WorldESPSpellbookBox", &Vars::ESP::World::SpellbookBox.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on Spellbooks");
					WToggle("Distance###WorldESPSpellbookDistance", &Vars::ESP::World::SpellbookDistance.Value); HelpMarker("Shows the distance from you to the Spellbook in meters");
					ColorPickerL("Spellbook Colour", Vars::Colours::Spellbook.Value); HelpMarker("Color for Spellbook ESP");

					SectionTitle("Gargoyles");
					WToggle("Name###WorldESPGargoyleName", &Vars::ESP::World::GargoyleName.Value); HelpMarker("Will draw ESP on Gargoyles");
					WToggle("Line###WorldESPGargoyleLine", &Vars::ESP::World::GargoyleLine.Value); HelpMarker("Will draw a line to Gargoyles");
					WCombo("Box###WorldESPGargoyleBox", &Vars::ESP::World::GargoyleBox.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on Gargoyles");
					WToggle("Distance###WorldESPGargoyleDistance", &Vars::ESP::World::GargoyleDistance.Value); HelpMarker("Shows the distance from you to the Gargoyle in meters");
					ColorPickerL("Gargoyle Colour", Vars::Colours::Gargoyle.Value); HelpMarker("Color for Gargoyle ESP");

					SectionTitle("Credits");
					WToggle("Name###WorldESPCreditName", &Vars::ESP::World::CreditName.Value); HelpMarker("Will draw ESP on Credits");
					WToggle("Line###WorldESPCreditLine", &Vars::ESP::World::CreditLine.Value); HelpMarker("Will draw a line to Credits");
					WCombo("Box###WorldESPCreditBox", &Vars::ESP::World::CreditBox.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on Credits");
					WToggle("Distance###WorldESPCreditDistance", &Vars::ESP::World::CreditDistance.Value); HelpMarker("Shows the distance from you to the Credits in meters");
					ColorPickerL("Credits Colour", Vars::Colours::Credits.Value); HelpMarker("Color for Credit ESP");

				} EndChild();

				EndTable();
			}
			break;
		}


		// Visuals: Chams
		case VisualsTab::Chams:
		{
			if (BeginTable("VisualsChamsTable", 2))
			{
				/* Column 1 */
				if (TableColumnChild("VisualsChamsCol1"))
				{
					SectionTitle("Chams Main");
					WToggle("Chams###ChamsMasterSwitch", &Vars::Chams::Main::Active.Value); HelpMarker("Chams master switch");

					static std::vector chamOptions{
						"Local",
						"FakeAngles",
						"Friends",
						"Enemies",
						"Teammates",
						"Target",
						"Ragdolls",
						"ViewModel",
						"VM Weapon"
					};
					static std::vector DMEProxyMaterials{
						"None",
						"Spectrum Splattered",
						"Electro Skulls",
						"Jazzy",
						"Frozen Aurora",
						"Hana",
						"IDK",
						"Ghost Thing",
						"Flames",
						"Spook Wood",
						"Edgy",
						"Starlight Serenity",
						"Fade",
						"Bad to the Bone",
						"Skulls and Roses"
					};
					static std::vector dmeGlowMaterial{
						"None",
						"Fresnel Glow",
						"Wireframe Glow"
					};

					static int currentSelected = 0; // 0.local 1.friends 2.enemies 3.team 4.target 5.ragdolls 6.hands 7.weapon
					Chams_t& currentStruct = ([&]() -> Chams_t&
						{
							switch (currentSelected)
							{
							case 0:
							{
								return Vars::Chams::Players::Local.Value;
							}
							case 1:
							{
								return Vars::Chams::Players::FakeAng.Value;
							}
							case 2:
							{
								return Vars::Chams::Players::Friend.Value;
							}
							case 3:
							{
								return Vars::Chams::Players::Enemy.Value;
							}
							case 4:
							{
								return Vars::Chams::Players::Team.Value;
							}
							case 5:
							{
								return Vars::Chams::Players::Target.Value;
							}
							case 6:
							{
								return Vars::Chams::Players::Ragdoll.Value;
							}
							case 7:
							{
								return Vars::Chams::DME::Hands.Value;
							}
							case 8:
							{
								return Vars::Chams::DME::Weapon.Value;
							}
							}

							return Vars::Chams::Players::Local.Value;
						}());
					static std::vector DMEChamMaterials{ "Original", "Shaded", "Shiny", "Flat", "Wireframe shaded", "Wireframe shiny", "Wireframe flat", "Fresnel", "Brick", "Custom" };

					//WToggle("Player chams###PlayerChamsBox", &Vars::Chams::Players::Active.Value); HelpMarker("Player chams master switch");

					MultiCombo({ "Render Wearable", "Render Weapon", "Fadeout Own Team" }, { &Vars::Chams::Players::Wearables.Value, &Vars::Chams::Players::Weapons.Value, &Vars::Chams::Players::FadeoutTeammates.Value }, "Flags");
					HelpMarker("Customize Chams");
					WCombo("Config", &currentSelected, chamOptions);
					{
						ColorPickerL("Colour", currentStruct.colour, 1);
						MultiCombo({ "Active", "Obstructed" }, { &currentStruct.chamsActive, &currentStruct.showObstructed }, "Options");

						WCombo("Material", &currentStruct.drawMaterial, DMEChamMaterials); HelpMarker("Which material the chams will apply to the player");
						if (currentStruct.drawMaterial == 7)
						{
							ColorPickerL("Fresnel base colour", currentStruct.fresnelBase, 1);
						}
						if (currentStruct.drawMaterial == 9)
						{
							MaterialCombo("Custom Material", &currentStruct.customMaterial);
						}
						WCombo("Glow Overlay", &currentStruct.overlayType, dmeGlowMaterial);
						ColorPickerL("Glow Colour", currentStruct.overlayColour, 1);
						WToggle("Rainbow Glow", &currentStruct.overlayRainbow);
						WToggle("Pulse Glow", &currentStruct.overlayPulse);
						WSlider("Glow Reduction", &currentStruct.overlayIntensity, 150.f, 0.1f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

						if (currentSelected == 7 || currentSelected == 8)
						{
							int& proxySkinIndex = currentSelected == 8 ? Vars::Chams::DME::WeaponsProxySkin.Value : Vars::Chams::DME::HandsProxySkin.Value;
							WCombo("Proxy Material", &proxySkinIndex, DMEProxyMaterials);

						}
					}

					SectionTitle("Backtrack chams");
					WToggle("Backtrack chams", &Vars::Backtrack::BtChams::Enabled.Value); HelpMarker("Draws chams to show where a player is");
					ColorPickerL("Backtrack colour", Vars::Backtrack::BtChams::BacktrackColor.Value);
					ColorPickerL("Backtrack colour 2", Vars::Backtrack::BtChams::BacktrackColor2.Value, 1);
					WToggle("Only draw last tick", &Vars::Backtrack::BtChams::LastOnly.Value); HelpMarker("Only draws the last tick (can save FPS)");
					if (!Vars::Backtrack::BtChams::LastOnly.Value)
					{
						WToggle("Gradient###BTChams", &Vars::Backtrack::BtChams::Gradient.Value); HelpMarker("Will draw a gradient on the backtrack chams");
					}
					WToggle("Enemy only", &Vars::Backtrack::BtChams::EnemyOnly.Value); HelpMarker("You CAN backtrack your teammates. (Whip, medigun)");

					static std::vector backtrackMaterial{
						"Original",
						"Shaded",
						"Shiny",
						"Flat",
						"Wireframe shaded",
						"Wireframe shiny",
						"Wireframe flat",
						"Fresnel",
						"Brick"
					};
					WCombo("Backtrack material", &Vars::Backtrack::BtChams::Material.Value, backtrackMaterial);
					WToggle("Ignore Z###BtIgnoreZ", &Vars::Backtrack::BtChams::IgnoreZ.Value); HelpMarker("Draws them through walls");
				} EndChild();


				/* Column 2 */
				if (TableColumnChild("VisualsChamsCol2"))
				{
					SectionTitle("Building Chams");
					WToggle("Building chams###BuildingChamsBox", &Vars::Chams::Buildings::Active.Value); HelpMarker("Building chams master switch");

					static std::vector chamOptionsBuilds{
						"Local",
						"Friends",
						"Enemies",
						"Teammates",
						"Target"
					};
					static std::vector dmeGlowMaterialBuilds{
						"None",
						"Fresnel Glow",
						"Wireframe Glow"
					};

					static int currentSelectedBuilds = 0; //
					Chams_t& currentStructBuilds = ([&]() -> Chams_t&
						{
							switch (currentSelectedBuilds)
							{
							case 0:
							{
								return Vars::Chams::Buildings::Local.Value;
							}
							case 1:
							{
								return Vars::Chams::Buildings::Friend.Value;
							}
							case 2:
							{
								return Vars::Chams::Buildings::Enemy.Value;
							}
							case 3:
							{
								return Vars::Chams::Buildings::Team.Value;
							}
							case 4:
							{
								return Vars::Chams::Buildings::Target.Value;
							}
							}

							return Vars::Chams::Buildings::Local.Value;
						}());
					static std::vector DMEChamMaterialsBuilds{ "Original", "Shaded", "Shiny", "Flat", "Wireframe shaded", "Wireframe shiny", "Wireframe flat", "Fresnel", "Brick", "Custom" };

					WCombo("Config###ChamsConfBuilds", &currentSelectedBuilds, chamOptionsBuilds);
						{
							ColorPickerL("Colour###ChamsClrBuilds", currentStructBuilds.colour, 1);
							MultiCombo({ "Active", "Obstructed" }, { &currentStructBuilds.chamsActive, &currentStructBuilds.showObstructed }, "Options###ChamsViewOptionsBuilds");

							WCombo("Material###ChamsMaterialsBuilds", &currentStructBuilds.drawMaterial, DMEChamMaterialsBuilds); HelpMarker("Which material the chams will apply to the player");
								if (currentStructBuilds.drawMaterial == 7)
							{
								ColorPickerL("Fresnel base colour###ChamsFrsBaseClrBuilds", currentStructBuilds.fresnelBase, 1);
							}
							if (currentStructBuilds.drawMaterial == 9)
							{
								MaterialCombo("Custom Material###ChamsCustomMaterialBuilds", &currentStructBuilds.customMaterial);
							}
							WCombo("Glow Overlay###ChamsGlowOverlayBuilds", &currentStructBuilds.overlayType, dmeGlowMaterialBuilds);
							ColorPickerL("Glow Colour###ChamsGlowClrBuilds", currentStructBuilds.overlayColour, 1);
							WToggle("Rainbow Glow###ChamsRnbwGlowBuilds", &currentStructBuilds.overlayRainbow);
							WToggle("Pulse Glow###ChamsPulseGlowBuilds", &currentStructBuilds.overlayPulse);
							WSlider("Glow Reduction###ChamsGlowRedBuilds", &currentStructBuilds.overlayIntensity, 150.f, 0.1f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
						}

					SectionTitle("World Chams");
					WToggle("World chams###woldchamsbut", &Vars::Chams::World::Active.Value);

					static std::vector chamOptionsWorld{
						"Healthpacks",
						"Ammopacks",
						"Projectiles",
						"Spellbooks",
						"Gargoyles",
						"NPCs",
						"Credits"
					};
					static std::vector dmeGlowMaterialWorld{
						"None",
						"Fresnel Glow",
						"Wireframe Glow"
					};

					static int currentSelectedWorld = 0; //
					Chams_t& currentStructWorld = ([&]() -> Chams_t&
						{
							switch (currentSelectedWorld)
							{
							case 0:
							{
								return Vars::Chams::World::Health.Value;
							}
							case 1:
							{
								return Vars::Chams::World::Ammo.Value;
							}
							case 2:
							{
								return Vars::Chams::World::Projectiles.Value;
							}
							case 3:
							{
								return Vars::Chams::World::Spellbooks.Value;
							}
							case 4:
							{
								return Vars::Chams::World::Gargoyles.Value;
							}
							case 5:
							{
								return Vars::Chams::World::NPCs.Value;
							}
							case 6:
							{
								return Vars::Chams::World::Credits.Value;
							}
							}

							return Vars::Chams::World::Health.Value;
						}());
					static std::vector DMEChamMaterialsWorld{ "Original", "Shaded", "Shiny", "Flat", "Wireframe shaded", "Wireframe shiny", "Wireframe flat", "Fresnel", "Brick", "Custom" };

					WCombo("Config###ChamsConfWorld", &currentSelectedWorld, chamOptionsWorld);
					{
						ColorPickerL("Colour###ChamsClrWorld", currentStructWorld.colour, 1);
						MultiCombo({ "Active", "Obstructed" }, { &currentStructWorld.chamsActive, &currentStructWorld.showObstructed }, "Options###ChamsViewOptionsWorld");

						WCombo("Material###ChamsMaterialsWorld", &currentStructWorld.drawMaterial, DMEChamMaterialsWorld); HelpMarker("Which material the chams will apply to the player");
						if (currentStructWorld.drawMaterial == 7)
						{
							ColorPickerL("Fresnel base colour###ChamsFrsBaseClrWorld", currentStructWorld.fresnelBase, 1);
						}
						if (currentStructWorld.drawMaterial == 9)
						{
							MaterialCombo("Custom Material###ChamsCustomMaterialWorld", &currentStructWorld.customMaterial);
						}
						WCombo("Glow Overlay###ChamsOverlayWorld", &currentStructWorld.overlayType, dmeGlowMaterialWorld);
						ColorPickerL("Glow Colour###ChamsGlowClrWorld", currentStructWorld.overlayColour, 1);
						WToggle("Rainbow Glow###ChamsRnbwGlowWorld", &currentStructWorld.overlayRainbow);
						WToggle("Pulse Glow###ChamsPulseGlowWorld", &currentStructWorld.overlayPulse);
						WSlider("Glow Reduction###ChamsGlowRedWorld", &currentStructWorld.overlayIntensity, 150.f, 0.1f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
					}
				} EndChild();
				EndTable();
			}
			break;
		}

		// Visuals: Glow
		case VisualsTab::Glow:
		{
			if (BeginTable("VisualsGlowTable", 2))
			{
				/* Column 1 */
				if (TableColumnChild("VisualsGlowCol1"))
				{
					SectionTitle("Glow Main");
					WToggle("Glow", &Vars::Glow::Main::Active.Value);
					WCombo("Glow Type###GlowTypeSelect", &Vars::Glow::Main::Type.Value, { "Blur", "Stencil", "FPStencil", "Wireframe" }); HelpMarker("Method in which glow will be rendered");
					if (Vars::Glow::Main::Type.Value != 1) { WSlider("Glow scale", &Vars::Glow::Main::Scale.Value, 1, 10, "%d", ImGuiSliderFlags_AlwaysClamp); }

					SectionTitle("Player Glow");
					WToggle("Player glow###PlayerGlowButton", &Vars::Glow::Players::Active.Value); HelpMarker("Player glow master switch");
					WToggle("Self glow###SelfGlow", &Vars::Glow::Players::ShowLocal.Value); HelpMarker("Draw glow on the local player");
					WToggle("Self rainbow glow###SelfGlowRainbow", &Vars::Glow::Players::LocalRainbow.Value); HelpMarker("Homosapien");
					WCombo("Ignore team###IgnoreTeamGlowp", &Vars::Glow::Players::IgnoreTeammates.Value, { "Off", "All", "Only friends" }); HelpMarker("Which teammates the glow will ignore drawing on");
					WToggle("Wearable glow###PlayerWearableGlow", &Vars::Glow::Players::Wearables.Value); HelpMarker("Will draw glow on player cosmetics");
					WToggle("Weapon glow###PlayerWeaponGlow", &Vars::Glow::Players::Weapons.Value); HelpMarker("Will draw glow on player weapons");
					WSlider("Glow alpha###PlayerGlowAlpha", &Vars::Glow::Players::Alpha.Value, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
					WCombo("Glow colour###GlowColour", &Vars::Glow::Players::Color.Value, { "Team", "Health" }); HelpMarker("Which colour the glow will draw");

					SectionTitle("Misc Glow");
					WToggle("Prediction glow", &Vars::Glow::Misc::MovementSimLine.Value);
					WToggle("Projectile Trajectory Glow", &Vars::Glow::Misc::ProjectileTrajectory.Value);
					WToggle("Sightline glow", &Vars::Glow::Misc::Sightlines.Value);
					WToggle("Bullet tracer glow", &Vars::Glow::Misc::BulletTracers.Value);
				} EndChild();


				/* Column 2 */
				if (TableColumnChild("VisualsGlowCol2"))
				{
					SectionTitle("Building Glow");
					WToggle("Building glow###BuildiongGlowButton", &Vars::Glow::Buildings::Active.Value);
					WToggle("Ignore team buildings###buildingglowignoreteams", &Vars::Glow::Buildings::IgnoreTeammates.Value);
					WSlider("Glow alpha###BuildingGlowAlpha", &Vars::Glow::Buildings::Alpha.Value, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
					WCombo("Glow colour###GlowColourBuildings", &Vars::Glow::Buildings::Color.Value, { "Team", "Health" });

					SectionTitle("World Glow");
					WToggle("World glow###Worldglowbutton", &Vars::Glow::World::Active.Value);
					WToggle("Healthpacks###worldhealthpackglow", &Vars::Glow::World::Health.Value);
					WToggle("Ammopacks###worldammopackglow", &Vars::Glow::World::Ammo.Value);
					WToggle("NPCs###worldnpcs", &Vars::Glow::World::NPCs.Value);
					WToggle("Bombs###worldbombglow", &Vars::Glow::World::Bombs.Value);
					WToggle("Spellbooks###worldspellbookglow", &Vars::Glow::World::Spellbooks.Value);
					WToggle("Gargoyles###worldgargoyleglow", &Vars::Glow::World::Gargoyles.Value);
					WToggle("Credits###worldcreditglow", &Vars::Glow::World::Credits.Value);
					WCombo("Projectile glow###teamprojectileglow", &Vars::Glow::World::Projectiles.Value, { "Off", "All", "Only enemies" });
					WSlider("Glow alpha###WorldGlowAlpha", &Vars::Glow::World::Alpha.Value, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
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
				static std::vector fontFlagNames{ "Italic", "Underline", "Strikeout", "Symbol", "Antialias", "Gaussian", "Rotary", "Dropshadow", "Additive", "Outline", "Custom" };
				static std::vector fontFlagValues{ 0x001, 0x002, 0x004, 0x008, 0x010, 0x020, 0x040, 0x080, 0x100, 0x200, 0x400 };

				/* Column 1 */
				if (TableColumnChild("VisualsFontCol1"))
				{
					SectionTitle("ESP Font");
					WInputText("Font name###espfontname", &Vars::Fonts::FONT_ESP::szName.Value);
					WInputInt("Font height###espfontheight", &Vars::Fonts::FONT_ESP::nTall.Value);
					WInputInt("Font weight###espfontweight", &Vars::Fonts::FONT_ESP::nWeight.Value);
					MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP::nFlags.Value, "Font flags###FONT_ESP");

					SectionTitle("Name Font");
					WInputText("Font name###espfontnamename", &Vars::Fonts::FONT_ESP_NAME::szName.Value);
					WInputInt("Font height###espfontnameheight", &Vars::Fonts::FONT_ESP_NAME::nTall.Value);
					WInputInt("Font weight###espfontnameweight", &Vars::Fonts::FONT_ESP_NAME::nWeight.Value);
					MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP_NAME::nFlags.Value, "Font flags###FONT_ESP_NAME");
				} EndChild();

				/* Column 2 */
				if (TableColumnChild("VisualsFontCol2"))
				{
					SectionTitle("Condition Font");
					WInputText("Font name###espfontcondname", &Vars::Fonts::FONT_ESP_COND::szName.Value);
					WInputInt("Font height###espfontcondheight", &Vars::Fonts::FONT_ESP_COND::nTall.Value);
					WInputInt("Font weight###espfontcondweight", &Vars::Fonts::FONT_ESP_COND::nWeight.Value);
					MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP_COND::nFlags.Value, "Font flags###FONT_ESP_COND");

					SectionTitle("Pickup Font");
					WInputText("Font name###espfontpickupsname", &Vars::Fonts::FONT_ESP_PICKUPS::szName.Value);
					WInputInt("Font height###espfontpickupsheight", &Vars::Fonts::FONT_ESP_PICKUPS::nTall.Value);
					WInputInt("Font weight###espfontpickupsweight", &Vars::Fonts::FONT_ESP_PICKUPS::nWeight.Value);
					MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP_PICKUPS::nFlags.Value, "Font flags###FONT_ESP_PICKUPS");
				} EndChild();

				/* Column 3 */
				if (TableColumnChild("VisualsFontCol3"))
				{
					SectionTitle("Menu Font");
					WInputText("Font name###espfontnamenameneby", &Vars::Fonts::FONT_MENU::szName.Value);
					WInputInt("Font height###espfontnameheightafsdfads", &Vars::Fonts::FONT_MENU::nTall.Value);
					WInputInt("Font weight###espfontnameweightasfdafsd", &Vars::Fonts::FONT_MENU::nWeight.Value);
					MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_MENU::nFlags.Value, "Font flags###FONT_MENU");

					SectionTitle("Indicator Font");
					WInputText("Font name###espfontindicatorname", &Vars::Fonts::FONT_INDICATORS::szName.Value);
					WInputInt("Font height###espfontindicatorheight", &Vars::Fonts::FONT_INDICATORS::nTall.Value);
					WInputInt("Font weight###espfontindicatorweight", &Vars::Fonts::FONT_INDICATORS::nWeight.Value);
					MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_INDICATORS::nFlags.Value, "Font flags###FONT_INDICATORS");

					if (Button("Apply settings###fontapply"))
					{
						g_Draw.RemakeFonts();
					}
				} EndChild();

				EndTable();
			}
			break;
		}

			// Visuals: Misc
		case VisualsTab::World:
		{
			if (BeginTable("VisualsWorldTable", 2))
			{
				/* Column 1 */
				if (TableColumnChild("VisualsWorldCol1"))
				{
					SectionTitle("World");
					WSlider("Field of view", &Vars::Visuals::FieldOfView.Value, 70, 150, "%d"); HelpMarker("How many degrees of field of vision you would like");
					WCombo("Vision modifiers", &Vars::Visuals::VisionModifier.Value, { "Off", "Pyrovision", "Halloween", "Romevision" }); HelpMarker("Vision modifiers");
					MultiCombo({ "World", "Sky", "Prop Wireframe" }, { &Vars::Visuals::WorldModulation.Value, &Vars::Visuals::SkyModulation.Value, &Vars::Visuals::PropWireframe.Value }, "Modulations");
					if (ColorPickerL("World modulation colour", Vars::Colours::WorldModulation.Value) ||
						ColorPickerL("Sky modulation colour", Vars::Colours::SkyModulation.Value, 1) ||
						ColorPickerL("Prop modulation colour", Vars::Colours::StaticPropModulation.Value, 2))
					HelpMarker("Select which types of modulation you want to enable");
					if (WCombo("Screen Overlay", &Vars::Visuals::VisualOverlay.Value, { "None", "Fire", "Jarate", "Bleed", "Stealth", "Dodge" }))
					{
						I::ViewRender->SetScreenOverlayMaterial(nullptr);
					}
					{
						G::ShouldUpdateMaterialCache = true;
					}
					MultiCombo({ "Scope", "Zoom", "Disguises", "Taunts", "Interpolation", "Input Delay", "View Punch", "MOTD", "Screen Effects", "Angle Forcing", "Ragdolls", "Screen Overlays", "DSP", "Convar Queries"}, {&Vars::Visuals::RemoveScope.Value, &Vars::Visuals::RemoveZoom.Value, &Vars::Visuals::RemoveDisguises.Value, &Vars::Visuals::RemoveTaunts.Value, &Vars::Misc::DisableInterpolation.Value, &Vars::Misc::FixInputDelay.Value, &Vars::Visuals::RemovePunch.Value, &Vars::Visuals::RemoveMOTD.Value, &Vars::Visuals::RemoveScreenEffects.Value, &Vars::Visuals::PreventForcedAngles.Value, &Vars::Visuals::RemoveRagdolls.Value, &Vars::Visuals::RemoveScreenOverlays.Value, &Vars::Visuals::RemoveDSP.Value, &Vars::Visuals::RemoveConvarQueries.Value}, "Removals");
					HelpMarker("Select what you want to remove");
					WToggle("Particle Colors", &Vars::Visuals::ParticleColors.Value);
					if (Vars::Visuals::ParticleColors.Value)
					{
						WToggle("Rainbow Particles", &Vars::Visuals::RGBParticles.Value);
					}
					if (Vars::Visuals::RGBParticles.Value)
					{
						WSlider("Rainbow Speed", &Vars::Visuals::RainbowSpeed.Value, 0.1, 5, "%.2f");
					}
					ColorPickerL("Particle Color", Vars::Colours::ParticleColor.Value);
					if (Vars::Visuals::HalloweenSpellFootsteps.Value)
					{
						if (!Vars::Visuals::ParticleColors.Value) //Particle colors overrides the color picker
						{
							WCombo("Color Type", &Vars::Visuals::ColorType.Value, { "Color Picker", "Rainbow" });
							if (Vars::Visuals::ColorType.Value == 0)
							{
								ColorPickerL("Footstep Color", Vars::Colours::FeetColor.Value);
							}
						}
						WToggle("Dash only", &Vars::Visuals::DashOnly.Value);
					}
					WToggle("Particles IgnoreZ", &Vars::Visuals::ParticlesIgnoreZ.Value); HelpMarker("All particles will draw through walls");
					WToggle("Clean Screenshots", &Vars::Visuals::CleanScreenshots.Value); HelpMarker("Attempt to remove all visuals in screenshots");
					WToggle("Post processing", &Vars::Visuals::DoPostProcessing.Value); HelpMarker("Toggle post processing effects");
					WToggle("No prop fade", &Vars::Visuals::NoStaticPropFade.Value); HelpMarker("Make props not fade");
					WSlider("Fade Out Team FoV", &Vars::Visuals::FadeOutFoV.Value, 0.f, 60.f); HelpMarker("Fades teammates within FoV (0 means disabled)");

					SectionTitle("AimPos");
					WToggle("Crosshair aim position", &Vars::Visuals::CrosshairAimPos.Value); HelpMarker("Moves the crosshair to the current aim position");
					WToggle("Box aim position", &Vars::Visuals::AimPosSquare.Value);
					WToggle("Viewmodel aim position", &Vars::Visuals::AimbotViewmodel.Value); HelpMarker("Moves the viewmodel to the current aim position");

					SectionTitle("ViewModel");
					WToggle("Viewmodel sway", &Vars::Visuals::ViewmodelSway.Value);
					if (Vars::Visuals::ViewmodelSway.Value)
					{
						WSlider("Viewmodel Sway Scale", &Vars::Visuals::ViewmodelSwayScale.Value, 0.01, 5, "%.2f");
						WSlider("Viewmodel Sway Interp", &Vars::Visuals::ViewmodelSwayInterp.Value, 0.01, 1, "%.2f"); HelpMarker("How long until the viewmodel returns to its original position (in seconds)");
					}
					WSlider("VM Off X", &Vars::Visuals::VMOffsets.Value.x, -45.f, 45.f);
					WSlider("VM Off Y", &Vars::Visuals::VMOffsets.Value.y, -45.f, 45.f);
					WSlider("VM Off Z", &Vars::Visuals::VMOffsets.Value.z, -45.f, 45.f);
					WSlider("VM Roll", &Vars::Visuals::VMRoll.Value, -180, 180);
					WToggle("Anti viewmodel flip", &Vars::Misc::AntiViewmodelFlip.Value); HelpMarker("This is scuffed");

					SectionTitle("Attribute Changer");

					WToggle("Equip region unlock", &Vars::Visuals::EquipRegionUnlock.Value); HelpMarker("This doesn't let you add the equip regions back once you turn it on.");

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
								Vars::Visuals::Skins::Particle.Value = 0;
								break;
							case 1:
								Vars::Visuals::Skins::Particle.Value = 701;
								break;
							case 2:
								Vars::Visuals::Skins::Particle.Value = 702;
								break;
							case 3:
								Vars::Visuals::Skins::Particle.Value = 703;
								break;
							case 4:
								Vars::Visuals::Skins::Particle.Value = 704;
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
								Vars::Visuals::Skins::Effect.Value = 0;
								break;
							case 1:
								Vars::Visuals::Skins::Effect.Value = 701;
								break;
							case 2:
								Vars::Visuals::Skins::Effect.Value = 702;
								break;
							case 3:
								Vars::Visuals::Skins::Effect.Value = 703;
								break;
							case 4:
								Vars::Visuals::Skins::Effect.Value = 704;
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
					WCombo("Sheen", &Vars::Visuals::Skins::Sheen.Value, sheens); HelpMarker("Which sheen to apply to the weapon");
					WToggle("Style override", &Vars::Visuals::Skins::Override.Value);

					if (Button("Apply", ImVec2(45, 20)))
					{
						F::AttributeChanger.SetAttribute();
					}
					SameLine();
					if (Button("Save", ImVec2(45, 20)))
					{
						F::AttributeChanger.SaveConfig();
					}
					SameLine();
					if (Button("Load", ImVec2(44, 20)))
					{
						F::AttributeChanger.LoadConfig();
					}

					SectionTitle("Ragdoll effects");
					WToggle("Enemy only###RagdollEnemyOnly", &Vars::Visuals::RagdollEffects::EnemyOnly.Value); HelpMarker("Only runs it on enemies");
					MultiCombo({ "Burning", "Electrocuted", "Become ash", "Dissolve" }, { &Vars::Visuals::RagdollEffects::Burning.Value, &Vars::Visuals::RagdollEffects::Electrocuted.Value, &Vars::Visuals::RagdollEffects::BecomeAsh.Value, &Vars::Visuals::RagdollEffects::Dissolve.Value }, "Effects###RagdollEffects");
					HelpMarker("Ragdoll particle effects");
					WCombo("Ragdoll model", &Vars::Visuals::RagdollEffects::RagdollType.Value, { "None", "Gold", "Ice" }); HelpMarker("Which ragdoll model should be used");
					HelpMarker("Will make their ragdoll ice");
				} EndChild();

				/* Column 2 */
				if (TableColumnChild("VisualsWorldCol2"))
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
					WToggle("Skybox changer", &Vars::Visuals::SkyboxChanger.Value); HelpMarker("Will change the skybox, either to a base TF2 one or a custom one");
					WCombo("Skybox", &Vars::Skybox::SkyboxNum.Value, skyNames);
					if (Vars::Visuals::SkyboxChanger.Value)
					{
						WCombo("Skybox##SkyboxCombo", &Vars::Skybox::SkyboxNum.Value, skyNames);
						if (Vars::Skybox::SkyboxNum.Value == 0)
						{
							WInputText("Custom skybox name", &Vars::Skybox::SkyboxName.Value); HelpMarker("Name of the skybox you want to you (tf/materials/skybox)");
						}
					}
					WCombo("Precipitation", &Vars::Visuals::Rain.Value, { "Off", "Rain", "Snow" });
					WToggle("World Textures Override", &Vars::Visuals::OverrideWorldTextures.Value); HelpMarker("Turn this off when in-game so you don't drop fps :p");
					WToggle("Bypass sv_pure", &Vars::Misc::BypassPure.Value); HelpMarker("Allows you to load any custom files, even if disallowed by the sv_pure setting");
					WToggle("Medal flip", &Vars::Misc::MedalFlip.Value); HelpMarker("Medal go spinny spinny weeeeeee");

					SectionTitle("Fog");
					WToggle("Disable fog", &Vars::Visuals::Fog::DisableFog.Value);
					WToggle("Custom fog", &Vars::Visuals::Fog::CustomFog.Value);
					if (Vars::Visuals::Fog::CustomFog.Value)
					{
						WSlider("Fog density", &Vars::Visuals::Fog::FogDensity.Value, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
						ColorPickerL("Fog colour", Vars::Visuals::Fog::FogColor.Value);
						WSlider("Fog start", &Vars::Visuals::Fog::FogStart.Value, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None);
						WSlider("Fog end", &Vars::Visuals::Fog::FogEnd.Value, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None);
						WSlider("Skybox fog density", &Vars::Visuals::Fog::FogDensitySkybox.Value, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Skybox fog requires r_3dsky 1");
						ColorPickerL("Skybox fog colour", Vars::Visuals::Fog::FogColorSkybox.Value);
						WSlider("Skybox fog start", &Vars::Visuals::Fog::FogStartSkybox.Value, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None);
						WSlider("Skybox fog end", &Vars::Visuals::Fog::FogEndSkybox.Value, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None);
					}

					SectionTitle("Thirdperson");
					WToggle("Thirdperson", &Vars::Visuals::ThirdPerson.Value); HelpMarker("Will move your camera to be in a thirdperson view");
					InputKeybind("Thirdperson key", Vars::Visuals::ThirdPersonKey, true, false, "None"); HelpMarker("What key to toggle thirdperson, press ESC if no bind is desired");
					WToggle("Show real angles###tpRealAngles", &Vars::Visuals::ThirdPersonSilentAngles.Value); HelpMarker("Will show your real angles on thirdperson (not what others see)");
					WToggle("Instant yaw###tpInstantYaw", &Vars::Visuals::ThirdPersonInstantYaw.Value); HelpMarker("Will set your yaw instantly in thirdperson, showing your actual angle, instead of what others see");
					WToggle("Show server hitboxes (localhost only)###tpShowServer", &Vars::Visuals::ThirdPersonServerHitbox.Value); HelpMarker("Will show the server angles in thirdperson");

					WToggle("Thirdperson offsets", &Vars::Visuals::ThirdpersonOffset.Value); HelpMarker("These will mess you up if you use a small FoV");
					if (Vars::Visuals::ThirdpersonOffset.Value)
					{
						WSlider("Thirdperson distance", &Vars::Visuals::ThirdpersonDist.Value, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
						WSlider("Thirdperson right", &Vars::Visuals::ThirdpersonRight.Value, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
						WSlider("Thirdperson up", &Vars::Visuals::ThirdpersonUp.Value, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
						WToggle("Thirdperson crosshair", &Vars::Visuals::ThirdpersonCrosshair.Value);
						WToggle("Offset with arrow keys", &Vars::Visuals::ThirdpersonOffsetWithArrows.Value);
						InputKeybind("Move offset key", Vars::Visuals::ThirdpersonArrowOffsetKey, false);
					}
				} EndChild();

				EndTable();
			}
			break;
		}

		case VisualsTab::Radar:
		{
			if (BeginTable("VisualsRadarTable", 2))
			{
				/* Column 1 */
				if (TableColumnChild("VisualsRadarCol1"))
				{
					SectionTitle("Main");
					WToggle("Enable Radar###RadarActive", &Vars::Radar::Main::Active.Value); HelpMarker("Will show nearby things relative to your player");
					WSlider("Range###RadarRange", &Vars::Radar::Main::Range.Value, 50, 3000, "%d"); HelpMarker("The range of the radar");
					WSlider("Background alpha###RadarBGA", &Vars::Radar::Main::BackAlpha.Value, 0, 255, "%d"); HelpMarker("The background alpha of the radar");
					WSlider("Line alpha###RadarLineA", &Vars::Radar::Main::LineAlpha.Value, 0, 255, "%d"); HelpMarker("The line alpha of the radar");
					WToggle("No Title Gradient", &Vars::Radar::Main::NoTitleGradient.Value);

					SectionTitle("Players");
					WToggle("Show players###RIOJSADFIOSAJIDPFOJASDFOJASOPDFJSAPOFDJOPS", &Vars::Radar::Players::Active.Value); HelpMarker("lol");
					WCombo("Icon###radari", &Vars::Radar::Players::IconType.Value, { "Scoreboard", "Portraits", "Avatar" }); HelpMarker("What sort of icon to represent players with");
					WCombo("Background###radarb", &Vars::Radar::Players::BackGroundType.Value, { "Off", "Rectangle", "Texture" }); HelpMarker("What sort of background to put on players on the radar");
					WToggle("Outline###radaro", &Vars::Radar::Players::Outline.Value); HelpMarker("Will put an outline on players on the radar");
					WCombo("Ignore teammates###radarplayersteam", &Vars::Radar::Players::IgnoreTeam.Value, { "Off", "All", "Keep friends" }); HelpMarker("Which teammates the radar will ignore drawing on");
					WCombo("Ignore cloaked###radarplayerscloaked", &Vars::Radar::Players::IgnoreCloaked.Value, { "Off", "All", "Keep friends" }); HelpMarker("Which cloaked players the radar will ignore drawing on");
					WToggle("Health bar###radarhealt", &Vars::Radar::Players::Health.Value); HelpMarker("Will show players health on the radar");
					WSlider("Icon size###playersizeiconradar", &Vars::Radar::Players::IconSize.Value, 12, 30, "%d"); HelpMarker("The icon size of players on the radar");
					WToggle("Height indicator###RadarPlayersZ", &Vars::Radar::Players::Height.Value); HelpMarker("Shows a little arrow indicating the height of the player");
				} EndChild();

				/* Column 2 */
				if (TableColumnChild("VisualsRadarCol2"))
				{
					SectionTitle("Building");
					WToggle("Show buildings###radarbuildingsa", &Vars::Radar::Buildings::Active.Value);
					WToggle("Outline###radarbuildingsao", &Vars::Radar::Buildings::Outline.Value);
					WToggle("Ignore team###radarbuildingsb", &Vars::Radar::Buildings::IgnoreTeam.Value);
					WToggle("Health bar###radarbuildingsc", &Vars::Radar::Buildings::Health.Value);
					WSlider("Icon size###buildingsizeiconradar", &Vars::Radar::Buildings::IconSize.Value, 12, 30, "%d");

					SectionTitle("World");
					WToggle("Active###radarworldd", &Vars::Radar::World::Active.Value);
					WToggle("Healthpack###radarworldda", &Vars::Radar::World::Health.Value);
					WToggle("Ammopack###radarworlddb", &Vars::Radar::World::Ammo.Value);
					WSlider("Icon size###worldsizeiconradar", &Vars::Radar::World::IconSize.Value, 12, 30, "%d");
				} EndChild();

				EndTable();
			}
			break;
		}

		case VisualsTab::Indicators:
		{
			if (BeginTable("VisualsIndicatorsTable", 2))
			{
				/* Column 1 */
				if (TableColumnChild("VisualsIndicatorsCol1"))
				{
					SectionTitle("DT Indicator");
					WCombo("DT indicator style", &Vars::Misc::CL_Move::DTBarStyle.Value, { "Off", "Default", "Nitro", "Rijin", "SEOwned", "Numeric" }); HelpMarker("What style the bar should draw in.");
					Text("Charging Gradient");
					ColorPickerL("DT charging right", Vars::Colours::DTBarIndicatorsCharging.Value.endColour);
					ColorPickerL("DT charging left", Vars::Colours::DTBarIndicatorsCharging.Value.startColour, 1);
					Text("Charged Gradient");
					ColorPickerL("DT charged right", Vars::Colours::DTBarIndicatorsCharged.Value.endColour);
					ColorPickerL("DT charged left", Vars::Colours::DTBarIndicatorsCharged.Value.startColour, 1);

					SectionTitle("Spy Warning");
					WToggle("Active###spywarn", &Vars::Visuals::SpyWarning.Value); HelpMarker("Will alert you when spies with their knife out may attempt to backstab you");
					if (Vars::Visuals::SpyWarning.Value)
					{
						WToggle("Voice command###spywarn1", &Vars::Visuals::SpyWarningAnnounce.Value); HelpMarker("Will make your character say \"Spy!\" when a spy is detected");
						WToggle("Visible only###spywarn2", &Vars::Visuals::SpyWarningVisibleOnly.Value); HelpMarker("Will only alert you to visible spies");
						WToggle("Ignore friends###spywarn3", &Vars::Visuals::SpyWarningIgnoreFriends.Value); HelpMarker("Will ignore spies who are on your friends list");
						WCombo("Warning style", &Vars::Visuals::SpyWarningStyle.Value, { "Arrow", "Flash" }); HelpMarker("Choose the style of the spy indicator");
					}

					SectionTitle("Out of FOV arrows");
					WToggle("Active###fovar", &Vars::Visuals::OutOfFOVArrows.Value); HelpMarker("Will draw arrows to players who are outside of the range of your FoV");
					if (Vars::Visuals::OutOfFOVArrows.Value)
					{
						WToggle("Outline arrows###OutlinedArrows", &Vars::Visuals::OutOfFOVArrowsOutline.Value); HelpMarker("16 missed calls");
						WSlider("Arrow length", &Vars::Visuals::ArrowLength.Value, 5.f, 50.f, "%.2f"); HelpMarker("How long the arrows are");
						WSlider("Arrow angle", &Vars::Visuals::ArrowAngle.Value, 5.f, 180.f, "%.2f"); HelpMarker("The angle of the arrow");
						WSlider("Distance from center", &Vars::Visuals::FovArrowsDist.Value, 0.01f, 0.2f, "%.3f"); HelpMarker("How far from the center of the screen the arrows will draw");
						WSlider("Max distance", &Vars::Visuals::MaxDist.Value, 0.f, 4000.f, "%.2f"); HelpMarker("How far until the arrows will not show");
						WSlider("Min distance", &Vars::Visuals::MinDist.Value, 0.f, 1000.f, "%.2f"); HelpMarker("How close until the arrows will be fully opaque");
					}
				} EndChild();

				/* Column 2 */
				if (TableColumnChild("VisualsIndicatorsCol2"))
				{
					SectionTitle("Beams & Tracers");
					{
						using namespace Vars::Visuals;

						WToggle("Enable beams", &Beans::Active.Value); HelpMarker("he loves beans?");
						if (&Beans::Active.Value)
						{
							WToggle("Rainbow beams", &Beans::Rainbow.Value);
							ColorPickerL("Beam colour", Beans::BeamColour.Value);
							WToggle("Custom model", &Beans::UseCustomModel.Value);
							if (Beans::UseCustomModel.Value)
							{
								WInputText("Model", &Beans::Model.Value);
							}
							WSlider("Beam lifespan", &Beans::Life.Value, 0.0f, 10.f);
							WSlider("Beam width", &Beans::Width.Value, 0.0f, 10.f);
							WSlider("Beam end width", &Beans::EndWidth.Value, 0.0f, 10.f);
							WSlider("Beam fade length", &Beans::FadeLength.Value, 0.0f, 30.f);
							WSlider("Beam amplitude", &Beans::Amplitude.Value, 0.0f, 10.f);
							WSlider("Beam brightness", &Beans::Brightness.Value, 0.0f, 255.f);
							WSlider("Beam speed", &Beans::Speed.Value, 0.0f, 5.f);
							WSlider("Segments", &Beans::Segments.Value, 1, 10); //what are good values for this

							// TODO: Reward this ugly code
							{
								static std::vector flagNames{ "STARTENTITY", "ENDENTITY", "FADEIN", "FADEOUT", "SINENOISE", "SOLID", "SHADEIN", "SHADEOUT", "ONLYNOISEONCE", "NOTILE", "USE_HITBOXES", "STARTVISIBLE", "ENDVISIBLE", "ISACTIVE", "FOREVER", "HALOBEAM", "REVERSED", };
								static std::vector flagValues{ 0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080, 0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000, 0x00010000 };
								MultiFlags(flagNames, flagValues, &Beans::Flags.Value, "Beam Flags###BeamFlags");
							}
						}
					}
					WCombo("Particle tracer", &Vars::Visuals::ParticleTracer.Value, { "Off", "Machina", "C.A.P.P.E.R", "Short Circuit", "Merasmus ZAP", "Merasmus ZAP Beam 2", "Big Nasty", "Distortion Trail", "Black Ink", "Custom" });
					if (Vars::Visuals::ParticleTracer.Value == 9)
					{
						WInputText("Custom Tracer", &Vars::Visuals::ParticleName.Value); HelpMarker("If you want to use a custom particle tracer");
					}
					WToggle("Bullet tracers", &Vars::Visuals::BulletTracer.Value);
					ColorPickerL("Bullet tracer colour", Vars::Colours::BulletTracer.Value);
					WToggle("Rainbow tracers", &Vars::Visuals::BulletTracerRainbow.Value); HelpMarker("Bullet tracer color will be dictated by a changing color");

					SectionTitle("Visualisation");
					MultiCombo({ "Line", "Seperators" }, { &Vars::Visuals::MoveSimLine.Value, &Vars::Visuals::MoveSimSeperators.Value }, "Proj Aim Lines");
					ColorPickerL("Prediction Line Color", Vars::Aimbot::Projectile::PredictionColor.Value);
					if (Vars::Visuals::MoveSimSeperators.Value)
					{
						WSlider("Seperator Length", &Vars::Visuals::SeperatorLength.Value, 2, 16, "%d", ImGuiSliderFlags_Logarithmic);
						WSlider("Seperator Spacing", &Vars::Visuals::SeperatorSpacing.Value, 1, 64, "%d", ImGuiSliderFlags_Logarithmic);
					}
					WToggle("Projectile trajectory", &Vars::Visuals::ProjectileTrajectory.Value);
					ColorPickerL("Projectile Line Color", Vars::Aimbot::Projectile::ProjectileColor.Value);
					WToggle("Draw Hitboxes", &Vars::Aimbot::Global::showHitboxes.Value); HelpMarker("Shows client hitboxes for enemies once they are attacked (not bbox)");
					ColorPickerL("Hitbox matrix face colour", Vars::Colours::HitboxFace.Value);
					ColorPickerL("Hitbox matrix edge colour", Vars::Colours::HitboxEdge.Value, 1);
					WSlider("Hitbox Draw Time", &Vars::Aimbot::Global::HitboxLifetime.Value, 1, 5); HelpMarker("Removes previous drawn hitboxes after n seconds");
					WToggle("Clear Hitboxes", &Vars::Aimbot::Global::ClearPreviousHitbox.Value); HelpMarker("Removes previous drawn hitboxes to mitigate clutter");

					SectionTitle("Camera");
					InputKeybind("Proj Cam Key", Vars::Visuals::ProjectileCameraKey, true, false, "None");  HelpMarker("Makes your camera snap to the projectile you most recently fired.");
					InputKeybind("Freecam Key", Vars::Visuals::FreecamKey, true, false, "None");  HelpMarker("Allows you to freely move your camera when holding the key");
					WCombo("Camera mode", &Vars::Visuals::CameraMode.Value, { "Off", "Mirror", "Spy", "Teleporter", "Teleporter (Portal)" }); HelpMarker("What the camera should display");
					if (Vars::Visuals::CameraMode.Value != 0)
					{
						WSlider("Camera FOV", &Vars::Visuals::CameraFOV.Value, 40.f, 130.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("FOV of the camera window");
					}
					WSlider("Freecam Speed", &Vars::Visuals::FreecamSpeed.Value, 1.f, 20.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Movement speed of freecam");

					SectionTitle("Logging");
					WCombo("Spectator list", &Vars::Visuals::SpectatorList.Value, { "Off", "Draggable", "Static", "Static + Avatars" });
					if (Vars::Visuals::SpectatorList.Value != 1)
					{
						WSlider("Static SpecList height", &Vars::Visuals::SpectatorListHeight.Value, 1, 1000); HelpMarker("Try raising or lowering the height if it doesn't show up");
						WToggle("Show Freeze and Deathcam", &Vars::Visuals::SLShowOthers.Value); HelpMarker("Will hide the Death and Freezecam");
					}
					{
						static std::vector flagNames{ "Text", "Console", "Chat", "Party", "Verbose" };
						static std::vector flagValues{ 1, 2, 4, 8, 32 };
						MultiFlags(flagNames, flagValues, &Vars::Misc::VotingOptions.Value, "Vote Logger###VoteLoggingOptions");
					}
					MultiCombo({ "Damage Logs (Console)", "Damage Logs (Text)", "Damage Logs (Chat)", "Class Changes (Text)", "Class Changes (Chat)" }, { &Vars::Visuals::DamageLoggerConsole.Value, &Vars::Visuals::DamageLoggerText.Value, &Vars::Visuals::DamageLoggerChat.Value, &Vars::Visuals::ChatInfoText.Value, &Vars::Visuals::ChatInfoChat.Value }, "Event Logging");
					HelpMarker("What & How should events be logged");
					WToggle("ChatInfo Grayscale", &Vars::Visuals::ChatInfoGrayScale.Value);
					MultiCombo({ "Line", "Gradient" }, { &Vars::Visuals::DrawNotifLine.Value, &Vars::Visuals::DrawNotifGradient.Value }, "Draw Notification");
					ColorPickerL("GUI Notif Background", Vars::Colours::NotifBG.Value);
					ColorPickerL("GUI Notif Outline", Vars::Colours::NotifOutline.Value, 1);
					ColorPickerL("GUI Notif Colour", Vars::Colours::NotifText.Value, 2);
					WSlider("GUI Notif Time", &Vars::Visuals::NotificationLifetime.Value, 0.5f, 3.f, "%.1f");
					SectionTitle("On-Screen");
					WToggle("On Screen Local Conditions", &Vars::Visuals::DrawOnScreenConditions.Value); HelpMarker("Render your local conditions on your screen");
					WToggle("On Screen Ping", &Vars::Visuals::DrawOnScreenPing.Value); HelpMarker("Render your ping and your scoreboard ping on the screen");
					WToggle("Killstreak weapon", &Vars::Misc::KillstreakWeapon.Value); HelpMarker("Enables the killstreak counter on any weapon");
					WToggle("Noscope lines", &Vars::Visuals::ScopeLines.Value); HelpMarker("Will draw a custom overlay");
					ColorPickerL("Inner line color", Vars::Colours::NoscopeLines1.Value);
					ColorPickerL("Outer line color", Vars::Colours::NoscopeLines2.Value, 1);
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
	if (BeginTable("HvHTable", 3))
	{
		/* Column 1 */
		if (TableColumnChild("HvHCol1"))
		{
			/* Section: Tickbase Exploits */
			SectionTitle("Tickbase Exploits");
			WToggle("Enable Tickbase Exploits", &Vars::Misc::CL_Move::Enabled.Value); HelpMarker("Allows tickbase shifting");
			ColorPickerL("DT bar outline colour", Vars::Colours::DtOutline.Value);
			InputKeybind("Recharge key", Vars::Misc::CL_Move::RechargeKey, true, false, "None"); HelpMarker("Recharges ticks for shifting");
			InputKeybind("Teleport key", Vars::Misc::CL_Move::TeleportKey, true, false, "None"); HelpMarker("Shifts ticks to warp");
			if (Vars::Misc::CL_Move::DTMode.Value == 0 || Vars::Misc::CL_Move::DTMode.Value == 2)
			{
				InputKeybind("Doubletap key", Vars::Misc::CL_Move::DoubletapKey, true, false, "None"); HelpMarker("Only doubletap when the key is pressed. Leave as (None) for always active.");
			}

			WCombo("Teleport Mode", &Vars::Misc::CL_Move::TeleportMode.Value, { "Plain", "Smooth", "Streamed Smooth"}); HelpMarker("How the teleport should be done");
			if (Vars::Misc::CL_Move::TeleportMode.Value)
			{
				WSlider("Smooth Teleport Factor", &Vars::Misc::CL_Move::TeleportFactor.Value, 2, 6, "%d");
			}
			MultiCombo({ "Recharge While Dead", "Auto Recharge", "Wait for DT", "Anti-warp", "Avoid airborne", "Retain Fakelag", "Safe Tick", "Safe Tick Airborne", "Auto Retain" }, { &Vars::Misc::CL_Move::RechargeWhileDead.Value, &Vars::Misc::CL_Move::AutoRecharge.Value, &Vars::Misc::CL_Move::WaitForDT.Value, &Vars::Misc::CL_Move::AntiWarp.Value, &Vars::Misc::CL_Move::NotInAir.Value, &Vars::Misc::CL_Move::RetainFakelag.Value, &Vars::Misc::CL_Move::SafeTick.Value, &Vars::Misc::CL_Move::SafeTickAirOverride.Value, &Vars::Misc::CL_Move::AutoRetain.Value }, "Options");
			HelpMarker("Enable various features regarding tickbase exploits");
			WCombo("Doubletap Mode", &Vars::Misc::CL_Move::DTMode.Value, { "On key", "Always", "Disable on key", "Disabled" }); HelpMarker("How should DT behave");
			const int ticksMax = g_ConVars.sv_maxusrcmdprocessticks->GetInt() - 3;	// remove the 2 backup cmd's and the cmd that we will send when we dt.
			WSlider("Ticks to shift", &Vars::Misc::CL_Move::DTTicks.Value, 1, ticksMax ? ticksMax : 21, "%d"); HelpMarker("How many ticks to shift");
			WSlider("Passive Recharge Factor", &Vars::Misc::CL_Move::PassiveRecharge.Value, 0, 21, "%d"); HelpMarker("How fast your ticks should charge passively");
			WToggle("SpeedHack", &Vars::Misc::CL_Move::SEnabled.Value); HelpMarker("Speedhack Master Switch");
			if (Vars::Misc::CL_Move::SEnabled.Value)
			{
				WSlider("SpeedHack Factor", &Vars::Misc::CL_Move::SFactor.Value, 1, 66, "%d");
			}
			HelpMarker("High values are not recommended");

			/* Section: Fakelag */
			SectionTitle("Fakelag");
			WToggle("Enable Fakelag", &Vars::Misc::CL_Move::Fakelag.Value);
			WToggle("Only While Moving", &Vars::Misc::CL_Move::WhileMoving.Value);
			WCombo("Fakelag Mode###FLmode", &Vars::Misc::CL_Move::FakelagMode.Value, { "Plain", "Random", "Adaptive" }); HelpMarker("Controls how fakelag will be controlled.");
			WToggle("Retain BlastJump", &Vars::Misc::CL_Move::RetainBlastJump.Value); HelpMarker("Will attempt to retain the blast jumping condition as soldier and runs independently of fakelag.");
			WToggle("Unchoke On Attack", &Vars::Misc::CL_Move::UnchokeOnAttack.Value); HelpMarker("Will exit a fakelag cycle if you are attacking.");

			switch (Vars::Misc::CL_Move::FakelagMode.Value)
			{
				case 0: { WSlider("Fakelag value", &Vars::Misc::CL_Move::FakelagValue.Value, 1, 22, "%d"); HelpMarker("How much lag you should fake(?)"); break; }
				case 1:
				{
					WSlider("Random max###flRandMax", &Vars::Misc::CL_Move::FakelagMax.Value, Vars::Misc::CL_Move::FakelagMin.Value + 1, 22, "%d"); HelpMarker("Maximum random fakelag value");
					WSlider("Random min###flRandMin", &Vars::Misc::CL_Move::FakelagMin.Value, 1, Vars::Misc::CL_Move::FakelagMax.Value - 1, "%d"); HelpMarker("Minimum random fakelag value");
					break;
				}
			}	//	add more here if you add your own fakelag modes :D

		} EndChild();

		if (TableColumnChild("HvHCol2"))
		{
			SectionTitle("Cheater Detection");
			WToggle("Enable Cheater Detection", &Vars::Misc::CheaterDetection::Enabled.Value);
			if (Vars::Misc::CheaterDetection::Enabled.Value)
			{
				{
					static std::vector flagNames{ "Accuracy", "Score", "Simtime Changes", "Packet Choking", "Bunnyhopping", "Aim Flicking", "OOB Angles", "Aimbot", "Duck Speed" };
					static std::vector flagValues{ 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7, 1 << 8 };
					MultiFlags(flagNames, flagValues, &Vars::Misc::CheaterDetection::Methods.Value, "Detection Methods###CheaterDetectionMethods");
					HelpMarker("Methods by which to detect bad actors.");
				}
				{
					static std::vector flagNames{ "Double Scans", "Lagging Client", "Timing Out" };
					static std::vector flagValues{ 1 << 0, 1 << 1, 1 << 2 };
					MultiFlags(flagNames, flagValues, &Vars::Misc::CheaterDetection::Protections.Value, "Ignore Conditions###CheaterDetectionIgnoreMethods");
					HelpMarker("Don't scan in certain scenarios (prevents false positives).");
				}
				WSlider("Suspicion Gate", &Vars::Misc::CheaterDetection::SuspicionGate.Value, 5, 50, "%d"); HelpMarker("Infractions required to mark somebody as a cheater.");

				if (Vars::Misc::CheaterDetection::Methods.Value & (1 << 1))
				{
					WSlider("Analytical High Score Mult", &Vars::Misc::CheaterDetection::ScoreMultiplier.Value, 1.5f, 4.f, "%.2f"); HelpMarker("How much to multiply the average score to treat as a max score per second.");
				}

				if (Vars::Misc::CheaterDetection::Methods.Value & (1 << 3 | 1 << 2))
				{
					WSlider("Packet Manipulation Gate", &Vars::Misc::CheaterDetection::PacketManipGate.Value, 1, 22, "%d"); HelpMarker("Used as the minimum amount of average packet manipulation to infract someone as a cheater.");
				}

				if (Vars::Misc::CheaterDetection::Methods.Value & (1 << 4))
				{
					WSlider("BHop Sensitivity", &Vars::Misc::CheaterDetection::BHopMaxDelay.Value, 1, 5, "%d"); HelpMarker("How many ticks a player can be on the ground before their next jump isn't counted as a bhop.");
					WSlider("BHop Minimum Detections", &Vars::Misc::CheaterDetection::BHopDetectionsRequired.Value, 2, 15, "%d"); HelpMarker("How many concurrent bunnyhops need to be executed before someone is infracted.");
				}

				if (Vars::Misc::CheaterDetection::Methods.Value & (1 << 5))
				{
					WSlider("Minimum Aim-Flick", &Vars::Misc::CheaterDetection::MinimumFlickDistance.Value, 5.f, 30.f, "%.1f"); HelpMarker("The distance someones view angles must flick prior to be being suspected by the cheat detector.");
					WSlider("Maximum Post Flick Noise", &Vars::Misc::CheaterDetection::MaximumNoise.Value, 5.f, 15.f, "%.1f"); HelpMarker("The maximum distance the players mouse can move post-flick before the cheat detector considers it as a legit flick (mouse moved fast, etc).");
				}

				if (Vars::Misc::CheaterDetection::Methods.Value & (1 << 7))
				{
					WSlider("Maximum Scaled Aimbot FoV", &Vars::Misc::CheaterDetection::MaxScaledAimbotFoV.Value, 5.f, 30.f, "%.1f"); HelpMarker("The maximum FoV (post scaling) for the aimbot detection.");
					WSlider("Minimum Aimbot FoV", &Vars::Misc::CheaterDetection::MinimumAimbotFoV.Value, 5.f, 30.f, "%.1f"); HelpMarker("The minimum FoV to infract a player for aimbot.");
				}
			}
			SectionTitle("Resolver");
			WToggle("Enable Resolver", &Vars::AntiHack::Resolver::Resolver.Value); HelpMarker("Enables the anti-aim resolver.");
			if (Vars::AntiHack::Resolver::Resolver.Value)
			{
				WToggle("Ignore in-air", &Vars::AntiHack::Resolver::IgnoreAirborne.Value); HelpMarker("Doesn't resolve players who are in the air.");
			}
		} EndChild();

		/* Column 3 */
		if (TableColumnChild("HvHCol3"))
		{
			/* Section: Anti Aim */
			SectionTitle("Anti Aim");
			WToggle("Enable Anti-aim", &Vars::AntiHack::AntiAim::Active.Value);
			InputKeybind("Anti-aim Key", Vars::AntiHack::AntiAim::ToggleKey, true, false, "None"); HelpMarker("The key to toggle anti aim");
			if (Vars::AntiHack::AntiAim::YawFake.Value == 7 || Vars::AntiHack::AntiAim::YawReal.Value == 7) {
				InputKeybind("Invert Key", Vars::AntiHack::AntiAim::InvertKey, true, false, "None");
			}

			WCombo("Real Pitch", &Vars::AntiHack::AntiAim::PitchReal.Value, { "None", "Up", "Down", "Zero", "Custom"}); HelpMarker("The pitch your hitboxes will be built around.");
			WCombo("Fake Pitch", &Vars::AntiHack::AntiAim::PitchFake.Value, { "None", "Up", "Down"}); HelpMarker("The pitch that other players will see.");
			if (Vars::AntiHack::AntiAim::PitchReal.Value == 4) {
				WSlider("Custom Real Pitch", &Vars::AntiHack::AntiAim::CustomRealPitch.Value, -89.f, 89.f, "%.0f", 0);
			}

			WCombo("Real yaw", &Vars::AntiHack::AntiAim::YawReal.Value, { "None", "Left", "Right", "Forward", "Backwards", "Spin", "Edge", "Invert"}); HelpMarker("The yaw your hitboxes will be built around.");
			WCombo("Fake yaw", &Vars::AntiHack::AntiAim::YawFake.Value, { "None", "Left", "Right", "Forward", "Backwards", "Spin", "Edge", "Invert" }); HelpMarker("The yaw that other players will see.");
			if (Vars::AntiHack::AntiAim::YawFake.Value == 5 || Vars::AntiHack::AntiAim::YawReal.Value == 5) {
				WSlider("Spin Speed", &Vars::AntiHack::AntiAim::SpinSpeed.Value, -30.f, 30.f, "%.1f", 0); HelpMarker("How many degrees you will rotate in a tick (15 milliseconds).");
			}

			WCombo("Base Yaw", &Vars::AntiHack::AntiAim::BaseYawMode.Value, { "Offset", "FOV Player", "FOV Player + Offset" }); HelpMarker("The yaw that your fake & real yaws are added to.");
			if (Vars::AntiHack::AntiAim::BaseYawMode.Value != 1) {
				WSlider("Base Yaw Offset", &Vars::AntiHack::AntiAim::BaseYawOffset.Value, -180, 180);
			}
			
			MultiCombo({"Jitter Legs", "HidePitchOnShot", "Anti-Backstab", "Rehide Angle Post-Shot"}, {&Vars::AntiHack::AntiAim::LegJitter.Value, &Vars::AntiHack::AntiAim::InvalidShootPitch.Value, &Vars::AntiHack::AntiAim::AntiBackstab.Value, &Vars::AntiHack::AntiAim::RehideAntiAimPostShot.Value }, "Misc.");

			/* Section: Auto Peek */
			SectionTitle("Auto Peek");
			InputKeybind("Autopeek Key", Vars::Misc::CL_Move::AutoPeekKey, true, false, "None"); HelpMarker("Hold this key while peeking and use A/D to set the peek direction");
			WSlider("Max Distance", &Vars::Misc::CL_Move::AutoPeekDistance.Value, 50.f, 400.f, "%.0f"); HelpMarker("Maximum distance that auto peek can walk");
			WToggle("Free move", &Vars::Misc::CL_Move::AutoPeekFree.Value); HelpMarker("Allows you to move freely while peeking");
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
			SectionTitle("Movement");
			WCombo("No Push###MovementNoPush", &Vars::Misc::NoPush.Value, { "Off", "Always", "While Moving", "Partial While AFK" });
			WCombo("Fast Stop", &Vars::Misc::AccurateMovement.Value, { "Off", "Legacy", "Instant", "Adaptive" }); HelpMarker("Will stop you from sliding once you stop pressing movement buttons");

			MultiFlags({ "Fast Strafe", "Fast Accel", "Crouch Speed", "Kart Control" }, { 8, 4, 2, 1 }, &Vars::Misc::AltMovement.Value, "Movement");
			WToggle("Duck Jump", &Vars::Misc::DuckJump.Value); HelpMarker("Will duck when bhopping");
			WToggle("Bunnyhop", &Vars::Misc::AutoJump.Value); HelpMarker("Will jump as soon as you touch the ground again, keeping speed between jumps");
			WCombo("Autostrafe", &Vars::Misc::AutoStrafe.Value, { "Off", "Legit", "Directional" }); HelpMarker("Will strafe for you in air automatically so that you gain speed");
			if (Vars::Misc::AutoStrafe.Value == 2)
			{
				WToggle("Only on movement key", &Vars::Misc::DirectionalOnlyOnMove.Value); HelpMarker("This makes it so that you dont always go forward if u just hold space");
			}
			WToggle("Edge jump", &Vars::Misc::EdgeJump.Value); HelpMarker("Will jump at the very end of whatever platform you're on, allowing you to perfectly make longer jumps.");
			if (Vars::Misc::EdgeJump.Value)
			{
				InputKeybind("Edge jump key", Vars::Misc::EdgeJumpKey, true);  HelpMarker("Edge jump bind, leave as None for always on");
			}


			SectionTitle("Automation");
			InputKeybind("Infinite Sandwich key", Vars::Misc::InfiniteEatKey, true, false, "None");  HelpMarker("Glutton bind, none for off.");
			InputKeybind("Sticky Spam key", Vars::Misc::StickySpamKey, true, false, "None");  HelpMarker("Sticky Spam Bind, none for off.");
			if (Vars::Misc::StickySpamKey.Value) {
				WSlider("Sticky Charge Percent", &Vars::Misc::StickyChargePercent.Value, 0, 100, "%d", ImGuiSliderFlags_AlwaysClamp);
			}
			WToggle("Auto rocket jump", &Vars::Misc::AutoRocketJump.Value); HelpMarker("Will rocket jump at the angle you're looking at when you press RMB with a rocket launcher");
			if (Vars::Misc::AutoRocketJump.Value)
			{
				WToggle("Non Lethal Rocket Jump", &Vars::Misc::NonLethalRocketJump.Value);
			}
			WToggle("Auto FaN jump", &Vars::Misc::AutoScoutJump.Value); HelpMarker("Performs an FaN jump when pressing RMB");
			WToggle("Anti-AFK", &Vars::Misc::AntiAFK.Value); HelpMarker("Will make you jump every now and then so you don't get kicked for idling");
			WToggle("Auto Vote", &Vars::Misc::AutoVote.Value); HelpMarker("Automatically votes yes/no depending on the target");
			WToggle("Taunt slide", &Vars::Misc::TauntSlide.Value); HelpMarker("Allows you to input in taunts");
			WToggle("Taunt control", &Vars::Misc::TauntControl.Value); HelpMarker("Gives full control if enabled with taunt slide");
			WToggle("Taunt follows camera", &Vars::Misc::TauntFollowsCamera.Value);
			WToggle("Taunt spin", &Vars::Misc::TauntSpin.Value);
			if (Vars::Misc::TauntSpin.Value)
			{
				InputKeybind("Taunt spin key", Vars::Misc::TauntSpinKey, true, false, "None");	//	why was this set to disallow none?
				WSlider("Taunt spin speed", &Vars::Misc::TauntSpinSpeed.Value, 0.1f, 100.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			}

			WCombo("Pick Class", &Vars::Misc::AutoJoin.Value, { "Off", "Scout", "Soldier", "Pyro", "Demoman", "Heavy", "Engineer", "Medic", "Sniper", "Spy" }); HelpMarker("Automatically joins the given class");
			WToggle("Rage retry", &Vars::Misc::RageRetry.Value); HelpMarker("Will automatically reconnect when your health is low");
			if (Vars::Misc::RageRetry.Value)
			{
				WSlider("Rage Retry health", &Vars::Misc::RageRetryHealth.Value, 1, 99, "%d%%"); HelpMarker("Minimum health percentage that will cause a retry");
			}
			WToggle("Pseudo Spectator", &Vars::Misc::ExtendFreeze.Value); HelpMarker("Causes an infinite respawn/spectator time");
			WToggle("Auto accept item drops", &Vars::Misc::AutoAcceptItemDrops.Value); HelpMarker("Automatically accepts all item drops");

			SectionTitle("Queueing");
			WToggle("Region selector", &Vars::Misc::RegionChanger.Value);

			MultiFlags({ "Europe",	"North America",	"South America",	"Asia",		"Africa",	"Australia",	"Unknown"},
					   { (1 << 0),	(1 << 1),			(1 << 2),			(1 << 3),	(1 << 4),	(1 << 5),		(1 << 6)},
					   &Vars::Misc::RegionsAllowed.Value,
					   "Regions"
			);
			WCombo("Match accept notification", &Vars::Misc::InstantAccept.Value, { "Default", "Instant join", "Freeze timer" }); HelpMarker("Will skip the 10 second delay before joining a match or let you never join");
			WCombo("Auto casual queue", &Vars::Misc::AutoCasualQueue.Value, { "Off", "In menu", "Always" }); HelpMarker("Automatically starts queueuing for casual");
			WToggle("Auto VAC Join", &Vars::Misc::AntiVAC.Value); HelpMarker("Tries to join a server while having a VAC ban");

			SectionTitle("Sound");
			MultiFlags({ "Footsteps", "Noisemaker" }, { 1 << 0, 1 << 1 }, &Vars::Misc::SoundBlock.Value, "Block Sounds###SoundRemovals");

			SectionTitle("Killsays");
			Checkbox("Killsays", &Vars::Misc::Killsay.Value);
			std::vector<std::string> vecKillsays = {};
			for (const auto& entry : std::filesystem::directory_iterator(g_CFG.GetConfigPath() + "\\Killsays"))
			{
				std::string a(std::filesystem::path(entry).filename().string());
				vecKillsays.push_back(a);
			}

			if (!vecKillsays.empty())
			{
				SetNextItemWidth(Vars::Menu::Style::ItemWidth.Value);
				if (ImGui::BeginCombo("Killsay file", Vars::Misc::KillsayFile.Value.c_str()))
				{
					for (const auto& killsay : vecKillsays)
					{
						bool bThisKillsayIsValid = (killsay == Vars::Misc::KillsayFile.Value);
						if (ImGui::Selectable(killsay.c_str(), &bThisKillsayIsValid))
						{
							F::Killsay.m_bFilled = false;
							Vars::Misc::KillsayFile.Value = killsay;
						}
					}
					
					ImGui::EndCombo();
				}
			}


		} EndChild();

		/* Column 2 */
		if (TableColumnChild("MiscCol2"))
		{
			SectionTitle("Chat");

			WToggle("Chat Flags", &Vars::Misc::ChatFlags.Value); HelpMarker("Adds advanced prefixes to chat messages");
			WToggle("Runescape chat", &Vars::Misc::RunescapeChat.Value); HelpMarker("Draws text on top of peoples heads when they type like in runescape (wholesome)");
			WCombo("Chat spam", &Vars::Misc::ChatSpam.Value, { "Off", "Fedoraware", "Lmaobox", "Cathook" });
			WCombo("Voicechat spam", &Vars::Misc::VoicechatSpam.Value, { "Off", "Medic!", "Help!", "Nice Shot", "Random" });
			WSlider("Spam interval", &Vars::Misc::SpamInterval.Value, 0.1f, 10.f, "%.1f"); HelpMarker("How often chat/voice spam should run");
			WCombo("Medieval Mode", &Vars::Misc::MedievalChat.Value, { "Default", "Never", "Always" }); HelpMarker("By the Immeasurable Nether Regions of Enlightened Dionysus, this enableth medieval chattery. Anon!");

			SectionTitle("Exploits");
			WToggle("Anti Autobalance", &Vars::Misc::AntiAutobal.Value); HelpMarker("Prevents auto balance by reconnecting to the server");
			WToggle("sv_cheats Bypass", &Vars::Misc::CheatsBypass.Value); HelpMarker("Allows you to use some sv_cheats commands (clientside)");
			WToggle("Join spam", &Vars::Misc::JoinSpam.Value); HelpMarker("Spams join/disconnect messages in the chat");
			WToggle("Noisemaker Spam", &Vars::Misc::NoisemakerSpam.Value); HelpMarker("Spams the noisemaker without reducing it's charges");
			WToggle("Ping reducer", &Vars::Misc::PingReducer.Value); HelpMarker("Reduces your ping on the scoreboard");
			if (Vars::Misc::PingReducer.Value)
			{
				WSlider("Target ping", &Vars::Misc::PingTarget.Value, 0, 200); HelpMarker("Target ping that should be reached");
			}

			SectionTitle("Party Networking");
			WToggle("Enable###PartyNetEnable", &Vars::Misc::PartyNetworking.Value); HelpMarker("Enables party networking between Fedoraware users");
			WToggle("Party crasher###PartyNetCrash", &Vars::Misc::PartyCrasher.Value); HelpMarker("Annoy your friends by crashing their game");
			InputKeybind("Party marker", Vars::Misc::PartyMarker, true, false, "None");  HelpMarker("Sends a marker to other Fedoraware users in your party");
			WToggle("Party ESP###PartyNetESP", &Vars::Misc::PartyESP.Value); HelpMarker("Sends player locations to your party members");

			SectionTitle("Followbot");
			WToggle("Enable Followbot###FollowbotEnable", &Vars::Misc::Followbot::Enabled.Value); HelpMarker("Follows a player around.");
			if (Vars::Misc::Followbot::Enabled.Value)
			{
				WToggle("Friends only###FollowbotFriends", &Vars::Misc::Followbot::FriendsOnly.Value); HelpMarker("Only follow friends");
				WSlider("Follow Distance###FollowbotDistance", &Vars::Misc::Followbot::Distance.Value, 50.f, 400.f, "%.0f"); HelpMarker("How close we should follow the target");
			}

			/*SectionTitle("Leaderboard");
			WToggle("Send statistics", &Vars::Misc::StoreStatistics.Value); HelpMarker("Will send your steamid/kills/deaths/highest killstreak whenever you leave the server");

			if (Button("Open leaderboards", { GetWindowSize().x - 2 * GetStyle().WindowPadding.x, 20 }))
			{
				ShellExecute(0, 0, L"http://198.244.189.210:4077/leaderboard", 0, 0, SW_SHOW);
			}*/
		} EndChild();

		/* Column 3 */
		if (TableColumnChild("MiscCol3"))
		{
			SectionTitle("Discord RPC");
			if (F::DiscordRPC.IsLoaded())
			{
				WToggle("Discord RPC", &Vars::Misc::Discord::EnableRPC.Value); HelpMarker("Enable Discord Rich Presence");
				if (Vars::Misc::Discord::EnableRPC.Value)
				{
					WToggle("Include map", &Vars::Misc::Discord::IncludeMap.Value); HelpMarker("Should Discord Rich Presence contain current map name?");
					WToggle("Include class", &Vars::Misc::Discord::IncludeClass.Value); HelpMarker("Should Discord Rich Presence contain current class?");
					WToggle("Include timestamp", &Vars::Misc::Discord::IncludeTimestamp.Value); HelpMarker("Should time since you started playing TF2 be included?");
					WCombo("Image Options", &Vars::Misc::Discord::WhatImagesShouldBeUsed.Value, { "Big fedora + Small TF2", "Big TF2 + Small fedora" });	
				}
			}
			else
			{
				Text("Discord RPC not installed.");
			}

			SectionTitle("Steam RPC");
			WToggle("Steam RPC", &Vars::Misc::Steam::EnableRPC.Value); HelpMarker("Enable Steam Rich Presence"); HelpMarker("Enable Steam Rich Presence");
			if (Vars::Misc::Steam::EnableRPC.Value)
			{
				WCombo("Match group", &Vars::Misc::Steam::MatchGroup.Value, { "Special Event", "MvM Mann Up", "Competitive", "Casual", "MvM Boot Camp" }); HelpMarker("Which match group should be used?");
				WToggle("Override in menu", &Vars::Misc::Steam::OverrideMenu.Value); HelpMarker("Override match group to \"Main Menu\" when in main menu");
				WCombo("Map text", &Vars::Misc::Steam::MapText.Value, { "Custom", "Fedoraware", "Figoraware", "Meowhook.club", "Rathook.cc", "Nitro.tf" }); HelpMarker("Which map text should be used?");
				if (Vars::Misc::Steam::MapText.Value == 0)
				{
					WInputText("Custom map text", &Vars::Misc::Steam::CustomText.Value); HelpMarker(R"(For when "Custom" is selcted in "Map text". Sets custom map text.)");
				}
				WInputInt("Group size", &Vars::Misc::Steam::GroupSize.Value); HelpMarker("Sets party size");
			}

			SectionTitle("Utilities");
			if (Button("Full update", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("cl_fullupdate");
			if (Button("Reload HUD", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("hud_reloadscheme");
			if (Button("Restart sound", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("snd_restart");
			if (Button("Resync NoSpread", SIZE_FULL_WIDTH))
				F::NoSpread.Reset();
			if (Button("Stop sound", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("stopsound");
			if (Button("Status", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("status");
			if (Button("Ping", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("ping");
			if (Button("Pong", SIZE_FULL_WIDTH))
				F::Pong.IsOpen = !F::Pong.IsOpen;
			if (Button("Retry", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("retry");
			if (Button("Exit", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("exit");
			if (Button("Console", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("showconsole");
			if (Button("Demo playback", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("demoui");
			if (Button("Demo trackbar", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("demoui2");
			if (Button("Itemtest", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("itemtest");
			if (Button("Fix Chams", SIZE_FULL_WIDTH))
			{
				F::DMEChams.CreateMaterials();
				F::Glow.CreateMaterials();
			}
			if (Button("Print Hashes", SIZE_FULL_WIDTH))
			{
				Hash::PrintHash();
			}
			//if (Button("CPrint", ImVec2(btnWidth, 20))){
			//	I::CenterPrint->Print((char*)"niggaz");
			//}


			if (!I::EngineClient->IsConnected())
			{
				if (Button("Unlock all achievements", SIZE_FULL_WIDTH))
				{
					F::Misc.UnlockAchievements();
				}
				if (Button("Lock all achievements", SIZE_FULL_WIDTH))
				{
					F::Misc.LockAchievements();
				}
			}
		} EndChild();

		EndTable();
	}
}
#pragma endregion

void CMenu::MenuDesignWindow() {
	using namespace ImGui;
	if (!ShowDesignerWindow) return;
	static int currentTab = 0;
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
	SetNextWindowSize(ImVec2(350, 700));
	if (Begin("Menu Design", &ShowDesignerWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings)) {

		if (BeginTable("menuTabs", 2)) {
			if (TabButton("Colours", currentTab == 0)) currentTab = 0;
			if (TabButton("Style", currentTab == 1)) currentTab = 1;
			EndTable();
		}

		if (!currentTab) {
			if (ColorPicker("Main accent", Vars::Menu::Colors::MenuAccent.Value)) {LoadStyle(); } SameLine(); Text("Main accent");
			if (ColorPicker("Tab Bar", Vars::Menu::Colors::TabBar.Value)) { LoadStyle(); } SameLine(); Text("Tab Bar");
			if (ColorPicker("Window Background", Vars::Menu::Colors::WindowBg.Value)) { LoadStyle(); } SameLine(); Text("Window Background");
			if (ColorPicker("Popup Background", Vars::Menu::Colors::PopupBg.Value)) { LoadStyle(); } SameLine(); Text("Popup Background");
			Dummy({ 0, 5 });
			if (ColorPicker("Title Background", Vars::Menu::Colors::TitleBg.Value)) { LoadStyle(); }  SameLine(); Text("Title Background");
			if (ColorPicker("Active Title Background", Vars::Menu::Colors::TitleBgActive.Value)) { LoadStyle(); }  SameLine(); Text("Active Title Background");
			Dummy({ 0, 5 });
			if (ColorPicker("Frame Background", Vars::Menu::Colors::FrameBg.Value)) { LoadStyle(); } SameLine(); Text("Frame Background");
			if (ColorPicker("Hovered Frame Background", Vars::Menu::Colors::FrameBgHovered.Value)) { LoadStyle(); } SameLine(); Text("Hovered Frame Background");
			if (ColorPicker("Active Frame Background", Vars::Menu::Colors::FrameBgActive.Value)) { LoadStyle(); } SameLine(); Text("Active Frame Background");
			if (ColorPicker("Modal Background Dim", Vars::Menu::Colors::ModalWindowDimBg.Value)) { LoadStyle(); } SameLine(); Text("Modal Background Dim");
			Dummy({ 0, 5 });
			if (ColorPicker("Button", Vars::Menu::Colors::Button.Value)) { LoadStyle(); } SameLine(); Text("Button");
			if (ColorPicker("Hovered Button", Vars::Menu::Colors::ButtonHovered.Value)) { LoadStyle(); } SameLine(); Text("Hovered Button");
			if (ColorPicker("Active Button", Vars::Menu::Colors::ButtonActive.Value)) { LoadStyle(); } SameLine(); Text("Active Button");
			Dummy({ 0, 5 });
			if (ColorPicker("Checkmark", Vars::Menu::Colors::CheckMark.Value)) { LoadStyle(); } SameLine(); Text("CheckmarkCheckmark");
			if (ColorPicker("Text", Vars::Menu::Colors::Text.Value)) { LoadStyle(); } SameLine(); Text("Text");
			if (ColorPicker("Border", Vars::Menu::Colors::Border.Value)) { LoadStyle(); } SameLine(); Text("Border");
			Dummy({ 0, 5 });
			if (ColorPicker("Slider Grab", Vars::Menu::Colors::SliderGrab.Value)) { LoadStyle(); } SameLine(); Text("Slider Grab");
			if (ColorPicker("Active Slider Grab", Vars::Menu::Colors::SliderGrabActive.Value)) { LoadStyle(); } SameLine(); Text("Active Slider Grab");
			Dummy({ 0, 5 });
			if (ColorPicker("Resize Grip", Vars::Menu::Colors::ResizeGrip.Value)) { LoadStyle(); } SameLine(); Text("Resize Grip");
			if (ColorPicker("Active Resize Grip", Vars::Menu::Colors::ResizeGripActive.Value)) { LoadStyle(); } SameLine(); Text("Active Resize Grip");
			Dummy({ 0, 5 });
			if (ColorPicker("Scroll Bar Background", Vars::Menu::Colors::ScrollbarBG.Value)) { LoadStyle(); } SameLine(); Text("Scroll Bar Background");
			if (ColorPicker("Scroll Bar Grap", Vars::Menu::Colors::ScrollbarGrab.Value)) { LoadStyle(); }; SameLine(); Text("Scroll Bar Grap");
			if (ColorPicker("Hovered Scroll Bar Grap", Vars::Menu::Colors::ScrollbarGrabHovered.Value)) { LoadStyle(); }; SameLine(); Text("Hovered Scroll Bar Grap");
			if (ColorPicker("Active Scroll Bar Grap", Vars::Menu::Colors::ScrollbarGrabActive.Value)) { LoadStyle(); }; SameLine(); Text("Active Scroll Bar Grap");
			Dummy({ 0, 5 });
			if (ColorPicker("Header", Vars::Menu::Colors::Header.Value)) { LoadStyle(); } SameLine(); Text("Header");
			if (ColorPicker("Hovered Header", Vars::Menu::Colors::HeaderHovered.Value)) { LoadStyle(); } SameLine(); Text("Hovered Header");
			if (ColorPicker("Active Header", Vars::Menu::Colors::HeaderActive.Value)) { LoadStyle();  } SameLine(); Text("Active Header");
		}
		else {
			Text("Be careful you can make your visuals very wacky!");
			if (ImGui::SliderFloat("Item Width", &Vars::Menu::Style::ItemWidth.Value, 1.f, 250.f)) LoadStyle(); // a bit above readable but eh
			if (WSlider("Alpha", &Vars::Menu::Style::Alpha.Value, 0.01f, 1.0f)) LoadStyle();
			if (WSlider("Disabled Alpha", &Vars::Menu::Style::DisabledAlpha.Value, 0.0f, 1.0f)) LoadStyle();
			if (WSlider2("Window Padding", &Vars::Menu::Style::WindowPadding.Value.x, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Window Rounding", &Vars::Menu::Style::WindowRounding.Value, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Window Border Size", &Vars::Menu::Style::WindowBorderSize.Value, 0.0f, 5.0f)) LoadStyle();
			if (WSlider2("Window Min Size", &Vars::Menu::Style::WindowMinSize.Value.x, 1.0f, 100.0f)) LoadStyle();
			if (WSlider2("Window Title Align", &Vars::Menu::Style::WindowTitleAlign.Value.x, 0.f, 1.0f)) LoadStyle();
			if (WCombo("Window Menu Button Position", &Vars::Menu::Style::WindowMenuButtonPosition.Value, {"None", "Left", "Right"})) LoadStyle();
			if (WSlider("Child Rounding", &Vars::Menu::Style::ChildRounding.Value, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Child Border Size", &Vars::Menu::Style::ChildBorderSize.Value, 0.0f, 5.0f)) LoadStyle();
			if (WSlider("Popup Rounding", &Vars::Menu::Style::PopupRounding.Value, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Popup Border Size", &Vars::Menu::Style::PopupBorderSize.Value, 0.0f, 5.0f)) LoadStyle();
			if (WSlider2("Frame Padding", &Vars::Menu::Style::FramePadding.Value.x, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Frame Rounding", &Vars::Menu::Style::FrameRounding.Value, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Frame Border Size", &Vars::Menu::Style::FrameBorderSize.Value, 0.0f, 2.0f)) LoadStyle();
			if (WSlider2("Item Spacing", &Vars::Menu::Style::ItemSpacing.Value.x, 0.0f, 20.0f)) LoadStyle();
			if (WSlider2("Item Inner Spacing", &Vars::Menu::Style::ItemInnerSpacing.Value.x, 0.0f, 20.0f)) LoadStyle();
			if (WSlider2("Cell Padding", &Vars::Menu::Style::CellPadding.Value.x, 0.0f, 20.0f)) LoadStyle();
			if (WSlider2("Touch Extra Padding", &Vars::Menu::Style::TouchExtraPadding.Value.x, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Indent Spacing", &Vars::Menu::Style::IndentSpacing.Value, 0.0f, 40.0f)) LoadStyle();
			if (WSlider("Columns Min Spacing", &Vars::Menu::Style::ColumnsMinSpacing.Value, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Scrollbar Size", &Vars::Menu::Style::ScrollbarSize.Value, 0.01, 40.0f)) LoadStyle();
			if (WSlider("Scrollbar Rounding", &Vars::Menu::Style::ScrollbarRounding.Value, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Grab Min Size", &Vars::Menu::Style::GrabMinSize.Value, 0.01f, 20.0f)) LoadStyle();
			if (WSlider("Grab Rounding", &Vars::Menu::Style::GrabRounding.Value, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Log Slider Deadzone", &Vars::Menu::Style::LogSliderDeadzone.Value, 0.0f, 10.0f)) LoadStyle();
			if (WSlider("Tab Rounding", &Vars::Menu::Style::TabRounding.Value, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Tab Border Size", &Vars::Menu::Style::TabBorderSize.Value, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Tab Min Width For Close Button", &Vars::Menu::Style::TabMinWidthForCloseButton.Value, 0.0f, 20000.0f)) LoadStyle();
			if (WCombo("Color Button Position", &Vars::Menu::Style::ColorButtonPosition.Value, {"Left", "Right"})) LoadStyle();
			if (WSlider2("Button Text Align", &Vars::Menu::Style::ButtonTextAlign.Value.x, 0.f, 1.0f)) LoadStyle();
			if (WSlider2("Selectable Text Align", &Vars::Menu::Style::SelectableTextAlign.Value.x, 0.f, 1.0f)) LoadStyle();
			if (WSlider("Separator Text Border Size", &Vars::Menu::Style::SeparatorTextBorderSize.Value, 0.0f, 5.0f)) LoadStyle();
			if (WSlider2("Separator Text Align", &Vars::Menu::Style::SeparatorTextAlign.Value.x, 0.f, 1.0f)) LoadStyle();
			if (WSlider2("Separator Text Padding", &Vars::Menu::Style::SeparatorTextPadding.Value.x, 0.0f, 20.0f)) LoadStyle();
			if (WSlider2("Display Window Padding", &Vars::Menu::Style::DisplayWindowPadding.Value.x, 0.0f, 40.0f)) LoadStyle();
			if (WSlider2("Display Safe Area Padding", &Vars::Menu::Style::DisplaySafeAreaPadding.Value.x, 0.0f, 20.0f)) LoadStyle();
			if (WSlider("Mouse Cursor Scale", &Vars::Menu::Style::MouseCursorScale.Value, 0.0f, 5.0f)) LoadStyle();
			if (ImGui::Checkbox("Enable Anti-Aliased Lines", &Vars::Menu::Style::AntiAliasedLines.Value)) LoadStyle();
			if (ImGui::Checkbox("Enable Anti-Aliased Lines Using Texture", &Vars::Menu::Style::AntiAliasedLinesUseTex.Value)) LoadStyle();
			if (ImGui::Checkbox("Enable Anti-Aliased Fill", &Vars::Menu::Style::AntiAliasedFill.Value)) LoadStyle();
			if (WSlider("Curve Tessellation Tolerance", &Vars::Menu::Style::CurveTessellationTol.Value, 0.01f, 5.0f)) LoadStyle();
			if (WSlider("Circle Tessellation Max Error", &Vars::Menu::Style::CircleTessellationMaxError.Value, 0.01f, 5.0f)) LoadStyle();
		}
		End();
	}
	PopStyleVar();
}

/* Settings Window */
void CMenu::SettingsWindow()
{
	using namespace ImGui;
	if (!ShowSettings) { return; }

	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
	PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(10, 10));
	SetNextWindowSize(ImVec2(230.f, 0.f));

	if (Begin("Settings", &ShowSettings, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		PushItemWidth(-1);

		/* General Menu Settings */
		if (CollapsingHeader("Menu Settings"))
		{
			if (Button("Open Menu Designer")) { ShowDesignerWindow = !ShowDesignerWindow; }
			if (Checkbox("Alternative Design", &Vars::Menu::ModernDesign.Value)) { LoadStyle(); }
			if (Checkbox("Draw Weather", &Vars::Menu::DrawWeather.Value)) { LoadStyle(); }
			if (Checkbox("Menu Vignette", &Vars::Menu::Vignette.Value))
			{
				I::ViewRender->SetScreenOverlayMaterial(nullptr);
			}
			Checkbox("Close Menu on Unfocus", &Vars::Menu::CloseOnUnfocus.Value);

			WInputText("Cheat Name", &Vars::Menu::CheatName.Value);
			WInputText("Chat Prefix", &Vars::Menu::CheatPrefix.Value);

			SetNextItemWidth(100);
			InputKeybind("Extra Menu key", Vars::Menu::MenuKey, true, true, "None");
		}

		Dummy({ 0, 5 });

		/* Config Tool Buttons */
		if (Button("Open configs folder", SIZE_FULL_WIDTH))
		{
			ShellExecuteA(nullptr, nullptr, g_CFG.GetConfigPath().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
		}
		if (Button("Open visuals folder", SIZE_FULL_WIDTH))
		{
			ShellExecuteA(nullptr, nullptr, g_CFG.GetVisualsPath().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
		}

		Dummy({ 0, 5 });

		/* Config Tabs */
		PushFont(SectionFont);
		PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
		PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
		if (BeginTable("ConfigTable", 2))
		{
			PushStyleColor(ImGuiCol_Text, ImGui::ColorToVec(Vars::Menu::Colors::Text.Value));
			if (TabButton("General", CurrentConfigTab == ConfigTab::General))
			{
				CurrentConfigTab = ConfigTab::General;
			}

			if (TabButton("Visuals", CurrentConfigTab == ConfigTab::Visuals))
			{
				CurrentConfigTab = ConfigTab::Visuals;
			}

			PopStyleColor(1);
			EndTable();
		}
		PopStyleVar(2);
		PopFont();

		static std::string selected;
		static std::string selectedvis;

		/* Config list*/
		if (CurrentConfigTab == ConfigTab::General)
		{
			// Current config
			const std::string cfgText = "Loaded: " + g_CFG.GetCurrentConfig();
			Text(cfgText.c_str());

			// Config name field
			std::string newConfigName = {};
			if (InputTextWithHint("###configname", "New config name", &newConfigName, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (!std::filesystem::exists(g_CFG.GetConfigPath() + "\\" + newConfigName))
				{
					g_CFG.SaveConfig(newConfigName);
				}
			}

			// Config list
			for (const auto& entry : std::filesystem::directory_iterator(g_CFG.GetConfigPath()))
			{
				if (!entry.is_regular_file()) { continue; }
				if (entry.path().extension() != g_CFG.ConfigExtension) { continue; }

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
					if (Button(configName.c_str(), SIZE_FULL_WIDTH))
					{
						selected = configName;
					}
					PopStyleColor();

					if (BeginTable("ConfigActions", 3))
					{
						// Save config button
						TableNextColumn();
						if (Button("Save", SIZE_FULL_WIDTH))
						{
							if (configName != g_CFG.GetCurrentConfig())
							{
								OpenPopup("Save config?");
							}
							else
							{
								g_CFG.SaveConfig(selected);
								selected.clear();
							}
						}

						// Load config button
						TableNextColumn();
						if (Button("Load", SIZE_FULL_WIDTH))
						{
							g_CFG.LoadConfig(selected);
							selected.clear();
							LoadStyle();
						}

						// Remove config button
						TableNextColumn();
						if (Button("Remove", SIZE_FULL_WIDTH))
						{
							OpenPopup("Remove config?");
						}

						// Dialogs
						{
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

						EndTable();
					}
				}
				else if (configName == g_CFG.GetCurrentConfig())
				{
					PushStyleColor(ImGuiCol_Button, GetStyle().Colors[ImGuiCol_ButtonActive]);
					std::string buttonText = "> " + configName + " <";
					if (Button(buttonText.c_str(), SIZE_FULL_WIDTH))
					{
						selected = configName;
					}
					PopStyleColor();
				}
				else
				{
					if (Button(configName.c_str(), SIZE_FULL_WIDTH))
					{
						selected = configName;
					}
				}
			}

		}
		else if (CurrentConfigTab == ConfigTab::Visuals)
		{
			// Current config
			const std::string cfgText = "Loaded: " + g_CFG.GetCurrentVisuals();
			Text(cfgText.c_str());

			// Config name field
			std::string newConfigName = {};

			if (InputTextWithHint("###configname", "New config name", &newConfigName, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (!std::filesystem::exists(g_CFG.GetVisualsPath() + "\\" + newConfigName))
				{
					g_CFG.SaveVisual(newConfigName);
				}
			}

			// Visuals list
			for (const auto& entry : std::filesystem::directory_iterator(g_CFG.GetVisualsPath()))
			{
				if (!entry.is_regular_file()) { continue; }
				if (entry.path().extension() != g_CFG.ConfigExtension) { continue; }

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
					if (Button(configName.c_str(), SIZE_FULL_WIDTH))
					{
						selected = configName;
					}
					PopStyleColor();

					// Visuals action buttons
					if (BeginTable("ConfigActions", 3))
					{
						// Visuals config button
						TableNextColumn();
						if (Button("Save", SIZE_FULL_WIDTH))
						{
							if (configName != g_CFG.GetCurrentVisuals())
							{
								OpenPopup("Save visuals?");
							}
							else
							{
								g_CFG.SaveVisual(selected);
								selected.clear();
							}
						}

						// Load visuals button
						TableNextColumn();
						if (Button("Load", SIZE_FULL_WIDTH))
						{
							g_CFG.LoadVisual(selected);
							selected.clear();
							LoadStyle();
						}

						// Remove visual button
						TableNextColumn();
						if (Button("Remove", SIZE_FULL_WIDTH))
						{
							OpenPopup("Remove visuals?");
						}

						// Dialogs
						{
							// Save config dialog
							if (BeginPopupModal("Save visuals?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
							{
								Text("Do you really want to override this config?");

								Separator();
								if (Button("Yes, override!", ImVec2(150, 0)))
								{
									g_CFG.SaveVisual(selected);
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
							if (BeginPopupModal("Remove visuals?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
							{
								Text("Do you really want to delete this config?");

								Separator();
								if (Button("Yes, remove!", ImVec2(150, 0)))
								{
									g_CFG.RemoveVisual(selected);
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

						EndTable();
					}
				}
				else if (configName == g_CFG.GetCurrentVisuals())
				{
					PushStyleColor(ImGuiCol_Button, GetStyle().Colors[ImGuiCol_ButtonActive]);
					std::string buttonText = "> " + configName + " <";
					if (Button(buttonText.c_str(), SIZE_FULL_WIDTH))
					{
						selected = configName;
					}
					PopStyleColor();
				}
				else
				{
					if (Button(configName.c_str(), SIZE_FULL_WIDTH))
					{
						selected = configName;
					}
				}
			}
		}

		PopItemWidth();
		End();
	}

	PopStyleVar(2);
}

/* Debug Menu */
void CMenu::DebugMenu()
{
	using namespace ImGui;
	if (!ShowDebugMenu) { return; }

	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
	PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(200, 200));

	ImGui::SetNextWindowSize({ 400.f, 0.f });
	if (Begin("Debug", &ShowDebugMenu, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
	{
		const auto& pLocal = g_EntityCache.GetLocal();

		Checkbox("Show Debug info", &Vars::Debug::DebugInfo.Value);
		Checkbox("Console Logging", &Vars::Debug::Logging.Value);
		Checkbox("Allow secure servers", I::AllowSecureServers);

		bool* m_bPendingPingRefresh = reinterpret_cast<bool*>(I::TFGCClientSystem + 828);
		Checkbox("Pending Ping Refresh", m_bPendingPingRefresh);

		if (Button("Update Discord RPC"))
		{
			F::DiscordRPC.Update();
		}

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

		End();
	}

	PopStyleVar(2);
}

/* Window for the camera feature */
void CMenu::DrawCameraWindow()
{
	if (I::EngineClient->IsInGame() && Vars::Visuals::CameraMode.Value != 0)
	{
		// Draw the camera window
		ImGui::SetNextWindowSize({ static_cast<float>(F::CameraWindow.ViewRect.w), static_cast<float>(F::CameraWindow.ViewRect.h) }, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos({ static_cast<float>(F::CameraWindow.ViewRect.x), static_cast<float>(F::CameraWindow.ViewRect.y) }, ImGuiCond_FirstUseEver);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.1f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 60.f, 60.f });
		if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus))
		{
			const ImVec2 winPos = ImGui::GetWindowPos();
			const ImVec2 winSize = ImGui::GetWindowSize();

			F::CameraWindow.ViewRect.x = static_cast<int>(winPos.x);
			F::CameraWindow.ViewRect.y = static_cast<int>(winPos.y);
			F::CameraWindow.ViewRect.w = static_cast<int>(winSize.x);
			F::CameraWindow.ViewRect.h = static_cast<int>(winSize.y);

			ImGui::End();
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}
}

//void CMenu::DrawCritDrag()
//{
//	if (Vars::CritHack::Indicators.Value)
//	{
//
//	}
//}

void CMenu::AddDraggable(const char* szTitle, DragBox_t& info, bool bShouldDraw, bool setSize)
{
	constexpr int titlebarheight = 20;

	if (bShouldDraw)
	{
		if (info.update)
		{
			if (setSize)
			{
				ImGui::SetNextWindowSize({ static_cast<float>(info.w), static_cast<float>(info.h) + titlebarheight }, ImGuiCond_Always);
			}
			else
			{
				ImGui::SetNextWindowSize({ 80.f, 60.f }, ImGuiCond_Always);
			}
			ImGui::SetNextWindowPos({ static_cast<float>(info.x), static_cast<float>(info.y) - titlebarheight }, ImGuiCond_Always);
			info.update = false;
		}
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.1f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 50.f, 21.f });

		const auto bResize = setSize ? 0 : ImGuiWindowFlags_NoResize;
		if (ImGui::Begin(szTitle, nullptr, bResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing))
		{
			const ImVec2 winPos = ImGui::GetWindowPos();
			const ImVec2 winSize = ImGui::GetWindowSize();

			info.x = static_cast<int>(winPos.x);
			info.y = static_cast<int>(winPos.y + titlebarheight);	//	fix title bars
			if (setSize)
			{
				info.w = static_cast<int>(winSize.x);
				info.h = static_cast<int>(winSize.y - titlebarheight);	//	account for title bar fix
			}
			info.c = static_cast<int>(info.x + ((setSize ? info.w : 80.f) / 2));

			ImGui::End();
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}
}

void CMenu::DrawKeybinds()
{
	if (!Vars::Menu::ShowKeybinds.Value) { return; }

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.35f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.f, 0.f, 0.f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.f, 0.f, 0.f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4.f, 4.f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	ImGui::SetNextWindowSize({ 200.f, 0.f });

	if (ImGui::Begin("Keybinds", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		auto drawOption = [](const char* name, bool active)
		{
			ImGui::Text(name);
			ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - ImGui::CalcTextSize(active ? "[On]" : "[Off]").x);
			ImGui::Text(active ? "[On]" : "[Off]");
		};

		auto isActive = [](bool active, bool needsKey, int key)
		{
			return active && (!needsKey || GetAsyncKeyState(key) & 0x8000);
		};

		drawOption("Aimbot", isActive(Vars::Aimbot::Global::Active.Value, Vars::Aimbot::Global::AimKey.Value, Vars::Aimbot::Global::AimKey.Value));
		drawOption("Auto Shoot", Vars::Aimbot::Global::AutoShoot.Value);
		drawOption("Double Tap", isActive(Vars::Misc::CL_Move::DTMode.Value != 3, Vars::Misc::CL_Move::DTMode.Value == 0, Vars::Misc::CL_Move::DoubletapKey.Value));
		drawOption("Anti Aim", Vars::AntiHack::AntiAim::Active.Value);
		drawOption("Triggerbot", isActive(Vars::Triggerbot::Global::Active.Value, Vars::Triggerbot::Global::TriggerKey.Value, Vars::Triggerbot::Global::TriggerKey.Value));
	}

	ImGui::End();
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(3);
}

void CMenu::Render(IDirect3DDevice9* pDevice)
{
	if (!ConfigLoaded) { return; }

	static std::once_flag initFlag;
	std::call_once(initFlag, [&]
				   {
					   Init(pDevice);
				   });

	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

	// Toggle menu (default is 'insert' can be changed in menu)
	static KeyHelper menuKey{ &Vars::Menu::MenuKey.Value };
	if (menuKey.Pressed() || GetAsyncKeyState(VK_INSERT) & 0x1)
	{
		F::Menu.IsOpen = !F::Menu.IsOpen;
		I::ViewRender->SetScreenOverlayMaterial(nullptr);
		I::VGuiSurface->SetCursorAlwaysVisible(F::Menu.IsOpen);
	}

	if (F::Menu.IsOpen && Vars::Menu::CloseOnUnfocus.Value)
	{
		if (!Utils::IsGameWindowInFocus())
		{
			F::Menu.IsOpen = false;
			I::ViewRender->SetScreenOverlayMaterial(nullptr);
			I::VGuiSurface->SetCursorAlwaysVisible(F::Menu.IsOpen);
		}
	}

	// Begin current frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::PushFont(Verdana);

	// Window that should always be visible
	DrawKeybinds();
	F::Radar.DrawWindow();

	if (IsOpen)
	{
		DrawMenu();
		DrawCameraWindow();
		AddDraggable("Conditions", Vars::Visuals::OnScreenConditions.Value, Vars::Visuals::DrawOnScreenConditions.Value, true);
		AddDraggable("Ping", Vars::Visuals::OnScreenPing.Value, Vars::Visuals::DrawOnScreenPing.Value, true);
		AddDraggable("DT Bar", Vars::Misc::CL_Move::DTIndicator.Value, Vars::Misc::CL_Move::DTBarStyle.Value, true);
		AddDraggable("Crits", Vars::CritHack::IndicatorPos.Value, Vars::CritHack::Indicators.Value, true);

		SettingsWindow();
		MenuDesignWindow();
		DebugMenu();
		F::MaterialEditor.Render();
		F::PlayerList.Render();
		F::Pong.Render();
	}

	// End frame and render
	ImGui::PopFont();
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

		auto& style = ImGui::GetStyle();
		style.Alpha = Vars::Menu::Style::Alpha.Value;
		style.DisabledAlpha = Vars::Menu::Style::DisabledAlpha.Value;
		style.WindowPadding = ImGui::Vec2ToIm(Vars::Menu::Style::WindowPadding.Value);
		style.WindowRounding = Vars::Menu::Style::WindowRounding.Value;
		style.WindowBorderSize = Vars::Menu::Style::WindowBorderSize.Value;
		style.WindowMinSize = ImGui::Vec2ToIm(Vars::Menu::Style::WindowMinSize.Value);
		style.WindowTitleAlign = ImGui::Vec2ToIm(Vars::Menu::Style::WindowTitleAlign.Value);
		style.WindowMenuButtonPosition = static_cast<ImGuiDir>(Vars::Menu::Style::WindowMenuButtonPosition.Value - 1);
		style.ChildRounding = Vars::Menu::Style::ChildRounding.Value;
		style.ChildBorderSize = Vars::Menu::Style::ChildBorderSize.Value;
		style.PopupRounding = Vars::Menu::Style::PopupRounding.Value;
		style.PopupBorderSize = Vars::Menu::Style::PopupBorderSize.Value;
		style.FramePadding = ImGui::Vec2ToIm(Vars::Menu::Style::FramePadding.Value);
		style.FrameRounding = Vars::Menu::Style::FrameRounding.Value;
		style.FrameBorderSize = Vars::Menu::Style::FrameBorderSize.Value;
		style.ItemSpacing = ImGui::Vec2ToIm(Vars::Menu::Style::ItemSpacing.Value);
		style.ItemInnerSpacing = ImGui::Vec2ToIm(Vars::Menu::Style::ItemInnerSpacing.Value);
		style.CellPadding = ImGui::Vec2ToIm(Vars::Menu::Style::CellPadding.Value);
		style.TouchExtraPadding = ImGui::Vec2ToIm(Vars::Menu::Style::TouchExtraPadding.Value);
		style.IndentSpacing = Vars::Menu::Style::IndentSpacing.Value;
		style.ColumnsMinSpacing = Vars::Menu::Style::ColumnsMinSpacing.Value;
		style.ScrollbarSize = Vars::Menu::Style::ScrollbarSize.Value;
		style.ScrollbarRounding = Vars::Menu::Style::ScrollbarRounding.Value;
		style.GrabMinSize = Vars::Menu::Style::GrabMinSize.Value;
		style.GrabRounding = Vars::Menu::Style::GrabRounding.Value;
		style.LogSliderDeadzone = Vars::Menu::Style::LogSliderDeadzone.Value;
		style.TabRounding = Vars::Menu::Style::TabRounding.Value;
		style.TabBorderSize = Vars::Menu::Style::TabBorderSize.Value;
		style.TabMinWidthForCloseButton = Vars::Menu::Style::TabMinWidthForCloseButton.Value;
		style.ColorButtonPosition = static_cast<ImGuiDir>(Vars::Menu::Style::ColorButtonPosition.Value);
		style.ButtonTextAlign = ImGui::Vec2ToIm(Vars::Menu::Style::ButtonTextAlign.Value);
		style.SelectableTextAlign = ImGui::Vec2ToIm(Vars::Menu::Style::SelectableTextAlign.Value);
		style.SeparatorTextBorderSize = Vars::Menu::Style::SeparatorTextBorderSize.Value;
		style.SeparatorTextAlign = ImGui::Vec2ToIm(Vars::Menu::Style::SeparatorTextAlign.Value);
		style.SeparatorTextPadding = ImGui::Vec2ToIm(Vars::Menu::Style::SeparatorTextPadding.Value);
		style.DisplayWindowPadding = ImGui::Vec2ToIm(Vars::Menu::Style::DisplayWindowPadding.Value);
		style.DisplaySafeAreaPadding = ImGui::Vec2ToIm(Vars::Menu::Style::DisplaySafeAreaPadding.Value);
		style.MouseCursorScale = Vars::Menu::Style::MouseCursorScale.Value;
		style.AntiAliasedLines = Vars::Menu::Style::AntiAliasedLines.Value;
		style.AntiAliasedLinesUseTex = Vars::Menu::Style::AntiAliasedLinesUseTex.Value;
		style.AntiAliasedFill = Vars::Menu::Style::AntiAliasedFill.Value;
		style.CurveTessellationTol = Vars::Menu::Style::CurveTessellationTol.Value;
		style.CircleTessellationMaxError = Vars::Menu::Style::CircleTessellationMaxError.Value;


		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Border] = ImGui::ColorToVec(Vars::Menu::Colors::Border.Value);
		colors[ImGuiCol_WindowBg] = ImGui::ColorToVec(Vars::Menu::Colors::WindowBg.Value);
		colors[ImGuiCol_TitleBg] = ImGui::ColorToVec(Vars::Menu::Colors::TitleBg.Value);
		colors[ImGuiCol_TitleBgActive] = ImGui::ColorToVec(Vars::Menu::Colors::TitleBgActive.Value);
		colors[ImGuiCol_ModalWindowDimBg] = ImGui::ColorToVec(Vars::Menu::Colors::ModalWindowDimBg.Value);
		colors[ImGuiCol_Button] = ImGui::ColorToVec(Vars::Menu::Colors::Button.Value);
		colors[ImGuiCol_ButtonHovered] = ImGui::ColorToVec(Vars::Menu::Colors::ButtonHovered.Value);
		colors[ImGuiCol_ButtonActive] = ImGui::ColorToVec(Vars::Menu::Colors::ButtonActive.Value);
		colors[ImGuiCol_PopupBg] = ImGui::ColorToVec(Vars::Menu::Colors::PopupBg.Value);
		colors[ImGuiCol_FrameBg] = ImGui::ColorToVec(Vars::Menu::Colors::FrameBg.Value);
		colors[ImGuiCol_FrameBgHovered] = ImGui::ColorToVec(Vars::Menu::Colors::FrameBgHovered.Value);
		colors[ImGuiCol_FrameBgActive] = ImGui::ColorToVec(Vars::Menu::Colors::FrameBgActive.Value);
		colors[ImGuiCol_CheckMark] = ImGui::ColorToVec(Vars::Menu::Colors::CheckMark.Value);
		colors[ImGuiCol_Text] = ImGui::ColorToVec(Vars::Menu::Colors::Text.Value);

		colors[ImGuiCol_ScrollbarBg] = ImGui::ColorToVec(Vars::Menu::Colors::ScrollbarBG.Value);
		colors[ImGuiCol_ScrollbarGrab] = ImGui::ColorToVec(Vars::Menu::Colors::ScrollbarGrab.Value);
		colors[ImGuiCol_ScrollbarGrabActive] = ImGui::ColorToVec(Vars::Menu::Colors::ScrollbarGrabActive.Value);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::ColorToVec(Vars::Menu::Colors::ScrollbarGrabHovered.Value);
		colors[ImGuiCol_SliderGrab] = ImGui::ColorToVec(Vars::Menu::Colors::SliderGrab.Value);
		colors[ImGuiCol_SliderGrabActive] = ImGui::ColorToVec(Vars::Menu::Colors::SliderGrabActive.Value);
		colors[ImGuiCol_ResizeGrip] = ImGui::ColorToVec(Vars::Menu::Colors::ResizeGrip.Value);
		colors[ImGuiCol_ResizeGripActive] = ImGui::ColorToVec(Vars::Menu::Colors::ResizeGripActive.Value);
		colors[ImGuiCol_ResizeGripHovered] = ImGui::ColorToVec(Vars::Menu::Colors::ResizeGripHovered.Value);
		colors[ImGuiCol_Header] = ImGui::ColorToVec(Vars::Menu::Colors::Header.Value);
		colors[ImGuiCol_HeaderActive] = ImGui::ColorToVec(Vars::Menu::Colors::HeaderActive.Value);
		colors[ImGuiCol_HeaderHovered] = ImGui::ColorToVec(Vars::Menu::Colors::HeaderHovered.Value);



		// Alternative Designs
		if (Vars::Menu::ModernDesign.Value)
		{
			ItemWidth = 150.f;

			style.FrameBorderSize = 0.f;
			style.FrameRounding = 2.f;
			style.GrabRounding = 2.f;
		}
	}

	// Misc
	ImColor trans = ImGui::ColorToVec(Vars::Menu::Colors::MenuAccent.Value);
	trans.Value.w = 0;

	{
		TitleGradient.ClearMarks();
		TitleGradient.AddMark(0.f, trans);
		TitleGradient.AddMark(0.25f, trans);
		TitleGradient.AddMark(0.5f, ImGui::ColorToVec(Vars::Menu::Colors::MenuAccent.Value));
		TitleGradient.AddMark(0.75f, trans);
		TitleGradient.AddMark(1.f, trans);
	}
	{
		MainGradient.ClearMarks();
		MainGradient.AddMark(0.f, trans);
		MainGradient.AddMark(0.2f, trans);
		MainGradient.AddMark(0.5f, ImGui::ColorToVec(Vars::Menu::Colors::MenuAccent.Value));
		MainGradient.AddMark(0.8f, trans);
		MainGradient.AddMark(1.f, trans);
	}

	{
		TabGradient.ClearMarks();
		TabGradient.AddMark(0.f, trans);
		TabGradient.AddMark(0.2f, trans);
		TabGradient.AddMark(0.5f, ImGui::ColorToVec(Vars::Menu::Colors::MenuAccent.Value));
		TabGradient.AddMark(0.8f, trans);
		TabGradient.AddMark(1.f, trans);
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

		VerdanaSmall = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\verdana.ttf)", 12.0f, &fontConfig, fontRange);
		Verdana = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\verdana.ttf)", 14.0f, &fontConfig, fontRange);
		VerdanaBold = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\verdanab.ttf)", 18.0f, &fontConfig, fontRange);

		SectionFont = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\verdana.ttf)", 16.0f, &fontConfig, fontRange);
		TitleFont = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\verdanab.ttf)", 20.0f, &fontConfig, fontRange);

		constexpr ImWchar iconRange[]{ ICON_MIN_MD, ICON_MAX_MD, 0 };
		ImFontConfig iconConfig;
		iconConfig.MergeMode = true;
		iconConfig.PixelSnapH = true;
		IconFont = io.Fonts->AddFontFromMemoryCompressedTTF(MaterialFont_compressed_data, MaterialFont_compressed_size, 16.f, &iconConfig, iconRange);

		io.Fonts->Build();
	}

	LoadStyle();
	F::MaterialEditor.Init();
}
