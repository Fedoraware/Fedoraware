#include "../Hooks.h"

MAKE_HOOK(C_Material_DeleteIfUnreferenced, g_Pattern.Find(L"materialsystem.dll", L"56 8B F1 83 7E 1C 00 7F 51"), void, __fastcall,
	IMaterial* eax)
{
	if (eax) {
		const std::string materialName = eax->GetName();
		if (materialName.find("m_pmat") != std::string::npos)
		{
			I::CVars->ConsolePrintf("Preventing material %s from being deleted\n", materialName);
			return;
		}
	}

	return Hook.Original<FN>()(eax);
}
