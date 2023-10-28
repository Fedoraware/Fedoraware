#pragma once

//Fuck you
typedef unsigned int size_owned;

//Main hash function
//And prehashed strings allowing for fast comparisong.
namespace Hash
{
	void PrintHash();

	//Panels
	inline constexpr size_owned FocusOverlayPanel = 2347069313;
	inline constexpr size_owned HudScope = 4197140386;
	inline constexpr size_owned HudMOTD = 263456517; // MOTD panel

	//Health packs
	inline constexpr size_owned MedKitBanana = 48961140;
	inline constexpr size_owned MedKitSmall = 1138108014;
	inline constexpr size_owned MedKitMedium = 3591694894;
	inline constexpr size_owned MedKitLarge = 2085245990;
	inline constexpr size_owned MedKitSmallBday = 3780270629;
	inline constexpr size_owned MedKitMediumBday = 2229940197;
	inline constexpr size_owned MedKitLargeBday = 2131865389;
	inline constexpr size_owned MedKitSandwich = 430753038;
	inline constexpr size_owned MedKitSandwichXmas = 3376545636;
	inline constexpr size_owned MedKitSandwichRobo = 1299884225;
	inline constexpr size_owned MedKitSteak = 2817177407;
	inline constexpr size_owned MedKitDalokahs = 4000983528;
	inline constexpr size_owned MedKitFishCake = 3413381804;
	inline constexpr size_owned MedKitSmallHalloween = 2634597576;
	inline constexpr size_owned MedKitMediumHalloween = 4166466968;
	inline constexpr size_owned MedKitLargeHalloween = 3893084692;
	inline constexpr size_owned MedKitLargeMushroom = 980493366;
	inline constexpr size_owned MedievalMeat = 969509793;
	inline constexpr size_owned FoodCan = 710669651;

	//Ammo packs
	inline constexpr size_owned AmmoSmall = 2534489767;
	inline constexpr size_owned AmmoMedium = 455139957;
	inline constexpr size_owned AmmoLarge = 1018735539;
	inline constexpr size_owned AmmoLargeBday = 1501082414;
	inline constexpr size_owned AmmoMediumBday = 1283759932;
	inline constexpr size_owned AmmoSmallBday = 2832603626;

	//Models
	inline constexpr size_owned SentryBuster = 2509256133;
	inline constexpr size_owned AtomBomb = 3090011807;
	inline constexpr size_owned ResupplyLocker = 3947990550;

	//Spells
	inline constexpr size_owned SpellbookFlying = 1949854555;
	inline constexpr size_owned SpellbookUpright = 1753814975;
	inline constexpr size_owned SpellbookUprightMajor = 2461783237;
	inline constexpr size_owned CrystalBall = 3715827197;
	inline constexpr size_owned CrystalBallMajor = 4112151211;
	inline constexpr size_owned FlaskGreen = 3852856031;
	inline constexpr size_owned FlaskPurple = 1077944092; // prop_dynamic in the map, probably won't work

	bool IsAmmo(const char* szName);
	bool IsHealth(const char* szName);
	bool IsSpell(const char* szName);
	bool IsResupply(const char* szName);
	bool IsHudScope(const char* szName);
	bool IsHudMotd(const char* szName);
}