#pragma once

#include "../SDK/SDK.h"

template <class T>
class CVar
{
public:
	T Value;
};

namespace Vars
{
	namespace Menu
	{
		inline std::string CheatName = "Fedoraware";
		inline std::string CheatPrefix = "[FeD]";

		inline bool ModernDesign = false;
		inline bool ShowPlayerlist = false;
		inline bool ShowKeybinds = false;
		inline CVar<bool> ShowDVD{ false };
		inline CVar<bool> Vignette{ true };
		inline CVar<bool> CloseOnUnfocus{ true };

		inline CVar<int> MenuKey{ 0 };

		namespace Colors
		{
			inline Color_t MenuAccent = { 255, 101, 101, 255 };
		}
	}

	namespace CritHack
	{
		inline CVar<bool> Active{ false };
		inline CVar<bool> Indicators{ false };
		inline DragBox_t IndicatorPos{};
		inline CVar<bool> AvoidRandom{ false };
		inline CVar<bool> AlwaysMelee{ false };
		inline CVar<int> CritKey{ VK_SHIFT };
		inline CVar<bool> AutoMeleeCrit{ false };
	}

	namespace Backtrack
	{
		inline CVar<bool> Enabled{ false };
		inline CVar<int> Latency{ 0 };
		

		namespace BtChams
		{
			inline CVar<bool> Enabled{ false };
			inline CVar<bool> LastOnly{ false };
			inline CVar<bool> EnemyOnly{ false };
			inline CVar<bool> IgnoreZ{ false };
			inline CVar<int> Material{ 1 };
			inline Color_t BacktrackColor{ 255, 255, 255, 255 };
		}
	}

	namespace Aimbot
	{
		namespace Global
		{
			inline CVar<bool> Active{ false };
			inline CVar<int> AimKey{ VK_XBUTTON1 };
			inline CVar<float> AimFOV{ 15.0f };
			inline CVar<bool> AutoShoot{ false };
			inline CVar<bool> DontWaitForShot{ false };
			inline CVar<bool> FlickatEnemies{ false };
			inline CVar<bool> AimPlayers{ true };
			inline CVar<bool> AimBuildings{ false };
			inline CVar<bool> AimStickies{ false };
			inline CVar<bool> AimNPC{ false };
			inline CVar<bool> AimBombs{ false };
			inline CVar<int> IgnoreOptions{ 0b0000000 }; //disguised, fakelagging players, vaccinator, taunting, friends, deadringer,cloaked, invul
			inline CVar<int> TickTolerance{ 7 };
			inline CVar<bool> BAimLethal{ false }; // This is in global cause i remmebered hunterman exists
			inline CVar<bool> showHitboxes{ false }; // original codenz
			inline CVar<bool> ClearPreviousHitbox{ false };
			inline CVar<int> HitboxLifetime{ 2 };
		}


		namespace Hitscan
		{
			//inline CVar<bool> Active			{ false };
			inline CVar<int> SortMethod{ 0 }; //0 - FOV,		1 - Distance
			inline CVar<int> BackTrackMethod{ 0 };
			inline CVar<bool> RespectFOV{ true };
			inline CVar<int> AimMethod{ 2 }; //0 - Normal,	1 - Smooth, 2 - Silent
			inline CVar<int> AimHitbox{ 2 }; //0 - Head,		1 - Body,	2 - Auto
			inline CVar<int> ScanHitboxes{ 0b00111 }; // {legs, arms, body, pelvis, head}
			inline CVar<int> MultiHitboxes{ 0b00101 }; // {legs, arms, body, pelvis, head}
			inline CVar<int> StaticHitboxes{ 0b11000 }; // {legs, arms, body, pelvis, head}
			inline CVar<float> PointScale{ .54f };
			inline CVar<int> SmoothingAmount{ 4 };
			inline CVar<int> TapFire{ 0 }; //0 - Off, 1 - Distance, 2 - Always
			inline CVar<float> TapFireDist { 1000.f };
			inline CVar<bool> ScanBuildings{ false };
			inline CVar<bool> WaitForHeadshot{ false };
			inline CVar<bool> WaitForCharge{ false };
			inline CVar<bool> SpectatedSmooth{ false };
			inline CVar<bool> ScopedOnly{ false };
			inline CVar<bool> AutoScope{ false };
			inline CVar<bool> AutoRev{ false };
			inline CVar<bool>ExtinguishTeam{ false };
		}

