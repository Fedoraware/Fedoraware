#pragma once

#include "../SDK/SDK.h"

template <class T>
class CVar
{
public:
	T m_Var;
	const wchar_t* m_szDisplayName = L"";
};

namespace Vars
{
	namespace Menu
	{
		inline Rect_t Position = { 100, 100, 800, 500 };
		inline const auto Title = _(L"Team Fortress 2");
		inline int TitleBarH = 25;

		inline int SpacingX = 8;
		inline int SpacingY = 4;
		inline int SpacingText = 4;

		inline int CheckBoxW = 14;
		inline int CheckBoxH = 14;
		inline int CheckBoxFillSize = 3;

		inline int ButtonW = 90;
		inline int ButtonH = 20;

		inline int ButtonWSmall = 70;
		inline int ButtonHSmall = 16;

		inline int ComboBoxW = 90;
		inline int ComboBoxH = 14;

		inline int InputBoxW = 90;
		inline int InputBoxH = 14;
		inline int InputColorBoxW = 30;

		inline bool ModernDesign = false;
		inline bool BlurBackground = false;
		inline bool ShowPlayerlist = false;

		namespace Colors
		{
			inline Color_t MenuAccent = { 255, 101, 101, 255 };
		}
	}

	namespace CritHack
	{
		inline CVar<bool> Active{ false, L"Active" };
		inline CVar<bool> indicators{ true };
		inline CVar<bool> avoidrandom{ true };
		inline CVar<bool> AlwaysMelee{ true };
		inline CVar<int> CritKey{ VK_SHIFT, L"Crit Key" };
	}

	namespace Backtrack
	{
		inline CVar<bool> Enabled{ false, L"Backtrack master switch" };
		inline CVar<bool> Aim{ false, L"Aims at last tick" };
		namespace BtChams
		{
			inline CVar<bool> Enabled{ false, L"Backtrack chams" };
			inline CVar<bool> LastOnly{ true, L"Only draws last" };
			inline CVar<bool> EnemyOnly{ true, L"Enemy only" };
			inline CVar<bool> IgnoreZ{ true, L"IgnoreZ" };
			inline CVar<int> Material{ 1, L"Backtrack material" };
			inline Color_t BacktrackColor{ 255,255,255,255 };
		}
	}

	namespace Aimbot
	{
		namespace Global
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<int> AimKey{ VK_XBUTTON1, L"Aim Key" };
			inline CVar<float> AimFOV{ 15.0f, L"Aim FOV" };
			inline CVar<bool> AutoShoot{ true, L"Auto Shoot" };
			inline CVar<bool> AimPlayers{ true, L"Aim Players" };
			inline CVar<bool> AimBuildings{ true, L"Aim Buildings" };
			inline CVar<bool> AimStickies{ true };
			inline CVar<bool> IgnoreInvlunerable{ true, L"Ignore Invulnerable" };
			inline CVar<bool> IgnoreCloaked{ false, L"Ignore Cloaked" };
			inline CVar<bool> IgnoreFriends{ true, L"Ignore Friends" };
			inline CVar<bool> IgnoreTaunting{ true, L"Ignore Taunting" };
			inline CVar<bool> BAimLethal{ false, L"Body aim if lethal" }; // This is in global cause i remmebered hunterman exists
			inline CVar<bool> showHitboxes{ true, L"Show Hitboxes" }; // original codenz
			inline CVar<bool> clearPreviousHitbox{ true, L"Clear Hitboxes" };
			inline CVar<int> hitboxTime{ 2, L"Hitbox Show Time" };
		}



		namespace Hitscan
		{
			//inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<int> SortMethod{ 0, L"Sort Method" };	//0 - FOV,		1 - Distance
			inline CVar<int> AimMethod{ 2, L"Aim Method" };	//0 - Normal,	1 - Smooth, 2 - Silent
			inline CVar<int> AimHitbox{ 2, L"Aim Hitbox" };	//0 - Head,		1 - Body,	2 - Auto
			inline CVar<int> ScanHitboxes{ 7 };	// Binary: 00111 {legs, arms, body, pelvis, head}
			inline CVar<int> MultiHitboxes{ 5 };	// Binary: 00101 {legs, arms, body, pelvis, head}
			inline CVar<int> SmoothingAmount{ 4, L"Smoothing Amount" };
			inline CVar<int> TapFire{ 1, L"Tap Fire" };	//0 - Off, 1 - Distance, 2 - Always
			inline CVar<bool> ScanBuildings{ true, L"Scan Buildings" };
			inline CVar<bool> WaitForHeadshot{ true, L"Wait For Headshot" };
			inline CVar<bool> WaitForCharge{ true, L"Wait For Charge" };
			inline CVar<bool> SpectatedSmooth{ true, L"Spectated Smooth" };
			inline CVar<bool> ScopedOnly{ false, L"Scoped Only" };
			inline CVar<bool> AutoScope{ false, L"Auto Scope" };
			inline CVar<bool> AutoRev{ false, L"Auto Rev" };

		}

