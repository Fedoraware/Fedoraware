#include "../Hooks.h"
#include "../../Features/AntiHack/CheaterDetection/CheaterDetection.h"

// @https://www.unknowncheats.me/forum/team-fortress-2-a/488217-chat-flags-titles.html
//	i swear its not pasted i just used this as inspiration, credits myzarfin.

struct ChatFlags_t {
	std::string Colour;
	const char* Name;
};

MAKE_HOOK(CBaseHudChat_ChatPrintf, Utils::GetVFuncPtr(I::ClientMode->m_pChatElement, 19), void, __cdecl,
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

	std::string final_msg = msg;

	if (Vars::Misc::ChatFlags.Value && iPlayerIndex) {
		ChatFlags_t flag;
		bool set = false;
		PlayerInfo_t pi{ };

		if (iPlayerIndex == I::Engine->GetLocalPlayer()) {
			//#A043E7
			flag = { { '\x7', 'A', '0', '4', '3', 'E', '7' }, ("[You]") };
			set = true;
		}
		else if (g_EntityCache.IsFriend(iPlayerIndex)) {
			//#699A00
			flag = { { '\x7', '6', '9', '9', 'A', '0', '0' }, ("[Friend]") };
			set = true;
		}
		else if (I::Engine->GetPlayerInfo(iPlayerIndex, &pi) && F::BadActors.MarkedCheaters[pi.friendsID]) {
			//#9B0000
			flag = { { '\x7', '9', 'B', '0', '0', '0', '0' }, ("[Cheater]") };
			set = true;
		}

		if (set) {
			final_msg = flag.Colour + flag.Name + (" \x3") + final_msg;

			if (auto offset = final_msg.find(":")) {
				std::string unneeded = final_msg.substr(offset + 1);
				final_msg = final_msg.replace(offset, 1, ("\x1:"));
			}
		}
	}

	Hook.Original<FN>()(ecx, iPlayerIndex, iFilter, "%s", final_msg.c_str());
}