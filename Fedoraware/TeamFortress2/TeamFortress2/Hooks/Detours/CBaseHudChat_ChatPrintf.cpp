#include "../Hooks.h"
#include "../../Features/AntiHack/CheaterDetection/CheaterDetection.h"
#include "../../Features/Visuals/Visuals.h"

// @https://www.unknowncheats.me/forum/team-fortress-2-a/488217-chat-flags-titles.html
//	i swear its not pasted i just used this as inspiration, credits myzarfin.



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
	if (!*msg)
		return;

	std::string final_msg = msg, name = {};

	
	PlayerInfo_t info{};
	ChatFlags_t flag;
	bool set = false;
	if (!I::EngineClient->GetPlayerInfo(iPlayerIndex, &info))
	{
		return Hook.Original<FN>()(ecx, iPlayerIndex, iFilter, "%s", final_msg.c_str());
	}
	else
	{
		name = info.name;
		if (final_msg.find(name) == std::string::npos)
		{
			return Hook.Original<FN>()(ecx, iPlayerIndex, iFilter, "%s", final_msg.c_str());
		}
	}
	if (iPlayerIndex && Vars::Misc::RunescapeChat.Value)
	{
		if (const auto& pEntity = I::ClientEntityList->GetClientEntity(iPlayerIndex))
		{
			auto backup_msg = final_msg;
			if (auto offset = backup_msg.find(name))
			{
				backup_msg = backup_msg.erase(offset, offset + name.length() + 2);
				backup_msg.erase(std::remove_if(backup_msg.begin(), backup_msg.end(), [](char c) -> bool { return c == '\x3'; }), backup_msg.end());
				if (backup_msg.rfind("(Voice)", 0) != 0) {
					F::RSChat.PushChat(pEntity, backup_msg);
				}
				F::RSChat.PushChat(pEntity, backup_msg);
			}
		}
	}

	if (iPlayerIndex && Vars::Misc::ChatFlags.Value)
	{
		if (iPlayerIndex == I::EngineClient->GetLocalPlayer())
		{
			flag = { Colors::Local.to_hex_alpha(), "[You]"};
			set = true;
		}
		else if (g_EntityCache.IsFriend(iPlayerIndex))
		{
			flag = { Colors::Friend.to_hex_alpha(), "[Friend]" };
			set = true;
		}
		else if (I::EngineClient->GetPlayerInfo(iPlayerIndex, &info) && F::BadActors.MarkedCheaters[info.friendsID])
		{
			static const auto red = Color_t{255, 0, 0, 255};
			flag = { red.to_hex_alpha(), "[Cheater]"};
			set = true;
		}

		if (set)
		{
			final_msg = flag.Colour + flag.Name + " \x3" + final_msg;
			if (auto offset = final_msg.find(name))
			{
				final_msg.substr(offset + name.length());
				final_msg = final_msg.replace(offset + name.length(), 0, "\x1");
			}
		}
	}
	Hook.Original<FN>()(ecx, iPlayerIndex, iFilter, "%s", final_msg.c_str());
}