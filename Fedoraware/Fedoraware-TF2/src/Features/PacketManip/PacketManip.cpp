#include "PacketManip.h"

inline bool CPacketManip::WillTimeOut() {
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!iNetChan) {
		return false;
	}

	return iNetChan->m_nChokedPackets >= 21;
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

void CPacketManip::CreateMove(CUserCmd* pCmd, bool* pSendPacket, const int nOldGroundInt, const int nOldFlags) {
	*pSendPacket = true;

	//prevent overchoking by just not running anything below if we believe it will cause us to time out
	if (!WillTimeOut()) {
		//anti aim will no longer set pSendPacket to false/true
		if (AACheck(pCmd) || G::SilentTime) {
			*pSendPacket = false;
			F::AntiAim.Run(pCmd, pSendPacket);
			return;
		}
		//F::AntiAim.Run(pCmd, pSendPacket);
		RunFakeLag(pCmd, pSendPacket, nOldGroundInt, nOldFlags);
	}
	
	if (G::ShouldShift) {
		*pSendPacket = G::ShiftedTicks == 1;
	}

	return F::AntiAim.Run(pCmd, pSendPacket);;
}