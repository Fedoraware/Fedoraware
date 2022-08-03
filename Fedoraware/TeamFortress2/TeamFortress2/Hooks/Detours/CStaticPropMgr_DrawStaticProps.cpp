#include "../Hooks.h"

MAKE_HOOK(CStaticPropMgr_DrawStaticProps, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC 10 8B 15 ? ? ? ? 53 56 57 33 F6 89 4D FC 33 FF 89 75 F0 89 7D F4 8B 42 08"), void, __fastcall,
		  void* ecx, void* edx, IClientRenderable** pProps, int count, bool bShadowDepth, bool drawVCollideWireframe)
{
	G::DrawingStaticProps = true;
	Hook.Original<FN>()(ecx, edx, pProps, count, bShadowDepth, Vars::Visuals::PropWireframe.Value);
	G::DrawingStaticProps = false;
}