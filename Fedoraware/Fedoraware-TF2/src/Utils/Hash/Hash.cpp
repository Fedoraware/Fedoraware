#include "Hash.h"

#include <stdio.h>
#include <xstring>

#include "../../SDK/Interfaces/Interfaces.h"

std::hash<std::string_view> m_Hash;

//OBSOLETE - NOT IN USE ANYMORE
//The magic of number 33, i.e. why it works better than many other constants,
//prime or not, has never been adequately explained by anyone.
int String(const char* szOrg)
{
	int iHash = 5381;

	while (int iStr = *szOrg++) 
		iHash = iHash * 33 + iStr;

	return iHash;
}

const char* ToHash[] =
{
		"models/items/banana/plate_banana.mdl",
		"models/items/medkit_small.mdl",
		"models/items/medkit_medium.mdl",
		"models/items/medkit_large.mdl",
		"models/items/medkit_small_bday.mdl",
		"models/items/medkit_medium_bday.mdl",
		"models/items/medkit_large_bday.mdl",
		"models/items/plate.mdl",
		"models/items/plate_steak.mdl",
		"models/props_halloween/halloween_medkit_small.mdl",
		"models/props_halloween/halloween_medkit_medium.mdl",
		"models/props_halloween/halloween_medkit_large.mdl",
		"models/items/ld1/mushroom_large.mdl",
		"models/items/ammopack_small.mdl",
		"models/items/ammopack_medium.mdl",
		"models/items/ammopack_large.mdl",
		"models/items/ammopack_large_bday.mdl",
		"models/items/ammopack_medium_bday.mdl",
		"models/bots/demo/bot_sentry_buster.mdl",
		"models/props_td/atom_bomb.mdl",
		"FocusOverlayPanel",
		"models/props_gameplay/resupply_locker.mdl",
		"models/items/plate_sandwich_xmas.mdl",
		"models/items/ammopack_small_bday.mdl",
		"models/props_medieval/medieval_meat.mdl",
		"models/items/plate_sandwich_xmas.mdl",
		"models/items/plate_robo_sandwich.mdl",
		"models/workshop/weapons/c_models/c_chocolate/plate_chocolate.mdl",
		"models/workshop/weapons/c_models/c_fishcake/plate_fishcake.mdl",
		"HudScope",
		"info",
		"models/props_halloween/hwn_spellbook_flying.mdl",
		"models/props_halloween/hwn_spellbook_upright.mdl",
		"models/props_halloween/hwn_spellbook_upright_major.mdl",
		"models/items/crystal_ball_pickup.mdl",
		"models/items/crystal_ball_pickup_major.mdl",
		"models/props_monster_mash/flask_vial_green.mdl",
		"models/props_monster_mash/flask_vial_purple.mdl",
		"models/props_brine/foodcan.mdl",
		"models/props_halloween/gargoyle_ghost.mdl"
};

void Hash::PrintHash()
{
	int ArraySize = (sizeof(ToHash) / sizeof(ToHash[0]));

	for (int n = 0; n < ArraySize; n++)
	{
		std::string szPrintName = "";

		for (unsigned Char = 0; Char < strlen(ToHash[n]); Char++)
		{
			if (ToHash[n][Char] != '/' && ToHash[n][Char] != '.')
				szPrintName += ToHash[n][Char];
		}

		I::Cvar->ConsolePrintf("constexpr size_owned %s = %lu;\n", szPrintName.data(), m_Hash(ToHash[n]));
	}
}

bool Hash::IsHudScope(const char* szName)
{
	return (m_Hash(szName) == HudScope);
}

bool Hash::IsHudMotd(const char* szName)
{
	return (m_Hash(szName) == HudMOTD);
}

bool Hash::IsResupply(const char* szName)
{
	return (m_Hash(szName) == ResupplyLocker);
}

bool Hash::IsHealth(const char* szName)
{
	switch (m_Hash(szName))
	{
		case MedKitSmall:
		case MedKitMedium:
		case MedKitLarge:
		case MedKitSmallBday:
		case MedKitMediumBday:
		case MedKitLargeBday:
		case MedKitSmallHalloween:
		case MedKitMediumHalloween:
		case MedKitLargeHalloween:
		case MedKitSandwich:
		case MedKitSandwichXmas:
		case MedKitSandwichRobo:
		case MedKitSteak:
		case MedKitLargeMushroom:
		case MedKitBanana:
		case MedievalMeat:
		case MedKitDalokahs:
		case MedKitFishCake:
		case FoodCan:
			return true;
		default: 
			return false;
	}
}

bool Hash::IsAmmo(const char* szName)
{
	switch (m_Hash(szName))
	{
		case AmmoSmall:
		case AmmoMedium:
		case AmmoLarge:
		case AmmoSmallBday:
		case AmmoMediumBday:
		case AmmoLargeBday:
			return true;
		default:
			return false;
	}
}

bool Hash::IsSpell(const char* szName)
{
	switch (m_Hash(szName))
	{
		case SpellbookFlying:
			I::Cvar->ConsolePrintf("SpellbookFlying\n");
			return true;
		case SpellbookUpright:
		case SpellbookUprightMajor:
		case CrystalBall:
		case CrystalBallMajor:
		case FlaskGreen:
		case FlaskPurple:
			return true;
		default:
			return false;
	}
}