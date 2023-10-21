#include "../Hooks.h"
#include "../../Features/AntiHack/CheaterDetection/CheaterDetection.h"
#include "../../Features/Visuals/Visuals.h"

struct ChatFlags_t
{
	std::string Colour;
	const char* Name;
};

MAKE_HOOK(CBaseHudChat_ChatPrintf, Utils::GetVFuncPtr(I::ClientModeShared->m_pChatElement, 19), void, __cdecl,
		  void* ecx, int iPlayerIndex, int iFilter, const char* fmt, ...)
{
	va_list marker;
	char buffer[4096];
	va_start(marker, fmt);
	vsnprintf_s(buffer, sizeof(buffer), fmt, marker);
	va_end(marker);

	if (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '\n')
	{
		buffer[strlen(buffer) - 1] = 0;
	}
	char* msg = buffer;
	while (*msg && (*msg == '\n' || *msg == '\r' || *msg == '\x1A'))
	{
		msg++;
	}
	if (!*msg) { return; }

	std::string finalMsg = msg, name = {};

	PlayerInfo_t info{};
	if (!I::EngineClient->GetPlayerInfo(iPlayerIndex, &info))
	{
		return Hook.Original<FN>()(ecx, iPlayerIndex, iFilter, "%s", finalMsg.c_str());
	}

	name = info.name;
	if (finalMsg.find(name) == std::string::npos)
	{
		return Hook.Original<FN>()(ecx, iPlayerIndex, iFilter, "%s", finalMsg.c_str());
	}

	/*
	 *	Chat Flags
	 *	@https://www.unknowncheats.me/forum/team-fortress-2-a/488217-chat-flags-titles.html
		i swear its not pasted i just used this as inspiration, credits myzarfin.
	*/
	if (iPlayerIndex && Vars::Misc::ChatFlags.Value)
	{
		ChatFlags_t chatFlag;
		bool flagSet = false;

		if (iPlayerIndex == I::EngineClient->GetLocalPlayer())
		{
			chatFlag = { Vars::Colours::Local.Value.to_hex_alpha(), "[You]" };
			flagSet = true;
		}
		else if (g_EntityCache.IsFriend(iPlayerIndex) || I::EngineClient->GetPlayerInfo(iPlayerIndex, &info) && G::PlayerPriority[info.friendsID].Mode == 0)
		{
			chatFlag = { Vars::Colours::Friend.Value.to_hex_alpha(), "[Friend]" };
			flagSet = true;
		}
		else if (I::EngineClient->GetPlayerInfo(iPlayerIndex, &info) && G::PlayerPriority[info.friendsID].Mode == 1)
		{
			chatFlag = { Vars::Colours::Ignored.Value.to_hex_alpha(), "[Ignored]" };
			flagSet = true;
		}
		else if (I::EngineClient->GetPlayerInfo(iPlayerIndex, &info) && G::PlayerPriority[info.friendsID].Mode == 3)
		{
			chatFlag = { Vars::Colours::Rage.Value.to_hex_alpha(), "[Rage]" };
			flagSet = true;
		}
		else if (I::EngineClient->GetPlayerInfo(iPlayerIndex, &info) && G::PlayerPriority[info.friendsID].Mode == 4)
		{
			chatFlag = { Vars::Colours::Cheater.Value.to_hex_alpha(), "[Cheater]" };
			flagSet = true;
		}

		if (flagSet)
		{
			finalMsg = chatFlag.Colour + chatFlag.Name + " \x3" + finalMsg;
			if (auto offset = finalMsg.find(name))
			{
				finalMsg = finalMsg.replace(offset + name.length(), 0, "\x1");
			}
		}
	}

	Hook.Original<FN>()(ecx, iPlayerIndex, iFilter, "%s", finalMsg.c_str());
}