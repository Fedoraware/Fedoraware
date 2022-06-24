#include "Pong.h"
#include "../Menu.h"
#include "../../Fedworking/Fedworking.h"

enum MessageType {
	Broadcast,	// [ Type, SubType, SenderID ]
	Request,	// [ Type, SubType, SenderID, TargetID ]
	Update,		// [ Type, SubType, SenderID, PlayerY, PlayerVel, BallPosX, BallPosY, BallVelX, BallVelY ]
	Response,	// [ Type, SubType, SenderID, PlayerY, PlayerVel ]
	Pong		// [ Type, SubType, SenderID, PlayerY, Score, BallVelX ]
};

enum class GameState {
	None,
	Hosting,
	Joining,
	Match
};

static int PlayerID = Utils::RandInt(100, 999);
static int EnemyID = 0;

static GameState CurrentState = GameState::None;
static bool IsMultiplayer = false;
static bool IsHost = false;
static std::map<int, float> FoundMatches; // <PlayerID, Time>

void CPong::Render()
{
	if (!IsOpen)
	{
		if (IsMultiplayer)
		{
			Disonnect();
		}

		CurrentState = GameState::None;
		return;
	}

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 1.f));
	ImGui::SetNextWindowSize({ WindowSize.x, WindowSize.y });
	
	if (ImGui::Begin("Pong", &IsOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		if (CurrentState != GameState::Match)
		{
			DrawMenu();
		} else if (CurrentState == GameState::Match)
		{
			DrawGame();
			UpdateGame();
			CheckCollisions();
			UpdateInput();

			// Reset if one player winds
			if (LeftScore >= 10 || RightScore >= 10)
			{
				Init();
				I::Engine->ClientCmd_Unrestricted("play ui/duel_challenge");
			}
		}

		UpdateNetwork();
	}
	ImGui::End();

	ImGui::PopStyleColor();
}

/* Draw the main menu */
void CPong::DrawMenu()
{
	if (CurrentState == GameState::None)
	{
		if (ImGui::Button("Singleplayer"))
		{
			Init();
			CurrentState = GameState::Match;
			IsMultiplayer = false;
		}

		if (Vars::Misc::PartyNetworking.Value)
		{
			ImGui::SameLine();
			if (ImGui::Button("Host Match"))
			{
				CurrentState = GameState::Hosting;
				IsMultiplayer = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("Join Match"))
			{
				CurrentState = GameState::Joining;
				IsMultiplayer = true;
			}
		} else
		{
			ImGui::Text("Enable 'Party Networking' to play online!");
		}
	} else
	{
		if (CurrentState == GameState::Hosting)
		{
			ImGui::Text("Hosting match in your party...");
		}

		if (CurrentState == GameState::Joining)
		{
			ImGui::Text("Searching for matches in your party...");

			for (const auto& match : FoundMatches)
			{
				if (I::Engine->Time() - match.second > 15.f) { continue; }
				
				if (ImGui::Button(tfm::format("Match #%i", match.first).c_str()))
				{
					JoinMatch(match.first);
				}
			}
		}

		if (ImGui::Button("Cancel"))
		{
			CurrentState = GameState::None;
			IsMultiplayer = false;
		}
	}
}

/* Draws the players, ball and scores */
void CPong::DrawGame()
{
	const auto drawList = ImGui::GetWindowDrawList();
	const auto windowPos = ImGui::GetWindowPos();

	// Draw Player
	drawList->AddRectFilled({ windowPos.x + XOffset, windowPos.y + LeftY - (0.5f * RacketSize.y) },
		{ windowPos.x + XOffset + RacketSize.x, windowPos.y + LeftY + (0.5f * RacketSize.y) },
		ImColor(255, 255, 255));

	// Draw Enemy
	drawList->AddRectFilled({ windowPos.x + (WindowSize.x - XOffset), windowPos.y + RightY - (0.5f * RacketSize.y) },
		{ windowPos.x + (WindowSize.x - XOffset - RacketSize.x), windowPos.y + RightY + (0.5f * RacketSize.y) },
		ImColor(255, 255, 255));

	// Draw Ball
	drawList->AddCircleFilled({ windowPos.x + BallPos.x, windowPos.y + BallPos.y },
		BallSize,
		ImColor(255, 255, 255));

	// Draw Scores
	drawList->AddText({ windowPos.x + XOffset, windowPos.y + 30.f }, ImColor(255, 255, 255), std::to_string(LeftScore).c_str());
	drawList->AddText({ windowPos.x + (WindowSize.x - XOffset), windowPos.y + 30.f }, ImColor(255, 255, 255), std::to_string(RightScore).c_str());
}

