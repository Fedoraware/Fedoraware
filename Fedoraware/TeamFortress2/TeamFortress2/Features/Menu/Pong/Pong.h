#pragma once
#include "../../../SDK/SDK.h"

class CPong {
	void Reset();

	int PlayerScore = 0;
	int EnemyScore = 0;

	float PlayerY = 200.f;
	float EnemyY = 200.f;

	float PlayerVelocity = 0.f;
	float EnemyVelocity = 0.f;

	Vec2 BallPos = { 300.f, 200.f };
	Vec2 BallVelocity = { 1.f, -1.f };
	float BallMultiplier = 1.f;

public:
	void Render();
	bool IsOpen = false;
};

ADD_FEATURE(CPong, Pong)