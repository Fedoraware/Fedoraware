#pragma once
#include "Activity.h"

#define ANIM_LAYER_ACTIVE		0x0001
#define ANIM_LAYER_AUTOKILL		0x0002
#define ANIM_LAYER_KILLME		0x0004
#define ANIM_LAYER_DONTRESTORE	0x0008
#define ANIM_LAYER_CHECKACCESS	0x0010
#define ANIM_LAYER_DYING		0x0020

class C_BaseAnimatingOverlay;

class CAnimationLayer
{
public:
	int		m_fFlags;
	bool	m_bSequenceFinished;
	bool	m_bLooping;
	int		m_nSequence;
	float	m_flCycle;
	float	m_flPrevCycle;
	float	m_flWeight;
	float	m_flPlaybackRate;
	float	m_flBlendIn;
	float	m_flBlendOut;
	float	m_flKillRate;
	float	m_flKillDelay;
	float	m_flLayerAnimtime;
	float	m_flLayerFadeOuttime;

	Activity m_nActivity;

	int	m_nPriority;
	int m_nOrder;

	bool IsActive() { return ((m_fFlags & ANIM_LAYER_ACTIVE) != 0); }
	bool IsAutokill() { return ((m_fFlags & ANIM_LAYER_AUTOKILL) != 0); }
	bool IsKillMe() { return ((m_fFlags & ANIM_LAYER_KILLME) != 0); }
	bool IsAutoramp() { return (m_flBlendIn != 0.0 || m_flBlendOut != 0.0); }
	void KillMe() { m_fFlags |= ANIM_LAYER_KILLME; }
	void Dying() { m_fFlags |= ANIM_LAYER_DYING; }
	bool IsDying() { return ((m_fFlags & ANIM_LAYER_DYING) != 0); }
	void Dead() { m_fFlags &= ~ANIM_LAYER_DYING; }

	float m_flLastEventCheck;
	float m_flLastAccess;

	// Network state changes get forwarded here.
	C_BaseAnimatingOverlay* m_pOwnerEntity;
};