#pragma once
#include "../../../SDK/SDK.h"

class CPong {
	void Init();
	void Reset();
	void DrawMenu();
	void DrawGame();
	void UpdateGame();
	void CheckCollisions();
	void UpdateInput();

	// Networking
	void Disonnect();
	void UpdateNetwork();
	void BroadcastMatch();
	void JoinMatch(int targetID);
	void SendMatch(float playerY, float playerVel, const Vec2& ballPos, const Vec2& ballVel);
	void SendResponse(float playerY, float playerVel);
	void SendPong(float playerY, int score, float ballVelX);

	int LeftScore = 0;
	int RightScore = 0;

	float LeftY = 200.f;
	float RightY = 200.f;

	float LeftVelocity = 0.f;
	float RightVelocity = 0.f;

	Vec2 RacketSize = { 5, 40 };
	float XOffset = 20.f;

	Vec2 BallPos = { 300.f, 200.f };
	Vec2 BallVelocity = { 1.f, -1.f };
	float BallSize = 5.f;

	Vec2 WindowSize = { 600.f, 400.f };

public:
	void Render();
	void ReceiveData(const std::vector<std::string>& dataVector);

	bool IsOpen = false;
};

ADD_FEATURE(CPong, Pong)