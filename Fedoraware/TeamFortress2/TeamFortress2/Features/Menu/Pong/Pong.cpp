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
		const auto windowSize = ImGui::GetWindowSize();
		const auto cursorPos = ImGui::GetMousePos();

		// Draw Player
		drawList->AddRectFilled({ windowPos.x + 20.f, windowPos.y + PlayerY - (0.5f * RacketSize.y) },
			{ windowPos.x + 20.f + RacketSize.x, windowPos.y + PlayerY + (0.5f * RacketSize.y) },
			ImColor(255, 255, 255));

		// Draw Enemy
		drawList->AddRectFilled({ windowPos.x + (windowSize.x - 20.f), windowPos.y + EnemyY - (0.5f * RacketSize.y) },
			{ windowPos.x + (windowSize.x - 20.f - RacketSize.x), windowPos.y + EnemyY + (0.5f * RacketSize.y) },
			ImColor(255, 255, 255));

		// Draw Ball
		drawList->AddCircleFilled({ windowPos.x + BallPos.x, windowPos.y + BallPos.y },
			BallSize,
			ImColor(255, 255, 255));

		// Draw Scores
		drawList->AddText({ windowPos.x + 20.f, windowPos.y + 30.f }, ImColor(255, 255, 255), std::to_string(PlayerScore).c_str());
		drawList->AddText({ windowPos.x + 575.f, windowPos.y + 30.f }, ImColor(255, 255, 255), std::to_string(EnemyScore).c_str());

		static Timer updateTimer{ };
		if (updateTimer.Run(5))
		{
			BallPos += BallVelocity * std::max(BallMultiplier, 1.f);
			EnemyY += EnemyVelocity;
			PlayerY += PlayerVelocity;

			Math::Clamp(EnemyY, 20.f, 400.f);
			Math::Clamp(PlayerY, 20.f, 400.f);
		}

		/* Collisions */

		// X-Walls (Loss)
		if (BallPos.x + BallSize >= windowSize.x - 7.f)
		{
			PlayerScore++;
			BallMultiplier -= 0.02f;
			Reset();
		} else if (BallPos.x - BallSize <= 0.f)
		{
			EnemyScore++;
			BallMultiplier -= 0.04f;
			Reset();
		}

		// Y-Walls (Bounce)
		if (BallPos.y - 25.f - BallSize <= 0.f)
		{
			BallVelocity.y = 1.f;
		} else if (BallPos.y + BallSize >= windowSize.y)
		{
			BallVelocity.y = -1.f;
		}

		// Enemy
		if (BallPos.x + BallSize >= windowSize.x - 20.f - RacketSize.x
			&& BallPos.y - BallSize < EnemyY + (0.5f * RacketSize.y) && BallPos.y + BallSize > EnemyY - (0.5f * RacketSize.y))
		{
			if (BallVelocity.x > 0)
			{
				I::Engine->ClientCmd_Unrestricted("play ui/cyoa_switch");
			}
			BallVelocity.x = -1.f;
		}

		// Player
		if (BallPos.x - BallSize <= 20.f + RacketSize.x
			&& BallPos.y - BallSize < PlayerY + (0.5f * RacketSize.y) && BallPos.y + BallSize > PlayerY - (0.5f * RacketSize.y))
		{
			if (BallVelocity.x < 0)
			{
				I::Engine->ClientCmd_Unrestricted("play ui/cyoa_switch");
			}
			BallVelocity.x = 1.f;
			BallMultiplier += 0.02f;
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

		// Reset if one player winds
		if (PlayerScore >= 10 || EnemyScore >= 10)
		{
			Init();
			I::Engine->ClientCmd_Unrestricted("play ui/duel_challenge");
		}
	}
	ImGui::End();

	ImGui::PopStyleColor();
}

void CPong::Init()
{
	PlayerScore = 0;
	EnemyScore = 0;
	PlayerY = 200.f;
	EnemyY = 200.f;
	BallPos = { 300.f, 200.f };
	BallVelocity = { 1.f, -1.f };
}

void CPong::Reset()
{
	BallPos = { 300.f, 200.f };
	BallVelocity = { 1.f, -1.f };
	I::Engine->ClientCmd_Unrestricted("play ui/chat_display_text");
}
