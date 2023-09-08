#include "../Hooks.h"

const std::unordered_map<std::string, unsigned> DATA_CENTER_HASH
{
	{"ams",  DC_AMS},
	{"atl",  DC_ATL},
	{"bom",  DC_BOM},
	{"dxb",  DC_DXB},
	{"eat",  DC_EAT},
	{"mwh",  DC_MWH},
	{"fra",  DC_FRA},
	{"gnrt", DC_GNRT},
	{"gru",  DC_GRU},
	{"hkg",  DC_HKG},
	{"iad",  DC_IAD},
	{"jnb",  DC_JNB},
	{"lax",  DC_LAX},
	{"lhr",  DC_LHR},
	{"lim",  DC_LIM},
	{"lux",  DC_LUX},
	{"maa",  DC_MAA},
	{"mad",  DC_MAD},
	{"man",  DC_MAN},
	{"okc",  DC_OKC},
	{"ord",  DC_ORD},
	{"par",  DC_PAR},
	{"scl",  DC_SCL},
	{"sea",  DC_SEA},
	{"sgp",  DC_SGP},
	{"sto",  DC_STO},
	{"sto2", DC_STO},
	{"syd",  DC_SYD},
	{"tyo",  DC_TYO},
	{"tyo2", DC_TYO},
	{"tyo1", DC_TYO},
	{"vie",  DC_VIE},
	{"waw",  DC_WAW}
};

void POPID_ToString(SteamNetworkingPOPID popID, char* out)
{
	out[0] = static_cast<char>(popID >> 16);
	out[1] = static_cast<char>(popID >> 8);
	out[2] = static_cast<char>(popID);
	out[3] = static_cast<char>(popID >> 24);
	out[4] = 0;
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
		const unsigned value = pos->second;
		const bool isAllowed = Vars::Misc::RegionsAllowed.Value & value;
		return isAllowed ? 1 : 999999;
	}

	return Hook.Original<FN>()(ecx, edx, popID);
}