/* Updates game movements every 5ms */
void CPong::UpdateGame()
{
	static Timer updateTimer{ };
	if (updateTimer.Run(5))
	{
		BallPos += BallVelocity;
		RightY += RightVelocity;
		LeftY += LeftVelocity;

		Math::Clamp(RightY, 25.f, WindowSize.y);
		Math::Clamp(LeftY, 25.f, WindowSize.y);
	}
}

/* Checks for collisions with borders and rackets */
void CPong::CheckCollisions()
{
	// X-Walls (Loss)
	if (BallPos.x + BallSize >= WindowSize.x && (!IsMultiplayer || IsHost))
	{
		LeftScore++;
		Reset();
	}
	else if (BallPos.x - BallSize <= 0.f && (!IsMultiplayer || !IsHost))
	{
		RightScore++;
		Reset();
	}

	// Y-Walls (Bounce)
	if (BallPos.y - 25.f - BallSize <= 0.f)
	{
		BallVelocity.y = 1.f;
	}
	else if (BallPos.y + BallSize >= WindowSize.y)
	{
		BallVelocity.y = -1.f;
	}

	// Enemy (Right)
	if (BallPos.x + BallSize >= WindowSize.x - XOffset - RacketSize.x
		&& BallPos.y - BallSize < RightY + (0.5f * RacketSize.y) && BallPos.y + BallSize > RightY - (0.5f * RacketSize.y))
	{
		if (BallVelocity.x > 0)
		{
			I::Engine->ClientCmd_Unrestricted("play ui/cyoa_switch");
		}
		BallVelocity.x = -1.f;

		if (IsMultiplayer && !IsHost)
		{
			SendPong(RightY, RightScore, BallVelocity.x);
		}
	}

	// Player (Left)
	if (BallPos.x - BallSize <= XOffset + RacketSize.x
		&& BallPos.y - BallSize < LeftY + (0.5f * RacketSize.y) && BallPos.y + BallSize > LeftY - (0.5f * RacketSize.y))
	{
		if (BallVelocity.x < 0)
		{
			I::Engine->ClientCmd_Unrestricted("play ui/cyoa_switch");
		}
		BallVelocity.x = 1.f;
	}
}

/* Updates the player and AI controllers */
void CPong::UpdateInput()
{
	const auto windowPos = ImGui::GetWindowPos();
	const auto cursorPos = ImGui::GetMousePos();

	// Local match
	if (!IsMultiplayer)
	{
		// Enemy controller (CPU)
		if (BallPos.y > RightY)
		{
			RightVelocity = 1.f;
		}
		else if (BallPos.y < RightY)
		{
			RightVelocity = -1.f;
		}
		else
		{
			RightVelocity = 0.f;
		}
	}

	if (!IsMultiplayer || IsHost)
	{
		// Player controller (Left)
		if (cursorPos.y - windowPos.y < LeftY)
		{
			LeftVelocity = -1.f;
		}
		else if (cursorPos.y - windowPos.y > LeftY)
		{
			LeftVelocity = 1.f;
		}
		else
		{
			LeftVelocity = 0.f;
		}
	} else
	{
		// Player controller (Right)
		if (cursorPos.y - windowPos.y < RightY)
		{
			RightVelocity = -1.f;
		}
		else if (cursorPos.y - windowPos.y > RightY)
		{
			RightVelocity = 1.f;
		}
		else
		{
			RightVelocity = 0.f;
		}
	}
}

/* Initializes a new game */
void CPong::Init()
{
	LeftScore = 0;
	RightScore = 0;
	LeftY = 200.f;
	RightY = 200.f;
	BallPos = { 300.f, 200.f };
	BallVelocity = { 1.f, -1.f };
}

/* Resets the ball and players */
void CPong::Reset()
{
	BallPos = { 300.f, 200.f };
	BallVelocity = { 1.f, -1.f };
	I::Engine->ClientCmd_Unrestricted("play ui/chat_display_text");
}

