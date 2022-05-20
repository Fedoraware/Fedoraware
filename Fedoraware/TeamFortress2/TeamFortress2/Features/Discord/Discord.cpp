#include "Discord.h"
#include <chrono>
#include "../../SDK/Interfaces/Interfaces.h"
#include "../../SDK/Main/EntityCache/EntityCache.h"
#include "../Vars.h"

inline const char* lol(int nClassNum)
{
	//literally just took this from ESP cuz too lazy lol
	static const char* szClasses[] = {"Unknown Class", "Scout", "Sniper", "Soldier", "Demoman", "Medic", "Heavy", "Pyro", "Spy", "Engineer"};
	return (nClassNum < 10 && nClassNum > 0) ? szClasses[nClassNum] : szClasses[0];
}

void CDiscordRPC::Update()
{
	if (Vars::Misc::Discord::EnableRPC.m_Var)
	{
		static int64_t startTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		DiscordRichPresence discordPresence = {};

		if (I::Engine->IsInGame())
		{
			if (Vars::Misc::Discord::IncludeMap.m_Var)
			{
				char mapName[256];
				const char* cLevelName = I::Engine->GetLevelName();

				if (cLevelName != nullptr)
				{
					std::string sLevelName;
					sLevelName.append(cLevelName);


					if (sLevelName.substr(sLevelName.find_last_of(".") + 1) == "bsp")
					{
						std::string sLevelNameFinal = sLevelName.substr(sLevelName.find_last_of("/") + 1);
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

			if (const auto pLocal = g_EntityCache.m_pLocal)
			{
				if (Vars::Misc::Discord::IncludeClass.m_Var)
				{
					if (pLocal->IsAlive())
					{
						char ClassNum[256];
						sprintf_s(ClassNum, "%s", lol(pLocal->GetClassNum()));
						discordPresence.state = ClassNum;
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

		if (Vars::Misc::Discord::IncludeTimestamp.m_Var)
		{
			discordPresence.startTimestamp = startTime;
		}
		else
		{
			discordPresence.startTimestamp = NULL;
		}

		discordPresence.endTimestamp = NULL;

		switch (Vars::Misc::Discord::WhatImagesShouldBeUsed.m_Var)
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
	}
	else
	{
		Discord_ClearPresence();
	}
}
