#include "Fedworking.h"
#include "../../Utils/Base64/Base64.hpp"

enum MessageType {
	None,
	Marker,	// [ Type, X-Pos, Y-Pos, Z-Pos, Player-IDX ]
	ESP		// [ Type, X-Pos, Y-Pos, Z-Pos, Player-IDX ]
};

void CFedworking::HandleMessage(const char* pMessage)
{
	std::string encMsg(pMessage);
	encMsg.erase(0, 4); // Remove FED@ prefix

	const std::string msg = Base64::Decode(encMsg);
	if (msg.empty()) { return; }

	const auto dataVector = Utils::SplitString(msg, "&");
	int msgType;
	try { msgType = std::stoi(dataVector[0]); }
	catch (...) { ConsoleLog("Failed to read message type!"); return; }

	switch (msgType) {
	case None:
		{
			// Undefined message type (Sent by party crasher)
			break;
		}

	case Marker:
		{
			if (dataVector.size() == 5) {
				try {
					const float xPos = std::stof(dataVector[1]);
					const float yPos = std::stof(dataVector[2]);
					const float zPos = std::stof(dataVector[3]);
					const int playerIndex = std::stoi(dataVector[4]);

					PlayerInfo_t playerInfo{};
					I::Engine->GetPlayerInfo(playerIndex, &playerInfo);

					if (playerInfo.userID != 0) {
						CGameEvent* markerEvent = I::GameEvent->CreateNewEvent("show_annotation");
						if (markerEvent) {
							markerEvent->SetInt("id", playerIndex);
							markerEvent->SetFloat("worldPosX", xPos);
							markerEvent->SetFloat("worldPosY", yPos);
							markerEvent->SetFloat("worldPosZ", zPos);
							markerEvent->SetFloat("lifetime", 5.0f);

							markerEvent->SetBool("show_distance", true);
							markerEvent->SetString("text", playerInfo.name);
							markerEvent->SetString("play_sound", "coach/coach_go_here.wav");

							I::GameEvent->FireEventClientSide(markerEvent);
						}
					}
				}
				catch (...) { ConsoleLog("Failed to read marker data!"); }
			}
			break;
		}

	case ESP:
		{
			if (dataVector.size() == 5) {
				try {
					const float xPos = std::stof(dataVector[1]);
					const float yPos = std::stof(dataVector[2]);
					const float zPos = std::stof(dataVector[3]);
					const int playerIndex = std::stoi(dataVector[4]);

					g_GlobalInfo.partyPlayerESP[playerIndex].Location = { xPos, yPos, zPos };
					g_GlobalInfo.partyPlayerESP[playerIndex].LastUpdate = I::Engine->Time();
				} catch (...) { ConsoleLog("Failed to read ESP data!"); }
			}
			break;
		}

	default:
		{
			// Unknown message type
			ConsoleLog("Unknown message received!");
			break;
		}
	}
}

void CFedworking::SendMarker(const Vec3& pPos, int pPlayerIdx)
{
	std::stringstream msg;
	msg << Marker << "&" << pPos.x << "&" << pPos.y << "&" << pPos.z << "&" << pPlayerIdx;
	SendMessage(msg.str());
}

void CFedworking::SendESP(CBaseEntity* pPlayer)
{
	if (!pPlayer->GetDormant() && pPlayer->IsInValidTeam() && pPlayer->IsAlive()) {
		const float lastUpdate = g_GlobalInfo.partyPlayerESP[pPlayer->GetIndex()].LastUpdate;
		if (lastUpdate == 0.f || I::Engine->Time() - lastUpdate >= 0.4f) {
			const Vec3 playerPos = pPlayer->GetVecOrigin();
			std::stringstream msg;
			msg << ESP << "&" << playerPos.x << "&" << playerPos.y << "&" << playerPos.z << "&" << pPlayer->GetIndex();
			SendMessage(msg.str());
		}
	}
}

void CFedworking::SendMessage(const std::string& pData)
{
	const std::string encMsg = Base64::Encode(pData);
	if (encMsg.size() <= 253) {
		std::string cmd = "tf_party_chat \"FED@";
		cmd.append(encMsg);
		cmd.append("\"");
		I::Engine->ClientCmd_Unrestricted(cmd.c_str());
	} else {
		ConsoleLog("Failed to send message! The message was too long.");
	}
}

void CFedworking::Run()
{
	if (!Vars::Misc::PartyNetworking.m_Var) { return; }

	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		// Party marker
		static KeyHelper markerKey{ &Vars::Misc::PartyMarker.m_Var };
		if (Vars::Misc::PartyMarker.m_Var && markerKey.Pressed()) {
			const Vec3 viewAngles = I::Engine->GetViewAngles();
			Vec3 vForward;
			Math::AngleVectors(viewAngles, &vForward);

			CGameTrace trace;
			CTraceFilterHitscan traceFilter = {};
			Ray_t traceRay;
			vForward = pLocal->GetEyePosition() + vForward * MAX_TRACE_LENGTH;
			traceRay.Init(pLocal->GetEyePosition(), vForward);
			I::EngineTrace->TraceRay(traceRay, MASK_SOLID, &traceFilter, &trace);
			if (trace.DidHit()) {
#ifdef _DEBUG
				I::DebugOverlay->AddLineOverlay(trace.vStartPos, trace.vEndPos, 255, 0, 0, false, 1.0f);
#endif
				g_Fedworking.SendMarker(trace.vEndPos, pLocal->GetIndex());
			}
		}

		// Party ESP
		if (Vars::Misc::PartyESP.m_Var) {
			SendESP(pLocal);
			for (const auto& player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
			{
				if (player->GetIndex() == pLocal->GetIndex()) { continue; }
				SendESP(player);
			}
		}
	}
}

void CFedworking::ConsoleLog(const std::string& pMessage)
{
	std::string consoleMsg = "[Fedworking] ";
	consoleMsg.append(pMessage);
	consoleMsg.append("\n");
	I::CVars->ConsoleColorPrintf({ 225, 177, 44, 255 }, consoleMsg.c_str());
}
