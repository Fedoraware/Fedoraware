#include "Menu.h"

#include "../Vars.h"
#include "../Camera/CameraWindow.h"
#include "../AttributeChanger/AttributeChanger.h"
#include "../Radar/Radar.h"
#include "../Misc/Misc.h"

#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_stdlib.h"
#include "Fonts/IconsMaterialDesign.h"
#include "Playerlist/Playerlist.h"
#include "MaterialEditor/MaterialEditor.h"
#include "Pong/Pong.h"

#include "Components.hpp"
#include "ConfigManager/ConfigManager.h"

#include <mutex>

int unuPrimary = 0;
int unuSecondary = 0;

/* The main menu */
void CMenu::DrawMenu()
{
	ImGui::SetNextWindowSize(ImVec2(700, 700), ImGuiCond_FirstUseEver);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 700, 500 });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

	if (ImGui::Begin("Fedoraware", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
	{
		const auto drawList = ImGui::GetWindowDrawList();
		const auto windowSize = ImGui::GetWindowSize();
		const auto windowPos = ImGui::GetWindowPos();

		// Title gradient setup
		{
			TitleGradient.ClearMarks();
			TitleGradient.AddMark(0.f, ImGui::ColorToVec(Color_t{ 0,0,0,0 }));
			TitleGradient.AddMark(0.25f, ImGui::ColorToVec(Color_t{ 0,0,0,0 }));
			TitleGradient.AddMark(0.5f, ImGui::ColorToVec(Vars::Menu::Colors::MenuAccent));
			TitleGradient.AddMark(0.75f, ImGui::ColorToVec(Color_t{ 0,0,0,0 }));
			TitleGradient.AddMark(1.0f, ImGui::ColorToVec(Color_t{ 0,0,0,0 }));
		}
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

			// Keybinds Icon
			ImGui::SetCursorPos({ currentX -= 25, 0 });
			if (ImGui::IconButton(ICON_MD_KEYBOARD))
			{
				Vars::Menu::ShowKeybinds = !Vars::Menu::ShowKeybinds;
			}
			ImGui::HelpMarker("Keybinds");

			// Material Editor Icon
			ImGui::SetCursorPos({ currentX -= 25, 0 });
			if (ImGui::IconButton(ICON_MD_BRUSH))
			{
				F::MaterialEditor.IsOpen = !F::MaterialEditor.IsOpen;
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

	ImGui::PopStyleVar(2);
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
			WToggle("Aimbot", &Vars::Aimbot::Global::Active.Value); HelpMarker("Aimbot master switch");
			ColorPickerL("Target", Colors::Target);
			InputKeybind("Aimbot key", Vars::Aimbot::Global::AimKey); HelpMarker("The key to enable aimbot");
			WSlider("Aimbot FoV####AimbotFoV", &Vars::Aimbot::Global::AimFOV.Value, 0.f, 180.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
			ColorPickerL("Aimbot FOV circle", Colors::FOVCircle);
			WToggle("Autoshoot###AimbotAutoshoot", &Vars::Aimbot::Global::AutoShoot.Value); HelpMarker("Automatically shoot when a target is found");
			MultiCombo({ "Players", "Buildings", "Stickies"}, {&Vars::Aimbot::Global::AimPlayers.Value, &Vars::Aimbot::Global::AimBuildings.Value, &Vars::Aimbot::Global::AimStickies.Value}, "Aim targets");
			HelpMarker("Choose which targets the Aimbot should aim at");
			{
				static std::vector flagNames{ "Invulnerable", "Cloaked", "Dead Ringer", "Friends", "Taunting", "Vaccinator"};
				static std::vector flagValues{ 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5 };
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Global::IgnoreOptions.Value, "Ignored targets###AimbotIgnoredTargets");
				HelpMarker("Choose which targets should be ignored");
			}
			ColorPickerL("Invulnerable colour", Colors::Invuln);

			SectionTitle("Crits");
			WToggle("Crit hack", &Vars::CritHack::Active.Value);  HelpMarker("Enables the crit hack (BETA)");
			MultiCombo({ "Indicators", "Avoid Random", "Always Melee" }, {&Vars::CritHack::Indicators.Value, &Vars::CritHack::AvoidRandom.Value, &Vars::CritHack::AlwaysMelee.Value }, "Misc###CrithackMiscOptions");
			HelpMarker("Misc options for crithack");
			InputKeybind("Crit key", Vars::CritHack::CritKey); HelpMarker("Will try to force crits when the key is held");

			SectionTitle("Backtrack");
			WToggle("Active", &Vars::Backtrack::Enabled.Value); HelpMarker("If you shoot at the backtrack manually it will attempt to hit it");
			WToggle("Aimbot aims last tick", &Vars::Backtrack::Aim.Value); HelpMarker("Aimbot aims at the last tick if visible");
			WToggle("Fake latency", &Vars::Backtrack::FakeLatency.Value); HelpMarker("Fakes your latency to hit records further in the past");
			WSlider("Amount of latency###BTLatency", &Vars::Backtrack::Latency.Value, 200.f, 1000.f, "%.fms", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_ClampOnInput); HelpMarker("This won't work on local servers");
		} EndChild();

		/* Column 2 */
		if (TableColumnChild("AimbotCol2"))
		{
			SectionTitle("Hitscan");
			WCombo("Sort method###HitscanSortMethod", &Vars::Aimbot::Hitscan::SortMethod.Value, { "FOV", "Distance" }); HelpMarker("Which method the aimbot uses to decide which target to aim at");
			if (Vars::Aimbot::Hitscan::SortMethod.Value == 1) {
				WToggle("Respect FOV", &Vars::Aimbot::Hitscan::RespectFOV.Value); HelpMarker("Respect the Aim FOV set when using distance sorting.");
			}
			WCombo("Aim method###HitscanAimMethod", &Vars::Aimbot::Hitscan::AimMethod.Value, { "Plain", "Smooth", "Silent" }); HelpMarker("Which method the aimbot uses to aim at the target");
			WCombo("Preferred Hitbox###HitscanHitbox", &Vars::Aimbot::Hitscan::AimHitbox.Value, { "Head", "Body", "Auto"}); // this could probably be removed entirely since it actually does nothing.
			WCombo("Tapfire###HitscanTapfire", &Vars::Aimbot::Hitscan::TapFire.Value, { "Off", "Distance", "Always" }); HelpMarker("How/If the aimbot chooses to tapfire enemies.");
			WSlider("Smooth factor###HitscanSmoothing", &Vars::Aimbot::Hitscan::SmoothingAmount.Value, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Changes how smooth the aimbot will aim at the target");
			{
				static std::vector flagNames{ "Head", "Body", "Pelvis", "Arms", "Legs"};
				static std::vector flagValues{ 0x00000001, 0x00000004, 0x00000002, 0x00000008, 0x00000010}; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Hitscan::ScanHitboxes.Value, "Scan Hitboxes###AimbotHitboxScanning");
			}
			{
				static std::vector flagNames{ "Head", "Body", "Pelvis", "Arms", "Legs" };
				static std::vector flagValues{ 0x00000001, 0x00000004, 0x00000002, 0x00000008, 0x00000010 }; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Hitscan::MultiHitboxes.Value, "Multipoint Hitboxes###AimbotMultipointScanning");
			}
			WSlider("Point Scale###HitscanMultipointScale", &Vars::Aimbot::Hitscan::PointScale.Value, 0.5f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			WToggle("Buildings Multipoint", &Vars::Aimbot::Hitscan::ScanBuildings.Value); HelpMarker("Scans the building hitbox to improve hitchance");
			WToggle("Wait for headshot", &Vars::Aimbot::Hitscan::WaitForHeadshot.Value); HelpMarker("The aimbot will wait until it can headshot (if applicable)");
			WToggle("Wait for charge", &Vars::Aimbot::Hitscan::WaitForCharge.Value); HelpMarker("The aimbot will wait until the rifle has charged long enough to kill in one shot");
			WToggle("Smooth if spectated", &Vars::Aimbot::Hitscan::SpectatedSmooth.Value); HelpMarker("The aimbot will switch to the smooth method if being spectated");
			WToggle("Scoped only", &Vars::Aimbot::Hitscan::ScopedOnly.Value); HelpMarker("The aimbot will only shoot if scoped");
			WToggle("Auto scope", &Vars::Aimbot::Hitscan::AutoScope.Value); HelpMarker("The aimbot will automatically scope in to shoot");
			WToggle("Auto rev minigun", &Vars::Aimbot::Hitscan::AutoRev.Value); HelpMarker("Will rev heavy's minigun regardless of if aimbot has a target");
			WToggle("Bodyaim if lethal", &Vars::Aimbot::Global::BAimLethal.Value); HelpMarker("The aimbot will aim for body when damage is lethal to it");
			WToggle("Extinguish Team", &Vars::Aimbot::Hitscan::ExtinguishTeam.Value); HelpMarker("Will aim at burning teammates with The Sydney Sleeper");
		} EndChild();

		/* Column 3 */
		if (TableColumnChild("AimbotCol3"))
		{
			SectionTitle("Projectile");
			WSlider("Prediction Time", &Vars::Aimbot::Projectile::PredictionTime.Value, 0.1f, 10.f, "%.1f");
		
			{
				WCombo("Sort method###ProjectileSortMethod", &Vars::Aimbot::Projectile::SortMethod.Value, { "FOV", "Distance" });
				if (Vars::Aimbot::Projectile::SortMethod.Value == 1) {
					WToggle("Respect FOV", &Vars::Aimbot::Projectile::RespectFOV.Value); HelpMarker("Respect the Aim FOV set when using distance sorting.");
				}
				WCombo("Aim method###ProjectileAimMethod", &Vars::Aimbot::Projectile::AimMethod.Value, { "Plain", "Silent" });
				WCombo("Priority Hitbox###ProjectileHitboxPriority", &Vars::Aimbot::Projectile::AimPosition.Value, { "Head", "Body", "Feet", "Auto"});
				{
					static std::vector flagNames{ "Head", "Body", "Feet" };
					static std::vector flagValues{ (1<<0), (1<<1), (1<<2)}; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
					MultiFlags(flagNames, flagValues, &Vars::Aimbot::Projectile::AllowedHitboxes.Value, "Allowed Hitboxes###ProjectileHitboxScanning"); HelpMarker("Controls what hitboxes the cheat is allowed to consider shooting at.");
				}
				WSlider("Point VisTest Limit", &Vars::Aimbot::Projectile::VisTestPoints.Value, 3, 15, "%d", ImGuiSliderFlags_AlwaysClamp);	HelpMarker("Controls how many points the cheat is allowed to consider.");
				WSlider("Point Scan Limit", &Vars::Aimbot::Projectile::ScanPoints.Value, 3, Vars::Aimbot::Projectile::VisTestPoints.Value, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Controls how many visible points the cheat needs to find before it picks one to aim at.");
				WSlider("Point Scale", &Vars::Aimbot::Projectile::ScanScale.Value, 0.7f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Controls the size of the hitbox as it's given to the cheat.");
			}
			WToggle("Feet aim on ground", &Vars::Aimbot::Projectile::FeetAimIfOnGround.Value); HelpMarker("Will aim at feet if target is on the ground");

			SectionTitle("Splash Prediction");
			{
				WToggle("Splash prediction", &Vars::Aimbot::Projectile::SplashPrediction.Value); HelpMarker("Tries to deal splash damage if an enemy isn't visible");
				WToggle("Viewmodel flipper", &Vars::Misc::ViewmodelFlip.Value); HelpMarker("Automatically flips your viewmodel if it's beneficial");
				//WToggle("No spread###ProjectileNospread", &Vars::Aimbot::Projectile::NoSpread.Value); HelpMarker("Tries to compensate the random projectile spread");
			}

			SectionTitle("Melee");
			{
				WCombo("Sort method###MeleeSortMethod", &Vars::Aimbot::Melee::SortMethod.Value, { "FOV", "Distance", }); HelpMarker("Which method the aimbot uses to decide which target to aim at");
				if (Vars::Aimbot::Melee::SortMethod.Value == 1) {
					WToggle("Respect FOV", &Vars::Aimbot::Melee::RespectFOV.Value); HelpMarker("Respect the Aim FOV set when using distance sorting.");
				}
				WCombo("Aim method###MeleeAimMethod", &Vars::Aimbot::Melee::AimMethod.Value, { "Plain", "Smooth", "Silent" }); HelpMarker("Which method the aimbot uses to aim at the target");
			}
			WSlider("Smooth factor###MeleeSmoothing", &Vars::Aimbot::Melee::SmoothingAmount.Value, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How smooth the aimbot should be");
			WToggle("Range check", &Vars::Aimbot::Melee::RangeCheck.Value); HelpMarker("Only aim at target if within melee range");
			WToggle("Swing prediction", &Vars::Aimbot::Melee::PredictSwing.Value); HelpMarker("Aimbot will attack preemptively, predicting you will be in range of the target");
			WToggle("Whip teammates", &Vars::Aimbot::Melee::WhipTeam.Value); HelpMarker("Aimbot will target teammates if holding the Disciplinary Action");
			//WToggle("Wait for hit", &Vars::Aimbot::Projectile::WaitForHit.Value); HelpMarker("Will avoid shooting until the last shot hits");

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
			MultiCombo({ "Invulnerable", "Cloaked", "Friends" }, { &Vars::Triggerbot::Global::IgnoreInvlunerable.Value, &Vars::Triggerbot::Global::IgnoreCloaked.Value, &Vars::Triggerbot::Global::IgnoreFriends.Value }, "Ignored targets###TriggerIgnoredTargets");
			HelpMarker("Choose which targets should be ignored");

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
			MultiCombo({ "Players", "Buildings", "Stickies" }, { &Vars::Triggerbot::Detonate::DetonateOnPlayer.Value, &Vars::Triggerbot::Detonate::DetonateOnBuilding.Value, &Vars::Triggerbot::Detonate::DetonateOnSticky.Value }, "Targets###AutoDetonateTargets");
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
			WSlider("FOV####AirBlastFov", &Vars::Triggerbot::Blast::Fov.Value, 0.f, 90.f, "%.f", ImGuiSliderFlags_AlwaysClamp);

			SectionTitle("Autouber");
			WToggle("Autouber###Triggeruber", &Vars::Triggerbot::Uber::Active.Value); HelpMarker("Auto uber master switch");
			WToggle("Only uber friends", &Vars::Triggerbot::Uber::OnlyFriends.Value); HelpMarker("Auto uber will only activate if healing steam friends");
			WToggle("Preserve self", &Vars::Triggerbot::Uber::PopLocal.Value); HelpMarker("Auto uber will activate if local player's health falls below the percentage");
			WToggle("Vaccinator resistances", &Vars::Triggerbot::Uber::AutoVacc.Value); HelpMarker("Auto uber will automatically find the best resistance and pop when needed (This doesn't work properly)");
			WSlider("Health left (%)###TriggerUberHealthLeft", &Vars::Triggerbot::Uber::HealthLeft.Value, 1.f, 99.f, "%.0f%%", 1.0f); HelpMarker("The amount of health the heal target must be below to actiavte");
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
	// Visuals: Players
	case VisualsTab::Players:
	{
		if (BeginTable("VisualsPlayersTable", 3))
		{
			/* Column 1 */
			if (TableColumnChild("VisualsPlayersCol1"))
			{
				SectionTitle("ESP Main");
				WToggle("ESP###EnableESP", &Vars::ESP::Main::Active.Value); HelpMarker("Global ESP master switch");
				WToggle("Outlined health bars", &Vars::ESP::Main::Outlinedbar.Value); HelpMarker("Will outline the health bars");
				WToggle("Relative colours", &Vars::ESP::Main::EnableTeamEnemyColors.Value); HelpMarker("Chooses colors relative to your team (team/enemy)");
				if (Vars::ESP::Main::EnableTeamEnemyColors.Value)
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
				WToggle("Player ESP###EnablePlayerESP", &Vars::ESP::Players::Active.Value); HelpMarker("Will draw useful information/indicators on players");
				WToggle("Name ESP###PlayerNameESP", &Vars::ESP::Players::Name.Value); HelpMarker("Will draw the players name");
				WToggle("Custom Name Color", &Vars::ESP::Players::NameCustom.Value); HelpMarker("Custom color for name esp");
				if (Vars::ESP::Players::NameCustom.Value)
				{
					ColorPickerL("Name ESP Color", Vars::ESP::Players::NameColor);
				}
				WToggle("Name ESP box###PlayerNameESPBox", &Vars::ESP::Players::NameBox.Value); HelpMarker("Will draw a box around players name to make it stand out");
				WToggle("Self ESP###SelfESP", &Vars::ESP::Players::ShowLocal.Value); HelpMarker("Will draw ESP on local player (thirdperson)");
				ColorPickerL("Local colour", Colors::Local);
				WCombo("Ignore team###IgnoreTeamESPp", &Vars::ESP::Players::IgnoreTeammates.Value, { "Off", "All", "Only friends" }); HelpMarker("Which teammates the ESP will ignore drawing on");
				ColorPickerL("Friend colour", Colors::Friend);
				WCombo("Ignore cloaked###IgnoreCloakESPp", &Vars::ESP::Players::IgnoreCloaked.Value, { "Off", "All", "Only enemies" }); HelpMarker("Which cloaked spies the ESP will ignore drawing on");
				ColorPickerL("Cloaked colour", Colors::Cloak);
				WCombo("Ubercharge###PlayerUber", &Vars::ESP::Players::Uber.Value, { "Off", "Text", "Bar" }); HelpMarker("Will draw how much ubercharge a medic has");
				ColorPickerL("Ubercharge colour", Colors::UberColor);
				WCombo("Class###PlayerIconClass", &Vars::ESP::Players::Class.Value, { "Off", "Icon", "Text", "Both" }); HelpMarker("Will draw the class the player is");
				WToggle("Weapon icons", &Vars::ESP::Players::WeaponIcon.Value); HelpMarker("Shows an icon for the weapon that the player has currently equipped");
				ColorPickerL("Invulnerable colour", Colors::WeaponIcon);
				WToggle("Health bar###ESPPlayerHealthBar", &Vars::ESP::Players::HealthBar.Value); HelpMarker("Will draw a bar visualizing how much health the player has");
				ColorPickerL("Health Bar Top", Colors::GradientHealthBar.startColour);
				ColorPickerL("Health Bar Bottom", Colors::GradientHealthBar.endColour, 1);
				WCombo("Health Text###ESPPlayerHealthText", &Vars::ESP::Players::HealthText.Value, { "Off", "Default", "Bar" }); HelpMarker("Draws the player health as a text");
				WToggle("Condition", &Vars::ESP::Players::Cond.Value); HelpMarker("Will draw what conditions the player is under");
				ColorPickerL("Condition colour", Colors::Cond);
				WToggle("GUID", &Vars::ESP::Players::GUID.Value); HelpMarker("Show's the players Steam ID");
				WToggle("Choked Packets", &Vars::ESP::Players::Choked.Value); HelpMarker("Shows how many packets the player has choked");
				ColorPickerL("Choked Bar Top", Colors::ChokedBar.startColour);
				ColorPickerL("Choked Bar Bottom", Colors::ChokedBar.endColour, 1);
				WToggle("Cheater Detection", &Vars::ESP::Players::CheaterDetection.Value); HelpMarker("Attempts to automatically mark cheaters.");
				WCombo("Box###PlayerBoxESP", &Vars::ESP::Players::Box.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on players");
				WCombo("Skeleton###PlayerSkellington", &Vars::ESP::Players::Bones.Value, { "Off", "Custom colour", "Health" }); HelpMarker("Will draw the bone structure of the player");
				ColorPickerL("Skellington colour", Colors::Bones);
				WToggle("Lines###Playerlines", &Vars::ESP::Players::Lines.Value); HelpMarker("Draws lines from the local players position to enemies position");
				WToggle("Dlights###PlayerDlights", &Vars::ESP::Players::Dlights.Value); HelpMarker("Will make players emit a dynamic light around them");
				WSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Players::DlightRadius.Value, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
				WSlider("ESP alpha###PlayerESPAlpha", &Vars::ESP::Players::Alpha.Value, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsPlayersCol2"))
			{
				SectionTitle("Chams Main");
				WToggle("Chams###ChamsMasterSwitch", &Vars::Chams::Main::Active.Value); HelpMarker("Chams master switch");

				static std::vector chamOptions{
					"Local",
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
				static std::vector dmeGlowMaterial{
					"None",
					"Fresnel Glow",
					"Wireframe Glow"
				};

				static int currentSelected = 0; // 0.local 1.friends 2.enemies 3.team 4.target 5.ragdolls 6.hands 7.weapon
				Chams_t& currentStruct = ([&]() -> Chams_t&
					{
						switch (currentSelected) {
							case 0: {
								return Vars::Chams::Players::Local;
							}
							case 1: {
								return Vars::Chams::Players::Friend;
							}
							case 2: {
								return Vars::Chams::Players::Enemy;
							}
							case 3: {
								return Vars::Chams::Players::Team;
							}
							case 4: {
								return Vars::Chams::Players::Target;
							}
							case 5: {
								return Vars::Chams::Players::Ragdoll;
							}
							case 6: {
								return Vars::Chams::DME::Hands;
							}
							case 7: {
								return Vars::Chams::DME::Weapon;
							}
						}

						return Vars::Chams::Players::Local;
					}());
				static std::vector DMEChamMaterials{ "Original", "Shaded", "Shiny", "Flat", "Wireframe shaded", "Wireframe shiny", "Wireframe flat", "Fresnel", "Brick", "Custom" };
				
				//WToggle("Player chams###PlayerChamsBox", &Vars::Chams::Players::Active.Value); HelpMarker("Player chams master switch");

				MultiCombo({ "Render Wearable", "Render Weapon", "Fadeout Own Team"}, {&Vars::Chams::Players::Wearables.Value, &Vars::Chams::Players::Weapons.Value, &Vars::Chams::Players::FadeoutTeammates.Value}, "Flags");
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
					WSlider("Glow Reduction", &currentStruct.overlayIntensity, 150.f, 0.1f, "%.1f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_ClampOnInput);

					if (currentSelected == 7 || currentSelected == 6) {
						int& proxySkinIndex = currentSelected == 7 ? Vars::Chams::DME::WeaponsProxySkin.Value : Vars::Chams::DME::HandsProxySkin.Value;
						WCombo("Proxy Material", &proxySkinIndex, DMEProxyMaterials);

					}
				}
			
				SectionTitle("Chams Misc");

				WToggle("DME chams###dmeactive", &Vars::Chams::DME::Active.Value); HelpMarker("DME chams master switch");

				SectionTitle("Backtrack chams");
				WToggle("Backtrack chams", &Vars::Backtrack::BtChams::Enabled.Value); HelpMarker("Draws chams to show where a player is");
				ColorPickerL("Backtrack colour", Vars::Backtrack::BtChams::BacktrackColor);
				WToggle("Only draw last tick", &Vars::Backtrack::BtChams::LastOnly.Value); HelpMarker("Only draws the last tick (can save FPS)");
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

				SectionTitle("Fakelag chams");
				WToggle("Fakelag chams", &Vars::Misc::CL_Move::FakelagIndicator.Value); HelpMarker("Draws chams to show your fakelag position");
				ColorPickerL("Fakelag colour", Vars::Misc::CL_Move::FLGChams::FakelagColor);
				WCombo("Fakelag material", &Vars::Misc::CL_Move::FLGChams::Material.Value, backtrackMaterial);
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsPlayersCol3"))
			{
				SectionTitle("Glow Main");
				WToggle("Glow", &Vars::Glow::Main::Active.Value);
				WToggle("Stencil glow", &Vars::Glow::Main::Stencil.Value);
				if (!Vars::Glow::Main::Stencil.Value) { WSlider("Glow scale", &Vars::Glow::Main::Scale.Value, 1, 10, "%d", ImGuiSliderFlags_AlwaysClamp); }

				SectionTitle("Player Glow");
				WToggle("Player glow###PlayerGlowButton", &Vars::Glow::Players::Active.Value); HelpMarker("Player glow master switch");
				WToggle("Self glow###SelfGlow", &Vars::Glow::Players::ShowLocal.Value); HelpMarker("Draw glow on the local player");
				WToggle("Self rainbow glow###SelfGlowRainbow", &Vars::Glow::Players::LocalRainbow.Value); HelpMarker("Homosapien");
				WCombo("Ignore team###IgnoreTeamGlowp", &Vars::Glow::Players::IgnoreTeammates.Value, { "Off", "All", "Only friends" }); HelpMarker("Which teammates the glow will ignore drawing on");
				WToggle("Wearable glow###PlayerWearableGlow", &Vars::Glow::Players::Wearables.Value); HelpMarker("Will draw glow on player cosmetics");
				WToggle("Weapon glow###PlayerWeaponGlow", &Vars::Glow::Players::Weapons.Value); HelpMarker("Will draw glow on player weapons");
				WSlider("Glow alpha###PlayerGlowAlpha", &Vars::Glow::Players::Alpha.Value, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
				WCombo("Glow colour###GlowColour", &Vars::Glow::Players::Color.Value, { "Team", "Health" }); HelpMarker("Which colour the glow will draw");
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
				WToggle("Building ESP###BuildinGESPSwioifas", &Vars::ESP::Buildings::Active.Value); HelpMarker("Will draw useful information/indicators on buildings");
				WToggle("Ignore team buildings###BuildingESPIgnoreTeammates", &Vars::ESP::Buildings::IgnoreTeammates.Value); HelpMarker("Whether or not to draw ESP on your teams buildings");
				WToggle("Name ESP###BuildingNameESP", &Vars::ESP::Buildings::Name.Value); HelpMarker("Will draw the players name");
				WToggle("Custom Name Color", &Vars::ESP::Buildings::NameCustom.Value); HelpMarker("Custom color for name esp");
				if (Vars::ESP::Buildings::NameCustom.Value)
				{
					ColorPickerL("Name ESP Color", Vars::ESP::Buildings::NameColor);
				}
				WToggle("Name ESP box###BuildingNameESPBox", &Vars::ESP::Buildings::NameBox.Value); HelpMarker("Will draw a box around the buildings name to make it stand out");
				WToggle("Health bar###Buildinghelathbar", &Vars::ESP::Buildings::HealthBar.Value); HelpMarker("Will draw a bar visualizing how much health the building has");
				WToggle("Health text###buildinghealth", &Vars::ESP::Buildings::Health.Value); HelpMarker("Will draw the building's health, as well as its max health");
				WToggle("Building owner###Buildingowner", &Vars::ESP::Buildings::Owner.Value); HelpMarker("Shows who built the building");
				WToggle("Building level###Buildinglevel", &Vars::ESP::Buildings::Level.Value); HelpMarker("Will draw what level the building is");
				WToggle("Building condition###Buildingconditions", &Vars::ESP::Buildings::Cond.Value); HelpMarker("Will draw what conditions the building is under");
				WToggle("Teleporter exit direction###Buildingteleexitdir", &Vars::ESP::Buildings::TeleExitDir.Value); HelpMarker("Show teleporter exit direction arrow");
				ColorPickerL("Teleporter exit direction arrow color", Vars::ESP::Buildings::TeleExitDirColor);
				WToggle("Lines###buildinglines", &Vars::ESP::Buildings::Lines.Value); HelpMarker("Draws lines from the local players position to the buildings position");
				WCombo("Box###PBuildingBoxESP", &Vars::ESP::Buildings::Box.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on buildings");
				WToggle("Dlights###PlayerDlights", &Vars::ESP::Buildings::Dlights.Value); HelpMarker("Will make buildings emit a dynamic light around them, although buildings can't move some I'm not sure that the lights are actually dynamic here...");
				WSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Buildings::DlightRadius.Value, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
				WSlider("ESP alpha###BuildingESPAlpha", &Vars::ESP::Buildings::Alpha.Value, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the ESP should be");
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsBuildingsCol2"))
			{
				SectionTitle("Building Chams");
				WToggle("Building chams###BuildingChamsBox", &Vars::Chams::Buildings::Active.Value); HelpMarker("Building chams master switch");

				static std::vector chamOptions{
					"Local",
					"Friends",
					"Enemies",
					"Teammates",
					"Target"
				};
				static std::vector dmeGlowMaterial{
					"None",
					"Fresnel Glow",
					"Wireframe Glow"
				};

				static int currentSelected = 0; //
				Chams_t& currentStruct = ([&]() -> Chams_t&
					{
						switch (currentSelected) {
							case 0: {
								return Vars::Chams::Buildings::Local;
							}
							case 1: {
								return Vars::Chams::Buildings::Friend;
							}
							case 2: {
								return Vars::Chams::Buildings::Enemy;
							}
							case 3: {
								return Vars::Chams::Buildings::Team;
							}
							case 4: {
								return Vars::Chams::Buildings::Target;
							}
						}

						return Vars::Chams::Buildings::Local;
					}());
				static std::vector DMEChamMaterials{ "Original", "Shaded", "Shiny", "Flat", "Wireframe shaded", "Wireframe shiny", "Wireframe flat", "Fresnel", "Brick", "Custom" };

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
					WSlider("Glow Reduction", &currentStruct.overlayIntensity, 150.f, 0.1f, "%.1f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_ClampOnInput);
				}
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsBuildingsCol3"))
			{
				SectionTitle("Building Glow");
				WToggle("Building glow###BuildiongGlowButton", &Vars::Glow::Buildings::Active.Value);
				WToggle("Ignore team buildings###buildingglowignoreteams", &Vars::Glow::Buildings::IgnoreTeammates.Value);
				WSlider("Glow alpha###BuildingGlowAlpha", &Vars::Glow::Buildings::Alpha.Value, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
				WCombo("Glow colour###GlowColourBuildings", &Vars::Glow::Buildings::Color.Value, { "Team", "Health" });
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
				WToggle("World ESP###WorldESPActive", &Vars::ESP::World::Active.Value); HelpMarker("World ESP master switch");
				WToggle("Healthpacks###WorldESPHealthPacks", &Vars::ESP::World::HealthText.Value); HelpMarker("Will draw ESP on healthpacks");
				ColorPickerL("Healthpack colour", Colors::Health); HelpMarker("Color for healthpack ESP");
				WToggle("Ammopacks###WorldESPAmmoPacks", &Vars::ESP::World::AmmoText.Value); HelpMarker("Will draw ESP on ammopacks");
				ColorPickerL("Ammopack colour", Colors::Ammo); HelpMarker("Color for ammopack ESP");
				WSlider("ESP alpha###WordlESPAlpha", &Vars::ESP::World::Alpha.Value, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the world ESP should be");
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsWorldCol2"))
			{
				SectionTitle("World Chams");
				WToggle("World chams###woldchamsbut", &Vars::Chams::World::Active.Value);

				static std::vector chamOptions{
					"Healthpacks",
					"Ammopacks",
					"Projectiles"
				};
				static std::vector dmeGlowMaterial{
					"None",
					"Fresnel Glow",
					"Wireframe Glow"
				};

				static int currentSelected = 0; //
				Chams_t& currentStruct = ([&]() -> Chams_t&
					{
						switch (currentSelected) {
							case 0: {
								return Vars::Chams::World::Health;
							}
							case 1: {
								return Vars::Chams::World::Ammo;
							}
							case 2: {
								return Vars::Chams::World::Projectiles;
							}
						}

						return Vars::Chams::World::Health;
					}());
				static std::vector DMEChamMaterials{ "Original", "Shaded", "Shiny", "Flat", "Wireframe shaded", "Wireframe shiny", "Wireframe flat", "Fresnel", "Brick", "Custom" };

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
					WSlider("Glow Reduction", &currentStruct.overlayIntensity, 150.f, 0.1f, "%.1f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_ClampOnInput);
				}
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsWorldCol3"))
			{
				SectionTitle("World Glow");
				WToggle("World glow###Worldglowbutton", &Vars::Glow::World::Active.Value);
				WToggle("Healthpacks###worldhealthpackglow", &Vars::Glow::World::Health.Value);
				WToggle("Ammopacks###worldammopackglow", &Vars::Glow::World::Ammo.Value);
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
			static std::vector fontFlagNames { "Italic", "Underline", "Strikeout", "Symbol", "Antialias", "Gaussian", "Rotary", "Dropshadow", "Additive", "Outline", "Custom" };
			static std::vector fontFlagValues {0x001, 0x002, 0x004, 0x008, 0x010, 0x020, 0x040, 0x080, 0x100, 0x200, 0x400 };

			/* Column 1 */
			if (TableColumnChild("VisualsFontCol1"))
			{
				SectionTitle("ESP Font");
				WInputText("Font name###espfontname", &Vars::Fonts::FONT_ESP::szName);
				WInputInt("Font height###espfontheight", &Vars::Fonts::FONT_ESP::nTall.Value);
				WInputInt("Font weight###espfontweight", &Vars::Fonts::FONT_ESP::nWeight.Value);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP::nFlags.Value, "Font flags###FONT_ESP");

				SectionTitle("Name Font");
				WInputText("Font name###espfontnamename", &Vars::Fonts::FONT_ESP_NAME::szName);
				WInputInt("Font height###espfontnameheight", &Vars::Fonts::FONT_ESP_NAME::nTall.Value);
				WInputInt("Font weight###espfontnameweight", &Vars::Fonts::FONT_ESP_NAME::nWeight.Value);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP_NAME::nFlags.Value, "Font flags###FONT_ESP_NAME");
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsFontCol2"))
			{
				SectionTitle("Condition Font");
				WInputText("Font name###espfontcondname", &Vars::Fonts::FONT_ESP_COND::szName);
				WInputInt("Font height###espfontcondheight", &Vars::Fonts::FONT_ESP_COND::nTall.Value);
				WInputInt("Font weight###espfontcondweight", &Vars::Fonts::FONT_ESP_COND::nWeight.Value);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP_COND::nFlags.Value, "Font flags###FONT_ESP_COND");

				SectionTitle("Pickup Font");
				WInputText("Font name###espfontpickupsname", &Vars::Fonts::FONT_ESP_PICKUPS::szName);
				WInputInt("Font height###espfontpickupsheight", &Vars::Fonts::FONT_ESP_PICKUPS::nTall.Value);
				WInputInt("Font weight###espfontpickupsweight", &Vars::Fonts::FONT_ESP_PICKUPS::nWeight.Value);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP_PICKUPS::nFlags.Value, "Font flags###FONT_ESP_PICKUPS");
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsFontCol3"))
			{
				SectionTitle("Menu Font");
				WInputText("Font name###espfontnamenameneby", &Vars::Fonts::FONT_MENU::szName);
				WInputInt("Font height###espfontnameheightafsdfads", &Vars::Fonts::FONT_MENU::nTall.Value);
				WInputInt("Font weight###espfontnameweightasfdafsd", &Vars::Fonts::FONT_MENU::nWeight.Value);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_MENU::nFlags.Value, "Font flags###FONT_MENU");

				SectionTitle("Indicator Font");
				WInputText("Font name###espfontindicatorname", &Vars::Fonts::FONT_INDICATORS::szName);
				WInputInt("Font height###espfontindicatorheight", &Vars::Fonts::FONT_INDICATORS::nTall.Value);
				WInputInt("Font weight###espfontindicatorweight", &Vars::Fonts::FONT_INDICATORS::nWeight.Value);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_INDICATORS::nFlags.Value, "Font flags###FONT_INDICATORS");

				if (Button("Apply settings###fontapply"))
				{
					const Font_t fontEsp = {
						0x0,
						Vars::Fonts::FONT_ESP::szName.c_str(),
						Vars::Fonts::FONT_ESP::nTall.Value,
						Vars::Fonts::FONT_ESP::nWeight.Value,
						Vars::Fonts::FONT_ESP::nFlags.Value
					};
					const Font_t fontEspName = {
						0x0,
						Vars::Fonts::FONT_ESP_NAME::szName.c_str(),
						Vars::Fonts::FONT_ESP_NAME::nTall.Value,
						Vars::Fonts::FONT_ESP_NAME::nWeight.Value,
						Vars::Fonts::FONT_ESP_NAME::nFlags.Value
					};
					const Font_t fontEspCond = {
						0x0,
						Vars::Fonts::FONT_ESP_COND::szName.c_str(),
						Vars::Fonts::FONT_ESP_COND::nTall.Value,
						Vars::Fonts::FONT_ESP_COND::nWeight.Value,
						Vars::Fonts::FONT_ESP_COND::nFlags.Value
					};
					const Font_t fontIndicator = {
						0x0,
						Vars::Fonts::FONT_INDICATORS::szName.c_str(),
						Vars::Fonts::FONT_INDICATORS::nTall.Value,
						Vars::Fonts::FONT_INDICATORS::nWeight.Value,
						Vars::Fonts::FONT_INDICATORS::nFlags.Value
					};
					const Font_t fontEspPickups = {
						0x0,
						Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(),
						Vars::Fonts::FONT_ESP_PICKUPS::nTall.Value,
						Vars::Fonts::FONT_ESP_PICKUPS::nWeight.Value,
						Vars::Fonts::FONT_ESP_PICKUPS::nFlags.Value
					};
					const Font_t fontMenu = {
						0x0,
						Vars::Fonts::FONT_MENU::szName.c_str(),
						Vars::Fonts::FONT_MENU::nTall.Value,
						Vars::Fonts::FONT_MENU::nWeight.Value,
						Vars::Fonts::FONT_MENU::nFlags.Value
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
				WSlider("Field of view", &Vars::Visuals::FieldOfView.Value, 70, 150, "%d"); HelpMarker("How many degrees of field of vision you would like");
				WCombo("Vision modifiers", &Vars::Visuals::VisionModifier.Value, { "Off", "Pyrovision", "Halloween", "Romevision" }); HelpMarker("Vision modifiers");
				MultiCombo({ "World", "Sky", "Prop Wireframe" }, { &Vars::Visuals::WorldModulation.Value, &Vars::Visuals::SkyModulation.Value, &Vars::Visuals::PropWireframe.Value }, "Modulations");
				HelpMarker("Select which types of modulation you want to enable");
				if (ColorPickerL("World modulation colour", Colors::WorldModulation) ||
					ColorPickerL("Sky modulation colour", Colors::SkyModulation, 1) ||
					ColorPickerL("Prop modulation colour", Colors::StaticPropModulation, 2))
				{
					G::ShouldUpdateMaterialCache = true;
				}
				MultiCombo({ "Scope", "Zoom", "Disguises", "Taunts", "Interpolation", "View Punch", "MOTD", "Screen Effects", "Angle Forcing", "Ragdolls", "Screen Overlays"}, {&Vars::Visuals::RemoveScope.Value, &Vars::Visuals::RemoveZoom.Value, &Vars::Visuals::RemoveDisguises.Value, &Vars::Visuals::RemoveTaunts.Value, &Vars::Misc::DisableInterpolation.Value, &Vars::Visuals::RemovePunch.Value, &Vars::Visuals::RemoveMOTD.Value, &Vars::Visuals::RemoveScreenEffects.Value, &Vars::Visuals::PreventForcedAngles.Value, &Vars::Visuals::RemoveRagdolls.Value, &Vars::Visuals::RemoveScreenOverlays.Value}, "Removals");
				HelpMarker("Select what you want to remove");
				if (WCombo("Screen Overlay", &Vars::Visuals::VisualOverlay.Value, { "None", "Fire", "Jarate", "Bleed", "Stealth", "Dodge" })) {
					I::ViewRender->SetScreenOverlayMaterial(nullptr);
				}
				WToggle("Crosshair aim position", &Vars::Visuals::CrosshairAimPos.Value);
				WToggle("Box aim position", &Vars::Visuals::AimPosSquare.Value);
				WToggle("Viewmodel aim position", &Vars::Visuals::AimbotViewmodel.Value);
				WToggle("Bullet tracers", &Vars::Visuals::BulletTracer.Value);
				ColorPickerL("Bullet tracer colour", Colors::BulletTracer);
				WToggle("Rainbow tracers", &Vars::Visuals::BulletTracerRainbow.Value); HelpMarker("Bullet tracer color will be dictated by a changing color");
				WToggle("Viewmodel sway", &Vars::Visuals::ViewmodelSway.Value);
				WToggle("Movement simulation lines", &Vars::Visuals::MoveSimLine.Value);
				ColorPickerL("Prediction Line Color", Vars::Aimbot::Projectile::PredictionColor);
				{
					static std::vector flagNames{ "Text", "Console", "Chat", "Party", "Verbose"};
					static std::vector flagValues{ 1, 2, 4, 8, 32 };
					MultiFlags(flagNames, flagValues, &Vars::Misc::VotingOptions.Value, "Vote Logger###VoteLoggingOptions");
				}
				MultiCombo({ "Damage Logs (Console)", "Damage Logs (Text)", "Damage Logs (Chat)", "Class Changes (Text)", "Class Changes (Chat)" }, { &Vars::Visuals::DamageLoggerConsole.Value, &Vars::Visuals::DamageLoggerText.Value, &Vars::Visuals::DamageLoggerChat.Value, &Vars::Visuals::ChatInfoText.Value, &Vars::Visuals::ChatInfoChat.Value }, "Event Logging");
				HelpMarker("What & How should events be logged");
				ColorPickerL("GUI Notif Background", Colors::NotifBG);
				ColorPickerL("GUI Notif Outline", Colors::NotifOutline, 1);
				ColorPickerL("GUI Notif Colour", Colors::NotifText, 2);
				WSlider("GUI Notif Time", &Vars::Visuals::NotificationLifetime.Value, 0.5f, 3.f, "%.1f");
				WCombo("Particle tracer", &Vars::Visuals::ParticleTracer.Value, { "Off", "Machina", "C.A.P.P.E.R", "Short Circuit", "Merasmus ZAP", "Merasmus ZAP Beam 2", "Big Nasty", "Distortion Trail", "Black Ink", "Custom" });
				if (Vars::Visuals::ParticleTracer.Value == 9)
				{
					WInputText("Custom Tracer", &Vars::Visuals::ParticleName); HelpMarker("If you want to use a custom particle tracer");
				}
				WToggle("Noscope lines", &Vars::Visuals::ScopeLines.Value); HelpMarker("Will draw a custom overlay");
				ColorPickerL("Inner line color", Colors::NoscopeLines1);
				ColorPickerL("Outer line color", Colors::NoscopeLines2, 1);
				WToggle("Pickup Timers", &Vars::Visuals::PickupTimers.Value); HelpMarker("Displays the respawn time of health and ammopacks");
				WToggle("Draw Hitboxes", &Vars::Aimbot::Global::showHitboxes.Value); HelpMarker("Shows client hitboxes for enemies once they are attacked (not bbox)");
				ColorPickerL("Hitbox matrix face colour", Colors::HitboxFace);
				ColorPickerL("Hitbox matrix edge colour", Colors::HitboxEdge, 1);
				WToggle("Clear Hitboxes", &Vars::Aimbot::Global::ClearPreviousHitbox.Value); HelpMarker("Removes previous drawn hitboxes to mitigate clutter");
				WSlider("Hitbox Draw Time", &Vars::Aimbot::Global::HitboxLifetime.Value, 1, 5); HelpMarker("Removes previous drawn hitboxes after n seconds");
				WCombo("Spectator list", &Vars::Visuals::SpectatorList.Value, { "Off", "Draggable", "Static", "Static + Avatars" });
				WToggle("Killstreak weapon", &Vars::Misc::KillstreakWeapon.Value); HelpMarker("Enables the killstreak counter on any weapon");
				WToggle("Post processing", &Vars::Visuals::DoPostProcessing.Value); HelpMarker("Toggle post processing effects");
				SectionTitle("Beams");
				{
					using namespace Vars::Visuals;

					WToggle("Enable beams", &Beans::Active.Value); HelpMarker("he loves beans?");
					WToggle("Rainbow beams", &Beans::Rainbow.Value);
					ColorPickerL("Beam colour", Beans::BeamColour);
					WToggle("Custom model", &Beans::UseCustomModel.Value);
					if (Beans::UseCustomModel.Value)
					{
						WInputText("Model", &Beans::Model);
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
						static std::vector flagNames { "STARTENTITY", "ENDENTITY","FADEIN","FADEOUT","SINENOISE","SOLID","SHADEIN","SHADEOUT","ONLYNOISEONCE","NOTILE","USE_HITBOXES","STARTVISIBLE","ENDVISIBLE","ISACTIVE","FOREVER","HALOBEAM","REVERSED", };
						static std::vector flagValues { 0x00000001, 0x00000002,0x00000004,0x00000008,0x00000010,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,0x00000400,0x00000800,0x00001000,0x00002000,0x00004000,0x00008000,0x00010000 };
						MultiFlags(flagNames, flagValues, &Beans::Flags.Value, "Beam Flags###BeamFlags");
					}
				}

				SectionTitle("Viewmodel Offset");
				WSlider("VM Off X", &Vars::Visuals::VMOffsets.x, -45.f, 45.f);
				WSlider("VM Off Y", &Vars::Visuals::VMOffsets.y, -45.f, 45.f);
				WSlider("VM Off Z", &Vars::Visuals::VMOffsets.z, -45.f, 45.f);
				WSlider("VM Roll", &Vars::Visuals::VMRoll.Value, -180, 180);

				SectionTitle("DT Indicator");
				WCombo("DT indicator style", &Vars::Misc::CL_Move::DTBarStyle.Value, { "Off", "Default", "Nitro", "Rijin" }); HelpMarker("Which style to do the bar style");
				ColorPickerL("DT charging right", Colors::DTBarIndicatorsCharging.endColour);
				ColorPickerL("DT charging left", Colors::DTBarIndicatorsCharging.startColour, 1);
				if (Vars::Misc::CL_Move::DTBarStyle.Value == 3)
				{
					WSlider("DT Bar height###dtBHeightNitro", &Vars::Misc::CL_Move::DTBarScaleY.Value, 1, 25);
					ColorPickerL("DT charged right", Colors::DTBarIndicatorsCharged.endColour);
					ColorPickerL("DT charged left", Colors::DTBarIndicatorsCharged.startColour, 1);
					WSlider("DT Bar width###dtBWidthNitro", &Vars::Misc::CL_Move::DTBarScaleX.Value, 100, 1000);
				}
				else
				{
					WSlider("DT Bar height###dtBHeight", &Vars::Misc::CL_Move::DtbarOutlineHeight.Value, 1, 30);
					ColorPickerL("DT charged right", Colors::DTBarIndicatorsCharged.endColour);
					ColorPickerL("DT charged left", Colors::DTBarIndicatorsCharged.startColour, 1);
					WSlider("DT Bar width###dtBWidth", &Vars::Misc::CL_Move::DtbarOutlineWidth.Value, 1, 30);
				}

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

				SectionTitle("Freecam");
				InputKeybind("Freecam Key", Vars::Visuals::FreecamKey);  HelpMarker("Allows you to freely move your camera when holding the key");
				WSlider("Freecam Speed", &Vars::Visuals::FreecamSpeed.Value, 1.f, 20.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Movement speed of freecam");

				SectionTitle("Camera");
				WCombo("Camera mode", &Vars::Visuals::CameraMode.Value, { "Off", "Mirror", "Spy", "Teleporter", "Teleporter (Portal)"}); HelpMarker("What the camera should display");
				WSlider("Camera FOV", &Vars::Visuals::CameraFOV.Value, 40.f, 130.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("FOV of the camera window");
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
				WToggle("Skybox changer", &Vars::Visuals::SkyboxChanger.Value); HelpMarker("Will change the skybox, either to a base TF2 one or a custom one");
				WCombo("Skybox", &Vars::Skybox::SkyboxNum, skyNames);
				if (Vars::Skybox::SkyboxNum == 0)
				{
					WInputText("Custom skybox name", &Vars::Skybox::SkyboxName); HelpMarker("Name of the skybox you want to you (tf/materials/skybox)");
				}
				WToggle("World Textures Override", &Vars::Visuals::OverrideWorldTextures.Value); HelpMarker("Turn this off when in-game so you don't drop fps :p");
				WToggle("Bypass sv_pure", &Vars::Misc::BypassPure.Value); HelpMarker("Allows you to load any custom files, even if disallowed by the sv_pure setting");
				WToggle("Medal flip", &Vars::Misc::MedalFlip.Value); HelpMarker("Medal go spinny spinny weeeeeee");
				WCombo("Precipitation", &Vars::Visuals::Rain.Value, { "Off", "Rain", "Snow" });

				SectionTitle("Custom fog");
				WToggle("Disable fog", &Vars::Visuals::Fog::DisableFog.Value);
				WToggle("Custom fog", &Vars::Visuals::Fog::CustomFog.Value);
				WSlider("Fog density", &Vars::Visuals::Fog::FogDensity.Value, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				ColorPickerL("Fog colour", Vars::Visuals::Fog::FogColor);
				WSlider("Fog start", &Vars::Visuals::Fog::FogStart.Value, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None);
				WSlider("Fog end", &Vars::Visuals::Fog::FogEnd.Value, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None);
				WSlider("Skybox fog density", &Vars::Visuals::Fog::FogDensitySkybox.Value, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Skybox fog requires r_3dsky 1");
				ColorPickerL("Skybox fog colour", Vars::Visuals::Fog::FogColorSkybox);
				WSlider("Skybox fog start", &Vars::Visuals::Fog::FogStartSkybox.Value, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None);
				WSlider("Skybox fog end", &Vars::Visuals::Fog::FogEndSkybox.Value, -10000.f, 10000.f, "%f", ImGuiSliderFlags_None);

				SectionTitle("Thirdperson");
				WToggle("Thirdperson", &Vars::Visuals::ThirdPerson.Value); HelpMarker("Will move your camera to be in a thirdperson view");
				InputKeybind("Thirdperson key", Vars::Visuals::ThirdPersonKey); HelpMarker("What key to toggle thirdperson, press ESC if no bind is desired");
				WToggle("Show real angles###tpRealAngles", &Vars::Visuals::ThirdPersonSilentAngles.Value); HelpMarker("Will show your real angles on thirdperson (not what others see)");
				WToggle("Instant yaw###tpInstantYaw", &Vars::Visuals::ThirdPersonInstantYaw.Value); HelpMarker("Will set your yaw instantly in thirdperson, showing your actual angle, instead of what others see");
				WToggle("Show server hitboxes (localhost only)###tpShowServer", &Vars::Visuals::ThirdPersonServerHitbox.Value); HelpMarker("Will show the server angles in thirdperson");

				WToggle("Thirdperson offsets", &Vars::Visuals::ThirdpersonOffset.Value); HelpMarker("These will mess you up if you use a small FoV");
				WSlider("Thirdperson distance", &Vars::Visuals::ThirdpersonDist.Value, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
				WSlider("Thirdperson right", &Vars::Visuals::ThirdpersonRight.Value, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
				WSlider("Thirdperson up", &Vars::Visuals::ThirdpersonUp.Value, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
				WToggle("Thirdperson crosshair", &Vars::Visuals::ThirdpersonCrosshair.Value);
				WToggle("Offset with arrow keys", &Vars::Visuals::ThirdpersonOffsetWithArrows.Value);
				InputKeybind("Move offset key", Vars::Visuals::ThirdpersonArrowOffsetKey, false);

				SectionTitle("Out of FOV arrows");
				WToggle("Active###fovar", &Vars::Visuals::OutOfFOVArrows.Value); HelpMarker("Will draw arrows to players who are outside of the range of your FoV");
				WToggle("Outline arrows###OutlinedArrows", &Vars::Visuals::OutOfFOVArrowsOutline.Value); HelpMarker("16 missed calls");
				WSlider("Arrow length", &Vars::Visuals::ArrowLength.Value, 5.f, 50.f, "%.2f"); HelpMarker("How long the arrows are");
				WSlider("Arrow angle", &Vars::Visuals::ArrowAngle.Value, 5.f, 180.f, "%.2f"); HelpMarker("The angle of the arrow");
				WSlider("Distance from center", &Vars::Visuals::FovArrowsDist.Value, 0.01f, 0.2f, "%.3f"); HelpMarker("How far from the center of the screen the arrows will draw");
				WSlider("Max distance", &Vars::Visuals::MaxDist.Value, 0.f, 4000.f, "%.2f"); HelpMarker("How far until the arrows will not show");
				WSlider("Min distance", &Vars::Visuals::MinDist.Value, 0.f, 1000.f, "%.2f"); HelpMarker("How close until the arrows will be fully opaque");

				SectionTitle("Spy Warning");
				WToggle("Active###spywarn", &Vars::Visuals::SpyWarning.Value); HelpMarker("Will alert you when spies with their knife out may attempt to backstab you");
				WToggle("Voice command###spywarn1", &Vars::Visuals::SpyWarningAnnounce.Value); HelpMarker("Will make your character say \"Spy!\" when a spy is detected");
				WToggle("Visible only###spywarn2", &Vars::Visuals::SpyWarningVisibleOnly.Value); HelpMarker("Will only alert you to visible spies");
				WToggle("Ignore friends###spywarn3", &Vars::Visuals::SpyWarningIgnoreFriends.Value); HelpMarker("Will ignore spies who are on your friends list");
				WCombo("Warning style", &Vars::Visuals::SpyWarningStyle.Value, { "Arrow", "Flash" }); HelpMarker("Choose the style of the spy indicator");
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
				WToggle("Enable Radar###RadarActive", &Vars::Radar::Main::Active.Value); HelpMarker("Will show nearby things relative to your player");
				WSlider("Range###RadarRange", &Vars::Radar::Main::Range.Value, 50, 3000, "%d"); HelpMarker("The range of the radar");
				WSlider("Background alpha###RadarBGA", &Vars::Radar::Main::BackAlpha.Value, 0, 255, "%d"); HelpMarker("The background alpha of the radar");
				WSlider("Line alpha###RadarLineA", &Vars::Radar::Main::LineAlpha.Value, 0, 255, "%d"); HelpMarker("The line alpha of the radar");

				SectionTitle("Players");
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
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsRadarCol3"))
			{
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
			WToggle("Doubletap", &Vars::Misc::CL_Move::Enabled.Value); HelpMarker("Shifts ticks when shooting for a rapid-fire effect");
			ColorPickerL("DT bar outline colour", Colors::DtOutline);
			InputKeybind("Recharge key", Vars::Misc::CL_Move::RechargeKey); HelpMarker("Recharges ticks for shifting");
			InputKeybind("Teleport key", Vars::Misc::CL_Move::TeleportKey); HelpMarker("Shifts ticks to warp");
			if (Vars::Misc::CL_Move::DTMode.Value == 0 || Vars::Misc::CL_Move::DTMode.Value == 2)
			{
				InputKeybind("Doubletap key", Vars::Misc::CL_Move::DoubletapKey); HelpMarker("Only doubletap when the key is pressed. Leave as (None) for always active.");
			}

			WCombo("Teleport Mode", &Vars::Misc::CL_Move::TeleportMode.Value, { "Plain", "Smooth" }); HelpMarker("How the teleport should be done");
			MultiCombo({ "Recharge While Dead", "Auto Recharge", "Wait for DT", "Anti-warp", "Avoid airborne", "Retain Fakelag", "Stop Recharge Movement", "Safe Tick" }, { &Vars::Misc::CL_Move::RechargeWhileDead.Value, &Vars::Misc::CL_Move::AutoRecharge.Value, &Vars::Misc::CL_Move::WaitForDT.Value, &Vars::Misc::CL_Move::AntiWarp.Value, &Vars::Misc::CL_Move::NotInAir.Value, &Vars::Misc::CL_Move::RetainFakelag.Value, &Vars::Misc::CL_Move::StopMovement.Value, &Vars::Misc::CL_Move::SafeTick.Value }, "Options");
			HelpMarker("Enable various features regarding tickbase exploits");
			WCombo("DT Mode", &Vars::Misc::CL_Move::DTMode.Value, { "On key", "Always", "Disable on key", "Disabled" }); HelpMarker("How should DT behave");
			const int ticksMax = g_ConVars.sv_maxusrcmdprocessticks->GetInt() - 2;
			WSlider("Ticks to shift", &Vars::Misc::CL_Move::DTTicks.Value, 1, ticksMax ? ticksMax : 22, "%d"); HelpMarker("How many ticks to shift");
			WToggle("SpeedHack", &Vars::Misc::CL_Move::SEnabled.Value); HelpMarker("Speedhack Master Switch");
			if (Vars::Misc::CL_Move::SEnabled.Value)
			{
				WSlider("SpeedHack Factor", &Vars::Misc::CL_Move::SFactor.Value, 1, 66, "%d");
			}
			HelpMarker("High values are not recommended");

			/* Section: Fakelag */
			SectionTitle("Fakelag");
			WToggle("Enable Fakelag", &Vars::Misc::CL_Move::Fakelag.Value);
			WCombo("Fakelag Mode###FLmode", &Vars::Misc::CL_Move::FakelagMode.Value, { "Plain", "Random", "Velocity Based" }); HelpMarker("Controls how fakelag will be controlled.");

			if (Vars::Misc::CL_Move::FakelagMode.Value == 0 || Vars::Misc::CL_Move::FakelagMode.Value == 2)
			{
				WSlider("Fakelag value", &Vars::Misc::CL_Move::FakelagValue.Value, 1, 22, "%d"); HelpMarker("How much lag you should fake(?)");
				if (Vars::Misc::CL_Move::FakelagMode.Value == 0)
				{
					WToggle("Fakelag on key", &Vars::Misc::CL_Move::FakelagOnKey.Value); HelpMarker("Fakelag will only activate when an assigned key is held");
					if (Vars::Misc::CL_Move::FakelagOnKey.Value)
					{
						InputKeybind("Fakelag key", Vars::Misc::CL_Move::FakelagKey); HelpMarker("The key to activate fakelag as long as it's held");
					}
				}
			}
			if (Vars::Misc::CL_Move::FakelagMode.Value == 1)
			{
				WSlider("Random max###flRandMax", &Vars::Misc::CL_Move::FakelagMax.Value, Vars::Misc::CL_Move::FakelagMin.Value + 1, 22, "%d"); HelpMarker("Maximum random fakelag value");
				WSlider("Random min###flRandMin", &Vars::Misc::CL_Move::FakelagMin.Value, 1, Vars::Misc::CL_Move::FakelagMax.Value - 1, "%d"); HelpMarker("Minimum random fakelag value");
			}
		} EndChild();

		/* Column 2 */
		if (TableColumnChild("HvHCol2"))
		{
			/* Section: Anti Aim */
			SectionTitle("Anti Aim");
			WToggle("Enable Anti-aim", &Vars::AntiHack::AntiAim::Active.Value);
			InputKeybind("Anti-aim Key", Vars::AntiHack::AntiAim::ToggleKey); HelpMarker("The key to toggle anti aim");
			WCombo("Pitch", &Vars::AntiHack::AntiAim::Pitch.Value, { "None", "Zero", "Up", "Down", "Fake up", "Fake down", "Random", "Half Up" }); HelpMarker("Which way to look up/down");
			WCombo("Real yaw", &Vars::AntiHack::AntiAim::YawReal.Value, { "None", "Forward", "Left", "Right", "Backwards", "Random", "Spin", "Edge", "On Hurt" }); HelpMarker("Which way to look horizontally");
			WCombo("Fake yaw", &Vars::AntiHack::AntiAim::YawFake.Value, { "None", "Forward", "Left", "Right", "Backwards", "Random", "Spin", "Edge", "On Hurt" }); HelpMarker("Which way to appear to look horizontally");
			if (Vars::AntiHack::AntiAim::YawFake.Value == 6 || Vars::AntiHack::AntiAim::YawReal.Value == 6)
			{
				WSlider("Spin Speed", &Vars::AntiHack::AntiAim::SpinSpeed.Value, -30.f, 30.f, "%.1f", 0); HelpMarker("You spin me right 'round, baby, right 'round");
			}
			if (Vars::AntiHack::AntiAim::Pitch.Value == 6 || Vars::AntiHack::AntiAim::YawFake.Value == 5 || Vars::AntiHack::AntiAim::YawReal.Value == 5)
			{
				WSlider("Random Interval", &Vars::AntiHack::AntiAim::RandInterval.Value, 0, 100, "%d"); HelpMarker("How often the random Anti-Aim should update");
			}
			WToggle("Resolver", &Vars::AntiHack::Resolver::Resolver.Value); HelpMarker("Enables Anti-aim resolver in the playerlist");
			MultiCombo({ "AntiOverlap", "Jitter Legs", "HidePitchOnShot", "Anti-Backstab"}, { &Vars::AntiHack::AntiAim::AntiOverlap.Value, &Vars::AntiHack::AntiAim::LegJitter.Value, &Vars::AntiHack::AntiAim::InvalidShootPitch.Value, &Vars::AntiHack::AntiAim::AntiBackstab.Value }, "Misc.");

			/* Section: Auto Peek */
			SectionTitle("Auto Peek");
			InputKeybind("Autopeek Key", Vars::Misc::CL_Move::AutoPeekKey); HelpMarker("Hold this key while peeking and use A/D to set the peek direction");
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
			SectionTitle("Automation");
			WToggle("No push", &Vars::Misc::NoPush.Value); HelpMarker("Will make teammates unable to push you around");
			WCombo("Quick stop", &Vars::Misc::AccurateMovement.Value, { "Off", "Legacy", "Instant" }); HelpMarker("Will stop you from sliding once you stop pressing movement buttons");
			WToggle("Duck Jump", &Vars::Misc::DuckJump.Value); HelpMarker("Will duck when bhopping");
			WToggle("Bunnyhop", &Vars::Misc::AutoJump.Value); HelpMarker("Will jump as soon as you touch the ground again, keeping speed between jumps");
			WCombo("Autostrafe", &Vars::Misc::AutoStrafe.Value, { "Off", "Legit", "Directional" }); HelpMarker("Will strafe for you in air automatically so that you gain speed");
			WToggle("Edge jump", &Vars::Misc::EdgeJump.Value); HelpMarker("Will jump at the very end of whatever platform you're on, allowing you to perfectly make longer jumps.");
			if (Vars::Misc::EdgeJump.Value)
			{
				InputKeybind("Edge jump key", Vars::Misc::EdgeJumpKey, true);  HelpMarker("Edge jump bind, leave as None for always on");
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
			
			WToggle("Fast Accel", &Vars::Misc::FastAccel.Value); HelpMarker("Makes you accelerate to full speed faster.");
			WToggle("Crouch Speed", &Vars::Misc::CrouchSpeed.Value); HelpMarker("Allows you to move at full speed while crouched.");
			if (&Vars::Misc::CrouchSpeed.Value || &Vars::Misc::FastAccel.Value) {
				WToggle("Hide Real Angle", &Vars::Misc::FakeAccelAngle.Value); HelpMarker("Tries to stop your angle from updating while using crouch speed / fast accel (janky).");
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

			MultiFlags({ "Amsterdam", "Atlanta", "Mumbai", "Dubai", "Moses Lake", "Washington", "Frankfurt", "Tokyo (GNRT)", "Sao Paulo", "Hong Kong", "Virginia", "Johannesburg", "Los Angeles", "London", "Lima", "Luxembourg", "Chennai", "Madrid", "Manilla", "Oklahoma City", "Chicago", "Paris", "Santiago", "Seattle", "Singapore", "Stockholm", "Sydney", "Tokyo", "Vienna", "Warsaw" },
					   { DC_AMS,      DC_ATL,    DC_BOM,   DC_DXB,  DC_EAT,		 DC_MWH,	   DC_FRA,		DC_GNRT,		DC_GRU,		 DC_HKG,	  DC_IAD,	  DC_JNB,		  DC_LAX,		 DC_LHR,   DC_LIM, DC_LUX,		 DC_MAA,	DC_MAD,	  DC_MAN,    DC_OKC,		  DC_ORD,	 DC_PAR,  DC_SCL,     DC_SEA,	 DC_SGP,	  DC_STO,	   DC_SYD,   DC_TYO,  DC_VIE,	DC_WAW },
					   &Vars::Misc::RegionsAllowed.Value,
					   "Regions"
			);
			WToggle("Instant accept match", &Vars::Misc::InstantAccept.Value); HelpMarker("Will skip the 10 second delay before joining a match");
			WToggle("Auto queue for casual", &Vars::Misc::AutoCasualQueue.Value); HelpMarker("Automatically starts queueuing for casual on the main menu");
			WToggle("Auto VAC Join", &Vars::Misc::AntiVAC.Value); HelpMarker("Tries to join a server while having a VAC ban");

			SectionTitle("Sound");
			MultiFlags({ "Footsteps", "Noisemaker" }, { 1 << 0, 1 << 1 }, &Vars::Misc::SoundBlock.Value, "Block Sounds###SoundRemovals");

			

		} EndChild();

		/* Column 2 */
		if (TableColumnChild("MiscCol2"))
		{
			SectionTitle("Chat");

			WToggle("Chat Flags", &Vars::Misc::ChatFlags.Value);
			//WToggle("Chat Censor", &Vars::Misc::ChatCensor.Value); HelpMarker("Clears the chat when someone accuses your");
			//WToggle("Allow Newlines", &Vars::Misc::ChatNL.Value); HelpMarker("Allows you to use \\n in the chat");
			WCombo("Chat spam", &Vars::Misc::ChatSpam.Value, { "Off", "Fedoraware", "Lmaobox", "Cathook" });
			WCombo("Mediaval Mode", &Vars::Misc::MedievalChat.Value, { "Default", "Never", "Always" }); HelpMarker("By the Immeasurable Nether Regions of Enlightened Dionysus, this enableth medieval chattery. Anon!");

			SectionTitle("Exploits");
			WToggle("Anti Autobalance", &Vars::Misc::AntiAutobal.Value); HelpMarker("Prevents auto balance by reconnecting to the server");
			WToggle("sv_cheats Bypass", &Vars::Misc::CheatsBypass.Value); HelpMarker("Allows you to use some sv_cheats commands (clientside)");
			WToggle("Join spam", &Vars::Misc::JoinSpam.Value); HelpMarker("Spams join/disconnect messages in the chat");
			WToggle("Noisemaker Spam", &Vars::Misc::NoisemakerSpam.Value); HelpMarker("Spams the noisemaker without reducing it's charges");
			//WToggle("Cat identify", &Vars::Misc::BeCat.m_Var); HelpMarker("Will mark you as a cathook instance to other cathook instances (basically catbots)");
			WToggle("Ping reducer", &Vars::Misc::PingReducer.Value); HelpMarker("Reduces your ping on the scoreboard");
			if (Vars::Misc::PingReducer.Value)
			{
				WSlider("Target ping", &Vars::Misc::PingTarget.Value, 0, 200); HelpMarker("Target ping that should be reached");
			}

			SectionTitle("Party Networking");
			WToggle("Enable###PartyNetEnable", &Vars::Misc::PartyNetworking.Value); HelpMarker("Enables party networking between Fedoraware users");
			WToggle("Party crasher###PartyNetCrash", &Vars::Misc::PartyCrasher.Value); HelpMarker("Annoy your friends by crashing their game");
			InputKeybind("Party marker", Vars::Misc::PartyMarker, true);  HelpMarker("Sends a marker to other Fedoraware users in your party");
			WToggle("Party ESP###PartyNetESP", &Vars::Misc::PartyESP.Value); HelpMarker("Sends player locations to your party members");

			SectionTitle("Followbot");
			WToggle("Enable Followbot###FollowbotEnable", &Vars::Misc::Followbot::Enabled.Value); HelpMarker("Follows a player around.");
			WToggle("Friends only###FollowbotFriends", &Vars::Misc::Followbot::FriendsOnly.Value); HelpMarker("Only follow friends");
			WSlider("Follow Distance###FollowbotDistance", &Vars::Misc::Followbot::Distance.Value, 50.f, 400.f, "%.0f"); HelpMarker("How close we should follow the target");
		} EndChild();

		/* Column 3 */
		if (TableColumnChild("MiscCol3"))
		{
			SectionTitle("Discord RPC");
			WToggle("Discord RPC", &Vars::Misc::Discord::EnableRPC.Value); HelpMarker("Enable Discord Rich Presence");
			WToggle("Include map", &Vars::Misc::Discord::IncludeMap.Value); HelpMarker("Should Discord Rich Presence contain current map name?");
			WToggle("Include class", &Vars::Misc::Discord::IncludeClass.Value); HelpMarker("Should Discord Rich Presence contain current class?");
			WToggle("Include timestamp", &Vars::Misc::Discord::IncludeTimestamp.Value); HelpMarker("Should time since you started playing TF2 be included?");
			WCombo("Image Options", &Vars::Misc::Discord::WhatImagesShouldBeUsed.Value, { "Big fedora + Small TF2", "Big TF2 + Small fedora" });

			SectionTitle("Steam RPC");
			WToggle("Steam RPC", &Vars::Misc::Steam::EnableRPC.Value); HelpMarker("Enable Steam Rich Presence"); HelpMarker("Enable Steam Rich Presence");
			WCombo("Match group", &Vars::Misc::Steam::MatchGroup.Value, { "Special Event", "MvM Mann Up", "Competitive", "Casual", "MvM Boot Camp" }); HelpMarker("Which match group should be used?");
			WToggle("Override in menu", &Vars::Misc::Steam::OverrideMenu.Value); HelpMarker("Override match group to \"Main Menu\" when in main menu");
			WCombo("Map text", &Vars::Misc::Steam::MapText.Value, { "Custom", "Fedoraware", "Figoraware", "Meowhook.club", "Rathook.cc", "Nitro.tf" }); HelpMarker("Which map text should be used?");
			if (Vars::Misc::Steam::MapText.Value == 0)
			{
				WInputText("Custom map text", &Vars::Misc::Steam::CustomText.Value); HelpMarker(R"(For when "Custom" is selcted in "Map text". Sets custom map text.)");
			}
			WInputInt("Group size", &Vars::Misc::Steam::GroupSize.Value); HelpMarker("Sets party size");

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
			if (Button("Pong", ImVec2(btnWidth, 20)))
				F::Pong.IsOpen = !F::Pong.IsOpen;
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
				F::Misc.UnlockAchievements();
			}
			if (Button("Lock all achievements", ImVec2(btnWidth, 20)))
			{
				F::Misc.LockAchievements();
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
		Checkbox("Show DVD bounce", &Vars::Menu::ShowDVD.Value);

		SetNextItemWidth(100);
		InputKeybind("Extra Menu key", Vars::Menu::MenuKey, true, true);

		Dummy({ 0, 5 });

		if (Button("Open configs folder", ImVec2(200, 0)))
		{
			ShellExecuteA(NULL, NULL, g_CFG.GetConfigPath().c_str(), NULL, NULL, SW_SHOWNORMAL);
		}

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
		const auto& pLocal = g_EntityCache.GetLocal();

		Checkbox("Show Debug info", &Vars::Debug::DebugInfo.Value);
		Checkbox("Allow secure servers", I::AllowSecureServers);

		bool* m_bPendingPingRefresh = reinterpret_cast<bool*>(I::TFGCClientSystem + 828);
		Checkbox("Pending Ping Refresh", m_bPendingPingRefresh);

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
			Checkbox("Debug Bool", &Vars::Debug::DebugBool.Value);
		}

		End();
	}

	PopStyleVar(2);
	#endif
}

/* Window for the camera feature */
void CMenu::DrawCameraWindow()
{
	if (I::Engine->IsInGame() && Vars::Visuals::CameraMode.Value != 0)
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

void CMenu::DrawKeybinds()
{
	if (!Vars::Menu::ShowKeybinds) { return; }

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.35f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.f, 0.f, 0.f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.f, 0.f, 0.f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4.f, 4.f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	ImGui::SetNextWindowSize({ 200.f, 0.f });

	if (ImGui::Begin("Keybinds", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		auto drawOption = [](const char* name, bool active) {
			ImGui::Text(name);
			ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - ImGui::CalcTextSize(active ? "[On]" : "[Off]").x);
			ImGui::Text(active ? "[On]" : "[Off]");
		};

		auto isActive = [](bool active, bool needsKey, int key) {
			return active && (!needsKey || GetAsyncKeyState(key) & 0x8000);
		};

		drawOption("Aimbot", isActive(Vars::Aimbot::Global::Active.Value, Vars::Aimbot::Global::AimKey.Value, Vars::Aimbot::Global::AimKey.Value));
		drawOption("Auto Shoot", Vars::Aimbot::Global::AutoShoot.Value);
		drawOption("Double Tap", isActive(Vars::Misc::CL_Move::DTMode.Value != 3, Vars::Misc::CL_Move::DTMode.Value == 0, Vars::Misc::CL_Move::DoubletapKey.Value));
		drawOption("Anti Aim", Vars::AntiHack::AntiAim::Active.Value);
		drawOption("Fakelag", isActive(Vars::Misc::CL_Move::Fakelag.Value, Vars::Misc::CL_Move::FakelagOnKey.Value, Vars::Misc::CL_Move::FakelagKey.Value));
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
	std::call_once(initFlag, [&] {
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
		I::Surface->SetCursorAlwaysVisible(F::Menu.IsOpen);
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

		SettingsWindow();
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
		Accent = ImGui::ColorToVec(Vars::Menu::Colors::MenuAccent);
		AccentDark = ImColor(Accent.Value.x * 0.8f, Accent.Value.y * 0.8f, Accent.Value.z * 0.8f, Accent.Value.w);

		auto& style = ImGui::GetStyle();
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f); // Center window title
		style.WindowMinSize = ImVec2(100, 100);
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
		//TitleGradient.ClearMarks();
		/*TitleGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		TitleGradient.AddMark(0.3f, ImColor(0, 0, 0, 0));
		TitleGradient.AddMark(0.5f, Accent);
		TitleGradient.AddMark(0.7f, ImColor(0, 0, 0, 0));
		TitleGradient.AddMark(1.f, ImColor(0, 0, 0, 0));*/
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
	F::MaterialEditor.Init();
}
