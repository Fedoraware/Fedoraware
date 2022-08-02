#pragma once

class WEngineClient {
	CEngineClient* EngineClient;

public:
	explicit WEngineClient(CEngineClient* engineClient) : EngineClient(engineClient) { }

	void ExecuteCommand(const char* cmd) { EngineClient->ClientCmd_Unrestricted(cmd); }
	int GetLocalPlayer() { return EngineClient->GetLocalPlayer(); }
	bool IsConnected() { return EngineClient->IsConnected(); }
	bool IsInGame() { return EngineClient->IsInGame(); }
};