		namespace Projectile
		{
			//inline CVar<bool> Active			{ false };
			inline CVar<int> SortMethod{ 0 }; //0 - FOV,		1 - Distance
			inline CVar<bool> RespectFOV{ true };
			inline CVar<int> AimMethod{ 1 }; //0 - Normal,	1 - Silent
			inline CVar<int> AimPosition{ 3 }; // 0/head, 1/body, 2/feet, 3/auto
			inline CVar<int> VisTestPoints{ 15 }; //how many points are we allowed to vis test before we stop scanning.
			inline CVar<int> ScanPoints{ 15 }; //how many "visible points" need to be reached before we stop searching.
			inline CVar<float> ScanScale{ 0.95f }; // how to scale the points.
			inline CVar<int> AllowedHitboxes{ 0b000 }; // 111, Feet, Body, Head.
			inline CVar<bool> FeetAimIfOnGround{ false };
			inline CVar<int> BounceKey{ 0x0 };
			inline CVar<bool> SplashPrediction{ false };
			inline CVar<int> MinSplashPredictionDistance{ 0 };
			inline CVar<int> MaxSplashPredictionDistance{ 10000 };
			inline Color_t PredictionColor{ 255, 255, 255, 255 };
			inline CVar<float> PredictionTime{ 2.0f };
			inline CVar<bool> PredictObscured{ false };
			inline CVar<bool> NoSpread{ false };
			inline CVar<bool> ChargeLooseCannon{ false };
			inline CVar<bool> StrafePredictionGround{ false };
			inline CVar<bool> StrafePredictionAir{ false };
			inline CVar<int> StrafePredictionSamples{ 10 };
			inline CVar<int> StrafePredictionMaxDistance{ 1000 };
			inline CVar<int> StrafePredictionMinDifference{ 10 };
			inline CVar<bool> WaitForHit{ false };
		}

		namespace Melee
		{
			//inline CVar<bool> Active			{ false };
			inline CVar<int> SortMethod{ 1 }; //0 - FOV,		1 - Distance
			inline CVar<bool> RespectFOV{ false };
			inline CVar<int> AimMethod{ 2 }; //0 - Normal,	1 - Smooth, 2 - Silent
			inline CVar<int> SmoothingAmount{ 8 };
			inline CVar<bool> RangeCheck{ false };
			inline CVar<bool> PredictSwing{ false };
			inline CVar<bool> WhipTeam{ false };
		}
	}

	namespace Triggerbot
	{
		namespace Global
		{
			inline CVar<bool> Active{ false };
			inline CVar<int> TriggerKey{ VK_XBUTTON2 };
			inline CVar<int> IgnoreOptions{ 0b00000 }; //disguised, fakelagging players, taunting, friends, cloaked, invul
		}

		namespace Shoot
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> TriggerPlayers{ false };
			inline CVar<bool> TriggerBuildings{ false };
			inline CVar<bool> HeadOnly{ false };
			inline CVar<bool> WaitForHeadshot{ false };
			inline CVar<bool> ScopeOnly{ false };
			inline CVar<float> HeadScale{ 0.7f };
		}

