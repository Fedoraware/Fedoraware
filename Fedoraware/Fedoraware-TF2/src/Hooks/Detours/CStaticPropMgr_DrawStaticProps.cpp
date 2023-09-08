#include "../Hooks.h"

MAKE_HOOK(CStaticPropMgr_DrawStaticProps, S::CStaticPropMgr_DrawStaticProps(), void, __fastcall,
		  void* ecx, void* edx, IClientRenderable** pProps, int count, bool bShadowDepth, bool drawVCollideWireframe)
{
	G::DrawingStaticProps = true;
	Hook.Original<FN>()(ecx, edx, pProps, count, bShadowDepth, Vars::Visuals::PropWireframe.Value);
	G::DrawingStaticProps = false;
}