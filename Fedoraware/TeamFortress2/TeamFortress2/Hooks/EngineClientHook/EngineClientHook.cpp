#include "EngineClientHook.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp> // Include for boost::split
#include <boost/algorithm/string/join.hpp>

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
	std::string cmdString(szCmdString);
	std::deque<std::string> cmdArgs;

	// Yes I will use boost for this
	boost::split(cmdArgs, cmdString, split_q());

	if (!cmdArgs.empty())
	{
		const std::string cmdName = cmdArgs.front();
		cmdArgs.pop_front();

		if (cmdName == "setcvar")
		{
			// Check if the user provided at least 2 args
			if (cmdArgs.size() < 2)
			{
				g_Interfaces.CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Usage: setcvar <cvar> <value>\n");
				return;
			}

			// Find the given CVar
			const auto foundCVar = g_Interfaces.CVars->FindVar(cmdArgs[0].c_str());
			const std::string cvarName = cmdArgs[0];
			if (!foundCVar)
			{
				g_Interfaces.CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Could not find %s\n", cvarName.c_str());
				return;
			}

			std::string newValue = boost::algorithm::join(cmdArgs, " ");
			boost::replace_all(newValue, "\"", "");
			foundCVar->SetValue(newValue.c_str());
			g_Interfaces.CVars->ConsoleColorPrintf({ 255,255,255,255 }, "Set %s to %s\n", cvarName.c_str(), newValue.c_str());
			return;
		}
	}

	// Allow newlines in chat chad
	if (Vars::Misc::ChatNL.m_Var && cmdString.rfind("say", 0) == 0)
	{
		boost::replace_all(cmdString, "\\n", "\n");
		g_Interfaces.Engine->ServerCmd(cmdString.c_str(), true);
		return;
	}

	Func.Original<fn>()(ecx, edx, cmdString.c_str());
}
