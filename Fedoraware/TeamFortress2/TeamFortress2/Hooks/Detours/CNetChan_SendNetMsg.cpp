#include "../Hooks.h"

//	"NetMsg"
//	@net_chan.cpp L2524
MAKE_HOOK(CNetChan_SendNetMsg, g_Pattern.Find(L"engine.dll", L"55 8B EC 57 8B F9 8D 8F ? ? ? ? E8 ? ? ? ? 85 C0 75 07 B0 01 5F 5D C2 0C 00"), bool, __fastcall,
	CNetChannel* netChannel, void* edi, INetMessage& msg, bool bForceReliable, bool bVoice)
{
	switch (msg.GetType()) {
	case clc_VoiceData: {		// stop lag with voice chat
		bVoice = true;
		break;
	}
	case clc_FileCRCCheck: {	// whitelist
		if (Vars::Misc::BypassPure.Value) {
			return false;
		}
	}
	}

	return Hook.Original<FN>()(netChannel, edi, msg, bForceReliable, bVoice);
}