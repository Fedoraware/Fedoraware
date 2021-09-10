#pragma once

#include "../SDK/SDK.h"

template <class T>
class CVar
{
public:
	T m_Var;
	const wchar_t* m_szDisplayName;
};

namespace Vars
{
	namespace Menu
	{
		inline Rect_t Position		= { 100, 100, 800, 500 };
		inline const auto Title		= XorStr(L"Team Fortress 2");
		inline int TitleBarH		= 25;

		inline int SpacingX			= 8;
		inline int SpacingY			= 4;
		inline int SpacingText		= 4;

		inline int CheckBoxW		= 14;
		inline int CheckBoxH		= 14;
		inline int CheckBoxFillSize = 3;

		inline int ButtonW			= 90;
		inline int ButtonH			= 20;

		inline int ButtonWSmall		= 70;
		inline int ButtonHSmall		= 16;

		inline int ComboBoxW		= 90;
		inline int ComboBoxH		= 14;

		inline int InputBoxW		= 90;
		inline int InputBoxH		= 14;
		inline int InputColorBoxW	= 30;

		namespace Colors
		{
			inline Color_t WindowBackground		= { 30, 30, 30, 255 };
			inline Color_t TitleBar				= { 50, 50, 50, 255 };
			inline Color_t Text					= { 255, 255, 255, 190 };
			inline Color_t Widget				= { 20, 20, 20, 255 };
			inline Color_t WidgetActive			= { 70, 70, 70, 255 };
			inline Color_t OutlineMenu			= { 255, 255, 255, 20 };
			inline Color_t FeatureOn			= { 255, 255, 255, 0 };
			inline Color_t FeatureOff			= { 255, 255, 255, 0 };
			inline Color_t FeatureBackground	= { 255, 255, 255, 0 };
			inline Color_t FeatureOutline		= { 255, 255, 255, 0 };
		}

		inline CVar<bool> LegacyMenu{ true, L"Legacy menu" };
	}

	namespace Aimbot
	{
		namespace Global
		{
			inline CVar<bool> Active				{ true, L"Active" };
			inline CVar<int> AimKey					{ VK_LSHIFT, L"Aim Key" };
			inline CVar<bool> AutoShoot				{ true, L"Auto Shoot" };
			inline CVar<bool> AimPlayers			{ true, L"Aim Players" };
			inline CVar<bool> AimBuildings			{ true, L"Aim Buildings" };
			inline CVar<bool> IgnoreInvlunerable	{ true, L"Ignore Invulnerable" };
			inline CVar<bool> IgnoreCloaked			{ false, L"Ignore Cloaked" };
			inline CVar<bool> IgnoreFriends			{ true, L"Ignore Friends" };
			inline CVar<bool> IgnoreTaunting		{ true, L"Ignore Taunting" };
		}

		namespace Hitscan
		{
			inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<int> SortMethod			{ 0, L"Sort Method" };	//0 - FOV,		1 - Distance
			inline CVar<int> AimMethod			{ 1, L"Aim Method" };	//0 - Normal,	1 - Smooth, 2 - Silent
			inline CVar<int> AimHitbox			{ 2, L"Aim Hitbox" };	//0 - Head,		1 - Body,	2 - Auto
			inline CVar<float> AimFOV			{ 15.0f, L"Aim FOV" };
			inline CVar<float> SmoothingAmount	{ 4.0f, L"Smoothing Amount" };
			inline CVar<int> TapFire			{ 1, L"Tap Fire" };	//0 - Off, 1 - Distance, 2 - Always
			inline CVar<bool> ScanHitboxes		{ true, L"Scan Body" };
			inline CVar<bool> ScanHead			{ true, L"Scan Head" };
			inline CVar<bool> ScanBuildings		{ true, L"Scan Buildings" };
			inline CVar<bool> WaitForHeadshot	{ true, L"Wait For Headshot" };
			inline CVar<bool> WaitForCharge     { true, L"Wait For Charge" };
			inline CVar<bool> SpectatedSmooth	{ false, L"Spectated Smooth" };
			inline CVar<bool> ScopedOnly		{ false, L"Scoped Only" };
			inline CVar<bool> AutoScope			{ false, L"Auto Scope" };
			inline CVar<bool> AutoRev			{ false, L"Auto Rev" };
		}

