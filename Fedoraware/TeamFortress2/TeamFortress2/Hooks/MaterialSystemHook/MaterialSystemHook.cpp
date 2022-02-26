#include "MaterialSystemHook.h"

void __fastcall MaterialSystemHook::CMaterial::Uncache::Func(void* ecx, void* edx, bool bPreserveVars)
{
	auto mat = reinterpret_cast<IMaterial*>(ecx);

	if (ecx && mat) {
		std::string materialName = mat->GetName();
		if (materialName.find("m_pmat") != std::string::npos) {
			if (materialName.find("dme_mat_") == std::string::npos) {
				g_Interfaces.CVars->ConsolePrintf("Preventing material %s from being uncached\n", mat->GetName());
				return;
			}
		}
	}

	if (ecx && !(bPreserveVars < 0 && bPreserveVars > 1)) {
		Hook.Original<fn>()(ecx, bPreserveVars);
	}
}

//Address of signature = materialsystem.dll + 0x00038650
//CMaterial__Uncache+A6	push    offset aMaterialsSVmt; "materials/%s.vmt"

void MaterialSystemHook::CMaterial::Uncache::Init()
{
	DWORD dwFN = g_Pattern.Find(L"MaterialSystem.dll", L"E8 ? ? \? ? 83 7E 1C 00 ") + 0x1;
	DWORD dwResolved = *(PDWORD)dwFN + dwFN + 4;
	fn FN = reinterpret_cast<fn>(dwResolved);
	Hook.Hook(FN, Func);
}
