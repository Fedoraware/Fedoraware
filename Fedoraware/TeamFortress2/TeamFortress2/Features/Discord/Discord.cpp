#include "Discord.h"
#include <chrono>
#include "../../SDK/Interfaces/Interfaces.h"
#include "../../SDK/Main/EntityCache/EntityCache.h"
#include "../Vars.h"

#include "../../SDK/Discord/include/discord_rpc.h"

const char* APPLICATION_ID = "889495873183154226";

void CDiscordRPC::Init()
{
	DiscordEventHandlers handlers = {};
	Discord_Initialize(APPLICATION_ID, &handlers, 0, "440");
	Discord_ClearPresence();
}

void CDiscordRPC::Shutdown()
{
	Discord_ClearPresence();
	Discord_Shutdown();
}

void CDiscordRPC::Update()
{
	if (!Vars::Misc::Discord::EnableRPC.Value)
	{
		if (m_IsActive)
		{
			Discord_ClearPresence();
			m_IsActive = false;
		}

		return;
	}

	static int64_t startTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	DiscordRichPresence discordPresence = {};

	// Status
	if (I::EngineClient->IsInGame())
	{
		if (Vars::Misc::Discord::IncludeMap.Value)
		{
			char mapName[256];
			const char* cLevelName = I::EngineClient->GetLevelName();

			if (cLevelName != nullptr)
			{
				std::string sLevelName;
				sLevelName.append(cLevelName);


				if (sLevelName.substr(sLevelName.find_last_of('.') + 1) == "bsp")
				{
					std::string sLevelNameFinal = sLevelName.substr(sLevelName.find_last_of('/') + 1);
					sLevelNameFinal.erase(sLevelNameFinal.length() - 4);

					sprintf_s(mapName, "%s", sLevelNameFinal.c_str());

					discordPresence.details = mapName;
				}
			}
		}
		else
		{
			discordPresence.details = nullptr;
		}

		if (const auto pLocal = g_EntityCache.GetLocal())
		{
			if (Vars::Misc::Discord::IncludeClass.Value)
			{
				if (pLocal->IsAlive())
				{
					char classBuffer[256];
					sprintf_s(classBuffer, "%s", Utils::GetClassByIndex(pLocal->GetClassNum()));
					discordPresence.state = classBuffer;
				}
			}
			else
			{
				discordPresence.state = nullptr;
			}
		}
	}
	else
	{
		discordPresence.state = "Main Menu";
	}

	// Timestampt
	if (Vars::Misc::Discord::IncludeTimestamp.Value)
	{
		discordPresence.startTimestamp = startTime;
	}
	else
	{
		discordPresence.startTimestamp = NULL;
	}

	discordPresence.endTimestamp = NULL;

	// Image & Icon
	switch (Vars::Misc::Discord::WhatImagesShouldBeUsed.Value)
	{
	case 0:
		discordPresence.smallImageKey = "tf2";
		discordPresence.largeImageKey = "fedora";
		discordPresence.smallImageText = "Team Fortress 2";
		discordPresence.largeImageText = "Fedoraware";
		break;
	case 1:
		discordPresence.smallImageKey = "fedora";
		discordPresence.largeImageKey = "tf2";
		discordPresence.smallImageText = "Fedoraware";
		discordPresence.largeImageText = "Team Fortress 2";
		break;
	}

	discordPresence.instance = 1;

	Discord_UpdatePresence(&discordPresence);
	m_IsActive = true;
}