		namespace Projectile
		{
			//inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<bool> PerformanceMode{ true, L"Performance Mode" };
			inline CVar<int> SortMethod{ 0, L"Sort Method" };	//0 - FOV,		1 - Distance
			inline CVar<int> AimMethod{ 1, L"Aim Method" };	//0 - Normal,	1 - Silent
			inline CVar<int> AimPosition{ 2, L"Aim Position" };	//0 - Body,		1 - Feet,	2 - Auto
			//inline CVar<float> AimFOV			{ 25.0f, L"Aim FOV" };
			inline CVar<bool> FeetAimIfOnGround{ false, L"Aim at feet if target is on the ground." };
			inline CVar<bool> MovementSimulation{ true, L"move sim" };
			inline Color_t PredictionColor{ 255,255,255,255 };
			inline CVar<bool> ManualZAdjust{ true, L"Toggle for manual z adjust on projectile prediction." };
			inline CVar<float> ZAdjustAmount{ 5.5f, L"Z Adjust factor." };
			inline CVar<float> predTime{ 2.0f, L"Prediction TIme" };
			inline CVar<bool> WaitForHit{ false, L"Will wait for the time the projectile is predicted to take to hit the target before attempting to shoot again." };
		}

		namespace Melee
		{
			//inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<int> SortMethod{ 1, L"Sort Method" };	//0 - FOV,		1 - Distance
			inline CVar<int> AimMethod{ 2, L"Aim Method" };	//0 - Normal,	1 - Smooth, 2 - Silent
			inline CVar<int> SmoothingAmount{ 8, L"Smoothing Amount" };
			inline CVar<bool> RangeCheck{ true, L"Range Check" };
			inline CVar<bool> PredictSwing{ true, L"Predict Swing" };
			inline CVar<bool> WhipTeam{ true, L"Whip Teammates" };
		}
	}

	namespace Triggerbot
	{
		namespace Global
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<int> TriggerKey{ VK_XBUTTON2, L"Trigger Key" };
			inline CVar<bool> IgnoreInvlunerable{ true, L"Ignore Invulnerable" };
			inline CVar<bool> IgnoreCloaked{ false, L"Ignore Cloaked" };
			inline CVar<bool> IgnoreFriends{ false, L"Ignore Friends" };
		}

		namespace Shoot
		{
			inline CVar<bool> Active{ false, L"Active" };
			inline CVar<bool> TriggerPlayers{ true, L"Trigger Players" };
			inline CVar<bool> TriggerBuildings{ true, L"Trigger Buildings" };
			inline CVar<bool> HeadOnly{ false, L"Head Only" };
			inline CVar<bool> WaitForCharge{ true, L"Wait For Charge" };
			inline CVar<float> HeadScale{ 0.7f, L"Head Scale" };
		}

		namespace Stab
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> RageMode{ false, L"Rage Mode" };
			inline CVar<bool> Silent{ true, L"Silent" };
			inline CVar<bool> Disguise{ false, L"Disguise" };
			inline CVar<bool> IgnRazor{ true, L"Ignore Razorback" };
			inline CVar<float> Range{ 0.9f, L"Range" };
		}

		namespace Detonate
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> Stickies{ true, L"Detonate Stickies" };
			inline CVar<bool> Flares{ true, L"Detonate Flares" };
			inline CVar<float> RadiusScale{ 1.0f, L"Radius Scale" };
		}

		namespace Blast
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> Rage{ false, L"Rage" };
			inline CVar<bool> Silent{ true, L"Silent" };
			inline CVar<int> Fov{ 55,    L"FOV" };
		}

		namespace Uber
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> OnlyFriends{ true, L"Only Friends" };
			inline CVar<bool> PopLocal{ true, L"Pop on local" };
			inline CVar<bool> AutoVacc{ true, L"Auto Vacc" };
			inline CVar<float> HealthLeft{ 35.0f, L"Health left" };
		}
	}

	namespace ESP
	{
		namespace Main
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> Outlinedbar{ true, L"Outlined health bar" }; //0 - OFF, 1 - Text Only, 2 - All
			inline CVar<bool> EnableTeamEnemyColors{ false, L"Enable team/enemy colors" };
		}



		namespace Players
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> ShowLocal{ true, L"Show Local" };
			inline CVar<int> IgnoreTeammates{ 2, L"Ignore Teammates" };	//0 - OFF, 1 - All, 2 - Keep Friends
			inline CVar<int> IgnoreCloaked{ 2, L"Ignore Cloaked" };	//0 - OFF, 1 - All, 2 - Enemies Only
			inline CVar<bool> Name{ true, L"Name" };
			inline CVar<bool> NameCustom{ false, L"Name Color" };
			inline Color_t NameColor{ 255,255,255,255 };
			inline CVar<bool> NameBox{ false, L"Name box" };
			inline CVar<int> Uber{ 2, L"Uber" }; //0 - Off, 1 - Text, 2 - Bar
			inline CVar<int> Class{ 1, L"Class" }; // 0 - Off, 1 - Icon, 2 - Text, 3 - Both
			inline CVar<bool> WeaponIcon{ false, L"weapon icon" };
			inline CVar<bool> Health{ false, L"Health" };
			inline CVar<bool> Cond{ true, L"Cond" };
			inline CVar<bool> HealthBar{ true, L"Health Bar" };
			inline CVar<int> Box{ 0, L"Box" };	//0 - OFF, 1 - Simple, 2 - Corners
			inline CVar<int> Bones{ 0, L"Bones" };
			inline CVar<bool> GUID{ false, L"GUID" };
			inline CVar<bool> Choked{ false, L"Choked Packets" };
			inline CVar<bool> Lines{ false, L"Lines" };
			inline CVar<bool> Dlights{ false, L"Dlights" };
			inline CVar<float> DlightRadius{ 200.0f, L"DLight Radius" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
			inline CVar<bool> CheaterDetection{ true };
		}

		namespace Buildings
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> IgnoreTeammates{ true, L"Ignore Teammates" };
			inline CVar<bool> Name{ true, L"Name" };
			inline CVar<bool> NameCustom{ false, L"Name Color" };
			inline Color_t NameColor{ 255,255,255,255 };
			inline CVar<bool> NameBox{ false, L"Name box" };
			inline CVar<bool> Health{ false, L"Health" };
			inline CVar<bool> Owner{ false, L"Owner" };
			inline CVar<bool> Level{ true, L"Level" };
			inline CVar<bool> Cond{ true, L"Cond" };
			inline CVar<bool> HealthBar{ true, L"Health Bar" };
			inline CVar<bool> TeleExitDir{ false, L"Teleporter Exit Dir" };
			inline Color_t TeleExitDirColor{ 255, 255, 255, 255 };
			inline CVar<bool> Lines{ false, L"Lines" };
			inline CVar<int> Box{ 0, L"Box" };	//0 - OFF, 1 - Simple, 2 - Corners
			inline CVar<bool> Dlights{ false, L"Dlights" };
			inline CVar<float> DlightRadius{ 200.0f, L"DLight Radius" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
		}

		namespace World
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> HealthText{ true, L"Health Text" };
			inline CVar<bool> AmmoText{ true, L"Ammo Text" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
		}
	}

	namespace Chams
	{
		namespace Main
		{
			inline CVar<bool> Active{ true , L"Active" };
		}

		namespace Players
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> Wearables{ true, L"Render Wearables" };
			inline CVar<bool> Weapons{ true, L"Render Weapons" };
			inline CVar<bool> FadeoutTeammates{ true };

			// {ignorez, material, overlay type (0 = off), active}
			inline Chams_t Local{};
			inline Chams_t Friend{};
			inline Chams_t Enemy{};
			inline Chams_t Team{};
			inline Chams_t Target{};
		}

		namespace Buildings
		{
			inline Chams_t Local{};
			inline Chams_t Friend{};
			inline Chams_t Enemy{};
			inline Chams_t Team{};
			inline Chams_t Target{};

			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<int> Material{ 3, L"Material" }; //0 - None, 1 - Shaded, 2 - Shiny, 3 - Flat
			inline CVar<bool> IgnoreZ{ false , L"IgnoreZ" };
		}

		namespace World
		{

			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<int> Projectilez{ 2, L"please replace this with a bool lol thnx" };
			inline Chams_t Health{};
			inline Chams_t Ammo{};
			inline Chams_t Projectiles{};
		}

		namespace DME
		{
			inline CVar<bool> Active{ true, L"Active" };

			inline CVar<int> Hands{ 0, L"Hands" }; //0 - Original, 1 - Shaded, 2 - Shiny, 3 - Flat, 4 - WF-Shaded, 5 - WF-Shiny, 6 - WF-Flat
			inline CVar<int> HandsGlowOverlay{ 0, L"Hands Glow" }; // 0 - Off,  1 - Fresnel Glow, 2 - Wireframe Glow
			inline CVar<int> HandsProxySkin{ 0, L"Hands proxy skin" };
			inline CVar<float> HandsGlowAmount{ 1, L"How glowy it is" };
			inline CVar<bool> HandsProxyWF{ false, L"Proxy wireframe on hands" };
			inline CVar<bool> HandsRainbow{ false, L"Rainbow hands" };
			inline CVar<bool> HandsOverlayRainbow{ false, L"Rainbow hands" };
			inline CVar<bool> HandsOverlayPulse{ false };

			inline CVar<int> Weapon{ 0, L"Weapon" }; //0 - Original, 1 - Shaded, 2 - Shiny, 3 - Flat, 4 - WF-Shaded, 5 - WF-Shiny, 6 - WF-Flat
			inline CVar<int> WeaponGlowOverlay{ 0, L"Weapon Glow" }; // 0 - Off,  1 - Fresnel Glow, 2 - Wireframe Glow
			inline CVar<int> WeaponsProxySkin{ 0, L"Weapon proxy skin" };
			inline CVar<float> WeaponGlowAmount{ 1, L"How glowy it is" };
			inline CVar<bool> WeaponsProxyWF{ false, L"Proxy wireframe on weapons" };
			inline CVar<bool> WeaponRainbow{ false, L"Rainbow weapons" };
			inline CVar<bool> WeaponOverlayRainbow{ false, L"Rainbow weapons" };
			inline CVar<bool> WeaponOverlayPulse{ false };
		}
	}

	namespace Glow
	{
		namespace Main
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> Stencil{ false, L"Stencil" };
			inline CVar<int> Scale{ 5, L"Scale" };
		}

		namespace Players
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> ShowLocal{ true, L"Show Local" };
			inline CVar<bool> LocalRainbow{ false, L"Local player is rainbow" };
			inline CVar<int> IgnoreTeammates{ 2, L"Ignore Teammates" };	//0 - OFF, 1 - All, 2 - Keep Friends
			inline CVar<bool> Wearables{ true, L"Render Wearables" };
			inline CVar<bool> Weapons{ true, L"Render Weapons" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
			inline CVar<int> Color{ 0, L"Color" }; //0 - Team, 1 - Health
		}

		namespace Buildings
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> IgnoreTeammates{ true, L"Ignore Teammates" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
			inline CVar<int> Color{ 0, L"Color" }; //0 - Team, 1 - Health
		}

		namespace World
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> Health{ true, L"Health" };
			inline CVar<bool> Ammo{ true, L"Ammo" };
			inline CVar<int> Projectiles{ 1, L"Projectiles" }; //0 - Off, 1 - All, 2 - Enemy Only
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
		}
	}

	namespace Radar
	{
		namespace Main
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<int>  BackAlpha{ 128, L"Back Alpha" };
			inline CVar<int>  LineAlpha{ 255, L"Line Alpha" };
			inline CVar<int>  Size{ 100, L"Size" };
			inline CVar<int>  Range{ 1500, L"Range" };
		}

		namespace Players
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<int>  IconType{ 1, L"Icon" }; //0 - Scoreboard, 1 - Portraits, 2 - Avatars
			inline CVar<int>  BackGroundType{ 2, L"Background" }; //0 - Off, 1 - Rect, 2 - Texture
			inline CVar<bool> Outline{ true, L"Outline" };
			inline CVar<int>  IgnoreTeam{ 2, L"Ignore Teammates" };  //0 - Off, 1 - All, 2 - Keep Friends
			inline CVar<int>  IgnoreCloaked{ 0, L"Ignore Cloaked" };	//0 - Off, 1 - All, 2 - Enemies Only
			inline CVar<bool> Health{ false, L"Health Bar" };
			inline CVar<int>  IconSize{ 24, L"Icon Size" };
		}

		namespace Buildings
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> Outline{ false, L"Outline" };
			inline CVar<bool> IgnoreTeam{ true, L"Ignore Team" };
			inline CVar<bool> Health{ false, L"Health Bar" };
			inline CVar<int>  IconSize{ 18, L"Icon Size" };
		}

		namespace World
		{
			inline CVar<bool> Active{ false, L"Active" };
			inline CVar<bool> Health{ true, L"Health" };
			inline CVar<bool> Ammo{ true, L"Ammo" };
			inline CVar<int>  IconSize{ 14, L"Icon Size" };
		}
	}

	namespace Visuals
	{
		inline CVar<bool> RemoveDisguises{ true, L"Remove Disguises" };
		inline CVar<bool> RemoveTaunts{ false, L"Remove Taunts" };
		inline CVar<int> FieldOfView{ 130, L"Field of View" };
		inline CVar<int> AimFOVAlpha{ 10, L"Aim FOV Alpha" };
		inline CVar<bool> RemoveScope{ true, L"Remove Scope" };
		inline CVar<bool> RemoveMOTD{ false, L"Remove MOTD" };
		inline CVar<bool> RemoveScreenEffects{ true };
		inline CVar<bool> PreventForcedAngles{ true };
		inline CVar<bool> ScopeLines{ false, L"Scope lines" };
		inline CVar<bool> PickupTimers{ false, L"Pickup Timers" };
		inline CVar<bool> RemoveZoom{ true, L"Remove Zoom" };
		inline CVar<bool> RemovePunch{ true, L"Remove Recoil" };
		inline CVar<bool> CrosshairAimPos{ true, L"Crosshair At Aim Pos" };
		inline CVar<bool> ChatInfoText{ false, L"Show Class Changes" };
		inline CVar<bool> ChatInfoChat{ false, L"Show Class Changes" };
		inline CVar<bool> OutOfFOVArrowsOutline{ false, L"balls" };
		inline CVar<float> FovArrowsDist{ 0.15f, L"balls" };
		inline CVar<int> SpectatorList{ 2, L"Spectator List" }; //0 - Off, 1 - Default, 2 - Classic, 3 - Classic Avatars
		inline CVar<bool> DebugInfo{ false }; // trash

		inline CVar<int> FreecamKey{ 0, L"Freecam Key" };
		inline CVar<float> FreecamSpeed{ 10.f, L"Freecam Speed" };

		inline CVar<int> CameraMode{ 0, L"Camera Mode" };
		inline CVar<float> CameraFOV{ 90.f, L"Camera FOV" };

		inline CVar<bool> SpyWarning{ true, L"Active" };
		inline CVar<bool> SpyWarningAnnounce{ true, L"Announce" };
		inline CVar<int> SpyWarningStyle{ 0, L"Style" }; //0 - Indicator, 1 - Flash
		inline CVar<bool> SpyWarningVisibleOnly{ true, L"Visible Only" };
		inline CVar<bool> SpyWarningIgnoreFriends{ true, L"Ignore Friends" };

		inline CVar<bool> Snow{ true, L"Menu Snow" };
		inline CVar<bool> ToolTips{ false, L"Menu tooltips" };

		inline CVar<bool> ThirdPerson{ false, L"Active" };
		inline CVar<int> ThirdPersonKey{ VK_B, L"Toggle Key" };
		inline CVar<bool> ThirdPersonSilentAngles{ true, L"Silent Angles" };
		inline CVar<bool> ThirdPersonInstantYaw{ true, L"Instant Yaw" };
		inline CVar<bool> ThirdPersonServerHitbox{ false, L"Server Hitboxes" };
		inline CVar<bool> ThirdpersonOffset{ false, L"Thirdperson offsets" };
		inline CVar<float> ThirdpersonDist{ 200.f, L"Thirdperson distance" };
		inline CVar<float> ThirdpersonRight{ 0.f, L"Thirdperson distance" };
		inline CVar<float> ThirdpersonUp{ 0.f, L"Thirdperson distance" };
		inline CVar<bool> ThirdpersonOffsetWithArrows{ false, L"Offset thirdperson with arrows" };
		inline CVar<int> ThirdpersonArrowOffsetKey{ VK_F, L"Which key to press to move the camera around" };
		inline CVar<bool> ThirdpersonCrosshair{ false, L"Make thirdperson where player is aiming in thirdperson" };


		inline CVar<bool> WorldModulation{ false, L"World Modulation" };
		inline CVar<bool> PropWireframe{ false, L"Prop Wireframe" };
		inline CVar<bool> OverrideWorldTextures{ false, L"World Texture Override" };
		inline CVar<bool> SkyboxChanger{ true, L"Skybox changer" };
		inline CVar<bool> SkyModulation{ true, L"Skybox modulation" };
		inline CVar<bool> BulletTracer{ false, L"Bullet tracers" };
		inline CVar<bool> AimbotViewmodel{ true, L"AimbotVM" };
		inline CVar<bool> ViewmodelSway{ false, L"AimbotVM" };
		inline CVar<bool> MoveSimLine{ true, L"Movement Simulation Line" };
		inline CVar<int> ParticleTracer{ 2, L"Particle tracers" };
		inline std::string ParticleName = "merasmus_zap_beam01"; // dont save this as a var its pointless
		namespace Beans
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> Rainbow { false };
			inline Color_t BeamColour{ 255, 255, 255, 255 };
			inline CVar<bool> UseCustomModel { false };
			inline std::string Model = "sprites/physbeam.vmt";
			inline CVar<float> Life { 2.f };
			inline CVar<float> Width { 2.f };
			inline CVar<float> EndWidth{ 2.f };
			inline CVar<float> FadeLength { 10.f };
			inline CVar<float> Amplitude { 2.f };
			inline CVar<float> Brightness { 255.f };
			inline CVar<float> Speed { 0.2f };
			inline CVar<int> Flags { 65792 };
			inline CVar<int> segments{ 2 };
		}
		inline CVar<bool> BulletTracerRainbow{ true, L"Rainbow tracers" };
		inline CVar<bool> AimPosSquare{ true, L"Aim position square" };
		inline CVar<bool> OutOfFOVArrows{ true, L"Out of FOV arrows" };
		inline CVar<float> ArrowLength{ 21.f, L"Out of FOV arrow length" };
		inline CVar<float> ArrowAngle{ 100.f, L"Out of FOV arrow angle" };
		inline CVar<float> MaxDist{ 1000.f, L"How far until the arrows are no longer visible" };
		inline CVar<float> MinDist{ 200.f, L"How many units till the arrows are fully opaque" };
		inline CVar<int> VMOffX{ 0, L"VM Offset" };
		inline CVar<int> VMOffY{ 0, L"VM Offset" };
		inline CVar<int> VMOffZ{ 0, L"VM Offset" };
		inline Vec3 VMOffsets{};
		inline CVar<int> VMRoll{ 0, L"VM Offset" };

		inline CVar<float> despawnTime{ 5.f, L"How many ticks to despawn a damage log event" };
		//inline CVar<int> damageLogger{ 2, L"Enable damage logger" };
		inline CVar<bool> damageLoggerText{ false, L"Enable damage logger" };
		inline CVar<bool> damageLoggerChat{ false, L"Enable damage logger" };
		inline CVar<bool> damageLoggerConsole{ false, L"Enable damage logger" };

		inline CVar<int> Vision{ false, L"vision modifier" };

		inline CVar<int> Rain{ false, L"0 - none, 1 - \"rain\", 2 - \"snow\"" };

		namespace RagdollEffects
		{
			inline CVar<bool> EnemyOnly{ false, L"Only on enemies" };
			inline CVar<bool> Burning{ false, L"Burn ragdoll" };
			inline CVar<bool> Electrocuted{ false, L"Electrocute ragdoll" };
			inline CVar<bool> BecomeAsh{ false, L"Ash ragdoll" };
			inline CVar<bool> Dissolve{ false, L"Dissolve ragdoll" };
			inline CVar<int> RagdollType{ 0 };
		}

		namespace Skins
		{
			inline CVar<bool> Enabled{ false, L"Active" };
			inline CVar<int> Sheen{ 0, L"Sheen" };
			inline CVar<int> Effect{ 0, L"Effect" };
			inline CVar<int> Particle{ 0, L"Particle" };
			inline CVar<bool> Acient{ false, L"Ancient" };
			inline CVar<bool> Override{ false, L"Style Override" };
			inline CVar<bool> Australium{ false, L"Australium" };
		}

		namespace Fog
		{
			inline CVar<bool> CustomFog{ false, L"Fog" };
			inline Color_t FogColor{ 255,255,255,255 };
			inline Color_t FogColorSkybox{ 255,255,255,255 };
			inline CVar<float> FogDensity{ 0, L"fog" };
			inline CVar<float> FogDensitySkybox{ 0, L"fog" };
			inline CVar<float> FogStart{ 0, L"fog" };
			inline CVar<float> FogStartSkybox{ 0, L"fog" };
			inline CVar<float> FogEnd{ 0, L"fog" };
			inline CVar<float> FogEndSkybox{ 0, L"fog" };
		}
	}

	namespace Misc
	{
		inline CVar<bool> AccurateMovement{ true };
		inline CVar<bool> AutoJump{ true, L"Auto Jump" };
		inline CVar<bool> DuckJump{ false };
		inline CVar<int> AutoStrafe{ 2, L"Auto Strafe" };
		inline CVar<bool> Directional{ false, L"Directional" };
		inline CVar<bool> TauntSlide{ true, L"Taunt Slide" };
		inline CVar<bool> TauntControl{ true, L"Taunt Control" };
		inline CVar<bool> BypassPure{ true, L"Bypass Pure" };
		inline CVar<bool> NoisemakerSpam{ false, L"Noisemaker Spam" };
		inline CVar<bool> DisableInterpolation{ true, L"Disable Interpolation" };
		inline CVar<bool> MedalFlip{ true, L"Medal Flip" };
		inline CVar<bool> AutoRocketJump{ false, L"Auto RocketJump" };
		inline CVar<int> ChatSpam{ 0, L"Chat Spam" };
		inline CVar<bool> NoPush{ true, L"No Push" };
		inline CVar<bool> EdgeJump{ false, L"Edge Jump" };
		inline CVar<int> EdgeJumpKey{ VK_MENU, L"Edge Jump key" };
		inline CVar<bool> AntiAFK{ false, L"Anti AFK" };
		inline CVar<int> VotingOptions{ 0b000011 }; // 000011 {verbose, autovote, party, chat, console, text}
		inline CVar<bool> CheatsBypass{ false, L"Force sv_cheats to 1 (clientside only)" };
		inline CVar<int> Roll{ false, L"Super Crouch Speed" };
		inline CVar<bool> ChatCensor{ false, L"Chat Censor" };
		inline CVar<bool> ChatNL{ true };
		inline CVar<bool> AntiAutobal{ false, L"AntiAutobal" };
		inline CVar<bool> RageRetry{ false, L"Rage Retry" };
		inline CVar<int> RageRetryHealth{ 20, L"Rage Retry health" };
		inline CVar<bool> MVMRes{ true, L"MVM Respawn" };
		inline CVar<bool> BeCat{ false, L"CatReply" };
		inline CVar<bool> PingReducer{ false, L"Ping reducer" };
		inline CVar<int> PingTarget{ 0, L"Ping target" };
		inline CVar<bool> ExtendFreeze{ false, L"Infinite Respawn" };
		inline CVar<int> AutoJoin{ false, L"Auto Join" };
		inline CVar<bool> KillstreakWeapon{ false, L"Killstreak Weapon" };
		inline CVar<bool> PartyNetworking{ false, L"Party Networking" };
		inline CVar<bool> PartyCrasher{ false, L"Party Crasher" };
		inline CVar<int> PartyMarker{ false, L"Party Marker" };
		inline CVar<bool> PartyESP{ false, L"Party ESP" };

		namespace Followbot
		{
			inline CVar<bool> Enabled{ false };
			inline CVar<bool> FriendsOnly{ false };
			inline CVar<float> Distance{ 150.f };
		}

		namespace CL_Move
		{
			inline CVar<bool> Enabled{ true, L"Enabled" };
			inline CVar<bool> Doubletap{ true, L"Doubletap" };
			inline CVar<bool> SEnabled{ false, L"Speedhack" };
			inline CVar<int> SFactor{ 1, L"Speedhack Factor" };
			inline CVar<bool> NotInAir{ true, L"Dont DT in air" };
			inline CVar<int> TeleportKey{ 0x52, L"Teleport Key" }; //R
			inline CVar<int> TeleportMode{ 0 };
			inline CVar<int> RechargeKey{ 0x48, L"Recharge Key" }; //H
			inline CVar<int> DoubletapKey{ 0x56, L"Doubletap Key" }; //V
			inline CVar<int> DTBarStyle{ 3, L"Doubletap bar style" };
			inline CVar<bool> RechargeWhileDead{ false, L"Recharge While Dead" };
			inline CVar<bool> AutoRecharge{ false, L"AutoRecharge" }; //H
			inline CVar<bool> AntiWarp{ true, L"Anti Warp" }; //H
			inline CVar<int> DTMode{ 0, L"DT Mode" }; // 0 - On Key, 1 - Always DT, 2 - Disable on key, 3 - Disabled
			inline CVar<int> DtbarOutlineHeight{ 5, L"Dt default bar height" };
			inline CVar<int> DtbarOutlineWidth{ 4, L"Dt default bar width" };
			inline CVar<int> DTTicks{ 21, L"Ticks for DT" };
			inline CVar<int> DTBarScaleY{ 12, L"DT Indicator Height" };
			inline CVar<int> DTBarScaleX{ 100, L"DT Indicator Widht" };
			inline CVar<int> DTBarY{ 60,L"DT Offset Y" };
			inline CVar<int> DTBarX{ 0,L"DT Offset X" };
			inline CVar<bool> WaitForDT{ true, L"Wait for DT" };
			inline CVar<bool> Fakelag{ true, L"Fakelag" };
			inline CVar<int> FakelagMode{ 0, L"Fakelag Mode" }; // 0 - plain, 1 - random, 2 - vel based
			inline CVar<int> FakelagMin{ 1, L"Fakelag Min" }; //	only show when FakelagMode=2
			inline CVar<int> FakelagMax{ 22, L"Fakelag Max" };
			inline CVar<bool> FakelagIndicator{ true, L"Fakelag Indicator" };
			inline CVar<bool> FakelagOnKey{ true, L"Fakelag On Key" }; // dont show when fakelagmode=2|3
			inline CVar<int> FakelagKey{ 0x54, L"Fakelag Key" }; //T
			inline CVar<int> FakelagValue{ 1, L"Fakelag value" }; // dont show when fakelagmode=2
			inline CVar<int> AutoPeekKey{ false, L"Autopeek Key" };
			inline CVar<float> AutoPeekDistance{ 200.f, L"Auto Peek distance" };
			inline CVar<bool> AutoPeekFree{ false, L"Autopeek free move" };
			namespace FLGChams
			{
				inline CVar<bool> Enabled{ false, L"Fakelag chams" };
				inline CVar<int> Material{ 1, L"Fakelag material" };
				inline Color_t FakelagColor{ 255,255,255,255 };
			}
		}

		namespace Discord
		{
			inline CVar<bool>EnableRPC{ true, L"Enable Discord RPC" };
			inline CVar<bool>IncludeClass{ true, L"Include class" };
			inline CVar<bool>IncludeMap{ true, L"Include map name" };
			inline CVar<bool>IncludeTimestamp{ true, L"Include timestamp" };
			inline CVar<int>WhatImagesShouldBeUsed{ 0, L"Image Options" }; // 0 - Big fedora, small TF2 logo; 1 - Big TF2 logo, small fedora
		}

		namespace Steam
		{
			inline CVar<bool>EnableRPC{ true, L"Enable Steam RPC" };
			inline CVar<int>MatchGroup{ 0, L"Match group" }; // 0 - Special Event; 1 - MvM Mann Up; 2 - Competitive; 3 - Casual; 4 - MvM Boot Camp;
			inline CVar<bool>OverrideMenu{ false, L"Override when in main menu" }; // Override matchgroup when in main menu
			inline CVar<int>MapText{ 1, L"Map Text" }; // 0 - Fedoraware; 1 - CUM.clab; 2 - Meowhook.club; 3 - rathook.cc; 4 - NNitro.tf; 5 - custom;
			inline CVar<int>GroupSize{ 1337, L"Player group size" };
			inline CVar<std::string> CustomText = { "M-FeD is gay", L"Custom Text" };
		}
	}

	namespace AntiHack
	{
		namespace AntiAim
		{
			inline CVar<bool> Active{ false, L"Active" };
			inline CVar<int> ToggleKey{ 0 };
			inline CVar<int> Pitch{ 0, L"Pitch" };		//0 - None, 1 - Zero, 2 - Up, 3 - Down, 4 - Fake Up, 5 - Fake Down
			inline CVar<int> YawReal{ 0, L"Yaw Real" };	//0 - None, 1 - Forward, 2 - Left, 3 - Right, 4 - Backwards
			inline CVar<int> YawFake{ 0, L"Yaw Fake" };	//0 - None, 1 - Forward, 2 - Left, 3 - Right, 4 - Backwards
			inline CVar<float> SpinSpeed{ 15.f, L"Spin Speed" };
			inline CVar<int> RandInterval{ 25, L"Random Interval" };
			inline CVar <bool> AntiBackstab{ false, L"Antibackstab" };
			inline CVar <bool> legjitter{ false, L"leg jitter" }; // frick u fourteen
			inline CVar<bool> AntiOverlap{ false, L"Anti Overlap" };
			inline CVar<bool> invalidshootpitch{ false, L"stupid feature" }; // i dont know what to name this its TRASH
			// no reason to do this for projectile and melee cause u have psilent lel
		}
		namespace Resolver
		{
			inline CVar<bool> Resolver{ false, L"Resolver" };
		}
	}

	namespace Skybox
	{
		inline int SkyboxNum = 0;
		inline std::string SkyboxName = "mr_04";
	}

	namespace Fonts
	{
		namespace FONT_ESP
		{
			inline std::string szName = "Tahoma";
			inline CVar<int> nTall = { 12, L"nTall" };
			inline CVar<int> nWeight = { 800, L"nWeight" };
			inline CVar<int> nFlags = { FONTFLAG_ANTIALIAS, L"nFlags" };
		}
		namespace FONT_ESP_NAME
		{
			inline std::string szName = "Tahoma";
			inline CVar<int> nTall = { 14, L"nTall" };
			inline CVar<int> nWeight = { 800, L"nWeight" };
			inline CVar<int> nFlags = { FONTFLAG_ANTIALIAS, L"nFlags" };
		}
		namespace FONT_ESP_COND
		{
			inline std::string szName = "Tahoma";
			inline CVar<int> nTall = { 10, L"nTall" };
			inline CVar<int> nWeight = { 800, L"nWeight" };
			inline CVar<int> nFlags = { FONTFLAG_ANTIALIAS, L"nFlags" };
		}
		namespace FONT_ESP_PICKUPS
		{
			inline std::string szName = "Tahoma";
			inline CVar<int> nTall = { 13, L"nTall" };
			inline CVar<int> nWeight = { 800, L"nWeight" };
			inline CVar<int> nFlags = { FONTFLAG_ANTIALIAS, L"nFlags" };
		}
		namespace FONT_MENU
		{
			inline std::string szName = "DejaVu Sans";
			inline CVar<int> nTall = { 16, L"nTall" };
			inline CVar<int> nWeight = { 200, L"nWeight" };
			inline CVar<int> nFlags = { FONTFLAG_ANTIALIAS, L"nFlags" };
		}
		namespace FONT_INDICATORS
		{
			inline std::string szName = "Verdana";
			inline CVar<int> nTall = { 12, L"nTall" };
			inline CVar<int> nWeight = { 0, L"nWeight" };
			inline CVar<int> nFlags = { FONTFLAG_OUTLINE, L"nFlags" };
		}
	}

	// Debug options - Don't save these!
	namespace Debug
	{
		inline CVar<bool> DebugBool{ false };
	}
}
