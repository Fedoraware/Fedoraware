#pragma once

//Fuck you
typedef unsigned int size_owned;

//Main hash function
//And prehashed strings allowing for fast comparisong.
namespace Hash
{
	void PrintHash();

	//TODO:
	//MISSING NEW SPELLBOOK, AND SPELL CHRYSTAL

	//Panels
	constexpr size_owned FocusOverlayPanel = 2347069313;
	constexpr size_owned HudScope = 4197140386;
	constexpr size_owned HudMOTD = 263456517; // MOTD panel

	//Health packs
	constexpr size_owned MedKitBanana = 48961140;
	constexpr size_owned MedKitSmall = 1138108014;
	constexpr size_owned MedKitMedium = 3591694894;
	constexpr size_owned MedKitLarge = 2085245990;
	constexpr size_owned MedKitSmallBday = 3780270629;
	constexpr size_owned MedKitMediumBday = 2229940197;
	constexpr size_owned MedKitLargeBday = 2131865389;
	constexpr size_owned MedKitSandwich = 430753038;
	constexpr size_owned MedKitSteak = 2817177407;
	constexpr size_owned MedKitSmallHalloween = 2634597576;
	constexpr size_owned MedKitMediumHalloween = 4166466968;
	constexpr size_owned MedKitLargeHalloween = 3893084692;
	constexpr size_owned MedKitLargeMushroom = 980493366;

	//Ammo packs
	constexpr size_owned AmmoSmall = 2534489767;
	constexpr size_owned AmmoMedium = 455139957;
	constexpr size_owned AmmoLarge = 1018735539;
	constexpr size_owned AmmoLargeBday = 1501082414;
	constexpr size_owned AmmoMediumBday = 1283759932;

	//Models
	constexpr size_owned SentryBuster = 2509256133;
	constexpr size_owned AtomBomb = 3090011807;
	constexpr size_owned ResupplyLocker = 3947990550;

	bool IsAmmo(const char* szName);
	bool IsHealth(const char* szName);
	bool IsSpell(const char* szName);
	bool IsResupply(const char* szName);
	bool IsHudScope(const char* szName);
	bool IsHudMotd(const char* szName);
}