		namespace Stab
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> RageMode{ false };
			inline CVar<bool> Silent{ false };
			inline CVar<bool> Disguise{ false };
			inline CVar<bool> IgnRazor{ false };
			inline CVar<float> Range{ 0.9f };
		}

		namespace Detonate
		{
			inline CVar<bool> Active{ false };
			inline CVar<int> DetonateTargets{ 0b00001 };
			inline CVar<bool> Stickies{ false };
			inline CVar<bool> Flares{ false };
			inline CVar<float> RadiusScale{ 1.0f };
		}

		namespace Blast
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> Rage{ false };
			inline CVar<bool> Silent{ false };
			inline CVar<bool> ExtinguishPlayers{ false };
			inline CVar<bool> DisableOnAttack{ false };
			inline CVar<int> Fov{ 55 };
		}

		namespace Uber
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> OnlyFriends{ false };
			inline CVar<bool> PopLocal{ false };
			inline CVar<bool> AutoVacc{ false };
			inline CVar<float> HealthLeft{ 35.0f };
			inline CVar<bool> VoiceCommand{ false };
			inline CVar<int> ReactFoV{25};
			inline CVar<bool> BulletRes{ true }; inline CVar<bool> BlastRes{ true }; inline CVar<bool> FireRes{ true };
			inline CVar<int> ReactClasses{0b000000000};	//	this is intuitive
		}
	}

	namespace ESP
	{
		namespace Main
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> Outlinedbar{ false };
			inline CVar<bool> EnableTeamEnemyColors{ false };
			inline CVar<bool> DistanceToAlpha{false};
			inline CVar<bool> DormantSoundESP{ false };
			inline CVar<float> DormantTime{1.f};
			inline CVar<int> DormantDist{1000};
			inline CVar<int> NetworkedDist{2000};
		}


		namespace Players
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> ShowLocal{ false };
			inline CVar<int> IgnoreTeammates{ 0 }; //0 - OFF, 1 - All, 2 - Keep Friends
			inline CVar<int> IgnoreCloaked{ 0 }; //0 - OFF, 1 - All, 2 - Enemies Only
			inline CVar<bool> Name{ false };
			inline CVar<bool> NameCustom{ false };
			inline Color_t NameColor{ 255, 255, 255, 255 };
			inline CVar<bool> NameBox{ false };
			inline CVar<int> Uber{ 0 }; //0 - Off, 1 - Text, 2 - Bar
			inline CVar<int> Class{ 0 }; // 0 - Off, 1 - Icon, 2 - Text, 3 - Both
			inline CVar<bool> WeaponIcon{ false };
			inline CVar<bool> WeaponText{ false };
			inline CVar<int> HealthText{ 0 }; // 0 - Off, 1 - Default, 2 - Bar
			inline CVar<bool> Cond{ false };
			inline CVar<bool> FriendCond{ false };
			inline CVar<bool> Distance{ false };
			inline CVar<bool> HealthBar{ false };
			inline CVar<int> HealthBarStyle{ 0 }; // 0 - gradient, 1 - old
			inline CVar<int> Box{ 0 }; //0 - OFF, 1 - Simple, 2 - Corners
			inline CVar<int> Bones{ 0 };
			inline CVar<bool> GUID{ false };
			inline CVar<bool> Choked{ false };
			inline CVar<bool> Lines{ false };
			inline CVar<bool> Dlights{ false };
			inline CVar<float> DlightRadius{ 200.0f };
			inline CVar<float> Alpha{ 1.0f };
			inline CVar<bool> CheaterDetection{ false };
			inline CVar<bool> SniperSightlines{ false };
		}

		namespace Buildings
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> IgnoreTeammates{ false };
			inline CVar<bool> Name{ false };
			inline CVar<bool> NameCustom{ false };
			inline Color_t NameColor{ 255, 255, 255, 255 };
			inline CVar<bool> NameBox{ false };
			inline CVar<bool> Health{ false };
			inline CVar<bool> Owner{ false };
			inline CVar<bool> Level{ false };
			inline CVar<bool> Distance{ false };
			inline CVar<bool> Cond{ false };
			inline CVar<bool> HealthBar{ false };
			inline CVar<bool> TeleExitDir{ false };
			inline Color_t TeleExitDirColor{ 255, 255, 255, 255 };
			inline CVar<bool> Lines{ false };
			inline CVar<int> Box{ 0 }; //0 - OFF, 1 - Simple, 2 - Corners
			inline CVar<bool> Dlights{ false };
			inline CVar<float> DlightRadius{ 200.0f };
			inline CVar<float> Alpha{ 1.0f };
		}

		namespace World
		{
			inline CVar<bool> Active{ false };

			inline CVar<bool> HealthName{ false };
			inline CVar<bool> HealthDistance{ false };
			inline CVar<int> HealthBox{ false };
			inline CVar<bool> HealthLine{ false };

			inline CVar<bool> AmmoName{ false };
			inline CVar<bool> AmmoDistance{ false };
			inline CVar<int> AmmoBox{ false };
			inline CVar<bool> AmmoLine{ false };

			inline CVar<bool> NPCName{ false };
			inline CVar<bool> NPCDistance{ false };
			inline CVar<int> NPCBox{ false };
			inline CVar<bool> NPCLine{ false };

			inline CVar<bool> BombName{ false };
			inline CVar<bool> BombDistance{ false };
			inline CVar<int> BombBox{ false };
			inline CVar<bool> BombLine{ false };

			inline CVar<float> Alpha{ 1.0f };
		}
	}

	namespace Chams
	{
		namespace Main
		{
			inline CVar<bool> Active{ false };
		}

		namespace Players
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> Wearables{ false };
			inline CVar<bool> Weapons{ false };
			inline CVar<bool> FadeoutTeammates{ false };

			// {ignorez, material, overlay type (0 = off), active}
			inline Chams_t Local{};
			inline Chams_t FakeAng{};
			inline Chams_t Friend{};
			inline Chams_t Enemy{};
			inline Chams_t Team{};
			inline Chams_t Target{};
			inline Chams_t Ragdoll{};
		}

		namespace Buildings
		{
			inline Chams_t Local{};
			inline Chams_t Friend{};
			inline Chams_t Enemy{};
			inline Chams_t Team{};
			inline Chams_t Target{};

			inline CVar<bool> Active{ false };
			inline CVar<int> Material{ 3 }; //0 - None, 1 - Shaded, 2 - Shiny, 3 - Flat
			inline CVar<bool> IgnoreZ{ false };
		}

		namespace World
		{
			inline CVar<bool> Active{ false };
			inline CVar<int> Projectilez{ 2 };
			inline Chams_t Health{};
			inline Chams_t Ammo{};
			inline Chams_t Projectiles{};
		}

		namespace DME
		{
			inline CVar<bool> Active{ false };

			inline Chams_t Hands{ 1, 0, 0, true };
			inline Chams_t Weapon{ 1, 0, 0, true };

			inline CVar<int> HandsGlowOverlay{ 0 }; // 0 - Off,  1 - Fresnel Glow, 2 - Wireframe Glow
			inline CVar<int> HandsProxySkin{ 0 };
			inline CVar<float> HandsGlowAmount{ 1 };
			inline CVar<bool> HandsProxyWF{ false };
			inline CVar<bool> HandsRainbow{ false };
			inline CVar<bool> HandsOverlayRainbow{ false };
			inline CVar<bool> HandsOverlayPulse{ false };

			inline CVar<int> WeaponGlowOverlay{ 0 }; // 0 - Off,  1 - Fresnel Glow, 2 - Wireframe Glow
			inline CVar<int> WeaponsProxySkin{ 0 };
			inline CVar<float> WeaponGlowAmount{ 1 };
			inline CVar<bool> WeaponsProxyWF{ false };
			inline CVar<bool> WeaponRainbow{ false };
			inline CVar<bool> WeaponOverlayRainbow{ false };
			inline CVar<bool> WeaponOverlayPulse{ false };
		}
	}

	namespace Glow
	{
		namespace Main
		{
			inline CVar<bool> Active{ false };
			inline CVar<int> Type{0};	//	blur, stencil, fps stencil, wireframe
			inline CVar<int> Scale{ 5 };
		}

		namespace Players
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> ShowLocal{ false };
			inline CVar<bool> LocalRainbow{ false };
			inline CVar<int> IgnoreTeammates{ 2 }; //0 - OFF, 1 - All, 2 - Keep Friends
			inline CVar<bool> Wearables{ false };
			inline CVar<bool> Weapons{ false };
			inline CVar<float> Alpha{ 1.0f };
			inline CVar<int> Color{ 0 }; //0 - Team, 1 - Health
		}

		namespace Buildings
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> IgnoreTeammates{ false };
			inline CVar<float> Alpha{ 1.0f };
			inline CVar<int> Color{ 0 }; //0 - Team, 1 - Health
		}

		namespace World
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> Health{ false };
			inline CVar<bool> Ammo{ false };
			inline CVar<bool> NPCs{ false };
			inline CVar<bool> Bombs{ false };
			inline CVar<int> Projectiles{ 1 }; //0 - Off, 1 - All, 2 - Enemy Only
			inline CVar<float> Alpha{ 1.0f };
		}

		namespace Misc
		{
			inline CVar<bool> MovementSimLine{ false };
			inline CVar<bool> Sightlines{ false };
			inline CVar<bool> BulletTracers{ false };
		}
	}

	namespace Radar
	{
		namespace Main
		{
			inline CVar<bool> Active{ false };
			inline CVar<int> BackAlpha{ 128 };
			inline CVar<int> LineAlpha{ 255 };
			inline CVar<int> Range{ 1500 };
			inline CVar<bool> NoTitleGradient{ false };
		}

		namespace Players
		{
			inline CVar<bool> Active{ false };
			inline CVar<int> IconType{ 1 }; //0 - Scoreboard, 1 - Portraits, 2 - Avatars
			inline CVar<int> BackGroundType{ 2 }; //0 - Off, 1 - Rect, 2 - Texture
			inline CVar<bool> Outline{ false };
			inline CVar<int> IgnoreTeam{ 2 }; //0 - Off, 1 - All, 2 - Keep Friends
			inline CVar<int> IgnoreCloaked{ 0 }; //0 - Off, 1 - All, 2 - Enemies Only
			inline CVar<bool> Health{ false };
			inline CVar<int> IconSize{ 24 };
			inline CVar<bool> Height{ false };
		}

		namespace Buildings
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> Outline{ false };
			inline CVar<bool> IgnoreTeam{ false };
			inline CVar<bool> Health{ false };
			inline CVar<int> IconSize{ 18 };
		}

		namespace World
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> Health{ false };
			inline CVar<bool> Ammo{ false };
			inline CVar<int> IconSize{ 14 };
		}
	}

	namespace Visuals
	{
		inline CVar<float> FadeOutFoV{0.f};
		inline CVar<bool> CleanScreenshots{ true };
		inline CVar<bool> RemoveDisguises{ false };
		inline CVar<bool> RemoveTaunts{ false };
		inline CVar<bool> DrawOnScreenConditions{ false };
		inline CVar<bool> DrawOnScreenPing{ false };
		inline DragBox_t OnScreenConditions{ };
		inline DragBox_t OnScreenPing{ };
		inline CVar<int> FieldOfView{ 90 };
		inline CVar<int> AimFOVAlpha{ 10 };
		inline CVar<bool> RemoveScope{ false };
		inline CVar<bool> RemoveRagdolls{ false };
		inline CVar<bool> RemoveMOTD{ false };
		inline CVar<bool> RemoveScreenEffects{ false };
		inline CVar<bool> RemoveScreenOverlays{ false };
		//inline CVar<bool> RemoveForcedConvars{ false };
		inline CVar<bool> RemoveConvarQueries{ false };
		inline CVar<bool> RemoveDSP{ false };
		inline CVar<int> VisualOverlay{ false };
		inline CVar<bool> PreventForcedAngles{ false };
		inline CVar<bool> ScopeLines{ false };
		inline CVar<bool> PickupTimers{ false };
		inline CVar<bool> RemoveZoom{ false };
		inline CVar<bool> RemovePunch{ false };
		inline CVar<bool> CrosshairAimPos{ false };
		inline CVar<bool> ChatInfoText{ false };
		inline CVar<bool> ChatInfoChat{ false };
		inline CVar<bool> ChatInfoGrayScale{ false };
		inline CVar<bool> OutOfFOVArrowsOutline{ false };
		inline CVar<float> FovArrowsDist{ 0.15f };
		inline CVar<int> SpectatorList{ 2 }; //0 - Off, 1 - Default, 2 - Classic, 3 - Classic Avatars

		inline CVar<int> FreecamKey{ 0 };
		inline CVar<float> FreecamSpeed{ 10.f };

		inline CVar<int> CameraMode{ 0 };
		inline CVar<float> CameraFOV{ 90.f };

		inline CVar<bool> SpyWarning{ false };
		inline CVar<bool> SpyWarningAnnounce{ false };
		inline CVar<int> SpyWarningStyle{ 0 }; //0 - Indicator, 1 - Flash
		inline CVar<bool> SpyWarningVisibleOnly{ false };
		inline CVar<bool> SpyWarningIgnoreFriends{ false };

		inline CVar<bool> Snow{ false };
		inline CVar<bool> ToolTips{ false };

		inline CVar<bool> ThirdPerson{ false };
		inline CVar<int> ThirdPersonKey{ VK_B };
		inline CVar<bool> ThirdPersonSilentAngles{ false };
		inline CVar<bool> ThirdPersonInstantYaw{ false };
		inline CVar<bool> ThirdPersonServerHitbox{ false };
		inline CVar<bool> ThirdpersonOffset{ false };
		inline CVar<float> ThirdpersonDist{ 200.f };
		inline CVar<float> ThirdpersonRight{ 0.f };
		inline CVar<float> ThirdpersonUp{ 0.f };
		inline CVar<bool> ThirdpersonOffsetWithArrows{ false };
		inline CVar<int> ThirdpersonArrowOffsetKey{ VK_F };
		inline CVar<bool> ThirdpersonCrosshair{ false };


		inline CVar<bool> WorldModulation{ false };
		inline CVar<bool> PropWireframe{ false };
		inline CVar<bool> OverrideWorldTextures{ false };
		inline CVar<bool> SkyboxChanger{ false };
		inline CVar<bool> SkyModulation{ false };
		inline CVar<bool> ParticleColors{ false };
		inline CVar<bool> RGBParticles{ false };
		inline CVar<float> RainbowSpeed{ 1.f };
		inline CVar<bool> BulletTracer{ false };
		inline CVar<bool> AimbotViewmodel{ false };
		inline CVar<bool> ViewmodelSway{ false };
		inline CVar<float> ViewmodelSwayScale{ 5.f };
		inline CVar<float> ViewmodelSwayInterp{ 0.05f };
		inline CVar<bool> MoveSimLine{ false };
		inline CVar<bool> MoveSimSeperators{ false };
		inline CVar<int> SeperatorLength{ 12 };
		inline CVar<int> SeperatorSpacing{ 6 };
		
		inline CVar<int> ParticleTracer{ 2 };
		inline std::string ParticleName = "merasmus_zap_beam01"; // dont save this as a var its pointless
		inline CVar<bool> DoPostProcessing{ false };

		namespace Beans
		{
			inline CVar<bool> Active{ false };
			inline CVar<bool> Rainbow{ false };
			inline Color_t BeamColour{ 255, 255, 255, 255 };
			inline CVar<bool> UseCustomModel{ false };
			inline std::string Model = "sprites/physbeam.vmt";
			inline CVar<float> Life{ 2.f };
			inline CVar<float> Width{ 2.f };
			inline CVar<float> EndWidth{ 2.f };
			inline CVar<float> FadeLength{ 10.f };
			inline CVar<float> Amplitude{ 2.f };
			inline CVar<float> Brightness{ 255.f };
			inline CVar<float> Speed{ 0.2f };
			inline CVar<int> Flags{ 65792 };
			inline CVar<int> Segments{ 2 };
		}

		inline CVar<bool> BulletTracerRainbow{ false };
		inline CVar<bool> AimPosSquare{ false };
		inline CVar<bool> OutOfFOVArrows{ false };
		inline CVar<float> ArrowLength{ 21.f };
		inline CVar<float> ArrowAngle{ 100.f };
		inline CVar<float> MaxDist{ 1000.f };
		inline CVar<float> MinDist{ 200.f };
		inline CVar<int> VMOffX{ 0 };
		inline CVar<int> VMOffY{ 0 };
		inline CVar<int> VMOffZ{ 0 };
		inline Vec3 VMOffsets{};
		inline CVar<int> VMRoll{ 0 };

		inline CVar<float> NotificationLifetime{ 5.f };
		inline CVar<bool> DamageLoggerText{ false };
		inline CVar<bool> DamageLoggerChat{ false };
		inline CVar<bool> DamageLoggerConsole{ false };

		inline CVar<int> VisionModifier{ false };

		inline CVar<int> Rain{ false };
		inline CVar<bool> EquipRegionUnlock{ false };

		inline CVar<bool> NoStaticPropFade{ false };
		inline CVar<bool> ParticlesIgnoreZ{ false };


		namespace RagdollEffects
		{
			inline CVar<bool> EnemyOnly{ false };
			inline CVar<bool> Burning{ false };
			inline CVar<bool> Electrocuted{ false };
			inline CVar<bool> BecomeAsh{ false };
			inline CVar<bool> Dissolve{ false };
			inline CVar<int> RagdollType{ 0 };
		}

		namespace Skins
		{
			inline CVar<bool> Enabled{ false };
			inline CVar<int> Sheen{ 0 };
			inline CVar<int> Effect{ 0 };
			inline CVar<int> Particle{ 0 };
			inline CVar<bool> Acient{ false };
			inline CVar<bool> Override{ false };
			inline CVar<bool> Australium{ false };
		}

		namespace Fog
		{
			inline CVar<bool> DisableFog{ false };
			inline CVar<bool> CustomFog{ false };
			inline Color_t FogColor{ 255, 255, 255, 255 };
			inline Color_t FogColorSkybox{ 255, 255, 255, 255 };
			inline CVar<float> FogDensity{ 0 };
			inline CVar<float> FogDensitySkybox{ 0 };
			inline CVar<float> FogStart{ 0 };
			inline CVar<float> FogStartSkybox{ 0 };
			inline CVar<float> FogEnd{ 0 };
			inline CVar<float> FogEndSkybox{ 0 };
		}
	}

	namespace Misc
	{
		inline CVar<bool> FastDeltaStrafe{ false };
		inline CVar<int> AccurateMovement{ false };
		inline CVar<bool> AutoJump{ false };
		inline CVar<bool> AutoVote{ false };
		inline CVar<bool> DuckJump{ false };
		inline CVar<int> AutoStrafe{ 2 };
		inline CVar<bool> DirectionalOnlyOnMove{ false };
		inline CVar<bool> Directional{ false };
		inline CVar<bool> TauntSlide{ false };
		inline CVar<bool> TauntControl{ false };
		inline CVar<bool> TauntSpin{ false };
		inline CVar<int> TauntSpinKey{ false };
		inline CVar<float> TauntSpinSpeed{ 5.f };
		inline CVar<bool> TauntFollowsCamera{ false };
		inline CVar<bool> BypassPure{ false };
		inline CVar<bool> NoisemakerSpam{ false };
		inline CVar<bool> DisableInterpolation{ false };
		inline CVar<bool> MedalFlip{ false };
		inline CVar<bool> AutoRocketJump{ false };
		inline CVar<bool> NonLethalRocketJump{ true };
		inline CVar<bool> AutoScoutJump{ false };
		inline CVar<int> ChatSpam{ 0 };
		inline CVar<int> VoicechatSpam{ 0 };
		inline CVar<float> SpamInterval{ 4.f };
		inline CVar<int> NoPush{ 0 };	//	0 off, 1 on, 2 on while not afk, 3 semi while afk
		inline CVar<bool> EdgeJump{ false };
		inline CVar<bool> StoreStatistics{ false };
		inline CVar<int> StickySpamKey{ 0x0 };
		inline CVar<int> StickyChargePercent{ 0 };
		inline CVar<int> InfiniteEatKey{ 0x0 };
		inline CVar<int> EdgeJumpKey{ VK_MENU };
		inline CVar<bool> AntiAFK{ false };
		inline CVar<int> VotingOptions{ 0b000011 }; // 000011 {verbose, autovote, party, chat, console, text}
		inline CVar<bool> CheatsBypass{ false };
		inline CVar<bool> CrouchSpeed{ false };
		inline CVar<bool> FastAccel{ false };
		inline CVar<bool> FakeAccelAngle{ false };
		inline CVar<bool> AntiAutobal{ false };
		inline CVar<bool> RageRetry{ false };
		inline CVar<int> RageRetryHealth{ 20 };
		inline CVar<bool> MVMRes{ false };
		inline CVar<bool> PingReducer{ false };
		inline CVar<int> PingTarget{ 0 };
		inline CVar<bool> ExtendFreeze{ false };
		inline CVar<bool> ViewmodelFlip{ false };
		inline CVar<bool> AntiViewmodelFlip{ false }; // scuffed
		inline CVar<int> AutoJoin{ false };
		inline CVar<bool> KillstreakWeapon{ false };
		inline CVar<bool> PartyNetworking{ false };
		inline CVar<bool> PartyCrasher{ false };
		inline CVar<int> PartyMarker{ false };
		inline CVar<bool> PartyESP{ false };
		inline CVar<int> SoundBlock{ 0 };
		inline CVar<bool> ChatFlags{ false };
		inline CVar<int> MedievalChat{ 0 };
		inline CVar<bool> AutoAcceptItemDrops{ false };
		inline CVar<bool> RegionChanger{ false };
		inline CVar<int> RegionsAllowed{ 0 };
		inline CVar<int> AutoCasualQueue{ 0 };
		inline CVar<bool> JoinSpam{ false };
		inline CVar<bool> AntiVAC{ false };
		inline CVar<int> InstantAccept{ false };
		inline CVar<bool> RunescapeChat{ false };

		inline CVar<bool> Killsay{ false };
		inline std::string KillsayFile = "";
		namespace Followbot
		{
			inline CVar<bool> Enabled{ false };
			inline CVar<bool> FriendsOnly{ false };
			inline CVar<float> Distance{ 150.f };
		}

		namespace CheaterDetection
		{
			inline CVar<bool> Enabled{false};
			inline CVar<int> Methods{0b111111100};			//	Duckspeed, Aimbot, OOB pitch, angles, bhop, fakelag, simtime, high score, high accuracy
			inline CVar<int> Protections{0b111};			//	double scans, lagging client, timing out
			inline CVar<int> SuspicionGate{10};				//	the amount of infractions prior to marking someone as a cheater
			inline CVar<int> PacketManipGate{14};			//	the avg choke for someone to receive and infraction for packet choking
			inline CVar<int> BHopMaxDelay{1};				//	max groundticks used when detecting a bunny hop.
			inline CVar<int> BHopDetectionsRequired{5};		//	how many times must some be seen bunny hopping to receive an infraction
			inline CVar<float> ScoreMultiplier{2.f};		//	multiply the avg score/s by this to receive the maximum amount
			inline CVar<float> MinimumFlickDistance{20.f};	//	min mouse flick size to suspect someone of angle cheats.
			inline CVar<float> MaximumNoise{5.f};			//	max mouse noise prior to a flick to mark somebody
			inline CVar<float> MinimumAimbotFoV{3.f};		//	scaled with how many ticks a player has choked up to ->
			inline CVar<float> MaxScaledAimbotFoV{20.f};	//	self explanatory
		}

		namespace CL_Move
		{
			inline CVar<bool> Enabled{ false };
			inline CVar<bool> Doubletap{ false };
			inline CVar<bool> SafeTick{ false };
			inline CVar<bool> SafeTickAirOverride{ false };
			inline CVar<int> PassiveRecharge{ 0 };
			inline CVar<bool> SEnabled{ false };
			inline CVar<int> SFactor{ 1 };
			inline CVar<bool> NotInAir{ false };
			inline CVar<bool> StopMovement{ false };
			inline CVar<int> TeleportKey{ 0x52 }; //R
			inline CVar<int> TeleportMode{ 0 };
			inline CVar<int> TeleportFactor{ 2 };
			inline CVar<int> RechargeKey{ 0x48 }; //H
			inline CVar<int> DoubletapKey{ 0x56 }; //V
			inline CVar<bool> AutoRetain{ true };
			inline CVar<bool> RetainFakelag{ false };
			inline CVar<bool> RechargeWhileDead{ false };
			inline CVar<bool> AutoRecharge{ false }; //H
			inline CVar<bool> AntiWarp{ false }; //H
			inline CVar<int> DTMode{ 0 }; // 0 - On Key, 1 - Always DT, 2 - Disable on key, 3 - Disabled
			inline CVar<int> DTBarStyle{ 3 };
			inline DragBox_t DTIndicator{g_ScreenSize.c, g_ScreenSize.c};
			inline CVar<int> DTTicks{ 21 };
			inline CVar<bool> WaitForDT{ false };
			inline CVar<bool> Fakelag{ false };
			inline CVar<int> FakelagMode{ 0 }; // 0 - plain, 1 - random
			inline CVar<bool> WhileMoving { false };
			inline CVar<bool> WhileVisible { false };
			inline CVar<bool> PredictVisibility { false };
			inline CVar<bool> WhileUnducking { false };
			inline CVar<bool> WhileInAir { false };
			inline CVar<int> FakelagMin{ 1 }; //	only show when FakelagMode=2
			inline CVar<int> FakelagMax{ 22 };
			inline CVar<bool> FakelagOnKey{ false }; // dont show when fakelagmode=2|3
			inline CVar<int> FakelagKey{ 0x54 }; //T
			inline CVar<int> FakelagValue{ 1 }; // dont show when fakelagmode=2
			inline CVar<int> AutoPeekKey{ false };
			inline CVar<float> AutoPeekDistance{ 200.f };
			inline CVar<bool> AutoPeekFree{ false };

			namespace FLGChams
			{
				inline CVar<bool> Enabled{ false };
				inline CVar<int> Material{ 1 };
				inline Color_t FakelagColor{ 255, 255, 255, 255 };
			}
		}

		namespace Discord
		{
			inline CVar<bool> EnableRPC{ false };
			inline CVar<bool> IncludeClass{ false };
			inline CVar<bool> IncludeMap{ false };
			inline CVar<bool> IncludeTimestamp{ false };
			inline CVar<int> WhatImagesShouldBeUsed{ 0 }; // 0 - Big fedora, small TF2 logo; 1 - Big TF2 logo, small fedora
		}

		namespace Steam
		{
			inline CVar<bool> EnableRPC{ false };
			inline CVar<int> MatchGroup{ 0 }; // 0 - Special Event; 1 - MvM Mann Up; 2 - Competitive; 3 - Casual; 4 - MvM Boot Camp;
			inline CVar<bool> OverrideMenu{ false }; // Override matchgroup when in main menu
			inline CVar<int> MapText{ 1 }; // 0 - Fedoraware; 1 - CUM.clab; 2 - Meowhook.club; 3 - rathook.cc; 4 - NNitro.tf; 5 - custom;
			inline CVar<int> GroupSize{ 1337 };
			inline CVar<std::string> CustomText = { "M-FeD is gay" };
		}
	}

	namespace AntiHack
	{
		namespace AntiAim
		{
			inline CVar<bool> Active{ false };
			inline CVar<int> ToggleKey{ 0 };
			inline CVar<int> InvertKey{ 0 };
			inline CVar<int> ManualKey{ 0 };
			inline CVar<int> Pitch{ 0 }; //0 - None, 1 - Zero, 2 - Up, 3 - Down, 4 - Fake Up, 5 - Fake Down
			inline CVar<int> YawReal{ 0 }; //0 - None, 1 - Forward, 2 - Left, 3 - Right, 4 - Backwards
			inline CVar<int> YawFake{ 0 }; //0 - None, 1 - Forward, 2 - Left, 3 - Right, 4 - Backwards
			inline CVar<int> BaseYawMode{ 0 };
			inline CVar<float> BaseYawOffset{ 0.f };
			inline CVar<float> SpinSpeed{ 15.f };
			inline CVar<float> CustomRealPitch{ 0.f };
			inline CVar<float> CustomRealYaw{ 0 };
			inline CVar<float> CustomFakeYaw{ 0 };
			inline CVar<float> FakeJitter{ 0 };
			inline CVar<float> RealJitter{ 0 };
			inline CVar<int> RandInterval{ 25 };
			inline CVar<bool> AntiBackstab{ false };
			inline CVar<bool> LegJitter{ false }; // frick u fourteen
			inline CVar<bool> AntiOverlap{ false };
			inline CVar<bool> InvalidShootPitch{ false }; // i dont know what to name this its TRASH
			// no reason to do this for projectile and melee cause u have psilent lel
		}

		namespace Resolver
		{
			inline CVar<bool> Resolver{ false };
			inline CVar<bool> AutoResolveCheaters{ false };
			inline CVar<bool> IgnoreAirborne{ false };
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
			inline CVar<int> nTall = { 12 };
			inline CVar<int> nWeight = { 800 };
			inline CVar<int> nFlags = { FONTFLAG_ANTIALIAS };
		}

		namespace FONT_ESP_NAME
		{
			inline std::string szName = "Tahoma";
			inline CVar<int> nTall = { 14 };
			inline CVar<int> nWeight = { 800 };
			inline CVar<int> nFlags = { FONTFLAG_ANTIALIAS };
		}

		namespace FONT_ESP_COND
		{
			inline std::string szName = "Tahoma";
			inline CVar<int> nTall = { 10 };
			inline CVar<int> nWeight = { 800 };
			inline CVar<int> nFlags = { FONTFLAG_ANTIALIAS };
		}

		namespace FONT_ESP_PICKUPS
		{
			inline std::string szName = "Tahoma";
			inline CVar<int> nTall = { 13 };
			inline CVar<int> nWeight = { 800 };
			inline CVar<int> nFlags = { FONTFLAG_ANTIALIAS };
		}

		namespace FONT_MENU
		{
			inline std::string szName = "DejaVu Sans";
			inline CVar<int> nTall = { 16 };
			inline CVar<int> nWeight = { 200 };
			inline CVar<int> nFlags = { FONTFLAG_ANTIALIAS };
		}

		namespace FONT_INDICATORS
		{
			inline std::string szName = "Verdana";
			inline CVar<int> nTall = { 12 };
			inline CVar<int> nWeight = { 0 };
			inline CVar<int> nFlags = { FONTFLAG_OUTLINE };
		}
	}

	// Debug options
	namespace Debug
	{
		inline CVar<bool> DebugInfo{ false };
	}
}