		namespace Projectile
		{
			inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<bool> PerformanceMode	{ true, L"Performance Mode" };
			inline CVar<int> SortMethod			{ 0, L"Sort Method" };	//0 - FOV,		1 - Distance
			inline CVar<int> AimMethod			{ 1, L"Aim Method" };	//0 - Normal,	1 - Silent
			inline CVar<int> AimPosition		{ 2, L"Aim Position" };	//0 - Body,		1 - Feet,	2 - Auto
			inline CVar<float> AimFOV			{ 25.0f, L"Aim FOV" };
		}

		namespace Melee
		{
			inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<int> SortMethod			{ 1, L"Sort Method" };	//0 - FOV,		1 - Distance
			inline CVar<int> AimMethod			{ 1, L"Aim Method" };	//0 - Normal,	1 - Smooth, 2 - Silent
			inline CVar<float> AimFOV			{ 45.0f, L"Aim FOV" };
			inline CVar<float> SmoothingAmount	{ 8.0f, L"Smoothing Amount" };
			inline CVar<bool> RangeCheck		{ true, L"Range Check" };
			inline CVar<bool> PredictSwing		{ true, L"Predict Swing" };
			inline CVar<bool> WhipTeam			{ true, L"Whip Teammates" };
		}
	}

	namespace Triggerbot
	{
		namespace Global
		{
			inline CVar<bool> Active				{ false, L"Active" };
			inline CVar<int> TriggerKey				{ VK_LSHIFT, L"Trigger Key" };
			inline CVar<bool> IgnoreInvlunerable	{ true, L"Ignore Invulnerable" };
			inline CVar<bool> IgnoreCloaked			{ false, L"Ignore Cloaked" };
			inline CVar<bool> IgnoreFriends			{ false, L"Ignore Friends" };
		}

		namespace Shoot
		{
			inline CVar<bool> Active			{ false, L"Active" };
			inline CVar<bool> TriggerPlayers	{ true, L"Trigger Players" };
			inline CVar<bool> TriggerBuildings	{ true, L"Trigger Buildings" };
			inline CVar<bool> HeadOnly			{ false, L"Head Only" };
			inline CVar<bool> WaitForCharge		{ true, L"Wait For Charge" };
			inline CVar<float> HeadScale		{ 0.7f, L"Head Scale" };
		}

		namespace Stab
		{
			inline CVar<bool> Active	{ true, L"Active" };
			inline CVar<bool> RageMode	{ false, L"Rage Mode" };
			inline CVar<bool> Silent	{ false, L"Silent" };
			inline CVar<bool> Disguise	{ false, L"Disguise" };
			inline CVar<bool> IgnRazor	{ false, L"Ignore Razorback" };
			inline CVar<float> Range	{ 0.9f, L"Range" };
		}

		namespace Detonate
		{
			inline CVar<bool> Active	    { true, L"Active" };
			inline CVar<bool> Stickies      { true, L"Detonate Stickies" };
			inline CVar<bool> Flares        { true, L"Detonate Flares" };
			inline CVar<float> RadiusScale	{ 1.0f, L"Radius Scale" };
		}

		namespace Blast
		{
			inline CVar<bool> Active    { false, L"Active" };
			inline CVar<bool> Rage      { false, L"Rage" };
			inline CVar<bool> Silent	{ false, L"Silent" };
			inline CVar<int> Fov	    { 55,    L"FOV" };
		}

