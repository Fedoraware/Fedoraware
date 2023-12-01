#pragma once
#include "../Feature.h"
#include "FakeLag/FakeLag.h"
#include "AntiAim/AntiAim.h"

class CPacketManip {
private:
	inline bool WillTimeOut();
	inline bool AACheck(CUserCmd* pCmd);
	inline void RunFakeLag(CUserCmd* pCmd, bool* pSendPacket, const int nOldGroundInt, const int nOldFlags);
public:
	void CreateMove(CUserCmd* pCmd, bool* pSendPacket, const int nOldGroundInt, const int nOldFlags);
};

ADD_FEATURE(CPacketManip, PacketManip)