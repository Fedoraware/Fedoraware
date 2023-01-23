#pragma once

class WEngineClient {
	CEngineClient* EngineClient;

public:
	explicit WEngineClient(CEngineClient* engineClient) : EngineClient(engineClient) { }

	bool IsValid()
	{
		return EngineClient != nullptr;
	}

	bool IsInGame() {
		if (!IsValid()) { return false; }
		return EngineClient->IsInGame();
	}

	bool IsConnected() {
		if (!IsValid()) { return false; }
		return EngineClient->IsConnected();
	}

	bool IsTakingScreenshot() {
		if (!IsValid()) { return false; }
		return EngineClient->IsTakingScreenshot();
	}

	void ExecuteCommand(const char* szCommand) {
		if (!IsValid()) { return; }
		EngineClient->ClientCmd_Unrestricted(szCommand);
	}

	int GetLocalPlayer() {
		if (!IsValid()) { return 0; }
		return EngineClient->GetLocalPlayer();
	}

	int GetMaxClients() {
		if (!IsValid()) { return 0; }
		return EngineClient->GetMaxClients();
	}

	const char* GetLevelName() {
		if (!IsValid()) { return ""; }
		return EngineClient->GetLevelName();
	}

	int GetPlayerForUserID(int userID)
	{
		if (!IsValid()) { return 0; }
		return EngineClient->GetPlayerForUserID(userID);
	}

	Vec3 GetScreenSize() {
		if (!IsValid()) { return { }; }

		int w, h;
		EngineClient->GetScreenSize(w, h);
		return { static_cast<float>(w), static_cast<float>(h), 0.f };
	}

	Vec3 GetViewAngles() {
		if (!IsValid()) { return { }; }

		Vec3 viewAngles;
		EngineClient->GetViewAngles(viewAngles);
		return viewAngles;
	}

	void SetViewAngles(Vec3 viewAngles) {
		if (!IsValid()) { return; }
		EngineClient->SetViewAngles(viewAngles);
	}
};