		namespace Uber
		{
			inline CVar<bool> Active		{ false, L"Active" };
			inline CVar<bool> OnlyFriends	{ false, L"Only Friends" };
			inline CVar<bool> PopLocal      { false, L"Pop on local" };
			inline CVar<float> HealthLeft   { 35.0f, L"Health left" };
		}
	}

	namespace ESP
	{
		namespace Main
		{
			inline CVar<bool> Active	{ true, L"Active" };
			inline CVar<int> Outline	{ 2, L"Outline" }; //0 - OFF, 1 - Text Only, 2 - All
		}

		namespace Players
		{
			inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<bool> ShowLocal			{ true, L"Show Local" };
			inline CVar<int> IgnoreTeammates	{ 2, L"Ignore Teammates" };	//0 - OFF, 1 - All, 2 - Keep Friends
			inline CVar<int> IgnoreCloaked		{ 0, L"Ignore Cloaked" };	//0 - OFF, 1 - All, 2 - Enemies Only
			inline CVar<bool> Name				{ true, L"Name" };
			inline CVar<bool> NameBox			{ true, L"Name box" };
			inline CVar<int> Uber				{ 2, L"Uber" }; //0 - Off, 1 - Text, 2 - Bar
			inline CVar<int> Class				{ 1, L"Class" }; // 0 - Off, 1 - Icon, 2 - Text, 3 - Both
			inline CVar<bool> Health			{ false, L"Health" };
			inline CVar<bool> Cond				{ true, L"Cond" };
			inline CVar<bool> HealthBar			{ true, L"Health Bar" };
			inline CVar<int> Box				{ 0, L"Box" };	//0 - OFF, 1 - Simple, 2 - Corners
			inline CVar<int> Bones				{ 0, L"Bones" };
			inline CVar<bool> GUID              { false, L"GUID" };
			inline CVar<bool> Lines             { false, L"Lines" };
			inline CVar<bool> Dlights			{ false, L"Dlights" };
			inline CVar<float> DlightRadius		{ 200.0f, L"DLight Radius" };
			inline CVar<float> Alpha			{ 1.0f, L"Alpha" };
		}

		namespace Buildings
		{
			inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<bool> IgnoreTeammates	{ true, L"Ignore Teammates" };
			inline CVar<bool> Name				{ true, L"Name" };
			inline CVar<bool> NameBox				{ true, L"Name box" };
			inline CVar<bool> Health			{ false, L"Health" };
			inline CVar<bool> Owner             { false, L"Owner" };
			inline CVar<bool> Level				{ true, L"Level" };
			inline CVar<bool> Cond				{ true, L"Cond" };
			inline CVar<bool> HealthBar			{ true, L"Health Bar" };
			inline CVar<bool> Lines				{ false, L"Lines" };
			inline CVar<int> Box				{ 0, L"Box" };	//0 - OFF, 1 - Simple, 2 - Corners
			inline CVar<bool> Dlights			{ false, L"Dlights" };
			inline CVar<float> DlightRadius		{ 200.0f, L"DLight Radius" };
			inline CVar<float> Alpha			{ 1.0f, L"Alpha" };
		}

		namespace World
		{
			inline CVar<bool> Active		{ true, L"Active" };
			inline CVar<bool> HealthText	{ true, L"Health Text" };
			inline CVar<bool> AmmoText		{ true, L"Ammo Text" };
			inline CVar<float> Alpha		{ 1.0f, L"Alpha" };
		}
	}

	namespace Chams
	{
		namespace Main
		{
			inline CVar<bool> Active{ false , L"Active" };
		}

		namespace Players
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> ShowLocal{ true, L"Show Local" };
			inline CVar<int> IgnoreTeammates{ 2, L"Ignore Teammates" };	//0 - OFF, 1 - All, 2 - Keep Friends
			inline CVar<bool> Wearables{ true, L"Render Wearables" };
			inline CVar<bool> Weapons{ true, L"Render Weapons" };
			inline CVar<int> Material{ 1, L"Material" }; //0 - None, 1 - Shaded, 2 - Shiny, 3 - Flat
			inline CVar<bool> IgnoreZ{ false , L"IgnoreZ" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
			inline CVar<bool> GlowOverlay { false , L"Glow overlay" };
		}

