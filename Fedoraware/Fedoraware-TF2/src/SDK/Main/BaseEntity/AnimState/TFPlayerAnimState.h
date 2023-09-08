#pragma once
#include "MultiPlayerAnimState.h"

class CTFPlayerAnimState : public CMultiPlayerAnimState
{
public:
	CBaseEntity* GetTFPlayer() { return m_pTFPlayer; }

	virtual void		ClearAnimationState() = 0;
	virtual Activity	TranslateActivity(Activity actDesired) = 0;
	virtual void		Update(float eyeYaw, float eyePitch) = 0;
	virtual void		CheckStunAnimation() = 0;
	virtual Activity	CalcMainActivity() = 0;
	virtual void		ComputePoseParam_AimYaw(CStudioHdr* pStudioHdr) = 0;
	virtual float		GetCurrentMaxGroundSpeed() = 0;
	virtual float		GetGesturePlaybackRate() = 0;
	virtual bool		ShouldUpdateAnimState() = 0;
	virtual void		GetOuterAbsVelocity(Vec3& vel) = 0;
	virtual void		RestartGesture(int iGestureSlot, Activity iGestureActivity, bool bAutoKill = true) = 0;
	void				SetRenderAngles(const Vec3& angles) { m_angRender = angles; }

	CBaseEntity* m_pTFPlayer;
	bool		 m_bInAirWalk;
	float		 m_flHoldDeployedPoseUntilTime;
	float		 m_flTauntMoveX;
	float		 m_flTauntMoveY;
	float		 m_flVehicleLeanVel;
	float		 m_flVehicleLeanPos;
	Vec3		 m_vecSmoothedUp;
};

#define MAXSTUDIOPOSEPARAM 24