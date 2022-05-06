#include "EngineClientHook.h"
#include <boost/algorithm/string/replace.hpp>

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

void __fastcall EngineClientHook::ClientCmd_Unrestricted::Hook(void* ecx, void* edx, const char* szCmdString)
{
	std::string cmdString(szCmdString);

	// Allow newlines in chat
	if (Vars::Misc::ChatNL.m_Var && cmdString.rfind("say", 0) == 0)
	{
		boost::replace_all(cmdString, "\\n", "\n");
		return g_Interfaces.Engine->ServerCmd(cmdString.c_str(), true);
	}

	Func.Original<fn>()(ecx, edx, cmdString.c_str());
}