		namespace Buildings
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> IgnoreTeammates{ true, L"Ignore Teammates" };
			inline CVar<int> Material{ 1, L"Material" }; //0 - None, 1 - Shaded, 2 - Shiny, 3 - Flat
			inline CVar<bool> IgnoreZ{ false , L"IgnoreZ" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
		}

		namespace World
		{
			inline CVar<bool> Active{ false, L"Active" };
			inline CVar<bool> Health{ true, L"Health" };
			inline CVar<bool> Ammo{ true, L"Ammo" };
			inline CVar<int> Projectiles{ 1, L"Projectiles" }; //0 - Off, 1 - All, 2 - Enemy Only
			inline CVar<int> Material{ 1, L"Material" }; //0 - None, 1 - Shaded, 2 - Shiny, 3 - Flat
			inline CVar<bool> IgnoreZ{ false , L"IgnoreZ" };
			inline CVar<float> Alpha{ 1.0f, L"Alpha" };
		}

		namespace DME
		{
			inline CVar<bool> Active{ false, L"Active" };

			inline CVar<int> Hands{ 0, L"Hands" }; //0 - Original, 1 - Shaded, 2 - Shiny, 3 - Flat, 4 - WF-Shaded, 5 - WF-Shiny, 6 - WF-Flat
			inline CVar<float> HandsAlpha{ 1.0f, L"Hands Alpha" };
			inline CVar<bool> HandsGlowOverlay{ false, L"Hands Glow" };
			inline CVar<int> HandsRimMultiplier { 10, L"Hands Rimlight Boost" };

			inline CVar<int> Weapon{ 0, L"Weapon" }; //0 - Original, 1 - Shaded, 2 - Shiny, 3 - Flat, 4 - WF-Shaded, 5 - WF-Shiny, 6 - WF-Flat
			inline CVar<float> WeaponAlpha{ 1.0f, L"Weapon Alpha" };
			inline CVar<bool> WeaponGlowOverlay{ false, L"Weapon Glow" };
			inline CVar<int> WeaponRimMultiplier{ 10, L"Weapon Rimlight Boost" };
		}
	}

	namespace Glow
	{
		namespace Main
		{
			inline CVar<bool> Active{ false, L"Active" };
			inline CVar<int> Scale{ 5, L"Scale" };
		}

		namespace Players
		{
			inline CVar<bool> Active{ true, L"Active" };
			inline CVar<bool> ShowLocal{ true, L"Show Local" };
			inline CVar<bool> LocalRainbow{ true, L"Local player is rainbow" };
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
			inline CVar<bool> Active{ false, L"Active" };
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
			inline CVar<int>  Size{ 100, L"Size" };
			inline CVar<int>  Range{ 1500, L"Range" };
		}

		namespace Players
		{
			inline CVar<bool> Active			{ true, L"Active" };
			inline CVar<int>  IconType			{ 1, L"Icon" }; //0 - Scoreboard, 1 - Portraits, 2 - Avatars
			inline CVar<int>  BackGroundType	{ 2, L"Background" }; //0 - Off, 1 - Rect, 2 - Texture
			inline CVar<bool> Outline			{ true, L"Outline" };
			inline CVar<int>  IgnoreTeam		{ 2, L"Ignore Teammates" };  //0 - Off, 1 - All, 2 - Keep Friends
			inline CVar<int>  IgnoreCloaked		{ 0, L"Ignore Cloaked" };	//0 - Off, 1 - All, 2 - Enemies Only
			inline CVar<bool> Health			{ false, L"Health Bar" };
			inline CVar<int>  IconSize			{ 24, L"Icon Size" };
		}

