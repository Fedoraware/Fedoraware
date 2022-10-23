#include "../Hooks.h"

MAKE_HOOK(CNewParticleEffect_DrawModel, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 30 53 8B D9 8B 0D ? ? ? ? 56 33 F6 89 75 DC"), int, __fastcall,
		  void* ecx, void* edx, int flags)
{
	if (!Vars::Visuals::ParticlesIgnoreZ.Value)
	{
		return Hook.Original<FN>()(ecx, edx, flags);
	}

	if (const auto& pRC = I::MaterialSystem->GetRenderContext())
	{
		pRC->DepthRange(0.0f, 0.02f);
		const int nReturn = Hook.Original<FN>()(ecx, edx, flags);
		pRC->DepthRange(0.0f, 1.0f);
		return nReturn;
	}

	return Hook.Original<FN>()(ecx, edx, flags);	//	warning
}