#pragma region Networking
void CPong::ReceiveData(const std::vector<std::string>& dataVector)
{
	int subType, senderID;
	try
	{
		subType = std::stoi(dataVector[1]);
		senderID = std::stoi(dataVector[2]);
	}
	catch (...) { return; }

	// Ignore our own messages
	if (senderID == PlayerID) { return; }

	switch (subType)
	{
	case Broadcast:
		{
			if (dataVector.size() == 3)
			{
				FoundMatches[senderID] = I::Engine->Time();
			}
			break;
		}

	case Request:
		{
			if (dataVector.size() == 4)
			{
				const int targetID = std::stoi(dataVector[3]);

				// Did we host this match?
				if (targetID == PlayerID)
				{
					EnemyID = senderID;
					CurrentState = GameState::Match;
					Init();
					IsHost = true;
				}

				// Remove this match from the available matches
				FoundMatches[targetID] = 0.f;
			}
			break;
		}

	case Update:
		{
			if (senderID == EnemyID && dataVector.size() == 9)
			{
				const float playerY = std::stof(dataVector[3]);
				const float playerVel = std::stof(dataVector[4]);
				const float ballX = std::stof(dataVector[5]);
				const float ballY = std::stof(dataVector[6]);
				const float ballVelX = std::stof(dataVector[7]);
				const float ballVelY = std::stof(dataVector[8]);

				// Update local data
				LeftY = playerY;
				LeftVelocity = playerVel * 0.8f;
				BallVelocity = { ballVelX, ballVelY };

				if (std::abs(ballX - BallPos.x) > 7.f || std::abs(ballY - BallPos.y) > 7.f)
				{
					BallPos = { ballX, ballY };
				}

				// Start the match if required
				if (IsMultiplayer && CurrentState != GameState::Match)
				{
					CurrentState = GameState::Match;
					IsHost = false;
					Init();
				}

				// Send a response
				if (!IsHost)
				{
					SendResponse(RightY, RightVelocity);
				}
			}
			break;
		}

	case Response:
		{
			if (senderID == EnemyID && dataVector.size() == 5)
			{
				const float playerY = std::stof(dataVector[3]);
				const float playerVel = std::stof(dataVector[4]);

				// Update local data
				RightY = playerY;
				RightVelocity = playerVel * 0.8f;
			}
			break;
		}

	case Pong:
		{
			if (senderID == EnemyID && dataVector.size() == 6)
			{
				const float playerY = std::stof(dataVector[3]);
				const int score = std::stoi(dataVector[4]);
				const float ballVelX = std::stof(dataVector[5]);

				if (IsHost)
				{
					RightY = playerY;
					RightScore = score;
				} else
				{
					LeftY = playerY;
					LeftScore = score;
				}

				BallVelocity.x = ballVelX;
			}
			break;
		}
	}
}

void CPong::Disonnect()
{
	EnemyID = 0;
	IsMultiplayer = false;
	IsHost = false;
	FoundMatches.clear();
}

void CPong::UpdateNetwork()
{
	static Timer netTimer{ };
	if (netTimer.Run(CurrentState == GameState::Match ? 300 : 500))
	{
		if (CurrentState == GameState::Hosting)
		{
			BroadcastMatch();
		} else if (CurrentState == GameState::Match && IsHost)
		{
			SendMatch(LeftY, LeftVelocity, BallPos, BallVelocity);
		}
	}
}

/* Broadcast out match */
void CPong::BroadcastMatch()
{
	std::stringstream msg;
	msg << Broadcast << "&" << PlayerID;
	F::Fedworking.SendPong(msg.str());
}

/* Request to join a match */
void CPong::JoinMatch(int targetID)
{
	std::stringstream msg;
	msg << Request << "&" << PlayerID << "&" << targetID;
	F::Fedworking.SendPong(msg.str());

	EnemyID = targetID;
}

/* Sent by the host */
void CPong::SendMatch(float playerY, float playerVel, const Vec2& ballPos, const Vec2& ballVel)
{
	std::stringstream msg;
	msg << Update << "&" << PlayerID << "&" <<
		static_cast<int>(playerY) << "&" << static_cast<int>(playerVel) << "&" <<
		static_cast<int>(ballPos.x) << "&" << static_cast<int>(ballPos.y) << "&" <<
		static_cast<int>(ballVel.x) << "&" << static_cast<int>(ballVel.y);
	F::Fedworking.SendPong(msg.str());
}

/* Send after we've received data from the host */
void CPong::SendResponse(float playerY, float playerVel)
{
	std::stringstream msg;
	msg << Response << "&" << PlayerID << "&" <<
		static_cast<int>(playerY) << "&" << static_cast<int>(playerVel);
	F::Fedworking.SendPong(msg.str());
}

/* Sent once a player hits the ball */
void CPong::SendPong(float playerY, int score, float ballVelX)
{
	std::stringstream msg;
	msg << Pong << "&" << static_cast<int>(playerY) << "&" << score << "&" <<
		static_cast<int>(ballVelX);
	F::Fedworking.SendPong(msg.str());
}
#pragma endregion