		namespace Buildings
		{
			inline CVar<bool> Active		{ true, L"Active" };
			inline CVar<bool> Outline		{ false, L"Outline" };
			inline CVar<bool> IgnoreTeam	{ true, L"Ignore Team" };
			inline CVar<bool> Health		{ false, L"Health Bar" };
			inline CVar<int>  IconSize		{ 18, L"Icon Size" };
		}

		namespace World
		{
			inline CVar<bool> Active     { false, L"Active" };
			inline CVar<bool> Health     { true, L"Health" };
			inline CVar<bool> Ammo       { true, L"Ammo" };
			inline CVar<int>  IconSize	 { 14, L"Icon Size" };
		}
	}

	namespace Visuals
	{
		inline CVar<bool> RemoveDisguises   { false, L"Remove Disguises" };
		inline CVar<bool> RemoveTaunts		{ false, L"Remove Taunts" };
		inline CVar<int> FieldOfView		{ 110, L"Field of View" };
		inline CVar<int> AimFOVAlpha		{ 10, L"Aim FOV Alpha" };
		inline CVar<bool> RemoveScope		{ true, L"Remove Scope" };
		inline CVar<bool> RemoveZoom		{ true, L"Remove Zoom" };
		inline CVar<bool> RemovePunch		{ false, L"Remove Recoil" };
		inline CVar<bool> CrosshairAimPos	{ true, L"Crosshair At Aim Pos" };
		inline CVar<bool> ChatInfo			{ true, L"Show Class Changes" };

		inline CVar<int> SpectatorList { 1, L"Spectator List" }; //0 - Off, 1 - Default, 2 - Classic, 3 - Classic Avatars

		inline CVar<bool> SpyWarning				{ true, L"Active" };
		inline CVar<bool> SpyWarningAnnounce		{ true, L"Announce" };
		inline CVar<int> SpyWarningStyle			{ 0, L"Style" }; //0 - Indicator, 1 - Flash
		inline CVar<bool> SpyWarningVisibleOnly		{ true, L"Visible Only" };
		inline CVar<bool> SpyWarningIgnoreFriends	{ true, L"Ignore Friends" };

		inline CVar<bool> Snow{ true, L"Menu Snow" };
		inline CVar<bool> ToolTips{ false, L"Menu tooltips" };

		inline CVar<bool> ThirdPerson				{ false, L"Active" };
		inline CVar<int> ThirdPersonKey				{ 0x0, L"Toggle Key"};
		inline CVar<bool> ThirdPersonSilentAngles	{ true, L"Silent Angles" };
		inline CVar<bool> ThirdPersonInstantYaw		{ true, L"Instant Yaw" };

		inline CVar<bool> WorldModulation			{ false, L"World Modulation" };
		inline CVar<bool> OverrideWorldTextures		{ false, L"World Texture Override" };
		inline CVar<bool> SkyboxChanger				{ true, L"Skybox changer" };
		inline CVar<bool> SkyModulation				{ true, L"Skybox modulation" };
		inline CVar<bool> BulletTracer				{ true, L"Bullet tracers" };
		inline CVar<bool> BulletTracerRainbow		{ true, L"Rainbow tracers" };
		inline CVar<bool> AimPosSquare{ true, L"Aim position square" };
		inline CVar<bool> OutOfFOVArrows			{ true, L"Out of FOV arrows" };
		inline CVar<float> ArrowLength{ 20.f, L"Out of FOV arrow length" };
		inline CVar<float> ArrowAngle{ 60.f, L"Out of FOV arrow angle" };
		inline CVar<float> MaxDist{ 1000.f, L"How far until the arrows are no longer visible" };
		inline CVar<float> MinDist{ 200.f, L"How many units till the arrows are fully opaque" };

		inline CVar<int> despawnTime{ 60, L"How many ticks to despawn a damage log event" };
		inline CVar<int> damageLogger{ 0, L"Enable damage logger" };


