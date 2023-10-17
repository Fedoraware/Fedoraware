#include "PacketManip.h"

inline bool CPacketManip::WillTimeOut() {
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!iNetChan) {
		return false;
	}

	return iNetChan->m_nChokedPackets >= 21;
}

inline bool CPacketManip::RunAntiAim(CUserCmd* pCmd, bool* pSendPacket) {
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!iNetChan) {
		return false;
	}

	F::AntiAim.Run(pCmd, pSendPacket);
	return !iNetChan->m_nChokedPackets;
}

inline void CPacketManip::RunFakeLag(CUserCmd* pCmd, bool* pSendPacket, const int nOldGroundInt, const int nOldFlags) {
	F::FakeLag.Run(pCmd, pSendPacket, nOldGroundInt, nOldFlags);
}

void CPacketManip::CreateMove(CUserCmd* pCmd, bool* pSendPacket, const int nOldGroundInt, const int nOldFlags) {
	*pSendPacket = true;

	//prevent overchoking by just not running anything below if we believe it will cause us to time out
	if (WillTimeOut()) {
		return;
	}

	//anti aim will no longer set pSendPacket to false/true
	if (RunAntiAim(pCmd, pSendPacket) || G::ShouldShift || G::SilentTime) {
		*pSendPacket = false;
		return;
	}

	//fakelag will ONLY be able to disable pSendPacket, if anyone adds code in here that enables it they should be sorely ashamed.
	RunFakeLag(pCmd, pSendPacket, nOldGroundInt, nOldFlags);
}