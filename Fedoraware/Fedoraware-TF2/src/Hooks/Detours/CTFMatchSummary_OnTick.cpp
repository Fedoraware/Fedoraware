#include "../Hooks.h"

// Credits: mfed
MAKE_HOOK(CTFMatchSummary_OnTick, S::CTFMatchSummary_OnTick(), int, __fastcall, void* ecx, void* edx)
{
    if (!reinterpret_cast<DWORD*>(ecx) + 750) { return Hook.Original<FN>()(ecx, edx); } //  m_iCurrentState == MS_STATE_INITIAL
	DWORD* flags = reinterpret_cast<DWORD*>(I::TFGCClientSystem + 1488);
    if (flags && *flags & 2)
    {
        *flags &= ~2;
    }

    return Hook.Original<FN>()(ecx, edx);
}
