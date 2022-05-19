#include "../Hooks.h"

#include "../../Features/Chams/Chams.h"
#include "../../Features/Chams/DMEChams.h"
#include "../../Features/Glow/Glow.h"

MAKE_HOOK(ModelRender_ForcedMaterialOverride, Utils::GetVFuncPtr(g_Interfaces.ModelRender, 1), void, __fastcall,
		  void* ecx, void* edx, IMaterial* mat, EOverrideType type)
{
	if (!g_DMEChams.m_bRendering)
	{
		if (g_Glow.m_bRendering && !g_Glow.IsGlowMaterial(mat) || g_Chams.m_bRendering && !g_Chams.IsChamsMaterial(mat))
		{
			return;
		}
	}

	Hook.Original<FN>()(ecx, edx, mat, type);
}