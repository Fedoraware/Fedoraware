#include "../Hooks.h"

MAKE_HOOK(CSequenceTransitioner_CheckForSequenceChange, S::CSequenceTransitioner_CheckForSequenceChange(), void, __fastcall,
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