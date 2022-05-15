#include "MaterialSystemHook.h"

void __fastcall MaterialSystemHook::CMaterial::Uncache::Func(void* ecx, void* edx, bool bPreserveVars)
{
	const auto mat = static_cast<IMaterial*>(ecx);

	if (ecx && mat)
	{
		const std::string materialName = mat->GetName();
		if (materialName.find("m_pmat") != std::string::npos)
		{
			if (materialName.find("dme_mat_") == std::string::npos)
			{
				g_Interfaces.CVars->ConsolePrintf("Preventing material %s from being uncached\n", mat->GetName());
				return;
			}
		}
	}

	if (ecx && !(bPreserveVars < 0 && bPreserveVars > 1))
	{
		Hook.Original<fn>()(ecx, bPreserveVars);
	}
}

//Address of signature = materialsystem.dll + 0x00038650
//CMaterial__Uncache+A6	push    offset aMaterialsSVmt; "materials/%s.vmt"

void MaterialSystemHook::CMaterial::Uncache::Init()
{
	const DWORD dwFN = g_Pattern.Find(L"MaterialSystem.dll", L"E8 ? ? ? ? 83 7E 1C 00 ") + 0x1;
	const DWORD dwResolved = *reinterpret_cast<PDWORD>(dwFN) + dwFN + 4;
	const auto FN = reinterpret_cast<fn>(dwResolved);
	Hook.Hook(FN, Func);
}
