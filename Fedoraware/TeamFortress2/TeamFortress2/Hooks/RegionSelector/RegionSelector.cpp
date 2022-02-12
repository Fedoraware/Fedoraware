#include "../../SDK/SDK.h"

#include <array>
#include <set>
#include <map>

typedef std::array<char, 5> CidStr_t;

struct SteamNetworkingPOPID_decl
{
    unsigned v;
    /* 'out' must point to array with capacity at least 5 */
    void ToString(char* out)
    {
        out[0] = char(v >> 16);
        out[1] = char(v >> 8);
        out[2] = char(v);
        out[3] = char(v >> 24);
        out[4] = 0;
    }
};

static std::set<CidStr_t> regionsSet;

static std::map<std::string, std::string> dc_name_map{
    {"ams", "Amsterdam"},
    {"atl", "Atlanta"},
    {"bom", "Mumbai"},
    {"dxb", "Dubai"},
    {"eat", "Moses Lake/Washington"},
    {"mwh", "Moses Lake/Washington"},
    {"fra", "Frankfurt"},
    {"gnrt", "Tokyo (gnrt)"},
    {"gru", "Sao Paulo"},
    {"hkg", "Honk Kong"},
    {"iad", "Sterling/Virginia"},
    {"jnb", "Johannesburg"},
    {"lax", "Los Angelos"},
    {"lhr", "London"},
    {"lim", "Lima"},
    {"lux", "Luxembourg"},
    {"maa", "Chennai"},
    {"mad", "Madrid"},
    {"man", "Manilla"},
    {"okc", "Oklahoma City"},
    {"ord", "Chicago"},
    {"par", "Paris"},
    {"scl", "Santiago"},
    {"sea", "Seaattle"},
    {"sgp", "Singapore"},
    {"sto", "Stockholm (Kista)"},
    {"sto2", "Stockholm (Bromma)"},
    {"syd", "Sydney"},
    {"tyo", "Tokyo (North)"},
    {"tyo2", "Tokyo (North)"},
    {"tyo1", "Tokyo (South)"},
    {"vie", "Vienna"},
    {"waw", "Warsaw"}
};

static std::vector<std::string> eu_datacenters = { { "ams" }, { "fra" }, { "lhr" }, { "mad" }, { "par" }, { "sto" }, { "sto2" }, { "waw" }, { "lux" }, { "lux1" }, { "lux2" } };
static std::vector<std::string> north_america_datacenters = { { "atl" }, { "eat" }, { "mwh" }, { "iad" }, { "lax" }, { "okc" }, { "ord" }, { "sea" } };
static std::vector<std::string> south_america_datacenters = { { "gru" }, { "lim" }, { "scl" } };
static std::vector<std::string> asia_datacenters = { { "bom" }, { "dxb" }, { "gnrt" }, { "hkg" }, { "maa" }, { "man" }, { "sgp" }, { "tyo" }, { "tyo2" }, { "tyo1" } };
static std::vector<std::string> oceana_datacenters = { { "syd" }, { "vie" } };
static std::vector<std::string> africa_datacenters = { { "jnb" } };

void PrintDatacenters() {
    static auto GetPOPCount = *reinterpret_cast<int(**)(void*)>(*reinterpret_cast<uintptr_t*>(g_SteamInterfaces.Networking) + 37);
    static auto GetPOPList = *reinterpret_cast<int(**)(void*, SteamNetworkingPOPID_decl*, int)>(*reinterpret_cast<uintptr_t*>(g_SteamInterfaces.Networking) + 41);

    char region[5];

    int count = GetPOPCount(g_SteamInterfaces.Networking);
    if (count <= 0)
    {
        g_Interfaces.CVars->ConsoleColorPrintf({255,180,30,255}, "List of regions is not available yet\n");
        return;
    }

    SteamNetworkingPOPID_decl* list = new SteamNetworkingPOPID_decl[count];
    GetPOPList(g_SteamInterfaces.Networking, list, count);

    auto it = list;
    while (count--)
    {
        (it++)->ToString(region);
        std::string region_name = dc_name_map[region];
        if (region_name == "")
            region_name = "Not set";
        g_Interfaces.CVars->ConsoleColorPrintf({ 255,180,30,255 }, "%s [%s]\n", region_name.c_str(), region);
    }
    delete[] list;
}