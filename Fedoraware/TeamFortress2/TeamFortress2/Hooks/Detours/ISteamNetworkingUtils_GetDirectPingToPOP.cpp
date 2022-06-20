#include "../Hooks.h"

static const std::map<std::string, int> DataCenterHashMap{
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

MAKE_HOOK(ISteamNetworkingUtils_GetDirectPingToPOP, Utils::GetVFuncPtr(g_SteamInterfaces.NetworkingUtils, 9), int, __fastcall,
		  void* ecx, void* edx, SteamNetworkingPOPID popID)
{
    if (!Vars::Misc::RegionChanger.Value)
    {
        return Hook.Original<FN>()(ecx, edx, popID);
    }


	char popIDName[5];
	popID.ToString(popIDName);

    for (auto& dc : DataCenterHashMap)
    {
        if (dc.first == popIDName && 
            Vars::Misc::RegionsAllowed.Value & dc.second)
        {
            return 1;
        }
        if (dc.first == popIDName &&
            !(Vars::Misc::RegionsAllowed.Value & dc.second))
        {
            return INT_MAX;
        }
    }


	return Hook.Original<FN>()(ecx, edx, popID);
}