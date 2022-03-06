#include "Fedworking.h"
#include "../../Utils/Base64/Base64.hpp"

enum MessageType {
	NONE,
	MARKER
};

Vec3 CFedworking::ReadPos(std::string pData, int pStart)
{
	int xPos = std::stoi(pData.substr(pStart, pStart + 4));
	int yPos = std::stoi(pData.substr(pStart + 4, pStart + 8));
	int zPos = std::stoi(pData.substr(pStart + 8, pStart + 12));

	return { static_cast<float>(xPos), static_cast<float>(yPos), static_cast<float>(zPos) };
}

void CFedworking::HandleMessage(const char* pMessage)
{
	std::string encMsg(pMessage);
	encMsg.erase(0, 4); // Remove FED@ prefix

	const std::string msg = Base64::Decode(encMsg.c_str(), encMsg.length());
	if (!msg.empty()) {
		const auto type = static_cast<MessageType>(std::stoi(msg.substr(0, 2)));

		switch (type) {
		case MARKER:
			{
				const Vec3 pos = ReadPos(msg, 2);

				CGameEvent* markerEvent = g_Interfaces.GameEvent->CreateNewEvent("show_annotation");
				if (markerEvent) {
					markerEvent->SetInt("id", g_Interfaces.Engine->GetLocalPlayer());
					markerEvent->SetFloat("worldPosX", pos.x);
					markerEvent->SetFloat("worldPosY", pos.y);
					markerEvent->SetFloat("worldPosZ", pos.z);
					markerEvent->SetFloat("lifetime", 10.0f);

					markerEvent->SetBool("show_distance", true);
					markerEvent->SetString("text", "FED: Walk here");
					markerEvent->SetString("play_sound", "coach/coach_go_here.wav");

					g_Interfaces.GameEvent->FireEvent(markerEvent);
				}

				break;
			}
		}
	}
}
