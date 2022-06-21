#include "Pong.h"
#include "../Menu.h"

void CPong::Render()
{
	if (!IsOpen) { return; }

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 1.f));
	ImGui::SetNextWindowSize({ 600.f, 400.f });
	
	if (ImGui::Begin("Pong", &IsOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		const auto drawList = ImGui::GetWindowDrawList();
		const auto windowPos = ImGui::GetWindowPos();
		const auto cursorPos = ImGui::GetMousePos();

		// Draw Player
		drawList->AddRectFilled({ windowPos.x + 20.f, windowPos.y + PlayerY - 20.f }, { windowPos.x + 25.f, windowPos.y + PlayerY + 20.f }, ImColor(255, 255, 255));

		// Draw Enemy
		drawList->AddRectFilled({ windowPos.x + 580.f, windowPos.y + EnemyY - 20.f }, { windowPos.x + 575.f, windowPos.y + EnemyY + 20.f }, ImColor(255, 255, 255));

		// Draw Ball
		drawList->AddCircleFilled({ windowPos.x + BallPos.x, windowPos.y + BallPos.y }, 5.f, ImColor(255, 255, 255));

		// Draw Scores
		drawList->AddText({ windowPos.x + 20.f, windowPos.y + 30.f }, ImColor(255, 255, 255), std::to_string(PlayerScore).c_str());
		drawList->AddText({ windowPos.x + 575.f, windowPos.y + 30.f }, ImColor(255, 255, 255), std::to_string(EnemyScore).c_str());

		static Timer updateTimer{ };
		if (updateTimer.Run(5))
		{
			BallPos += BallVelocity * BallMultiplier;
			EnemyY += EnemyVelocity;
			PlayerY += PlayerVelocity;

			Math::Clamp(EnemyY, 20.f, 400.f);
			Math::Clamp(PlayerY, 20.f, 400.f);
		}

		/* Collisions */

		// Enemy
		if (BallPos.x + 5.f >= 575.f && BallPos.x + 5.f <= 580.f
			&& BallPos.y < EnemyY + 20.f && BallPos.y > EnemyY - 20.f)
		{
			BallVelocity.x = -1.f;
		}

		// Player
		if (BallPos.x - 5.f <= 25.f && BallPos.x - 5.f >= 20.f
			&& BallPos.y < PlayerY + 20.f && BallPos.y > PlayerY - 20.f)
		{
			BallVelocity.x = 1.f;
			BallMultiplier += 0.02f;
		}

		// X-Walls (Loss)
		if (BallPos.x + 5.f >= 600.f)
		{
			PlayerScore++;
			BallPos = { 300.f, 200.f };
			BallVelocity = { 1.f, -1.f };
		} else if (BallPos.x - 5.f <= 0.f)
		{
			EnemyScore++;
			BallPos = { 300.f, 200.f };
			BallVelocity = { 1.f, -1.f };
		}

		// Y-Walls (Bounce)
		if (BallPos.y - 30.f <= 0.f)
		{
			BallVelocity.y = 1.f;
		} else if (BallPos.y + 5.f >= 400.f)
		{
			BallVelocity.y = -1.f;
		}

		// Enemy controller
		if (BallPos.y > EnemyY)
		{
			EnemyVelocity = 1.f;
		} else if (BallPos.y < EnemyY)
		{
			EnemyVelocity = -1.f;
		} else
		{
			EnemyVelocity = 0.f;
		}

		// Player controller
		if (cursorPos.y - windowPos.y < PlayerY)
		{
			PlayerVelocity = -1.f;
		} else if (cursorPos.y - windowPos.y > PlayerY)
		{
			PlayerVelocity = 1.f;
		} else
		{
			PlayerVelocity = 0.f;
		}

		if (PlayerScore >= 10 || EnemyScore >= 10)
		{
			Reset();
		}
	}
	ImGui::End();

	ImGui::PopStyleColor();
}

void CPong::Reset()
{
	PlayerScore = 0;
	EnemyScore = 0;
	PlayerY = 200.f;
	EnemyY = 200.f;
	BallPos = { 300.f, 200.f };
	BallVelocity = { 1.f, -1.f };
}