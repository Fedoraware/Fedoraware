#pragma once
#include "../../../SDK/SDK.h"

class CPong {
	void Init();
	void Reset();
	void DrawGame();
	void UpdateGame();
	void CheckCollisions();
	void UpdateInput();

	int PlayerScore = 0;
	int EnemyScore = 0;

	float PlayerY = 200.f;
	float EnemyY = 200.f;

	float PlayerVelocity = 0.f;
	float EnemyVelocity = 0.f;

	Vec2 RacketSize = { 5, 40 };
	float XOffset = 20.f;

	Vec2 BallPos = { 300.f, 200.f };
	Vec2 BallVelocity = { 1.f, -1.f };
	float BallSize = 5.f;
	float BallMultiplier = 1.f;

	Vec2 WindowSize = { 600.f, 400.f };

public:
	void Render();
	bool IsOpen = false;
};

ADD_FEATURE(CPong, Pong)