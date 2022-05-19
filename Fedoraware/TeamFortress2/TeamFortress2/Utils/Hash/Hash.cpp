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
		"HudScope",
		"info"
};

void Hash::PrintHash()
{
#ifdef _DEBUG
	int ArraySize = (sizeof(ToHash) / sizeof(ToHash[0]));

	for (int n = 0; n < ArraySize; n++)
	{
		std::string szPrintName = "";

		for (unsigned Char = 0; Char < strlen(ToHash[n]); Char++)
		{
			if (ToHash[n][Char] != '/' && ToHash[n][Char] != '.')
				szPrintName += ToHash[n][Char];
		}

		g_Interfaces.CVars->ConsolePrintf("constexpr size_owned %s = %lu;\n", szPrintName.data(), m_Hash(ToHash[n]));
	}

#endif
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
		case MedKitSteak:
		case MedKitLargeMushroom:
		case MedKitBanana:
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
		case 69: //SpellBook
		case 420: //ChrystalBall
			return true;
		default:
			return false;
	}
}