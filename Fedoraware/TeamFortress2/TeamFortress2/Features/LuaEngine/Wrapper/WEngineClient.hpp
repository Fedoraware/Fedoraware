#pragma once

class WEngineClient {
	CEngineClient* EngineClient;

public:
	explicit WEngineClient(CEngineClient* engineClient) : EngineClient(engineClient) { }

	bool IsInGame() {
		return EngineClient->IsInGame();
	}

	bool IsConnected() {
		return EngineClient->IsConnected();
	}

	bool IsTakingScreenshot() {
		return EngineClient->IsTakingScreenshot();
	}

	void ExecuteCommand(const char* szCommand) {
		EngineClient->ClientCmd_Unrestricted(szCommand);
	}

	int GetLocalPlayer() {
		return EngineClient->GetLocalPlayer();
	}

	int GetMaxClients() {
		return EngineClient->GetMaxClients();
	}

	const char* GetLevelName() {
		return EngineClient->GetLevelName();
	}

	Vec3 GetScreenSize() {
		int w, h;
		EngineClient->GetScreenSize(w, h);
		return { static_cast<float>(w), static_cast<float>(h), 0.f };
	}

	Vec3 GetViewAngles() {
		Vec3 viewAngles;
		EngineClient->GetViewAngles(viewAngles);
		return viewAngles;
	}

	void SetViewAngles(Vec3 viewAngles) {
		EngineClient->SetViewAngles(viewAngles);
	}
};