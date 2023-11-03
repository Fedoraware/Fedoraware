#pragma once

#include "../SDK/SDK.h"

#include <typeinfo>

// forward declartion of ConfigVar
template <class T>
class ConfigVar;

class CVarBase
{
public:
	size_t m_iType;
	std::string m_sName;
	bool m_bVisual; // TODO: Use group enum?

	// getter for ConfigVar
	template <class T>
	ConfigVar<T>* GetVar()
	{
		if (typeid(T).hash_code() != m_iType)
			return nullptr;

		return reinterpret_cast<ConfigVar<T>*>(this);
	}
};

template <class T>
class ConfigVar : public CVarBase
{
public:
	T Value;
	ConfigVar(T value, std::string name, bool visual = false);
};

inline std::vector<CVarBase*> g_Vars;

template<class T>
inline ConfigVar<T>::ConfigVar(T value, std::string name, bool visual)
{
	Value = value;
	m_iType = typeid(T).hash_code();
	m_sName = name;
	g_Vars.push_back(this);
	m_bVisual = visual;
}

// These are dumb workarounds to make this compatible with the old system... Hope this works :)
#define NAMESPACE_BEGIN(name) \
	namespace name { \
		inline std::string GetNamespace() { return "Vars::" + std::string(#name) + "::"; } \
		inline std::string GetSubname() { return ""; } \

#define SUBNAMESPACE_BEGIN(name) \
	namespace name { \
		inline std::string GetSubname() { return std::string(#name) + "::"; } \

// u dont actually have to put the name here, or even use it, but it makes it easier to read
#define NAMESPACE_END(name) \
	}
#define SUBNAMESPACE_END(name) \
	}

#define CVar(name, value, ...) \
	inline ConfigVar<decltype(value)> name{ value, GetNamespace() + GetSubname() + std::string(#name), __VA_ARGS__ }; \

#define IS_VISUAL true

// to indent stuff, just dont put a ; at the end of the previous line
namespace Vars
{
	NAMESPACE_BEGIN(Menu);

	CVar(CheatName, std::string("Fedoraware"), IS_VISUAL);
	CVar(CheatPrefix, std::string("[FeD]"), IS_VISUAL);
	CVar(ModernDesign, false, IS_VISUAL);
	CVar(DrawSnow, false, IS_VISUAL);
	CVar(ShowPlayerlist, false, IS_VISUAL);
	CVar(ShowKeybinds, false, IS_VISUAL);
	CVar(Vignette, false, IS_VISUAL);
	CVar(CloseOnUnfocus, false, IS_VISUAL);
	CVar(MenuKey, 0);

	SUBNAMESPACE_BEGIN(Colors)
		CVar(MenuAccent, Color_t(255, 101, 101, 255), IS_VISUAL);
	SUBNAMESPACE_END(Colors);

	NAMESPACE_END(Menu);

	NAMESPACE_BEGIN(CritHack);

	CVar(Active, false);
	CVar(Indicators, false, IS_VISUAL);
	CVar(IndicatorPos, DragBox_t(), IS_VISUAL);
	CVar(AvoidRandom, false);
	CVar(AlwaysMelee, false);
	CVar(CritKey, VK_SHIFT);
	CVar(AutoMeleeCrit, false);

	NAMESPACE_END(CritHack);

	NAMESPACE_BEGIN(Backtrack);

	CVar(Enabled, true);
	CVar(UnchokePrediction, true);
	CVar(AllowForward, true);
	CVar(Latency, 0);

	SUBNAMESPACE_BEGIN(BtChams)
		CVar(Enabled, false, IS_VISUAL)
		CVar(LastOnly, false, IS_VISUAL)
		CVar(EnemyOnly, false, IS_VISUAL)
		CVar(IgnoreZ, false, IS_VISUAL)
		CVar(Material, 1, IS_VISUAL)
		CVar(Gradient, false, IS_VISUAL)
		CVar(BacktrackColor, Color_t(255, 255, 255, 255), IS_VISUAL)
		CVar(BacktrackColor2, Color_t(0, 0, 0, 255), IS_VISUAL);
	SUBNAMESPACE_END(BacktrackChams);

	NAMESPACE_END(Backtrack);

	NAMESPACE_BEGIN(Aimbot);

	SUBNAMESPACE_BEGIN(Global)
		CVar(Active, false)
		CVar(AimKey, VK_XBUTTON1)
		CVar(AimFOV, 15.0f)
		CVar(AutoShoot, false)
		CVar(DontWaitForShot, false)
		CVar(FlickatEnemies, false)
		CVar(AimPlayers, true)
		CVar(AimBuildings, false)
		CVar(AimStickies, false)
		CVar(AimNPC, false)
		CVar(AimBombs, false)
		CVar(IgnoreOptions, 0b0000000) //disguised, fakelagging players, vaccinator, taunting, friends, deadringer,cloaked, invul
		CVar(TickTolerance, 7)
		CVar(BAimLethal, false) // This is in global cause i remmebered hunterman exists
		CVar(showHitboxes, false, IS_VISUAL) // original codenz
		CVar(ClearPreviousHitbox, false, IS_VISUAL)
		CVar(HitboxLifetime, 2, IS_VISUAL);
	SUBNAMESPACE_END(Global);

	SUBNAMESPACE_BEGIN(Hitscan)
		CVar(SortMethod, 0) //0 - FOV,		1 - Distance
		CVar(BackTrackMethod, 0)
		CVar(RespectFOV, true)
		CVar(AdaptiveMultiPoint, true)
		CVar(RandomPoints, 8)
		CVar(AimMethod, 2) //0 - Normal,	1 - Smooth, 2 - Silent
		CVar(AimHitbox, 2) //0 - Head,		1 - Body,	2 - Auto
		CVar(ScanHitboxes, 0b00111) // {legs, arms, body, pelvis, head}
		CVar(MultiHitboxes, 0b00101) // {legs, arms, body, pelvis, head}
		CVar(StaticHitboxes, 0b11000) // {legs, arms, body, pelvis, head}
		CVar(PointScale, .54f)
		CVar(SmoothingAmount, 4)
		CVar(TapFire, 0) //0 - Off, 1 - Distance, 2 - Always
		CVar(TapFireDist, 1000.f)
		CVar(TapFireCheckForNSS, false)
		CVar(ScanBuildings, false)
		CVar(WaitForHeadshot, false)
		CVar(WaitForCharge, false)
		CVar(SpectatedSmooth, false)
		CVar(ScopedOnly, false)
		CVar(AutoScope, false)
		CVar(AutoRev, false)
		CVar(ExtinguishTeam, false)
		CVar(PreserveTarget, false)
		CVar(IgnorePreservedFoV, false);
	SUBNAMESPACE_END(Hitscan);

	SUBNAMESPACE_BEGIN(Projectile)
		CVar(SortMethod, 0) //0 - FOV,		1 - Distance
		CVar(RespectFOV, true)
		CVar(AimMethod, 1) //0 - Normal,	1 - Smooth, 2 - Silent
		CVar(AimPosition, 3) // 0/head, 1/body, 2/feet, 3/auto
		CVar(VisTestPoints, 15) //how many points are we allowed to vis test before we stop scanning.
		CVar(ScanPoints, 15) //how many "visible points" need to be reached before we stop searching.
		CVar(ScanScale, 0.95f) // how to scale the points.
		CVar(AllowedHitboxes, 0b000) // 111, Feet, Body, Head.
		CVar(FeetAimIfOnGround, false)
		CVar(BounceKey, 0x0)
		CVar(SplashPrediction, false)
		CVar(MinSplashPredictionDistance, 0)
		CVar(MaxSplashPredictionDistance, 10000)
		CVar(PredictionColor, Color_t(255, 255, 255, 255), IS_VISUAL) // Why is this not in colours??? 
		CVar(ProjectileColor, Color_t(255, 255, 255, 255), IS_VISUAL)
		CVar(PredictionTime, 2.0f)
		CVar(PredictObscured, false)
		CVar(ChargeLooseCannon, false)
		CVar(StrafePredictionGround, false)
		CVar(StrafePredictionAir, false)
		CVar(StrafePredictionSamples, 10)
		CVar(StrafePredictionMaxDistance, 1000)
		CVar(StrafePredictionMinDifference, 10)
		CVar(WaitForHit, false);
	SUBNAMESPACE_END(Projectile);

	SUBNAMESPACE_BEGIN(Melee)
		CVar(SortMethod, 1) //0 - FOV,		1 - Distance
		CVar(RespectFOV, false)
		CVar(RequireBind, true)
		CVar(AimMethod, 2) //0 - Normal,	1 - Smooth, 2 - Silent
		CVar(SmoothingAmount, 8)
		CVar(RangeCheck, false)
		CVar(PredictSwing, false)
		CVar(WhipTeam, false);
	SUBNAMESPACE_END(Melee);

	NAMESPACE_END(Aimbot);

	NAMESPACE_BEGIN(NoSpread)
		CVar(Hitscan, false)
		CVar(Projectile, true)
		CVar(Indicator, false, IS_VISUAL)
		CVar(ExtremePred, false)
		CVar(CorrectPing, false)
		CVar(UseAvgLatency, false);
	NAMESPACE_END(NoSpread);

	NAMESPACE_BEGIN(Triggerbot);

	SUBNAMESPACE_BEGIN(Global)
		CVar(Active, false)
		CVar(TriggerKey, VK_XBUTTON2)
		CVar(IgnoreOptions, 0b00000); //disguised, fakelagging players, taunting, friends, cloaked, invul
	SUBNAMESPACE_END(Global);

	SUBNAMESPACE_BEGIN(Shoot)
		CVar(Active, false)
		CVar(TriggerPlayers, false)
		CVar(TriggerBuildings, false)
		CVar(HeadOnly, false)
		CVar(WaitForHeadshot, false)
		CVar(ScopeOnly, false)
		CVar(HeadScale, 0.7f);
	SUBNAMESPACE_END(Shoot);

	SUBNAMESPACE_BEGIN(Stab)
		CVar(Active, false)
		CVar(RageMode, false)
		CVar(Silent, false)
		CVar(Disguise, false)
		CVar(IgnRazor, false)
		CVar(Range, 0.9f);
	SUBNAMESPACE_END(Stab);

	SUBNAMESPACE_BEGIN(Detonate)
		CVar(Active, false)
		CVar(DetonateTargets, 0b00001)
		CVar(Stickies, false)
		CVar(Flares, false)
		CVar(RadiusScale, 1.0f);
	SUBNAMESPACE_END(Detonate);

	SUBNAMESPACE_BEGIN(Blast)
		CVar(Active, false)
		CVar(Rage, false)
		CVar(Silent, false)
		CVar(ExtinguishPlayers, false)
		CVar(DisableOnAttack, false)
		CVar(Fov, 55);
	SUBNAMESPACE_END(Blast);

	SUBNAMESPACE_BEGIN(Uber)
		CVar(Active, false)
		CVar(OnlyFriends, false)
		CVar(PopLocal, false)
		CVar(AutoVacc, false)
		CVar(HealthLeft, 35.0f)
		CVar(VoiceCommand, false)
		CVar(ReactFoV, 25)
		CVar(BulletRes, true)
		CVar(BlastRes, true)
		CVar(FireRes, true)
		CVar(ReactClasses, 0b000000000); //this is intuitive
	SUBNAMESPACE_END(Uber);

	NAMESPACE_END(Triggerbot);

	NAMESPACE_BEGIN(ESP);

	SUBNAMESPACE_BEGIN(Main)
		CVar(Active, false, IS_VISUAL)
		CVar(Outlinedbar, false, IS_VISUAL)
		CVar(EnableTeamEnemyColors, false, IS_VISUAL)
		CVar(DistanceToAlpha, false, IS_VISUAL)
		CVar(DormantSoundESP, false, IS_VISUAL)
		CVar(DormantTime, 1.f, IS_VISUAL)
		CVar(DormantDist, 1000, IS_VISUAL)
		CVar(NetworkedDist, 2000, IS_VISUAL);
	SUBNAMESPACE_END(Main);

	SUBNAMESPACE_BEGIN(Players)
		CVar(Active, false, IS_VISUAL)
		CVar(ShowLocal, false, IS_VISUAL)
		CVar(IgnoreFlags, 0, IS_VISUAL)
		CVar(IgnoreCloaked, 0, IS_VISUAL)
		CVar(Name, false, IS_VISUAL)
		CVar(NameColor, Color_t(255, 255, 255, 255), IS_VISUAL)
		CVar(NameCustom, false, IS_VISUAL)
		CVar(NameBox, false, IS_VISUAL)
		CVar(Uber, 0, IS_VISUAL)
		CVar(Class, 0, IS_VISUAL)
		CVar(WeaponIcon, false, IS_VISUAL)
		CVar(WeaponText, false, IS_VISUAL)
		CVar(HealthText, 0, IS_VISUAL)
		CVar(Cond, false, IS_VISUAL)
		CVar(FriendCond, false, IS_VISUAL)
		CVar(Distance, false, IS_VISUAL)
		CVar(HealthBar, false, IS_VISUAL)
		CVar(HealthBarStyle, 0, IS_VISUAL)
		CVar(Box, 0, IS_VISUAL)
		CVar(Bones, 0, IS_VISUAL)
		CVar(GUID, false, IS_VISUAL)
		CVar(Choked, false, IS_VISUAL)
		CVar(Lines, false, IS_VISUAL)
		CVar(Dlights, false, IS_VISUAL)
		CVar(DlightRadius, 200.0f, IS_VISUAL)
		CVar(Alpha, 1.0f, IS_VISUAL)
		CVar(Priority, false, IS_VISUAL)
		CVar(SniperSightlines, false, IS_VISUAL)
	SUBNAMESPACE_END(Players);

	SUBNAMESPACE_BEGIN(Buildings)
		CVar(Active, false, IS_VISUAL)
		CVar(IgnoreTeammates, false, IS_VISUAL)
		CVar(Name, false, IS_VISUAL)
		CVar(NameCustom, false, IS_VISUAL)
		CVar(NameColor, Color_t(255, 255, 255, 255), IS_VISUAL)
		CVar(NameBox, false, IS_VISUAL)
		CVar(Health, false, IS_VISUAL)
		CVar(Owner, false, IS_VISUAL)
		CVar(Level, false, IS_VISUAL)
		CVar(Cond, false, IS_VISUAL)
		CVar(Distance, false, IS_VISUAL)
		CVar(HealthBar, false, IS_VISUAL)
		CVar(TeleExitDir, false, IS_VISUAL)
		CVar(TeleExitDirColor, Color_t(255, 255, 255, 255), IS_VISUAL)
		CVar(Lines, false, IS_VISUAL)
		CVar(Box, 0) // 0 - OFF, 1 - Simple, 2 - Corner, IS_VISUAL)
		CVar(Dlights, false, IS_VISUAL)
		CVar(DlightRadius, 200.0f, IS_VISUAL)
		CVar(Alpha, 1.0f, IS_VISUAL);
	SUBNAMESPACE_END(Buildings);

	SUBNAMESPACE_BEGIN(World)
		CVar(Active, false, IS_VISUAL)
		CVar(HealthName, false, IS_VISUAL)
		CVar(HealthBox, 0, IS_VISUAL)
		CVar(HealthLine, false, IS_VISUAL)
		CVar(HealthDistance, false, IS_VISUAL)
		CVar(AmmoName, false, IS_VISUAL)
		CVar(AmmoBox, 0, IS_VISUAL)
		CVar(AmmoLine, false, IS_VISUAL)
		CVar(AmmoDistance, false, IS_VISUAL)
		CVar(NPCName, false, IS_VISUAL)
		CVar(NPCBox, 0, IS_VISUAL)
		CVar(NPCLine, false, IS_VISUAL)
		CVar(NPCDistance, false, IS_VISUAL)
		CVar(BombName, false, IS_VISUAL)
		CVar(BombBox, 0, IS_VISUAL)
		CVar(BombLine, false, IS_VISUAL)
		CVar(BombDistance, false, IS_VISUAL)
		CVar(SpellbookName, false, IS_VISUAL)
		CVar(SpellbookBox, 0, IS_VISUAL)
		CVar(SpellbookLine, false, IS_VISUAL)
		CVar(SpellbookDistance, false, IS_VISUAL)
		CVar(GargoyleName, false, IS_VISUAL)
		CVar(GargoyleBox, 0, IS_VISUAL)
		CVar(GargoyleLine, false, IS_VISUAL)
		CVar(GargoyleDistance, false, IS_VISUAL)
		CVar(Alpha, 1.0f, IS_VISUAL);
	SUBNAMESPACE_END(World);

	NAMESPACE_END(ESP);

	NAMESPACE_BEGIN(Chams);

	SUBNAMESPACE_BEGIN(Main)
		CVar(Active, false, IS_VISUAL);
	SUBNAMESPACE_END(Main);

	SUBNAMESPACE_BEGIN(Players)
		CVar(Active, false, IS_VISUAL)
		CVar(Wearables, false, IS_VISUAL)
		CVar(Weapons, false, IS_VISUAL)
		CVar(FadeoutTeammates, false, IS_VISUAL)
		CVar(Local, Chams_t(), IS_VISUAL)
		CVar(FakeAng, Chams_t(), IS_VISUAL)
		CVar(Friend, Chams_t(), IS_VISUAL)
		CVar(Enemy, Chams_t(), IS_VISUAL)
		CVar(Team, Chams_t(), IS_VISUAL)
		CVar(Target, Chams_t(), IS_VISUAL)
		CVar(Ragdoll, Chams_t(), IS_VISUAL);
	SUBNAMESPACE_END(Players);

	SUBNAMESPACE_BEGIN(Buildings)
		CVar(Local, Chams_t(), IS_VISUAL)
		CVar(Friend, Chams_t(), IS_VISUAL)
		CVar(Enemy, Chams_t(), IS_VISUAL)
		CVar(Team, Chams_t(), IS_VISUAL)
		CVar(Target, Chams_t(), IS_VISUAL)
		CVar(Active, false, IS_VISUAL)
		CVar(Material, 3, IS_VISUAL) //0 - None, 1 - Shaded, 2 - Shiny, 3 - Flat
		CVar(IgnoreZ, false, IS_VISUAL);
	SUBNAMESPACE_END(Buildings);

	SUBNAMESPACE_BEGIN(World)
		CVar(Active, false, IS_VISUAL)
		CVar(Projectilez, 2, IS_VISUAL)
		CVar(Health, Chams_t(), IS_VISUAL)
		CVar(Ammo, Chams_t(), IS_VISUAL)
		CVar(NPCs, Chams_t(), IS_VISUAL)
		CVar(Spellbooks, Chams_t(), IS_VISUAL)
		CVar(Gargoyles, Chams_t(), IS_VISUAL)
		CVar(Projectiles, Chams_t(), IS_VISUAL);
	SUBNAMESPACE_END(World);

	SUBNAMESPACE_BEGIN(DME)
		CVar(Hands, Chams_t(1, 0, 0, true), IS_VISUAL)
		CVar(Weapon, Chams_t(1, 0, 0, true), IS_VISUAL)
		CVar(HandsGlowOverlay, 0, IS_VISUAL) // 0 - Off,  1 - Fresnel Glow, 2 - Wireframe Glow
		CVar(HandsProxySkin, 0, IS_VISUAL)
		CVar(HandsGlowAmount, 1, IS_VISUAL)
		CVar(HandsProxyWF, false, IS_VISUAL)
		CVar(HandsRainbow, false, IS_VISUAL)
		CVar(HandsOverlayRainbow, false, IS_VISUAL)
		CVar(HandsOverlayPulse, false, IS_VISUAL)
		CVar(WeaponGlowOverlay, 0, IS_VISUAL) // 0 - Off,  1 - Fresnel Glow, 2 - Wireframe Glow
		CVar(WeaponsProxySkin, 0, IS_VISUAL)
		CVar(WeaponGlowAmount, 1, IS_VISUAL)
		CVar(WeaponsProxyWF, false, IS_VISUAL)
		CVar(WeaponRainbow, false, IS_VISUAL)
		CVar(WeaponOverlayRainbow, false, IS_VISUAL)
		CVar(WeaponOverlayPulse, false, IS_VISUAL)
	SUBNAMESPACE_END(DME);

	NAMESPACE_END(Chams);

	NAMESPACE_BEGIN(Glow);

	SUBNAMESPACE_BEGIN(Main)
		CVar(Active, false, IS_VISUAL)
		CVar(Type, 0, IS_VISUAL)	//	blur, stencil, fps stencil, wireframe
		CVar(Scale, 5, IS_VISUAL);
	SUBNAMESPACE_END(Main);

	SUBNAMESPACE_BEGIN(Players)
		CVar(Active, false, IS_VISUAL)
		CVar(ShowLocal, false, IS_VISUAL)
		CVar(LocalRainbow, false, IS_VISUAL)
		CVar(IgnoreTeammates, 2, IS_VISUAL) //0 - OFF, 1 - All, 2 - Keep Friends
		CVar(Wearables, false, IS_VISUAL)
		CVar(Weapons, false, IS_VISUAL)
		CVar(Alpha, 1.0f, IS_VISUAL)
		CVar(Color, 0, IS_VISUAL); //0 - Team, 1 - Health
	SUBNAMESPACE_END(Players);

	SUBNAMESPACE_BEGIN(Buildings)
		CVar(Active, false, IS_VISUAL)
		CVar(IgnoreTeammates, false, IS_VISUAL)
		CVar(Alpha, 1.0f, IS_VISUAL)
		CVar(Color, 0, IS_VISUAL); //0 - Team, 1 - Health
	SUBNAMESPACE_END(Buildings);

	SUBNAMESPACE_BEGIN(World)
		CVar(Active, false, IS_VISUAL)
		CVar(Health, false, IS_VISUAL)
		CVar(Ammo, false, IS_VISUAL)
		CVar(NPCs, false, IS_VISUAL)
		CVar(Bombs, false, IS_VISUAL)
		CVar(Spellbooks, false, IS_VISUAL)
		CVar(Gargoyles, false, IS_VISUAL)
		CVar(Projectiles, 1, IS_VISUAL) //0 - Off, 1 - All, 2 - Enemy Only
		CVar(Alpha, 1.0f, IS_VISUAL);
	SUBNAMESPACE_END(World);

	SUBNAMESPACE_BEGIN(Misc)
		CVar(MovementSimLine, false, IS_VISUAL)
		CVar(ProjectileTrajectory, false, IS_VISUAL)
		CVar(Sightlines, false, IS_VISUAL)
		CVar(BulletTracers, false, IS_VISUAL);
	SUBNAMESPACE_END(Misc);

	NAMESPACE_END(Glow);

	NAMESPACE_BEGIN(Radar);

	SUBNAMESPACE_BEGIN(Main)
		CVar(Active, false, IS_VISUAL)
		CVar(BackAlpha, 128, IS_VISUAL)
		CVar(LineAlpha, 255, IS_VISUAL)
		CVar(Range, 1500, IS_VISUAL)
		CVar(NoTitleGradient, false, IS_VISUAL);
	SUBNAMESPACE_END(Main);

	SUBNAMESPACE_BEGIN(Players)
		CVar(Active, false, IS_VISUAL)
		CVar(IconType, 1, IS_VISUAL)           // 0 - Scoreboard, 1 - Portraits, 2 - Avatars
		CVar(BackGroundType, 2, IS_VISUAL)    // 0 - Off, 1 - Rect, 2 - Texture
		CVar(Outline, false, IS_VISUAL)
		CVar(IgnoreTeam, 2, IS_VISUAL)         // 0 - Off, 1 - All, 2 - Keep Friends
		CVar(IgnoreCloaked, 0, IS_VISUAL)      // 0 - Off, 1 - All, 2 - Enemies Only
		CVar(Health, false, IS_VISUAL)
		CVar(IconSize, 24, IS_VISUAL)
		CVar(Height, false, IS_VISUAL);
	SUBNAMESPACE_END(Players);

	SUBNAMESPACE_BEGIN(Buildings)
		CVar(Active, false, IS_VISUAL)
		CVar(Outline, false, IS_VISUAL)
		CVar(IgnoreTeam, false, IS_VISUAL)
		CVar(Health, false, IS_VISUAL)
		CVar(IconSize, 18, IS_VISUAL);
	SUBNAMESPACE_END(Buildings);

	SUBNAMESPACE_BEGIN(World)
		CVar(Active, false, IS_VISUAL)
		CVar(Health, false, IS_VISUAL)
		CVar(Ammo, false, IS_VISUAL)
		CVar(IconSize, 14, IS_VISUAL);
	SUBNAMESPACE_END(World);

	NAMESPACE_END(Radar);

	NAMESPACE_BEGIN(Visuals);

	CVar(FadeOutFoV, 0.f, IS_VISUAL);
	CVar(CleanScreenshots, true, IS_VISUAL);
	CVar(RemoveDisguises, false, IS_VISUAL);
	CVar(RemoveTaunts, false, IS_VISUAL);
	CVar(DrawOnScreenConditions, false, IS_VISUAL);
	CVar(DrawOnScreenPing, false, IS_VISUAL);
	CVar(FieldOfView, 90, IS_VISUAL);
	CVar(AimFOVAlpha, 10, IS_VISUAL);
	CVar(RemoveScope, false, IS_VISUAL);
	CVar(RemoveRagdolls, false, IS_VISUAL);
	CVar(RemoveMOTD, false, IS_VISUAL);
	CVar(RemoveScreenEffects, false, IS_VISUAL);
	CVar(RemoveScreenOverlays, false, IS_VISUAL);
	CVar(RemoveConvarQueries, false, IS_VISUAL);
	CVar(RemoveDSP, false, IS_VISUAL);
	CVar(VisualOverlay, 0, IS_VISUAL);
	CVar(PreventForcedAngles, false, IS_VISUAL);
	CVar(ScopeLines, false, IS_VISUAL);
	CVar(PickupTimers, false, IS_VISUAL);
	CVar(RemoveZoom, false, IS_VISUAL);
	CVar(RemovePunch, false, IS_VISUAL);
	CVar(CrosshairAimPos, false, IS_VISUAL);
	CVar(ChatInfoText, false, IS_VISUAL);
	CVar(ChatInfoChat, false, IS_VISUAL);
	CVar(ChatInfoGrayScale, false, IS_VISUAL);
	CVar(OutOfFOVArrowsOutline, false, IS_VISUAL);
	CVar(FovArrowsDist, 0.15f, IS_VISUAL);
	CVar(SpectatorList, 2, IS_VISUAL);//0 - Off, 1 - Default, 2 - Classic, 3 - Classic Avat);
	CVar(SLShowOthers, true, IS_VISUAL);
	CVar(SpectatorListHeight, 300, IS_VISUAL);
	CVar(ProjectileCameraKey, 0, IS_VISUAL);
	CVar(FreecamKey, 0, IS_VISUAL);
	CVar(FreecamSpeed, 10.f, IS_VISUAL);
	CVar(CameraMode, 0, IS_VISUAL);
	CVar(CameraFOV, 90.f, IS_VISUAL);
	CVar(SpyWarning, false, IS_VISUAL);
	CVar(SpyWarningAnnounce, false, IS_VISUAL);
	CVar(SpyWarningStyle, 0, IS_VISUAL); //0 - Indicator, 1 - Fl);
	CVar(SpyWarningVisibleOnly, false, IS_VISUAL);
	CVar(SpyWarningIgnoreFriends, false, IS_VISUAL);
	CVar(Snow, false, IS_VISUAL);
	CVar(ToolTips, false, IS_VISUAL);
	CVar(ThirdPerson, false, IS_VISUAL);
	CVar(ThirdPersonKey, VK_B, IS_VISUAL);
	CVar(ThirdPersonSilentAngles, false, IS_VISUAL);
	CVar(ThirdPersonInstantYaw, false, IS_VISUAL);
	CVar(ThirdPersonServerHitbox, false, IS_VISUAL);
	CVar(ThirdpersonOffset, false, IS_VISUAL);
	CVar(ThirdpersonDist, 200.f, IS_VISUAL);
	CVar(ThirdpersonRight, 0.f, IS_VISUAL);
	CVar(ThirdpersonUp, 0.f, IS_VISUAL);
	CVar(ThirdpersonOffsetWithArrows, false, IS_VISUAL);
	CVar(ThirdpersonArrowOffsetKey, VK_F, IS_VISUAL);
	CVar(ThirdpersonCrosshair, false, IS_VISUAL);
	CVar(WorldModulation, false, IS_VISUAL);
	CVar(PropWireframe, false, IS_VISUAL);
	CVar(OverrideWorldTextures, false, IS_VISUAL);
	CVar(SkyboxChanger, false, IS_VISUAL);
	CVar(SkyModulation, false, IS_VISUAL);
	CVar(ParticleColors, false, IS_VISUAL);
	CVar(RGBParticles, false, IS_VISUAL);
	CVar(RainbowSpeed, 1.f, IS_VISUAL);
	CVar(HalloweenSpellFootsteps, false, IS_VISUAL);
	CVar(ColorType, 0, IS_VISUAL); //0 - color picker 1 - rain);
	CVar(DashOnly, false, IS_VISUAL);
	CVar(BulletTracer, false, IS_VISUAL);
	CVar(AimbotViewmodel, false, IS_VISUAL);
	CVar(ViewmodelSway, false, IS_VISUAL);
	CVar(ViewmodelSwayScale, 5.f, IS_VISUAL);
	CVar(ViewmodelSwayInterp, 0.05f, IS_VISUAL);
	CVar(MoveSimLine, false, IS_VISUAL);
	CVar(MoveSimSeperators, false, IS_VISUAL);
	CVar(SeperatorLength, 12, IS_VISUAL);
	CVar(SeperatorSpacing, 6, IS_VISUAL);
	CVar(ProjectileTrajectory, false, IS_VISUAL);
	CVar(ParticleTracer, 2, IS_VISUAL);
	CVar(DoPostProcessing, false, IS_VISUAL);
	CVar(BulletTracerRainbow, false, IS_VISUAL);
	CVar(AimPosSquare, false, IS_VISUAL);
	CVar(OutOfFOVArrows, false, IS_VISUAL);
	CVar(ArrowLength, 21.f, IS_VISUAL);
	CVar(ArrowAngle, 100.f, IS_VISUAL);
	CVar(MaxDist, 1000.f, IS_VISUAL);
	CVar(MinDist, 200.f, IS_VISUAL);
	CVar(VMOffX, 0, IS_VISUAL);
	CVar(VMOffY, 0, IS_VISUAL);
	CVar(VMOffZ, 0, IS_VISUAL);
	CVar(VMRoll, 0, IS_VISUAL);
	CVar(NotificationLifetime, 5.f, IS_VISUAL);
	CVar(DrawNotifLine, true, IS_VISUAL);
	CVar(DrawNotifGradient, true, IS_VISUAL);
	CVar(DamageLoggerText, false, IS_VISUAL);
	CVar(DamageLoggerChat, false, IS_VISUAL);
	CVar(DamageLoggerConsole, false, IS_VISUAL);
	CVar(VisionModifier, 0, IS_VISUAL);
	CVar(Rain, 0, IS_VISUAL);
	CVar(EquipRegionUnlock, false, IS_VISUAL);
	CVar(NoStaticPropFade, false, IS_VISUAL);
	CVar(ParticlesIgnoreZ, false, IS_VISUAL);
	CVar(OnScreenConditions, DragBox_t(), IS_VISUAL);
	CVar(OnScreenPing, DragBox_t(), IS_VISUAL);
	CVar(ParticleName, std::string("merasmus_zap_beam01"), IS_VISUAL);
	CVar(VMOffsets, Vec3(), IS_VISUAL);

	SUBNAMESPACE_BEGIN(Beans)
		CVar(Active, false, IS_VISUAL)
		CVar(Rainbow, false, IS_VISUAL)
		CVar(UseCustomModel, false, IS_VISUAL)
		CVar(Life, 2.f, IS_VISUAL)
		CVar(Width, 2.f, IS_VISUAL)
		CVar(EndWidth, 2.f, IS_VISUAL)
		CVar(FadeLength, 10.f, IS_VISUAL)
		CVar(Amplitude, 2.f, IS_VISUAL)
		CVar(Brightness, 255.f, IS_VISUAL)
		CVar(Speed, 0.2f, IS_VISUAL)
		CVar(Flags, 65792, IS_VISUAL)
		CVar(Segments, 2, IS_VISUAL)
		CVar(BeamColour, Color_t(255, 255, 255, 255), IS_VISUAL)
		CVar(Model, std::string("sprites/physbeam.vmt"), IS_VISUAL);
	SUBNAMESPACE_END(Beans);

	SUBNAMESPACE_BEGIN(RagdollEffects)
		CVar(EnemyOnly, false, IS_VISUAL)
		CVar(Burning, false, IS_VISUAL)
		CVar(Electrocuted, false, IS_VISUAL)
		CVar(BecomeAsh, false, IS_VISUAL)
		CVar(Dissolve, false, IS_VISUAL)
		CVar(RagdollType, 0, IS_VISUAL);
	SUBNAMESPACE_END(RagdollEffects);

	SUBNAMESPACE_BEGIN(Skins)
		CVar(Enabled, false, IS_VISUAL)
		CVar(Sheen, 0, IS_VISUAL)
		CVar(Effect, 0, IS_VISUAL)
		CVar(Particle, 0, IS_VISUAL)
		CVar(Acient, false, IS_VISUAL)
		CVar(Override, false, IS_VISUAL)
		CVar(Australium, false, IS_VISUAL);
	SUBNAMESPACE_END(Skins);

	SUBNAMESPACE_BEGIN(Fog)
		CVar(DisableFog, false, IS_VISUAL)
		CVar(CustomFog, false, IS_VISUAL)
		CVar(FogDensity, 0.f, IS_VISUAL)
		CVar(FogDensitySkybox, 0.f, IS_VISUAL)
		CVar(FogStart, 0.f, IS_VISUAL)
		CVar(FogStartSkybox, 0.f, IS_VISUAL)
		CVar(FogEnd, 0.f, IS_VISUAL)
		CVar(FogEndSkybox, 0.f, IS_VISUAL)
		CVar(FogColor, Color_t(255, 255, 255, 255), IS_VISUAL)
		CVar(FogColorSkybox, Color_t(255, 255, 255, 255), IS_VISUAL);
	SUBNAMESPACE_END(Fog);

	NAMESPACE_END(Visuals);

	NAMESPACE_BEGIN(Misc);

	CVar(AccurateMovement, 0);
	CVar(AltMovement, 0b0000);	//	0000 {Fast Strafe, Fast Accel, Fast Crouch, Kart Contr);
	CVar(AutoJump, false);
	CVar(AutoVote, false);
	CVar(DuckJump, false);
	CVar(AutoStrafe, 2);
	CVar(DirectionalOnlyOnMove, false);
	CVar(Directional, false);
	CVar(TauntSlide, false);
	CVar(TauntControl, false);
	CVar(TauntSpin, false);
	CVar(TauntSpinKey, 0);
	CVar(TauntSpinSpeed, 5.f);
	CVar(TauntFollowsCamera, false);
	CVar(BypassPure, false);
	CVar(NoisemakerSpam, false);
	CVar(DisableInterpolation, true);
	CVar(FixInputDelay, false);
	CVar(MedalFlip, false);
	CVar(AutoRocketJump, false);
	CVar(NonLethalRocketJump, true);
	CVar(AutoScoutJump, false);
	CVar(ChatSpam, 0);
	CVar(VoicechatSpam, 0);
	CVar(SpamInterval, 4.f);
	CVar(NoPush, 0);	//	0 off, 1 on, 2 on while not afk, 3 semi while );
	CVar(EdgeJump, false);
	CVar(StoreStatistics, false);
	CVar(StickySpamKey, 0x0);
	CVar(StickyChargePercent, 0);
	CVar(InfiniteEatKey, 0x0);
	CVar(EdgeJumpKey, VK_MENU);
	CVar(AntiAFK, false);
	CVar(VotingOptions, 0b000011); // 000011 {verbose, autovote, party, chat, console, te);
	CVar(CheatsBypass, false);
	CVar(AntiAutobal, false);
	CVar(RageRetry, false);
	CVar(RageRetryHealth, 20);
	CVar(MVMRes, false);
	CVar(PingReducer, false);
	CVar(PingTarget, 0);
	CVar(ExtendFreeze, false);
	CVar(ViewmodelFlip, false);
	CVar(AntiViewmodelFlip, false); // scuf);
	CVar(AutoJoin, 0);
	CVar(KillstreakWeapon, false);
	CVar(PartyNetworking, false);
	CVar(PartyCrasher, false);
	CVar(PartyMarker, 0);
	CVar(PartyESP, false);
	CVar(SoundBlock, 0);
	CVar(ChatFlags, false, IS_VISUAL);
	CVar(MedievalChat, 0, IS_VISUAL);
	CVar(AutoAcceptItemDrops, false);
	CVar(RegionChanger, false);
	CVar(RegionsAllowed, 0);
	CVar(AutoCasualQueue, 0);
	CVar(JoinSpam, false);
	CVar(AntiVAC, false);
	CVar(InstantAccept, 0);
	CVar(RunescapeChat, false, IS_VISUAL);
	CVar(Killsay, false);
	CVar(KillsayFile, std::string(""));

	SUBNAMESPACE_BEGIN(Followbot)
		CVar(Enabled, false)
		CVar(FriendsOnly, false)
		CVar(Distance, 150.f);
	SUBNAMESPACE_END(Followbot);

	//namespace CheaterDetection
	//{
	//	, Aimbot, OOB pitch, angles, bhop, fakelag, simtime, high score, high accuracy
	//	scans, lagging client, timing out
	//	amount of infractions prior to marking someone as a cheater
	//	avg choke for someone to receive and infraction for packet choking
	//	groundticks used when detecting a bunny hop.
	//	many times must some be seen bunny hopping to receive an infraction
	//	//	multiply the avg score/s by this to receive the maximum amount
	//	//	min mouse flick size to suspect someone of angle cheats.
	//	//	max mouse noise prior to a flick to mark somebody
	//	//	scaled with how many ticks a player has choked up to ->
	//	//	self explanatory
	//}
	// couldnt be bothered to put comments back, @Baan u can do it if u want :P
	SUBNAMESPACE_BEGIN(CheaterDetection)
		CVar(Enabled, false)
		CVar(Methods, 0b111111100)
		CVar(Protections, 0b111)
		CVar(SuspicionGate, 10)
		CVar(PacketManipGate, 14)
		CVar(BHopMaxDelay, 1)
		CVar(BHopDetectionsRequired, 5)
		CVar(ScoreMultiplier, 2.f)
		CVar(MinimumFlickDistance, 20.f)
		CVar(MaximumNoise, 5.f)
		CVar(MinimumAimbotFoV, 3.f)
		CVar(MaxScaledAimbotFoV, 20.f);
	SUBNAMESPACE_END(CheaterDetection);

	SUBNAMESPACE_BEGIN(CL_Move)
		CVar(Enabled, false)
		CVar(Doubletap, false)
		CVar(SafeTick, false)
		CVar(SafeTickAirOverride, false)
		CVar(PassiveRecharge, 0)
		CVar(SEnabled, false)
		CVar(SFactor, 1)
		CVar(NotInAir, false)
		CVar(TeleportKey, 0x52) //R
		CVar(TeleportMode, 0)
		CVar(TeleportFactor, 2)
		CVar(RechargeKey, 0x48) //H
		CVar(DoubletapKey, 0x56) //V
		CVar(AutoRetain, true)
		CVar(RetainFakelag, false)
		CVar(RetainBlastJump, false)
		CVar(UnchokeOnAttack, true)
		CVar(RechargeWhileDead, false)
		CVar(AutoRecharge, false) //H
		CVar(AntiWarp, false) //H
		CVar(DTMode, 0) // 0 - On Key, 1 - Always DT, 2 - Disable on key, 3 - Disabled
		CVar(DTBarStyle, 3, IS_VISUAL)
		CVar(DTTicks, 21)
		CVar(WaitForDT, false)
		CVar(Fakelag, false)
		CVar(FakelagMode, 0) // 0 - plain, 1 - random
		CVar(WhileMoving, false)
		CVar(WhileVisible, false)
		CVar(PredictVisibility, false)
		CVar(WhileUnducking, false)
		CVar(WhileInAir, false)
		CVar(FakelagMin, 1) //	only show when FakelagMode=2
		CVar(FakelagMax, 22)
		CVar(FakelagOnKey, false) // dont show when fakelagmode=2|3
		CVar(FakelagKey, 0x54) //T
		CVar(FakelagValue, 1) // dont show when fakelagmode=2
		CVar(AutoPeekKey, 0)
		CVar(AutoPeekDistance, 200.f)
		CVar(AutoPeekFree, false)
		CVar(DTIndicator, DragBox_t(g_ScreenSize.c, g_ScreenSize.c), IS_VISUAL);
	SUBNAMESPACE_END(CL_Move);

	SUBNAMESPACE_BEGIN(FLGChams)
		CVar(Enabled, false, IS_VISUAL)
		CVar(Material, 1, IS_VISUAL)
		CVar(FakelagColor, Color_t(255, 255, 255, 255), IS_VISUAL);
	SUBNAMESPACE_END(FLGChams);

	SUBNAMESPACE_BEGIN(Discord)
		CVar(EnableRPC, false)
		CVar(IncludeClass, false)
		CVar(IncludeMap, false)
		CVar(IncludeTimestamp, false)
		CVar(WhatImagesShouldBeUsed, 0); // 0 - Big fedora, small TF2 logo; 1 - Big TF2 logo, small fedora
	SUBNAMESPACE_END(Discord);

	SUBNAMESPACE_BEGIN(Steam)
		CVar(EnableRPC, false)
		CVar(MatchGroup, 0) // 0 - Special Event; 1 - MvM Mann Up; 2 - Competitive; 3 - Casual; 4 - MvM Boot Camp;
		CVar(OverrideMenu, false) // Override matchgroup when in main menu
		CVar(MapText, 1) // 0 - Fedoraware; 1 - CUM.clab; 2 - Meowhook.club; 3 - rathook.cc; 4 - NNitro.tf; 5 - custom;
		CVar(GroupSize, 1337)
		CVar(CustomText, std::string("M-FeD is gay"));
	SUBNAMESPACE_END(Steam);

	NAMESPACE_END(Misc);

	NAMESPACE_BEGIN(AntiHack);

	SUBNAMESPACE_BEGIN(AntiAim)
		CVar(Active, false)
		CVar(ToggleKey, 0)
		CVar(InvertKey, 0)
		CVar(ManualKey, 0)
		CVar(Pitch, 0) //0 - None, 1 - Zero, 2 - Up, 3 - Down, 4 - Fake Up, 5 - Fake Down
		CVar(YawReal, 0) //0 - None, 1 - Forward, 2 - Left, 3 - Right, 4 - Backwards
		CVar(YawFake, 0) //0 - None, 1 - Forward, 2 - Left, 3 - Right, 4 - Backwards
		CVar(BaseYawMode, 0)
		CVar(BaseYawOffset, 0.f)
		CVar(SpinSpeed, 15.f)
		CVar(CustomRealPitch, 0.f)
		CVar(CustomRealYaw, 0)
		CVar(CustomFakeYaw, 0)
		CVar(FakeJitter, 0)
		CVar(RealJitter, 0)
		CVar(RandInterval, 25)
		CVar(RehideAntiAimPostShot, true)
		CVar(AntiBackstab, false)
		CVar(LegJitter, false) // frick u fourteen
		CVar(AntiOverlap, false)
		CVar(InvalidShootPitch, false); // i dont know what to name this its TRASH
	SUBNAMESPACE_END(AntiAim);

	SUBNAMESPACE_BEGIN(Resolver)
		CVar(Resolver, false)
		CVar(AutoResolveCheaters, false)
		CVar(IgnoreAirborne, false);
	SUBNAMESPACE_END(Resolver);

	NAMESPACE_END(AntiHack);

	NAMESPACE_BEGIN(Skybox);

	CVar(SkyboxNum, 0, IS_VISUAL);
	CVar(SkyboxName, std::string("mr_04"), IS_VISUAL);

	NAMESPACE_END(Skybox);

	NAMESPACE_BEGIN(Fonts);

	SUBNAMESPACE_BEGIN(FONT_ESP)
		CVar(szName, std::string("Tahoma"), IS_VISUAL)
		CVar(nTall, 12, IS_VISUAL)
		CVar(nWeight, 800, IS_VISUAL)
		CVar(nFlags, int(FONTFLAG_ANTIALIAS), IS_VISUAL);
	SUBNAMESPACE_END(FONT_ESP);

	SUBNAMESPACE_BEGIN(FONT_ESP_NAME)
		CVar(szName, std::string("Tahoma"), IS_VISUAL)
		CVar(nTall, 14, IS_VISUAL)
		CVar(nWeight, 800, IS_VISUAL)
		CVar(nFlags, int(FONTFLAG_ANTIALIAS), IS_VISUAL);
	SUBNAMESPACE_END(FONT_ESP_NAME)

		SUBNAMESPACE_BEGIN(FONT_ESP_COND)
		CVar(szName, std::string("Tahoma"), IS_VISUAL)
		CVar(nTall, 10, IS_VISUAL)
		CVar(nWeight, 800, IS_VISUAL)
		CVar(nFlags, int(FONTFLAG_ANTIALIAS), IS_VISUAL);
	SUBNAMESPACE_END(FONT_ESP_COND);

	SUBNAMESPACE_BEGIN(FONT_ESP_PICKUPS)
		CVar(szName, std::string("Tahoma"), IS_VISUAL)
		CVar(nTall, 13, IS_VISUAL)
		CVar(nWeight, 800, IS_VISUAL)
		CVar(nFlags, int(FONTFLAG_ANTIALIAS), IS_VISUAL);
	SUBNAMESPACE_END(FONT_ESP_PICKUPS);

	SUBNAMESPACE_BEGIN(FONT_MENU)
		CVar(szName, std::string("DejaVu Sans"), IS_VISUAL)
		CVar(nTall, 16, IS_VISUAL)
		CVar(nWeight, 200, IS_VISUAL)
		CVar(nFlags, int(FONTFLAG_ANTIALIAS), IS_VISUAL);
	SUBNAMESPACE_END(FONT_MENU);

	SUBNAMESPACE_BEGIN(FONT_INDICATORS)
		CVar(szName, std::string("Verdana"), IS_VISUAL)
		CVar(nTall, 12, IS_VISUAL)
		CVar(nWeight, 0, IS_VISUAL)
		CVar(nFlags, int(FONTFLAG_OUTLINE), IS_VISUAL);
	SUBNAMESPACE_END(FONT_INDICATORS);

	NAMESPACE_END(Fonts);

	NAMESPACE_BEGIN(Colours);
		CVar(DTBarIndicatorsCharged, Gradient_t({ 106, 255, 131, 180 }, { 106, 255, 250, 180 }), IS_VISUAL);
		CVar(DTBarIndicatorsCharging, Gradient_t({ 255, 192, 81, 180 }, { 255, 134, 81, 180 }), IS_VISUAL);
		CVar(ChokedBar, Gradient_t({ 47, 39, 0, 255 }, { 255, 210, 0, 255 }), IS_VISUAL);
		CVar(GradientHealthBar, Gradient_t({ 255, 0, 0, 255 }, { 0, 202, 124, 255 }), IS_VISUAL);
		CVar(GradientOverhealBar, Gradient_t({ 0, 202, 124, 255 }, { 167, 255, 237, 255 }), IS_VISUAL);
		CVar(UberchargeBar, Gradient_t({ 255, 255, 255, 255 }, { 255, 0, 228, 255 }), IS_VISUAL);
		CVar(OutlineESP, Color_t( 0, 0, 0, 255 ), IS_VISUAL);
		CVar(Cond, Color_t(254, 202, 87, 255), IS_VISUAL);
		CVar(Target, Color_t(240, 147, 43, 255), IS_VISUAL);
		CVar(Invuln, Color_t(120, 111, 166, 255), IS_VISUAL);
		CVar(Cloak, Color_t(165, 177, 194, 255), IS_VISUAL);
		CVar(Friend, Color_t(32, 191, 107, 255), IS_VISUAL);
		CVar(Local, Color_t(168, 255, 211, 255), IS_VISUAL);
		CVar(Ignored, Color_t(32, 191, 107, 255), IS_VISUAL);
		CVar(Rage, Color_t(255, 255, 0, 255), IS_VISUAL);
		CVar(Cheater, Color_t(255, 0, 0, 255), IS_VISUAL);
		CVar(Overheal, Color_t(84, 160, 255, 255), IS_VISUAL);
		CVar(Health, Color_t(0, 230, 64, 255), IS_VISUAL);
		CVar(Ammo, Color_t(191, 191, 191, 255), IS_VISUAL);
		CVar(UberColor, Color_t(224, 86, 253, 255), IS_VISUAL);
		CVar(TeamRed, Color_t(255, 100, 87, 255), IS_VISUAL);
		CVar(TeamBlu, Color_t(30, 144, 255, 255), IS_VISUAL);
		CVar(Enemy, Color_t(255, 100, 87, 255), IS_VISUAL);
		CVar(Friendly, Color_t(30, 144, 255, 255), IS_VISUAL);
		CVar(WorldModulation, Color_t(255, 255, 255, 255), IS_VISUAL);
		CVar(SkyModulation, Color_t(255, 255, 255, 255), IS_VISUAL);
		CVar(StaticPropModulation, Color_t(255, 255, 255, 255), IS_VISUAL);
		CVar(ParticleColor, Color_t(255, 255, 255, 255), IS_VISUAL);
		CVar(FOVCircle, Color_t(255, 255, 255, 255), IS_VISUAL);
		CVar(Bones, Color_t(255, 255, 255, 255), IS_VISUAL);
		CVar(BulletTracer, Color_t(255, 255, 255, 255), IS_VISUAL);
		CVar(FeetColor, Color_t(255, 150, 0, 255), IS_VISUAL);
		CVar(DtOutline, Color_t(30, 30, 30, 180), IS_VISUAL);
		CVar(NotifBG, Color_t(30, 30, 30, 255), IS_VISUAL);
		CVar(NotifOutline, Color_t(255, 101, 101, 255), IS_VISUAL);
		CVar(NotifText, Color_t(255, 255, 255, 255), IS_VISUAL);
		CVar(HitboxFace, Color_t(255, 255, 255, 25), IS_VISUAL);
		CVar(HitboxEdge, Color_t(255, 255, 255, 175), IS_VISUAL);
		CVar(WeaponIcon, Color_t(255, 255, 255, 255), IS_VISUAL);
		CVar(NoscopeLines1, Color_t(0, 0, 0, 255), IS_VISUAL);
		CVar(NoscopeLines2, Color_t( 0, 0, 0, 100 ), IS_VISUAL);
		CVar(NPC, Color_t(255, 255, 255, 255), IS_VISUAL);
		CVar(Bomb, Color_t(255, 75, 0, 255), IS_VISUAL);
		CVar(Spellbook, Color_t(100, 0, 255, 255), IS_VISUAL);
		CVar(Gargoyle, Color_t(0, 150, 75, 255), IS_VISUAL);
	NAMESPACE_END(Colours);

	// Debug options
	NAMESPACE_BEGIN(Debug);

	CVar(DebugInfo, false, IS_VISUAL);
	CVar(Logging, false, IS_VISUAL);

	NAMESPACE_END(Debug);
}

