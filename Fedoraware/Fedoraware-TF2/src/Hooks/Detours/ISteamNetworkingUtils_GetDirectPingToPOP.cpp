#include "../Hooks.h"

const std::unordered_map<std::string, unsigned> DATA_CENTER_HASH
{
	{"ams", DC_AMS},
	{"fra", DC_FRA},
	{"lhr", DC_LHR},
	{"mad", DC_MAD},
	{"par", DC_PAR},
	{"sto", DC_STO},
	{"sto2", DC_STO},
	{"vie", DC_VIE},
	{"waw", DC_WAW},

	{"atl", DC_ATL},
	{"ord", DC_ORD},
	{"dfw", DC_DFW},
	{"lax", DC_LAX},
	{"eat", DC_EAT},
	{"sea", DC_SEA},
	{"iad", DC_IAD},

	{"eze", DC_EZE},
	{"lim", DC_LIM},
	{"scl", DC_SCL},
	{"gru", DC_GRU},

	{"maa", DC_MAA},
	{"bom", DC_BOM},
	{"dxb", DC_DXB},
	{"hkg", DC_HKG},
	{"seo", DC_SEO},
	{"sgp", DC_SGP},
	{"tyo", DC_TYO},
	{"tyo1", DC_TYO},
	{"can", DC_CAN},
	{"sha", DC_SHA},
	{"tsn", DC_TSN},

	{"jnb", DC_JNB},

	{"syd", DC_SYD},
};

void POPID_ToString(SteamNetworkingPOPID popID, char* out)
{
	out[0] = static_cast<char>(popID >> 16);
	out[1] = static_cast<char>(popID >> 8);
	out[2] = static_cast<char>(popID);
	out[3] = static_cast<char>(popID >> 24);
	out[4] = 0;
}

bool IsAllowed(const int nIndex) {
	if (nIndex <= 7) {
		return Vars::Misc::RegionsAllowed.Value & (1 << 0);
	}
	else if (nIndex <= 14) {
		return Vars::Misc::RegionsAllowed.Value & (1 << 1);
	}
	else if (nIndex <= 18) {
		return Vars::Misc::RegionsAllowed.Value & (1 << 2);
	}
	else if (nIndex <= 28) {
		return Vars::Misc::RegionsAllowed.Value & (1 << 3);
	}
	else if (nIndex <= 29) {
		return Vars::Misc::RegionsAllowed.Value & (1 << 4);
	}
	else if (nIndex <= 30) {
		return Vars::Misc::RegionsAllowed.Value & (1 << 5);
	}
	return Vars::Misc::RegionsAllowed.Value & (1 << 6);
}

MAKE_HOOK(ISteamNetworkingUtils_GetDirectPingToPOP, Utils::GetVFuncPtr(g_SteamInterfaces.NetworkingUtils, 9), int, __fastcall,
		  void* ecx, void* edx, SteamNetworkingPOPID popID)
{
	if (!Vars::Misc::RegionChanger.Value)
	{
		return Hook.Original<FN>()(ecx, edx, popID);
	}

	char popIDName[5];
	POPID_ToString(popID, popIDName);

	const auto pos = DATA_CENTER_HASH.find(popIDName);
	if (pos != DATA_CENTER_HASH.end())
	{
		const bool isAllowed = IsAllowed(pos->second);
		return isAllowed ? 1 : 999999;
	}

	return Hook.Original<FN>()(ecx, edx, popID);
}