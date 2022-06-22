#include "../Hooks.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp> // Include for boost::split
#include <boost/algorithm/string/join.hpp>
#include <numeric>

class split_q
{
public:
	split_q() : in_q(false) {}

	bool operator()(char ch) const
	{
		if (ch == '\"')
		{
			in_q = !in_q;
		}
		return !in_q && ch == ' ';
	}

private:
	mutable bool in_q;

};

MAKE_HOOK(EngineClient_ClientCmd_Unrestricted, Utils::GetVFuncPtr(I::Engine, 106), void, __fastcall,
		  void* ecx, void* edx, const char* szCmdString)
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
				I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Usage: setcvar <cvar> <value>\n");
				return;
			}

			// Find the given CVar
			const auto foundCVar = I::CVars->FindVar(cmdArgs[0].c_str());
			const std::string cvarName = cmdArgs[0];
			if (!foundCVar)
			{
				I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Could not find %s\n", cvarName.c_str());
				return;
			}

			// Set the CVar to the given value
			cmdArgs.pop_front();
			std::string newValue = boost::algorithm::join(cmdArgs, " ");
			boost::replace_all(newValue, "\"", "");
			foundCVar->SetValue(newValue.c_str());
			I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, "Set %s to %s\n", cvarName.c_str(), newValue.c_str());
			return;
		}
	}

	Hook.Original<FN>()(ecx, edx, cmdString.c_str());
}