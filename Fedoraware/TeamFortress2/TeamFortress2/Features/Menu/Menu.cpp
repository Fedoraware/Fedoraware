#include "Menu.h"
#include "../Vars.h"
#include "../Camera/CameraWindow.h"
#include "../AttributeChanger/AttributeChanger.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_color_gradient.h"
#include "Components.hpp"

#define WSLIDER(label, var, min, max, format, flagspower) \
ImGui::PushItemWidth(150); \
ImGui::SliderFloat(label, var, min, max, format, flagspower); \
ImGui::PopItemWidth()

#define WISLIDER(label, var, min, max, format, flagspower) \
ImGui::PushItemWidth(150); \
ImGui::SliderInt(label, var, min, max, format, flagspower); \
ImGui::PopItemWidth()

#define WCOMBO(label, var, items) \
ImGui::PushItemWidth(150); \
ImGui::Combo(label, var, items, IM_ARRAYSIZE(items)); \
ImGui::PopItemWidth()

#define TOGGLE(label, v) ImGui::Checkbox(label, v);

constexpr int MENU_KEY = VK_INSERT;
static ImGradient titleGradient;
static ImGradient mainGradient;

// Fonts
ImFont* SegoeLight = nullptr;	// 16px
ImFont* Segoe = nullptr;		// 16px
ImFont* SegoeBold = nullptr;	// 16px

ImFont* SectionFont = nullptr;	// 18px
ImFont* TabFont = nullptr;		// 22px
ImFont* TitleFont = nullptr;	// 26px

int unu1 = 0;
int unu2 = 0;

#pragma region Components
void SectionTitle(const char* title, float yOffset = 6)
{
	ImGui::Dummy({ 0, yOffset });
	ImGui::PushFont(SectionFont);
	const ImVec2 titleSize = ImGui::CalcTextSize(title);
	ImGui::SameLine((ImGui::GetColumnWidth()) / 2 - (titleSize.x / 2));
	ImGui::Text(title);
	ImGui::PopFont();

	const auto widgetPos = ImGui::GetCursorScreenPos();
	ImGui::GradientRect(&mainGradient, { widgetPos.x, widgetPos.y - 2 }, ImGui::GetColumnWidth(), 3);
}
#pragma endregion

/* The main menu */
void CMenu::DrawMenu()
{
	ImGui::GetStyle().WindowMinSize = ImVec2(800, 500);

	ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_Once);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	if (ImGui::Begin("Fedoraware", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
	{
		const auto drawList = ImGui::GetWindowDrawList();
		const auto windowSize = ImGui::GetWindowSize();
		const auto windowPos = ImGui::GetWindowPos();

		// Gradient line
		ImGui::GradientRect(&titleGradient, { windowPos.x, windowPos.y }, windowSize.x, 3.f);
		ImGui::Dummy({ 0, 2 });

		// Title Text
		{
			ImGui::PushFont(TitleFont);
			const auto titleWidth = ImGui::CalcTextSize("Fedoraware").x;
			drawList->AddText(TitleFont, TitleFont->FontSize, { windowPos.x + (windowSize.x / 2) - (titleWidth / 2), windowPos.y }, Accent, "Fedoraware");
			ImGui::PopFont();
		}

		// Tabbar
		ImGui::SetCursorPos({ 0, TitleHeight });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, BackgroundLight.Value);
		if (TabHeight > 5.f)
		{
			if (ImGui::BeginChild("Tabbar", { windowSize.x + 5, TabHeight }))
			{
				DrawTabbar();
			}
			ImGui::EndChild();
		}
		ImGui::PopStyleColor();

		// Main content
		ImGui::SetCursorPos({ 0, TitleHeight + TabHeight });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 15.f, 10.f });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, BackgroundDark.Value);
		if (ImGui::BeginChild("Content", { windowSize.x, windowSize.y - (TitleHeight + TabHeight) }, false, ImGuiWindowFlags_AlwaysUseWindowPadding))
		{
			ImGui::PushFont(Segoe);
			switch (CurrentTab)
			{
			case MenuTab::Aimbot: { MenuAimbot(); break; }
			case MenuTab::Trigger: { MenuTrigger(); break; }
			case MenuTab::Visuals: { MenuVisuals(); break; }
			case MenuTab::HvH: { MenuHvH(); break; }
			case MenuTab::Misc: { MenuMisc(); break; }
			case MenuTab::Configs: { MenuConfigs(); break; }
			}

			ImGui::PopFont();
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		// End
		ImGui::End();
	}
	ImGui::PopStyleVar();
}

void CMenu::DrawTabbar()
{
	ImGui::PushFont(TabFont);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
	if (ImGui::BeginTable("TabbarTable", 4))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, BackgroundLight.Value);
		ImGui::PushStyleColor(ImGuiCol_Text, TextLight.Value);
		if (ImGui::TabButton("Aimbot", CurrentTab == MenuTab::Aimbot))
		{
			CurrentTab = MenuTab::Aimbot;
		}

		if (ImGui::TabButton("Trigger", CurrentTab == MenuTab::Trigger))
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
	ImGui::PopStyleVar(2);
	ImGui::PopFont();
}