		namespace Skins
		{
			inline CVar<bool> Enabled				{ false, L"Active" };
			inline CVar<int> Sheen					{ 0, L"Sheen" };
			inline CVar<int> Effect					{ 0, L"Effect" };
			inline CVar<int> Particle				{ 0, L"Particle" };
			inline CVar<bool> Acient				{ false, L"Ancient" };
			inline CVar<bool> Override				{ false, L"Style Override" };
			inline CVar<bool> Australium				{ false, L"Australium" };
		}
	}

	namespace Misc
	{
		inline CVar<bool> AutoJump				{ true, L"Auto Jump" };
		inline CVar<int> AutoStrafe				{ 0, L"Auto Strafe" };
		inline CVar<bool> Directional			{ false, L"Directional" };
		inline CVar<bool> TauntSlide			{ false, L"Taunt Slide" };
		inline CVar<bool> TauntControl			{ false, L"Taunt Control" };
		inline CVar<bool> BypassPure            { true, L"Bypass Pure" };
		inline CVar<bool> NoisemakerSpam        { false, L"Noisemaker Spam" };
		inline CVar<bool> DisableInterpolation	{ true, L"Disable Interpolation" };
		inline CVar<bool> MedalFlip				{ true, L"Medal Flip" };
		inline CVar<bool> AutoRocketJump		{ false, L"Auto RocketJump" };
		inline CVar<bool> ChatSpam              { false, L"Chat Spam" };
		inline CVar<bool> NoPush				{ false, L"No Push" };
		inline CVar<bool> EdgeJump				{ false, L"Edge Jump" };
		inline CVar<int> EdgeJumpKey			{ VK_MENU, L"Edge Jump key"};
		inline CVar<bool> AntiAFK				{ false, L"Anti AFK" };
		inline CVar<bool> VoteRevealer			{ false, L"Reveal votes" };
		inline CVar<bool> VotesInChat			{ false, L"Reveal votes to party" };
		inline CVar<bool> CheatsBypass			{ false, L"Force sv_cheats to 1 (clientside only)" };
		inline CVar<int> Roll					{ false, L"Super Crouch Speed" };

		namespace CL_Move
		{
			inline CVar<bool> Enabled       { true, L"Enabled" };
			inline CVar<bool> Doubletap		{ true, L"Doubletap" };
			inline CVar<bool> NotInAir		{ true, L"Dont DT in air" };
			inline CVar<int> TeleportKey	{ 0x52, L"Teleport Key" }; //R
			inline CVar<int> RechargeKey	{ 0x48, L"Recharge Key" }; //H
			inline CVar<int> DoubletapKey	{ 0x56, L"Doubletap Key" }; //V
			inline CVar<int> DTBarStyle{ 0, L"Doubletap bar style" };
			inline CVar<bool> AlwaysDT{ true, L"DT when full" };
			inline CVar<bool> WaitForDT{ true, L"Wait for DT" };
			inline CVar<bool> Fakelag{ true, L"Fakelag" };
			inline CVar<bool> FakelagOnKey{ true, L"Fakelag On Key" };
			inline CVar<int> FakelagKey	{ 0x54, L"Fakelag Key" }; //T
			inline CVar<int> FakelagValue{ 1, L"Fakelag value" };
		}
	}

	namespace AntiHack
	{
		namespace AntiAim
		{
			inline CVar<bool> Active	{ false, L"Active" };
			inline CVar<int> Pitch		{ 0, L"Pitch" };		//0 - None, 1 - Up, 2 - Down, 3 - Fake Up, 4 - Fake Down
			inline CVar<int> YawReal	{ 0, L"Yaw Real" };	//0 - None, 1 - Left, 2 - Right, 3 - Backwards
			inline CVar<int> YawFake	{ 0, L"Yaw Fake" };	//0 - None, 1 - Left, 2 - Right, 3 - Backwards
		}
	}

	namespace Skybox
	{
		inline int skyboxnum = 0;
		inline std::string nigger = "hello hello :)";
		inline std::string SkyboxName = "mr_04";
	}
}