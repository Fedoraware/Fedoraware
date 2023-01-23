#pragma once

class WClientMode {
	CClientModeShared* ClientMode;

public:
	explicit WClientMode(CClientModeShared* clientMode) : ClientMode(clientMode) { }

	bool IsValid()
	{
		return ClientMode != nullptr;
	}

	void ChatPrintf(const char* msg)
	{
		if (!IsValid()) { return; }
		ClientMode->m_pChatElement->ChatPrintf(0, "%s", msg);
	}
};