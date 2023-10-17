#include "PacketManip.h"

inline bool CPacketManip::WillTimeOut() {
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!iNetChan) {
		return false;
	}

	return iNetChan->m_nChokedPackets >= 22;
}

inline bool CPacketManip::AACheck(CUserCmd* pCmd) {
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!iNetChan || !pLocal) {
		return false;
	}

	return !iNetChan->m_nChokedPackets && F::AntiAim.ShouldAntiAim(pLocal);
}

inline void CPacketManip::RunFakeLag(CUserCmd* pCmd, bool* pSendPacket, const int nOldGroundInt, const int nOldFlags) {
	F::FakeLag.Run(pCmd, pSendPacket, nOldGroundInt, nOldFlags);
}

inline void CPacketManip::Run(CUserCmd* pCmd, bool* pSendPacket, const int nOldGroundInt, const int nOldFlags) {
	

	//fakelag will ONLY be able to disable pSendPacket, if anyone adds code in here that enables it they should be sorely ashamed.
	
}

void CPacketManip::CreateMove(CUserCmd* pCmd, bool* pSendPacket, const int nOldGroundInt, const int nOldFlags) {
	*pSendPacket = true;

	//prevent overchoking by just not running anything below if we believe it will cause us to time out
	if (!WillTimeOut()) {
		//anti aim will no longer set pSendPacket to false/true
		F::AntiAim.RunReal(pCmd);
		if (AACheck(pCmd) || G::SilentTime) {
			*pSendPacket = false;
			return;
		}
		RunFakeLag(pCmd, pSendPacket, nOldGroundInt, nOldFlags);
	}
	if (G::ShouldShift) {
		*pSendPacket = G::ShiftedTicks == 1;
	}
	if (*pSendPacket) {
		F::AntiAim.RunFake(pCmd);
	}

	return;
}