/* Tab: Aimbot */
void CMenu::MenuAimbot()
{
	using namespace ImGui;
	if (BeginTable("AimbotTable", 3))
	{
		/* Column 1 */
		TableNextColumn();

		// Global Aimbot
		SectionTitle("Global");
		TOGGLE("Aimbot", &Vars::Aimbot::Global::Active.m_Var); HelpMarker("Aimbot master switch");
		ColorPickerL("Aim Target", Colors::Target);
		InputKeybind("Aimbot key", Vars::Aimbot::Global::AimKey); HelpMarker("The key to enable aimbot");
		WSLIDER("Aimbot FoV####AimbotFoV", &Vars::Aimbot::Global::AimFOV.m_Var, 0.f, 180.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
		ColorPickerL("Aimbot FOV circle", Colors::FOVCircle);
		TOGGLE("Autoshoot###AimbotAutoshoot", &Vars::Aimbot::Global::AutoShoot.m_Var); HelpMarker("Automatically shoot when a target is found");
		MultiCombo({ "Players", "Buildings" }, { &Vars::Aimbot::Global::AimPlayers.m_Var, &Vars::Aimbot::Global::AimBuildings.m_Var }, "Choose which targets the Aimbot should aim at", "Aim targets");
		MultiCombo({ "Invulnerable", "Cloaked", "Friends", "Taunting" }, { &Vars::Aimbot::Global::IgnoreInvlunerable.m_Var, &Vars::Aimbot::Global::IgnoreCloaked.m_Var, &Vars::Aimbot::Global::IgnoreFriends.m_Var, &Vars::Aimbot::Global::IgnoreTaunting.m_Var }, "Choose which targets should be ignored", "Ignored targets###HitscanIgnoredTargets");
		ColorPickerL("Invulnerable colour", Colors::Invuln);

		// Crithack
		SectionTitle("Crithack", 12);
		TOGGLE("Crit hack", &Vars::CritHack::Active.m_Var); HelpMarker("Enables the crit hack (BETA)");
		MultiCombo({ "Indicators", "Avoid Random" }, { &Vars::CritHack::indicators.m_Var, &Vars::CritHack::avoidrandom.m_Var }, "Misc options for crithack", "Misc###CrithackMiscOptions");
		InputKeybind("Crit key", Vars::CritHack::CritKey); HelpMarker("Will try to force crits when the key is held");

		// Backtrack
		SectionTitle("Backtrack", 12);
		TOGGLE("Active", &Vars::Backtrack::Enabled.m_Var); HelpMarker("If you shoot at the backtrack manually it will attempt to hit it");
		TOGGLE("Aimbot aims last tick", &Vars::Backtrack::Aim.m_Var); HelpMarker("Aimbot aims at the last tick if visible");

		/* Column 2 */
		TableNextColumn();

		// Hitscan options
		SectionTitle("Hitscan");
		static const char* sortMethodArr[]{ "FOV", "Distance", }; WCOMBO("Sort method###HitscanSortMethod", &Vars::Aimbot::Hitscan::SortMethod.m_Var, sortMethodArr, IM_ARRAYSIZE(sortMethodArr)); HelpMarker("Which method the aimbot uses to decide which target to aim at");
		static const char* aimMethodArr[]{ "Plain", "Smooth", "Silent" }; WCOMBO("Aim method###HitscanAimMethod", &Vars::Aimbot::Hitscan::AimMethod.m_Var, aimMethodArr); HelpMarker("Which method the aimbot uses to aim at the target");
		static const char* aimHitboxArr[]{ "Head", "Body", "Auto" }; WCOMBO("Hitbox###HitscanHitbox", &Vars::Aimbot::Hitscan::AimHitbox.m_Var, aimHitboxArr); HelpMarker("Which hitbox the aimbot will target");
		static const char* tapfireMethodArr[]{ "Off", "Distance", "Always" }; WCOMBO("Tapfire###HitscanTapfire", &Vars::Aimbot::Hitscan::TapFire.m_Var, tapfireMethodArr); HelpMarker("How/If the aimbot chooses to tapfire enemies.");
		WISLIDER("Smooth factor###HitscanSmoothing", &Vars::Aimbot::Hitscan::SmoothingAmount.m_Var, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Changes how smooth the aimbot will aim at the target");
		MultiCombo({ "Body", "Head", "Buildings" }, { &Vars::Aimbot::Hitscan::ScanHitboxes.m_Var, &Vars::Aimbot::Hitscan::ScanHead.m_Var, &Vars::Aimbot::Hitscan::ScanBuildings.m_Var }, "Choose what the aimbot should multipoint", "Multipoint");
		TOGGLE("Wait for headshot", &Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var); HelpMarker("The aimbot will wait until it can headshot (if applicable)");
		TOGGLE("Wait for charge", &Vars::Aimbot::Hitscan::WaitForCharge.m_Var); HelpMarker("The aimbot will wait until the rifle has charged long enough to kill in one shot");
		TOGGLE("Smooth if spectated", &Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var); HelpMarker("The aimbot will switch to the smooth method if being spectated");
		TOGGLE("Scoped only", &Vars::Aimbot::Hitscan::ScopedOnly.m_Var); HelpMarker("The aimbot will only shoot if scoped");
		TOGGLE("Auto scope", &Vars::Aimbot::Hitscan::AutoScope.m_Var); HelpMarker("The aimbot will automatically scope in to shoot");
		TOGGLE("Auto rev minigun", &Vars::Aimbot::Hitscan::AutoRev.m_Var); HelpMarker("Will rev heavy's minigun regardless of if aimbot has a target");
		TOGGLE("Bodyaim if lethal", &Vars::Aimbot::Global::BAimLethal.m_Var); HelpMarker("The aimbot will aim for body when damage is lethal to it");

		/* Column 3 */
		TableNextColumn();

		SectionTitle("Projectile");
		TOGGLE("Performance mode", &Vars::Aimbot::Projectile::PerformanceMode.m_Var); HelpMarker("Only target enemy closest to the crosshair");
		TOGGLE("Movement simulation", &Vars::Aimbot::Projectile::MovementSimulation.m_Var); HelpMarker("Uses game functions to predict where the player will be");
		ColorPickerL("Prediction Line Color", Vars::Aimbot::Projectile::PredictionColor);
		if (Vars::Aimbot::Projectile::MovementSimulation.m_Var)
		{
			WSLIDER("Prediction Time", &Vars::Aimbot::Projectile::predTime.m_Var, 0.1f, 10.f, "%.1f", ImGuiSliderFlags_None);
		}
		{
			static const char* sortMethodArr[]{ "FOV", "Distance", }; WCOMBO("Sort method###ProjectileSortMethod", &Vars::Aimbot::Projectile::SortMethod.m_Var, sortMethodArr);
			static const char* aimMethodArr[]{ "Plain", "Silent" }; WCOMBO("Aim method###ProjectileAimMethod", &Vars::Aimbot::Projectile::AimMethod.m_Var, aimMethodArr);
			static const char* aimHitboxArr[]{ "Body", "Feet", "Auto" }; WCOMBO("Hitbox###ProjectileHitbox", &Vars::Aimbot::Projectile::AimPosition.m_Var, aimHitboxArr);
		}
		TOGGLE("Feet aim on ground (Demoman)", &Vars::Aimbot::Projectile::FeetAimIfOnGround.m_Var); HelpMarker("Will aim at feet if target is on the ground");
		TOGGLE("Custom huntsman Z-Adjust", &Vars::Aimbot::Projectile::ManualZAdjust.m_Var); HelpMarker("Enables the ability to adjust the Z-Position for huntsman");
		if (Vars::Aimbot::Projectile::ManualZAdjust.m_Var)
		{
			WSLIDER("Z-Value###ZAdjustValue", &Vars::Aimbot::Projectile::ZAdjustAmount.m_Var, 0.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Manual Z-Adjust for projectiles");
		}

		SectionTitle("Melee", 12);
		{
			static const char* sortMethodArr[]{ "FOV", "Distance", }; WCOMBO("Sort method###MeleeSortMethod", &Vars::Aimbot::Melee::SortMethod.m_Var, sortMethodArr); HelpMarker("Which method the aimbot uses to decide which target to aim at");
			static const char* aimMethodArr[]{ "Plain", "Smooth", "Silent" }; WCOMBO("Aim method###MeleeAimMethod", &Vars::Aimbot::Melee::AimMethod.m_Var, aimMethodArr); HelpMarker("Which method the aimbot uses to aim at the target");
		}
		WISLIDER("Smooth factor###MeleeSmoothing", &Vars::Aimbot::Melee::SmoothingAmount.m_Var, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How smooth the aimbot should be");
		TOGGLE("Range check", &Vars::Aimbot::Melee::RangeCheck.m_Var); HelpMarker("Only aim at target if within melee range");
		TOGGLE("Swing prediction", &Vars::Aimbot::Melee::PredictSwing.m_Var); HelpMarker("Aimbot will attack preemptively, predicting you will be in range of the target");
		TOGGLE("Whip teammates", &Vars::Aimbot::Melee::WhipTeam.m_Var); HelpMarker("Aimbot will target teammates if holding the Disciplinary Action");

		EndTable();
	}
}

/* Tab: Trigger */
void CMenu::MenuTrigger()
{
	using namespace ImGui;
	if (BeginTable("TriggerTable", 3))
	{
		TableNextColumn();

		// Global Triggerbot
		SectionTitle("Global");
		TOGGLE("Triggerbot", &Vars::Triggerbot::Global::Active.m_Var); HelpMarker("Global triggerbot master switch");
		InputKeybind("Trigger key", Vars::Triggerbot::Global::TriggerKey); HelpMarker("The key which activates the triggerbot");
		MultiCombo({ "Invulnerable", "Cloaked", "Friends" }, { &Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var, &Vars::Triggerbot::Global::IgnoreCloaked.m_Var, &Vars::Triggerbot::Global::IgnoreFriends.m_Var }, "Choose which targets should be ignored", "Ignored targets###TriggerIgnoredTargets");

		// Autoshoot
		SectionTitle("Autoshoot");
		TOGGLE("Autoshoot###AutoshootTrigger", &Vars::Triggerbot::Shoot::Active.m_Var); HelpMarker("Shoots if mouse is over a target");
		MultiCombo({ "Players", "Buildings" }, { &Vars::Triggerbot::Shoot::TriggerPlayers.m_Var, &Vars::Triggerbot::Shoot::TriggerBuildings.m_Var }, "Choose which target the triggerbot should shoot at", "Trigger targets");
		TOGGLE("Head only###TriggerHeadOnly", &Vars::Triggerbot::Shoot::HeadOnly.m_Var); HelpMarker("Auto shoot will only shoot if you are aiming at the head");
		TOGGLE("Wait for charge###TriggerbotWaitForCharge", &Vars::Triggerbot::Shoot::WaitForCharge.m_Var); HelpMarker("Auto shoot will only shoot if the sniper is charged enough to kill in one hit / is fully charged");
		WSLIDER("Head scale###TriggerHeadScale", &Vars::Triggerbot::Shoot::HeadScale.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The scale at which the auto shoot will try to shoot the targets head");

		EndTable();
	}
}

/* Tab: Visuals */
void CMenu::MenuVisuals()
{
	using namespace ImGui;

	switch(CurrentVisualsTab)
	{
		// Visuals: Players
		case VisualsTab::Players:
		{
			if (BeginTable("VisualsPlayersTable", 3))
			{
				TableNextColumn();

				// ESP
				SectionTitle("ESP Main");
				TOGGLE("ESP###EnableESP", &Vars::ESP::Main::Active.m_Var); HelpMarker("Global ESP master switch");
				TOGGLE("Outlined health bars", &Vars::ESP::Main::Outlinedbar.m_Var); HelpMarker("Will outline the health bars");
				TOGGLE("Relative colours", &Vars::ESP::Main::EnableTeamEnemyColors.m_Var); HelpMarker("Chooses colors relative to your team (team/enemy)");
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

				// Player ESP
				SectionTitle("Player ESP");
				TOGGLE("Player ESP###EnablePlayerESP", &Vars::ESP::Players::Active.m_Var); HelpMarker("Will draw useful information/indicators on players");
				TOGGLE("Name ESP###PlayerNameESP", &Vars::ESP::Players::Name.m_Var); HelpMarker("Will draw the players name");
				TOGGLE("Custom Name Color", &Vars::ESP::Players::NameC.m_Var); HelpMarker("Custom color for name esp");
				if (Vars::ESP::Players::NameC.m_Var)
				{
					ColorPickerL("Name ESP Color", Vars::ESP::Players::NameColor);
				}
				TOGGLE("Name ESP box###PlayerNameESPBox", &Vars::ESP::Players::NameBox.m_Var); HelpMarker("Will draw a box around players name to make it stand out");
				TOGGLE("Self ESP###SelfESP", &Vars::ESP::Players::ShowLocal.m_Var); HelpMarker("Will draw ESP on local player (thirdperson)");
				ColorPickerL("Local colour", Colors::Local);
				static const char* ignoreTeamArr[]{ "Off", "All", "Only friends" }; WCOMBO("Ignore team###IgnoreTeamESPp", &Vars::ESP::Players::IgnoreTeammates.m_Var, ignoreTeamArr); HelpMarker("Which teammates the ESP will ignore drawing on");
				ColorPickerL("Friend colour", Colors::Friend);
				static const char* ignoreCloakArr[]{ "Off", "All", "Only enemies" }; WCOMBO("Ignore cloaked###IgnoreCloakESPp", &Vars::ESP::Players::IgnoreCloaked.m_Var, ignoreCloakArr); HelpMarker("Which cloaked spies the ESP will ignore drawing on");
				ColorPickerL("Cloaked colour", Colors::Cloak);
				static const char* espUberArr[]{ "Off", "Text", "Bar" }; WCOMBO("Ubercharge###PlayerUber", &Vars::ESP::Players::Uber.m_Var, espUberArr); HelpMarker("Will draw how much ubercharge a medic has");
				ColorPickerL("Ubercharge colour", Colors::UberColor);
				static const char* classArr[]{ "Off", "Icon", "Text", "Both" }; WCOMBO("Class###PlayerIconClass", &Vars::ESP::Players::Class.m_Var, classArr); HelpMarker("Will draw the class the player is");
				TOGGLE("Weapon icons", &Vars::ESP::Players::WeaponIcon.m_Var); HelpMarker("Shows an icon for the weapon that the player has currently equipped");
				ColorPickerL("Invulnerable colour", Colors::WeaponIcon);
				TOGGLE("Health bar###ESPPlayerHealthBar", &Vars::ESP::Players::HealthBar.m_Var); HelpMarker("Will draw a bar visualizing how much health the player has");
				ColorPickerL("Health Bar Top", Colors::HealthBar.startColour);
				ColorPickerL("Health Bar Bottom", Colors::HealthBar.endColour, 1);
				TOGGLE("Health text###ESPPlayerHealthText", &Vars::ESP::Players::Health.m_Var); HelpMarker("Will draw the players health, as well as their max health");
				TOGGLE("Condition", &Vars::ESP::Players::Cond.m_Var); HelpMarker("Will draw what conditions the player is under");
				ColorPickerL("Condition colour", Colors::Cond);
				TOGGLE("GUID", &Vars::ESP::Players::GUID.m_Var); HelpMarker("Show's the players Steam ID");
				TOGGLE("Choked Packets", &Vars::ESP::Players::Choked.m_Var); HelpMarker("Shows how many packets the player has choked");
				ColorPickerL("Choked Bar Top", Colors::ChokedBar.startColour);
				ColorPickerL("Choked Bar Bottom", Colors::ChokedBar.endColour, 1);
				static const char* boxArr[]{ "Off", "Bounding", "Cornered", "3D" }; WCOMBO("Box###PlayerBoxESP", &Vars::ESP::Players::Box.m_Var, boxArr); HelpMarker("What sort of box to draw on players");
				static const char* bonesESP[]{ "Off", "Custom colour", "Health" }; WCOMBO("Skeleton###PlayerSkellington", &Vars::ESP::Players::Bones.m_Var, bonesESP); HelpMarker("Will draw the bone structure of the player");
				ColorPicker("Skellington colour", Colors::Bones);
				TOGGLE("Lines###Playerlines", &Vars::ESP::Players::Lines.m_Var); HelpMarker("Draws lines from the local players position to enemies position");
				TOGGLE("Dlights###PlayerDlights", &Vars::ESP::Players::Dlights.m_Var); HelpMarker("Will make players emit a dynamic light around them");
				WSLIDER("Dlight radius###PlayerDlightRadius", &Vars::ESP::Players::DlightRadius.m_Var, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
				WSLIDER("ESP alpha###PlayerESPAlpha", &Vars::ESP::Players::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

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
				TableNextColumn();

				SectionTitle("Building ESP");
				TOGGLE("Building ESP###BuildinGESPSwioifas", &Vars::ESP::Buildings::Active.m_Var); HelpMarker("Will draw useful information/indicators on buildings");
				TOGGLE("Ignore team buildings###BuildingESPIgnoreTeammates", &Vars::ESP::Buildings::IgnoreTeammates.m_Var); HelpMarker("Whether or not to draw ESP on your teams buildings");
				TOGGLE("Name ESP###BuildingNameESP", &Vars::ESP::Buildings::Name.m_Var); HelpMarker("Will draw the players name");
				TOGGLE("Name ESP box###BuildingNameESPBox", &Vars::ESP::Buildings::NameBox.m_Var); HelpMarker("Will draw a box around the buildings name to make it stand out");
				TOGGLE("Health bar###Buildinghelathbar", &Vars::ESP::Buildings::HealthBar.m_Var); HelpMarker("Will draw a bar visualizing how much health the building has");
				TOGGLE("Health text###buildinghealth", &Vars::ESP::Buildings::Health.m_Var); HelpMarker("Will draw the building's health, as well as its max health");
				TOGGLE("Building owner###Buildingowner", &Vars::ESP::Buildings::Owner.m_Var); HelpMarker("Shows who built the building");
				TOGGLE("Building level###Buildinglevel", &Vars::ESP::Buildings::Level.m_Var); HelpMarker("Will draw what level the building is");
				TOGGLE("Building condition###Buildingconditions", &Vars::ESP::Buildings::Cond.m_Var); HelpMarker("Will draw what conditions the building is under");
				TOGGLE("Lines###buildinglines", &Vars::ESP::Buildings::Lines.m_Var); HelpMarker("Draws lines from the local players position to the buildings position");
				static const char* boxArr[]{ "Off", "Bounding", "Cornered", "3D" }; WCOMBO("Box###PBuildingBoxESP", &Vars::ESP::Buildings::Box.m_Var, boxArr, IM_ARRAYSIZE(boxArr)); HelpMarker("What sort of box to draw on buildings");
				TOGGLE("Dlights###PlayerDlights", &Vars::ESP::Buildings::Dlights.m_Var); HelpMarker("Will make buildings emit a dynamic light around them, although buildings can't move some I'm not sure that the lights are actually dynamic here...");
				WSLIDER("Dlight radius###PlayerDlightRadius", &Vars::ESP::Buildings::DlightRadius.m_Var, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
				WSLIDER("ESP alpha###BuildingESPAlpha", &Vars::ESP::Buildings::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the ESP should be");


				/* Column 2 */
				TableNextColumn();

				/* Column 3 */
				TableNextColumn();

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
				TableNextColumn();

				SectionTitle("World ESP");
				TOGGLE("World ESP###WorldESPActive", &Vars::ESP::World::Active.m_Var); HelpMarker("World ESP master switch");
				TOGGLE("Health packs###WorldESPHealthPacks", &Vars::ESP::World::HealthText.m_Var); HelpMarker("Will draw ESP on health packs");
				ColorPickerL("Health pack colour", Colors::Health); HelpMarker("Color for health pack ESP");
				TOGGLE("Ammo packs###WorldESPAmmoPacks", &Vars::ESP::World::AmmoText.m_Var); HelpMarker("Will draw chams on ammo packs");
				ColorPickerL("Ammo pack colour", Colors::Ammo); HelpMarker("Color for ammo pack ESP");
				WSLIDER("ESP alpha###WordlESPAlpha", &Vars::ESP::World::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the world ESP should be");

				/* Column 2 */
				TableNextColumn();

				/* Column 3 */
				TableNextColumn();

				EndTable();
			}
			break;
		}

		// Visuals: Font
		case VisualsTab::Font:
		{
			// TODO
			break;
		}

		// Visuals: Misc
		case VisualsTab::Misc:
		{
			if (BeginTable("VisualsMiscTable", 3))
			{
				/* Column 1 */
				TableNextColumn();

				SectionTitle("World & UI");
				WISLIDER("Field of view", &Vars::Visuals::FieldOfView.m_Var, 70, 150, "%d", ImGuiSliderFlags_None); HelpMarker("How many degrees of field of vision you would like");
				const char* visionModifiers[]{ "Off", "Pyrovision", "Halloween", "Romevision" }; WCOMBO("Vision modifiers", &Vars::Visuals::Vision.m_Var, visionModifiers, IM_ARRAYSIZE(visionModifiers)); HelpMarker("Vision modifiers");
				MultiCombo({ "World", "Sky", "Prop Wireframe" }, { &Vars::Visuals::WorldModulation.m_Var, &Vars::Visuals::SkyModulation.m_Var, &Vars::Visuals::PropWireframe.m_Var }, "Select which types of modulation you want to enable", "Modulations");
				ColorPickerL("World modulation colour", Colors::WorldModulation);
				ColorPickerL("Sky modulation colour", Colors::SkyModulation, 1);
				ColorPickerL("Prop modulation colour", Colors::StaticPropModulation, 2);
				MultiCombo({ "Scope", "Zoom", "Disguises", "Taunts", "Interpolation", "View Punch" }, { &Vars::Visuals::RemoveScope.m_Var, &Vars::Visuals::RemoveZoom.m_Var, &Vars::Visuals::RemoveDisguises.m_Var, &Vars::Visuals::RemoveTaunts.m_Var, &Vars::Misc::DisableInterpolation.m_Var, &Vars::Visuals::RemovePunch.m_Var }, "Select what you want to remove", "Removals");
				MultiCombo({ "Aimbot Crosshair", "Render Proj Line", "Bullet Tracers", "Viewmodel Aimbot", "Weapon Sway", "Move sim line" }, { &Vars::Visuals::CrosshairAimPos.m_Var, &Vars::Visuals::AimPosSquare.m_Var, &Vars::Visuals::BulletTracer.m_Var, &Vars::Visuals::AimbotViewmodel.m_Var, &Vars::Visuals::ViewmodelSway.m_Var, &Vars::Visuals::MoveSimLine.m_Var }, "What misc visual features should be run", "Misc");
				ColorPickerL("Bullet tracer colour", Colors::BulletTracer);
				MultiCombo({ "Votes (Console)", "Votes (Text)", "Votes (Chat)", "Votes (Party)", "Damage Logs (Console)", "Damage Logs (Text)", "Damage Logs (Chat)", "Class Changes (Text)", "Class Changes (Chat)" }, { &Vars::Misc::VoteRevealerConsole.m_Var, &Vars::Misc::VoteRevealerText.m_Var, &Vars::Misc::VoteRevealerChat.m_Var, &Vars::Misc::VoteRevealerParty.m_Var, &Vars::Visuals::damageLoggerConsole.m_Var, &Vars::Visuals::damageLoggerText.m_Var, &Vars::Visuals::damageLoggerChat.m_Var, &Vars::Visuals::ChatInfoText.m_Var, &Vars::Visuals::ChatInfoChat.m_Var }, "What & How should events be logged", "Event Logging");
				ColorPickerL("GUI Notif Background", Colors::NotifBG);
				ColorPickerL("GUI Notif Outline", Colors::NotifOutline, 1);
				ColorPickerL("GUI Notif Colour", Colors::NotifText, 2);
				WSLIDER("GUI Notif Time", &Vars::Visuals::despawnTime.m_Var, 0.5f, 3.f, "%.1f", ImGuiSliderFlags_None);
				static const char* bullettracers[]{ "Off", "Machina", "C.A.P.P.E.R", "Short Circuit", "Merasmus ZAP", "Merasmus ZAP Beam 2", "Big Nasty", "Distortion Trail", "Black Ink", "Custom" }; WCOMBO("Particle tracer", &Vars::Visuals::ParticleTracer.m_Var, bullettracers, IM_ARRAYSIZE(bullettracers));
				if (Vars::Visuals::ParticleTracer.m_Var == 9)
				{
					// TODO: InputText("Custom Tracer", &Vars::Visuals::ParticleName); ImGui::PopItemWidth(); HelpMarker("If you want to use a custom particle tracer");
				}
				if (Vars::Visuals::BulletTracer.m_Var)
				{
					TOGGLE("Rainbow tracers", &Vars::Visuals::BulletTracerRainbow.m_Var); HelpMarker("Bullet tracer color will be dictated by a changing color");
				}
				if (Vars::Visuals::RemoveScope.m_Var)
				{
					TOGGLE("Noscope lines", &Vars::Visuals::ScopeLines.m_Var); HelpMarker("Will draw a custom overlay");
					ColorPickerL("Inner line color", Colors::NoscopeLines1);
					ColorPickerL("Outer line color", Colors::NoscopeLines2, 1);
				}
				TOGGLE("Pickup Timers", &Vars::Visuals::PickupTimers.m_Var); HelpMarker("Displays the respawn time of health and ammo packs");
				TOGGLE("Draw Hitboxes", &Vars::Aimbot::Global::showHitboxes.m_Var); HelpMarker("Shows client hitboxes for enemies once they are attacked (not bbox)");
				ColorPickerL("Hitbox matrix face colour", Colors::HitboxFace);
				ColorPickerL("Hitbox matrix edge colour", Colors::HitboxEdge, 1);
				TOGGLE("Clear Hitboxes", &Vars::Aimbot::Global::clearPreviousHitbox.m_Var); HelpMarker("Removes previous drawn hitboxes to mitigate clutter");
				WISLIDER("Hitbox Draw Time", &Vars::Aimbot::Global::hitboxTime.m_Var, 1, 5, "%d", ImGuiSliderFlags_None); HelpMarker("Removes previous drawn hitboxes after n seconds");

				const char* specModes[]{ "Off", "Draggable", "Static", "Static + Avatars" }; WCOMBO("Spectator list", &Vars::Visuals::SpectatorList.m_Var, specModes, IM_ARRAYSIZE(specModes));

				/*SectionTitle("Viewmodel Offset");
				ImGui::PushItemWidth(150); ImGui::SliderFloat("VM Off X", &Vars::Visuals::VMOffsets.x, -45.f, 45.f);
				ImGui::PushItemWidth(150); ImGui::SliderFloat("VM Off Y", &Vars::Visuals::VMOffsets.y, -45.f, 45.f);
				ImGui::PushItemWidth(150); ImGui::SliderFloat("VM Off Z", &Vars::Visuals::VMOffsets.z, -45.f, 45.f);
				ImGui::PushItemWidth(150); ImGui::SliderInt("VM Roll", &Vars::Visuals::VMRoll.m_Var, -180, 180);
				ImGui::Dummy(ImVec2(0, 20));

				SectionTitle("DT Indicator");
				const char* dtBarStyles[]{ "Off", "Default", "Nitro", "Rijin" }; ImGui::PushItemWidth(150); ImGui::Combo("DT indicator style", &Vars::Misc::CL_Move::DTBarStyle.m_Var, dtBarStyles, IM_ARRAYSIZE(dtBarStyles)); ImGui::PopItemWidth(); HelpMarker("Which style to do the bar style");
				ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
				ImGui::SetNextItemWidth(20);
				ColorPicker("DT charging right", Colors::DTBarIndicatorsCharging.endColour);
				ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
				ImGui::SetNextItemWidth(44);
				ColorPicker("DT charging left", Colors::DTBarIndicatorsCharging.startColour);
				if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 3)
				{
					ImGui::PushItemWidth(150); ImGui::SliderInt("DT Bar height###dtBHeightNitro", &Vars::Misc::CL_Move::DTBarScaleY.m_Var, 1, 25);
					ImGui::PopItemWidth(); ImGui::SameLine(ImGui::GetContentRegionMax().x - 20);
					ImGui::SetNextItemWidth(20);
					ColorPicker("DT charged right", Colors::DTBarIndicatorsCharged.endColour);
					ImGui::SameLine(ImGui::GetContentRegionMax().x - 44);
					ImGui::SetNextItemWidth(44);
					ColorPicker("DT charged left", Colors::DTBarIndicatorsCharged.startColour);
					ImGui::PushItemWidth(150); ImGui::SliderInt("DT Bar width###dtBWidthNitro", &Vars::Misc::CL_Move::DTBarScaleX.m_Var, 100, 1000);
					ImGui::PopItemWidth();
				}
				else
				{
					ImGui::SliderInt("DT Bar height###dtBHeight", &Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var, 1, 30);
					ColorPickerL("DT charged right", Colors::DTBarIndicatorsCharged.endColour);
					ColorPickerL("DT charged left", Colors::DTBarIndicatorsCharged.startColour, 1);
					ImGui::SliderInt("DT Bar width###dtBWidth", &Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var, 1, 30);
				}

				const char* unuEffects[]{
					"None",
					"Hot",
					"Isotope",
					"Cool",
					"Energy orb"
				};
				const char* unuEffects2[]{
					"None",
					"Hot",
					"Isotope",
					"Cool",
					"Energy orb"
				};
				ImGui::Dummy(ImVec2(0, 20));

				SectionTitle("Attribute Changer");
				ImGui::PushItemWidth(150);
				if (ImGui::Combo("Unusual effect 1", &unu1, unuEffects, IM_ARRAYSIZE(unuEffects)))
				{
					switch (unu1)
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
				ImGui::PopItemWidth();
				HelpMarker("The first unusual effect to be applied to the weapon");

				ImGui::PushItemWidth(150);
				if (ImGui::Combo("Unusual effect 2", &unu2, unuEffects2, IM_ARRAYSIZE(unuEffects2)))
				{
					switch (unu2)
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
				ImGui::PopItemWidth();
				HelpMarker("The second unusual effect to be applied to the weapon");

				const char* sheens[]{
					"None",
					"Team shine",
					"Deadly daffodil",
					"Manndarin",
					"Mean green",
					"Agonizing emerald",
					"Villainous violet",
					"Hot rod"
				};
				WCOMBO("Sheen", &Vars::Visuals::Skins::Sheen.m_Var, sheens); HelpMarker("Which sheen to apply to the weapon");
				TOGGLE("Style override", &Vars::Visuals::Skins::Override.m_Var);
				if (ImGui::Button("Apply", ImVec2(45, 20)))
				{
					g_AttributeChanger.m_bSet = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("Save", ImVec2(45, 20)))
				{
					g_AttributeChanger.m_bSave = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("Load", ImVec2(44, 20)))
				{
					g_AttributeChanger.m_bLoad = true;
				}

				SectionTitle("Ragdoll effects", 16);
				TOGGLE("Enemy only###RagdollEnemyOnly", &Vars::Visuals::RagdollEffects::EnemyOnly.m_Var); HelpMarker("Only runs it on enemies");
				MultiCombo({ "Burning", "Electrocuted", "Become ash", "Dissolve" }, { &Vars::Visuals::RagdollEffects::Burning.m_Var, &Vars::Visuals::RagdollEffects::Electrocuted.m_Var, &Vars::Visuals::RagdollEffects::BecomeAsh.m_Var, &Vars::Visuals::RagdollEffects::Dissolve.m_Var }, "Ragdoll particle effects", "Effects###RagdollEffects");
				if (TOGGLE("Gold ragdoll", &Vars::Visuals::RagdollEffects::Gold.m_Var))
				{
					Vars::Visuals::RagdollEffects::Ice.m_Var = false;
				}
				HelpMarker("Will make their ragdoll gold");
				if (TOGGLE("Ice ragdoll", &Vars::Visuals::RagdollEffects::Ice.m_Var))
				{
					Vars::Visuals::RagdollEffects::Gold.m_Var = false;
				}
				HelpMarker("Will make their ragdoll ice");

				SectionTitle("Freecam", 16);
				InputKeybind("Freecam Key", Vars::Visuals::FreecamKey);  HelpMarker("Allows you to freely move your camera when holding the key");
				WSLIDER("Freecam Speed", &Vars::Visuals::FreecamSpeed.m_Var, 1.f, 20.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Movement speed of freecam");

				SectionTitle("Camera", 16);
				static const char* camModes[]{ "Off", "Mirror", "Spy" }; ImGui::PushItemWidth(100); ImGui::Combo("Camera mode", &Vars::Visuals::CameraMode.m_Var, camModes, IM_ARRAYSIZE(camModes)); ImGui::PopItemWidth(); HelpMarker("What the camera should display");
				WSLIDER("Camera FOV", &Vars::Visuals::CameraFOV.m_Var, 40.f, 130.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("FOV of the camera window");

				ImGui::PopStyleVar();*/

				/* Column 2 */
				TableNextColumn();

				/* Column 3 */
				TableNextColumn();

				EndTable();
			}
			break;
		}
	}
}

/* Tab: HvH */
void CMenu::MenuHvH()
{

}

/* Tab: Misc */
void CMenu::MenuMisc()
{

}

/* Tab: Configs */
void CMenu::MenuConfigs()
{

}

/* Window for the camera feature */
void CMenu::DrawCameraWindow()
{
	if (g_Interfaces.Engine->IsInGame() && Vars::Visuals::CameraMode.m_Var != 0)
	{
		int windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;
		if (Vars::Visuals::CameraMode.m_Var <= 1 || g_CameraWindow.CanDraw) {
			windowFlags |= ImGuiWindowFlags_NoBackground;
		}

		// Draw the camera window
		ImGui::SetNextWindowSize({ static_cast<float>(g_CameraWindow.ViewRect.w), static_cast<float>(g_CameraWindow.ViewRect.h) }, ImGuiCond_Once);
		ImGui::SetNextWindowPos({ static_cast<float>(g_CameraWindow.ViewRect.x), static_cast<float>(g_CameraWindow.ViewRect.y) }, ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 60.f, 60.f });
		ImGui::PushStyleColor(ImGuiCol_Text, TextDark.Value);
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
		ImGui::PopStyleColor();
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
		g_Interfaces.Surface->SetCursorAlwaysVisible(g_Menu.IsOpen);
	}

	// Begin current frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (g_Menu.IsOpen)
	{
		ImGui::PushFont(Segoe);
		DrawMenu();
		DrawCameraWindow();

		// TODO: Draw DT-Bar, Playerlist, Spectator list etc.
		ImGui::PopFont();
	}

	// End frame and render
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
}

void CMenu::Init(IDirect3DDevice9* pDevice)
{
	// Initialize ImGui and device
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(FindWindowA(nullptr, "Team Fortress 2"));
	ImGui_ImplDX9_Init(pDevice);

	const auto& io = ImGui::GetIO();
	auto& style = ImGui::GetStyle();

	// Fonts
	{
		auto fontConfig = ImFontConfig();
		fontConfig.OversampleH = 2;

		auto wideFontConfig = ImFontConfig();
		wideFontConfig.GlyphExtraSpacing = { 1.f, 0.f };

		constexpr ImWchar fontRange[]{ 0x0020, 0x00FF,0x0400, 0x044F,0 }; // Basic Latin, Latin Supplement and Cyrillic
		SegoeLight = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuisl.ttf", 16.0f, &fontConfig, fontRange);
		Segoe = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeui.ttf", 16.0f, &fontConfig, fontRange);
		SegoeBold = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuib.ttf", 16.0f, &fontConfig, fontRange);

		SectionFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeui.ttf", 18.0f, &wideFontConfig, fontRange);
		TabFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuisl.ttf", 20.0f, &fontConfig, fontRange);
		TitleFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuib.ttf", 22.0f, &fontConfig, fontRange);

		io.Fonts->Build();
	}

	// Style
	{
		// https://raais.github.io/ImStudio/

		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);	// Center window title
		style.WindowMinSize = ImVec2(700, 700);
		style.WindowPadding = ImVec2(0, 0);
		style.WindowBorderSize = 1.f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.4f);		// Center button text
		// style.CellPadding = ImVec2(12.f, 0.f);
		style.FrameBorderSize = 1.f;	// Old menu feeling
		style.FrameRounding = 0.f;
		style.ChildBorderSize = 0.f;
		style.ChildRounding = 0.f;
		style.ScrollbarSize = 4.f;
		style.GrabMinSize = 15.f;
		style.ItemSpacing = ImVec2(8.f, 5.f);

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Border] = ImColor(110, 110, 128);
		colors[ImGuiCol_WindowBg] = Background;
		colors[ImGuiCol_TitleBg] = BackgroundDark;
		colors[ImGuiCol_TitleBgActive] = Accent;
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.4f);
		colors[ImGuiCol_Button] = BackgroundLight;
		colors[ImGuiCol_ButtonHovered] = ImColor(69, 69, 77);
		colors[ImGuiCol_ButtonActive] = ImColor(82, 79, 87);
		colors[ImGuiCol_PopupBg] = BackgroundDark;
		colors[ImGuiCol_FrameBg] = ImColor(50, 50, 50);
		colors[ImGuiCol_FrameBgHovered] = ImColor(60, 60, 60);
		colors[ImGuiCol_FrameBgActive] = ImColor(60, 60, 60);
		colors[ImGuiCol_CheckMark] = Accent;
		colors[ImGuiCol_Text] = TextLight;

		colors[ImGuiCol_SliderGrab] = Accent;
		colors[ImGuiCol_SliderGrabActive] = AccentDark;
		colors[ImGuiCol_ResizeGrip] = AccentDark;
		colors[ImGuiCol_ResizeGripActive] = Accent;
		colors[ImGuiCol_ResizeGripHovered] = Accent;
		colors[ImGuiCol_HeaderActive] = Accent;
		colors[ImGuiCol_HeaderHovered] = Accent;
		colors[ImGuiCol_Header] = Accent;
	}

	// Misc
	{
		titleGradient.ClearMarks();
		titleGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		titleGradient.AddMark(0.3f, ImColor(0, 0, 0, 0));
		titleGradient.AddMark(0.5f, Accent);
		titleGradient.AddMark(0.7f, ImColor(0, 0, 0, 0));
		titleGradient.AddMark(1.f, ImColor(0, 0, 0, 0));
	}

	{
		mainGradient.ClearMarks();
		mainGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		mainGradient.AddMark(0.15f, ImColor(0, 0, 0, 0));
		mainGradient.AddMark(0.45f, Accent);
		mainGradient.AddMark(0.75f, ImColor(0, 0, 0, 0));
		mainGradient.AddMark(1.f, ImColor(0, 0, 0, 0));
	}
}

#undef SLIDER
#undef TOGGLE