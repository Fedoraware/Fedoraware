#pragma once
#include "../../Includes/Includes.h"

class CBaseClientDLL
{
public:
	CClientClass* GetAllClasses()
	{
		typedef CClientClass* (__thiscall* FN)(PVOID);
		return GetVFunc<FN>(this, 8)(this);
	}

	bool WriteUsercmdDeltaToBuffer(bf_write* buf, int from, int to, bool isnewcommand)
	{
		typedef bool(__thiscall* FN)(PVOID, bf_write*, int, int, bool);
		return GetVFunc<FN>(this, 23)(this, buf, from, to, isnewcommand);
	}

	//void CHLClient::VoiceStatus(int entindex, qboolean bTalking)
	//{
	//	GetClientVoiceMgr()->UpdateSpeakerStatus(entindex, !!bTalking);
	//}

	bool GetPlayerView(CViewSetup& playerView)
	{
		typedef bool(__thiscall* FN)(PVOID, CViewSetup&);
		return GetVFunc<FN>(this, 59)(this, playerView);
	}
};

#define CLIENT_DLL_INTERFACE_VERSION "VClient017"