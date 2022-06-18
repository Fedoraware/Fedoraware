#include "../Hooks.h"

enum EEconItemQuality
{
	AE_UNDEFINED = -1,
	AE_NORMAL = 0,
	AE_RARITY1 = 1,			// Genuine
	AE_RARITY2 = 2,			// Customized (unused)
	AE_VINTAGE = 3,			// Vintage has to stay at 3 for backwards compatibility
	AE_RARITY3,				// Artisan
	AE_UNUSUAL,				// Unusual
	AE_UNIQUE,
	AE_COMMUNITY,
	AE_DEVELOPER,
	AE_SELFMADE,
	AE_CUSTOMIZED,			// (unused)
	AE_STRANGE,
	AE_COMPLETED,
	AE_HAUNTED,
	AE_COLLECTORS,
	AE_PAINTKITWEAPON,
	AE_RARITY_DEFAULT,
	AE_RARITY_COMMON,
	AE_RARITY_UNCOMMON,
	AE_RARITY_RARE,
	AE_RARITY_MYTHICAL,
	AE_RARITY_LEGENDARY,
	AE_RARITY_ANCIENT,
	AE_MAX_TYPES,
	AE_DEPRECATED_UNIQUE = 3,
};



MAKE_HOOK(CBackpackPanel_GetItemQualityForBorder, g_Pattern.Find(L"client.dll", L"55 8B EC 56 8B 75 08 57 8B F9 8B CE E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? A1 ? ? ? ? 8B 40 30 85 C0 7F 0D"), int, __fastcall,
		  void* ecx, void* edx, void* pItemPanel)
{
	if (!I::BackpackPanel)
	{
		I::BackpackPanel = ecx;
	}
	return G::BackpackQuality;
}

