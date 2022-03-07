#include "Fedworking.h"
#include "../../Utils/Base64/Base64.hpp"

enum MessageType {
	None,
	Marker
};

void CFedworking::HandleMessage(const char* pMessage)
{
	std::string encMsg(pMessage);
	encMsg.erase(0, 4); // Remove FED@ prefix

	const std::string msg = Base64::Decode(encMsg);
	if (!msg.empty()) {
		const auto type = static_cast<MessageType>(std::stoi(msg.substr(0, 2)));

		switch (type) {
		case Marker:
			{
				const auto dataVector = Utils::Split(msg, ",");
				if (dataVector.size() == 4) {
					const float xPos = std::stof(dataVector[1]);
					const float yPos = std::stof(dataVector[2]);
					const float zPos = std::stof(dataVector[3]);
					const std::string title = dataVector[4];

					CGameEvent* markerEvent = g_Interfaces.GameEvent->CreateNewEvent("show_annotation");
					if (markerEvent) {
						markerEvent->SetInt("id", g_Interfaces.Engine->GetLocalPlayer());
						markerEvent->SetFloat("worldPosX", xPos);
						markerEvent->SetFloat("worldPosY", yPos);
						markerEvent->SetFloat("worldPosZ", zPos);
						markerEvent->SetFloat("lifetime", 10.0f);

						markerEvent->SetBool("show_distance", true);
						markerEvent->SetString("text", title.c_str());
						markerEvent->SetString("play_sound", "coach/coach_go_here.wav");

						g_Interfaces.GameEvent->FireEvent(markerEvent);
					}
				}
				break;
			}
		}
	}
}

void CFedworking::SendMarker(const Vec3& pPos, const std::string& pTitle)
{
	const std::string xPos = std::to_string(pPos.x);
	const std::string yPos = std::to_string(pPos.y);
	const std::string zPos = std::to_string(pPos.z);

	std::stringstream msg;
	msg << Marker << "," << xPos << "," << yPos << "," << zPos << "," << pTitle; // TYPE,xPos,yPos,zPos,pTitle
	SendMessage(msg.str());
}

void CFedworking::SendMessage(const std::string& pData)
{
	const std::string encMsg = Base64::Encode(pData);
	if (encMsg.size() <= 253) {
		std::string cmd = "tf_party_chat \"FED@";
		cmd.append(pData);
		cmd.append("\"");
		g_Interfaces.Engine->ClientCmd_Unrestricted(cmd.c_str());
	}
}
