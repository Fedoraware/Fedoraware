#include "GetDrawPositionHook.h"

#include "../../Features/Vars.h"

void __cdecl GetDrawPositionHook::Hook(float* pX, float* pY, bool* pbBehindCamera, Vec3 angleCrosshairOffset)
{
	if (Vars::Visuals::CrosshairAimPos.m_Var && !g_GlobalInfo.m_vAimPos.IsZero())
	{
		Vec3 vScreen;
		if (Utils::W2S(g_GlobalInfo.m_vAimPos, vScreen)) {
			if (pX) { *pX = vScreen.x; }
			if (pY) { *pY = vScreen.y; }
			if (pbBehindCamera) { *pbBehindCamera = false; }
		}
	}

	else Func.Original<fn>()(pX, pY, pbBehindCamera, angleCrosshairOffset);
}