#include "../Hooks.h"

DWORD UncacheAddress()
{
	const DWORD dwAddress = g_Pattern.Find(L"MaterialSystem.dll", L"E8 ? ? ? ? 83 7E 1C 00 ") + 0x1;
	return *reinterpret_cast<PDWORD>(dwAddress) + dwAddress + 4;
}

MAKE_HOOK(CMaterial_Uncache, UncacheAddress(), void, __fastcall,
		  IMaterial* ecx, void* edx, bool bPreserveVars)
{
	if (ecx)
	{
		const std::string materialName = ecx->GetName();
		if (materialName.find("m_pmat") != std::string::npos)
		{
			if (materialName.find("dme_mat_") == std::string::npos)
			{
				I::CVars->ConsolePrintf("Preventing material %s from being uncached\n", ecx->GetName());
				return;
			}
		}
	}

	if (ecx)
	{
		Hook.Original<FN>()(ecx, edx, bPreserveVars);
	}
}