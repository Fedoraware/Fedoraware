#include "../Hooks.h"

MAKE_HOOK(EngineClient_IsPlayingTimeDemo, Utils::GetVFuncPtr(I::Engine, 78), bool, __fastcall,
		  void* ecx, void* edx)
{
	static DWORD dwInterpolateServerEntities = g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC 83 EC 30 8B 0D ? ? ? ? 53 33 DB 89 5D DC 89 5D E0"));

	if (Vars::Misc::DisableInterpolation.m_Var)
	{
		if (reinterpret_cast<DWORD>(_ReturnAddress()) == (dwInterpolateServerEntities + 0xB8))
		{
			return true;
		}
	}

	return Hook.Original<FN>()(ecx, edx);
}