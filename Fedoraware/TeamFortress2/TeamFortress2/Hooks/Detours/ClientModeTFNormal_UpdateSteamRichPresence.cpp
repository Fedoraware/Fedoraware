#include "../Hooks.h"

#include "../../SDK/Discord/include/discord_rpc.h"
#include "../../Features/Discord/Discord.h"
#include "../../Features/Misc/Misc.h"

MAKE_HOOK(ClientModeTFNormal_UpdateSteamRichPresence, g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 8B 15 ? ? ? ? 53 33 DB 89 4D F8 89 5D E8 89 5D EC 8B 02 89 5D F0 89 5D F4 56 85 C0 74 3F"), void, __fastcall,
		  void* ecx, void* edx)
{
	F::DiscordRPC.Update();
	F::Misc.SteamRPC();
}