#include "UniformRandomStreamHook.h"

int __stdcall UniformRandomStreamHook::RandInt::Hook(int iMinVal, int iMaxVal)
{
	if (Vars::Misc::MedalFlip.m_Var && g_Interfaces.EngineVGui->IsGameUIVisible())
	{
		if (iMinVal == 0 && iMaxVal == 9)
			return 0;
	}

	return Table.Original<fn>(index)(g_Interfaces.UniformRandomStream, iMinVal, iMaxVal);
}
