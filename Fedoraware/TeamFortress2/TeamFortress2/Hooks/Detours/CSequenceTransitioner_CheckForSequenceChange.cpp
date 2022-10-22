#include "../Hooks.h"

MAKE_HOOK(CSequenceTransitioner_CheckForSequenceChange, g_Pattern.Find(L"client.dll", L"55 8B EC 53 8B 5D 08 57 8B F9 85 DB 0F 84 ? ? ? ? 83 7F 0C 00 75 05 E8 ? ? ? ? 6B 4F 0C 2C 0F 57 C0 56 8B 37 83 C6 D4 03 F1 F3 0F 10 4E ? 0F 2E C8 9F F6 C4 44 7B 62 8B 45 0C"), void, __fastcall,
		  void* ecx, void* edx, CStudioHdr* hdr, int nCurSequence, bool bForceNewSequence, bool bInterpolate)
{
	return Hook.Original<FN>()(ecx, edx, hdr, nCurSequence, bForceNewSequence, Vars::Misc::DisableInterpolation.Value ? false : bInterpolate);
}

/*
	// Feed the current state of the animation parameters to the sequence transitioner.
	// It will hand back either 1 or 2 animations in the queue to set, depending on whether
	// it's transitioning or not. We just dump those into the animation layers.
	pTransitioner->CheckForSequenceChange(
		m_pOuter->GetModelPtr(),
		iAimSequence,
		false,	// don't force transitions on the same anim
		true	// yes, interpolate when transitioning
		);
*/