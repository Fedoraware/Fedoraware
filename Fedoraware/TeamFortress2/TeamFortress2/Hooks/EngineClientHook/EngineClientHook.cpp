#include "EngineClientHook.h"

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
