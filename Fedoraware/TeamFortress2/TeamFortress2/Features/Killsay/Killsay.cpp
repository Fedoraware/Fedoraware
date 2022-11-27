#include "Killsay.h"

#include <fstream>
#include <filesystem>
#include <boost/algorithm/string/replace.hpp>

const char* ClassToString(const int nClass)
{
	switch (nClass)
	{
		default:
		{
			return "Unknown";
		}
		case CLASS_SCOUT:
		{
			return "Scout";
		}
		case CLASS_SNIPER:
		{
			return "Sniper";
		}
		case CLASS_SOLDIER:
		{
			return "Soldier";
		}
		case CLASS_DEMOMAN:
		{
			return "Demoman";
		}
		case CLASS_MEDIC:
		{
			return "Medic";
		}
		case CLASS_HEAVY:
		{
			return "Heavy";
		}
		case CLASS_PYRO:
		{
			return "Pyro";
		}
		case CLASS_SPY:
		{
			return "Spy";
		}
		case CLASS_ENGINEER:
		{
			return "Engineer";
		}
	}
}

void CKillsay::FireGameEvent(FNV1A_t uNameHash, CGameEvent* pEvent)
{
        if (!Vars::Misc::Killsay.Value) 
        {
                return;
        }

	if (Vars::Misc::KillsayFile.empty())
	{
		return;
	}

	if (!m_bFilled)
	{
		m_vecLoadedKillsays.clear();

		const std::string sCurrentPath = std::filesystem::current_path().string() + "\\FedFigs";

		if (!std::filesystem::exists(sCurrentPath))
		{
			std::filesystem::create_directory(sCurrentPath);
		}

		if (!std::filesystem::exists(sCurrentPath + "\\Killsays"))
		{
			std::filesystem::create_directory(sCurrentPath + "\\Killsays");
		}

		std::string sKillsayPath = (sCurrentPath + "\\Killsays\\" + Vars::Misc::KillsayFile);

		std::ifstream killsayFile(sKillsayPath);

		std::string currentKillsay;

		while (std::getline(killsayFile, currentKillsay))
		{
			if (currentKillsay.size() > 0)
			{
				m_vecLoadedKillsays.push_back(currentKillsay);
			}
		}

		if (m_vecLoadedKillsays.size())
		{
			m_bFilled = true;
		}
		else
		{
			Vars::Misc::KillsayFile.clear();
			return;
		}
	}

	if (m_vecLoadedKillsays.empty())
	{
		return;
	}

	if (uNameHash == FNV1A::HashConst("player_death"))
	{
		const int attacker = Utils::GetPlayerForUserID(pEvent->GetInt("attacker"));
		const int userid = Utils::GetPlayerForUserID(pEvent->GetInt("userid"));

		if (userid == I::EngineClient->GetLocalPlayer())
		{
			return;
		}

		if (attacker != I::EngineClient->GetLocalPlayer())
		{
			return;
		}

		// Lol lets hope u dont have medal flip on and 9 lines of killsay

		const auto& pEntity = I::ClientEntityList->GetClientEntity(userid);

		if (!pEntity)
		{
			return;
		}

		if (!pEntity->IsPlayer())
		{
			return;
		}

		PlayerInfo_t pi = {};

		if (!I::EngineClient->GetPlayerInfo(userid, &pi))
		{
			return;
		}

		int nSelectedKillsay = I::UniformRandomStream->RandomInt(0, m_vecLoadedKillsays.size() - 1);
		std::string selectedKillsay = m_vecLoadedKillsays.at(nSelectedKillsay);

		const char* szName = pi.name;
		int nClass = pEntity->GetClassNum();

		boost::replace_all(selectedKillsay, "%victim%", szName);
		boost::replace_all(selectedKillsay, "%class%", ClassToString(nClass));

		const std::string cmd = "say \"" + selectedKillsay + "\"";
		I::EngineClient->ServerCmd(cmd.c_str(), true);
	}
}
