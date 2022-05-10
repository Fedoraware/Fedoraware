#include "EngineClientHook.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp> // Include for boost::split
bool __stdcall EngineClientHook::IsPlayingTimeDemo::Hook()
{
	static DWORD dwInterpolateServerEntities = g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 30 8B 0D ? ? ? ? 53 33 DB 89 5D DC 89 5D E0"));

	if (Vars::Misc::DisableInterpolation.m_Var)
	{
		if (reinterpret_cast<DWORD>(_ReturnAddress()) == (dwInterpolateServerEntities + 0xB8))
			return true;
	}

	return Table.Original<fn>(index)(g_Interfaces.Engine);
}

class split_q
{
public:
	split_q() : in_q(false) {}
	bool operator() (char ch) const
	{
		if (ch == '\"') in_q = !in_q;
		return !in_q && ch == ' ';
	}

private:
	mutable bool in_q;

};

#include <numeric>

void __fastcall EngineClientHook::ClientCmd_Unrestricted::Hook(void* ecx, void* edx, const char* szCmdString)
{

	std::vector<std::string> vecArguments;
	std::string cmdString(szCmdString);

	// Yes I will use boost for this
	boost::split(vecArguments, cmdString, split_q());
	// Splitting it into a vector like this also allows for expansion on this idea

	// This is really bad code. @LNX if u wanna fix this :3
	if (vecArguments.at(0).find("setcvar") != std::string::npos)
	{
		vecArguments.erase(vecArguments.begin());
		if (vecArguments.size() < 1)
		{
			g_Interfaces.CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Usage: setcvar <cvar> <value>\n");
			return;
		}
		auto cvar = g_Interfaces.CVars->FindVar(vecArguments[0].c_str());
		std::string cvarname = vecArguments.at(0);
		if (!cvar)
		{
			g_Interfaces.CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Could not find %s\n", vecArguments[0].c_str());
			return;
		}
		else
		{
			vecArguments.erase(vecArguments.begin());
			if (vecArguments.size() < 1)
			{
				g_Interfaces.CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Usage: setcvar <cvar> <value>\n");
				return;
			}
			std::string command;
			for (auto& str : vecArguments)
			{
				command += str + " ";
			}
			command.pop_back();
			boost::replace_all(command, "\"", "");
			cvar->SetValue(command.c_str());
			g_Interfaces.CVars->ConsoleColorPrintf({ 255,255,255,255 }, "Set %s to %s\n", cvarname.c_str(), command.c_str());
		}
		return;
	}

	// Allow newlines in chat chad
	if (Vars::Misc::ChatNL.m_Var && cmdString.rfind("say", 0) == 0)
	{
		boost::replace_all(cmdString, "\\n", "\n");
		return g_Interfaces.Engine->ServerCmd(cmdString.c_str(), true);
	}

	Func.Original<fn>()(ecx, edx, cmdString.c_str());
}
