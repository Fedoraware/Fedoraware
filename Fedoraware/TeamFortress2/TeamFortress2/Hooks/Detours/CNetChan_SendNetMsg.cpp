#include "../Hooks.h"

//	"NetMsg"
//	@net_chan.cpp L2524
MAKE_HOOK(CNetChan_SendNetMsg, g_Pattern.Find(L"engine.dll", L"55 8B EC 56 8B F1 8D 8E ? ? ? ? E8 ? ? ? ? 85 C0 75 07"), bool, __fastcall,
	void* CNetChan, void* edi, INetMessage& msg, bool bForceReliable, bool bVoice)
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

	return Hook.Original<FN>()(CNetChan, edi, msg, bForceReliable, bVoice);
}