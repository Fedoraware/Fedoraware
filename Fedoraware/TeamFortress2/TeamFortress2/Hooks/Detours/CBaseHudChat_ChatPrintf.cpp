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

	if (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '\n') {
		buffer[strlen(buffer) - 1] = 0;
	}
	char* msg = buffer;
	while (*msg && (*msg == '\n' || *msg == '\r' || *msg == '\x1A')) {
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

	/* Runescape Chat */
	if (iPlayerIndex && Vars::Misc::RunescapeChat.Value)
	{
		if (const auto& pEntity = I::ClientEntityList->GetClientEntity(iPlayerIndex))
		{
			auto backupMsg = finalMsg;
			if (const auto offset = backupMsg.find(name))
			{
				backupMsg = backupMsg.erase(offset, offset + name.length() + 2);
				backupMsg.erase(std::remove_if(backupMsg.begin(), backupMsg.end(), [](char c) -> bool { return c == '\x3'; }), backupMsg.end());
				if (backupMsg.find("(Voice)") == std::string::npos) {
					F::RSChat.PushChat(pEntity, backupMsg);
				}
			}
		}
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
			chatFlag = { Colors::Local.to_hex_alpha(), "[You]"};
			flagSet = true;
		}
		else if (g_EntityCache.IsFriend(iPlayerIndex))
		{
			chatFlag = { Colors::Friend.to_hex_alpha(), "[Friend]" };
			flagSet = true;
		}
		else if (I::EngineClient->GetPlayerInfo(iPlayerIndex, &info) && G::PlayerPriority[info.friendsID].Mode == 4)
		{
			static constexpr auto RED = Color_t{255, 0, 0, 255};
			chatFlag = { RED.to_hex_alpha(), "[Cheater]"};
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