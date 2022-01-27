#pragma once
#include "BaseAnimatingOverlay.h"

#include "../../../../Utils/Math/Math.h"
#include "../../../../Utils/UtlVector/UtlVector.h"

class CBaseEntity;
class CBaseCombatWeapon;
class CStudioHdr;

enum PlayerAnimEvent_t
{
	PLAYERANIMEVENT_ATTACK_PRIMARY,
	PLAYERANIMEVENT_ATTACK_SECONDARY,
	PLAYERANIMEVENT_ATTACK_GRENADE,
	PLAYERANIMEVENT_RELOAD,
	PLAYERANIMEVENT_RELOAD_LOOP,
	PLAYERANIMEVENT_RELOAD_END,
	PLAYERANIMEVENT_JUMP,
	PLAYERANIMEVENT_SWIM,
	PLAYERANIMEVENT_DIE,
	PLAYERANIMEVENT_FLINCH_CHEST,
	PLAYERANIMEVENT_FLINCH_HEAD,
	PLAYERANIMEVENT_FLINCH_LEFTARM,
	PLAYERANIMEVENT_FLINCH_RIGHTARM,
	PLAYERANIMEVENT_FLINCH_LEFTLEG,
	PLAYERANIMEVENT_FLINCH_RIGHTLEG,
	PLAYERANIMEVENT_DOUBLEJUMP,

	// Cancel.
	PLAYERANIMEVENT_CANCEL,
	PLAYERANIMEVENT_SPAWN,

	// Snap to current yaw exactly
	PLAYERANIMEVENT_SNAP_YAW,

	PLAYERANIMEVENT_CUSTOM,				// Used to play specific activities
	PLAYERANIMEVENT_CUSTOM_GESTURE,
	PLAYERANIMEVENT_CUSTOM_SEQUENCE,	// Used to play specific sequences
	PLAYERANIMEVENT_CUSTOM_GESTURE_SEQUENCE,

	// TF Specific. Here until there's a derived game solution to this.
	PLAYERANIMEVENT_ATTACK_PRE,
	PLAYERANIMEVENT_ATTACK_POST,
	PLAYERANIMEVENT_GRENADE1_DRAW,
	PLAYERANIMEVENT_GRENADE2_DRAW,
	PLAYERANIMEVENT_GRENADE1_THROW,
	PLAYERANIMEVENT_GRENADE2_THROW,
	PLAYERANIMEVENT_VOICE_COMMAND_GESTURE,
	PLAYERANIMEVENT_DOUBLEJUMP_CROUCH,
	PLAYERANIMEVENT_STUN_BEGIN,
	PLAYERANIMEVENT_STUN_MIDDLE,
	PLAYERANIMEVENT_STUN_END,
	PLAYERANIMEVENT_PASSTIME_THROW_BEGIN,
	PLAYERANIMEVENT_PASSTIME_THROW_MIDDLE,
	PLAYERANIMEVENT_PASSTIME_THROW_END,
	PLAYERANIMEVENT_PASSTIME_THROW_CANCEL,

	PLAYERANIMEVENT_ATTACK_PRIMARY_SUPER,

	PLAYERANIMEVENT_COUNT
};

enum
{
	GESTURE_SLOT_ATTACK_AND_RELOAD,
	GESTURE_SLOT_GRENADE,
	GESTURE_SLOT_JUMP,
	GESTURE_SLOT_SWIM,
	GESTURE_SLOT_FLINCH,
	GESTURE_SLOT_VCD,
	GESTURE_SLOT_CUSTOM,

	GESTURE_SLOT_COUNT,
};

#define GESTURE_SLOT_INVALID	-1

struct GestureSlot_t
{
	int				 m_iGestureSlot;
	Activity		 m_iActivity;
	bool			 m_bAutoKill;
	bool			 m_bActive;
	CAnimationLayer *m_pAnimLayer;
};

struct MultiPlayerPoseData_t
{
	int			m_iMoveX;
	int			m_iMoveY;
	int			m_iAimYaw;
	int			m_iAimPitch;
	int			m_iBodyHeight;
	int			m_iMoveYaw;
	int			m_iMoveScale;

	float		m_flEstimateYaw;
	float		m_flLastAimTurnTime;

	void Init()
	{
		m_iMoveX = 0;
		m_iMoveY = 0;
		m_iAimYaw = 0;
		m_iAimPitch = 0;
		m_iBodyHeight = 0;
		m_iMoveYaw = 0;
		m_iMoveScale = 0;
		m_flEstimateYaw = 0.0f;
		m_flLastAimTurnTime = 0.0f;
	}
};

struct DebugPlayerAnimData_t
{
	float	m_flVelocity;
	float	m_flAimPitch;
	float	m_flAimYaw;
	float	m_flBodyHeight;
	Vec2	m_vecMoveYaw;

	void Init()
	{
		m_flVelocity		= 0.0f;
		m_flAimPitch	= 0.0f;
		m_flAimYaw		= 0.0f;
		m_flBodyHeight	= 0.0f;
		m_vecMoveYaw.Set();
	}
};

struct MultiPlayerMovementData_t
{
	// Set speeds to -1 if they are not used.
	float m_flWalkSpeed;
	float m_flRunSpeed;
	float m_flSprintSpeed;
	float m_flBodyYawRate;
};

typedef enum
{
	LEGANIM_9WAY,	// Legs use a 9-way blend, with "move_x" and "move_y" pose parameters.
	LEGANIM_8WAY,	// Legs use an 8-way blend with "move_yaw" pose param.
	LEGANIM_GOLDSRC	// Legs always point in the direction he's running and the torso rotates.
} LegAnimType_t;

#pragma warning (disable : 26812) //unscoped enum for Activity

class CMultiPlayerAnimState
{
public:
	virtual				~CMultiPlayerAnimState() = 0;
	virtual void		ClearAnimationState() = 0;
	virtual void		DoAnimationEvent(PlayerAnimEvent_t event, int nData = 0) = 0;
	virtual Activity	CalcMainActivity() = 0;
	virtual void		Update(float eyeYaw, float eyePitch) = 0;
	virtual void		Release() = 0;
	virtual Activity	TranslateActivity(Activity actDesired) = 0;
	virtual void		SetRunSpeed(float flSpeed) { m_MovementData.m_flRunSpeed = flSpeed; }
	virtual void		SetWalkSpeed(float flSpeed) { m_MovementData.m_flWalkSpeed = flSpeed; }
	virtual void		SetSprintSpeed(float flSpeed) { m_MovementData.m_flSprintSpeed = flSpeed; }
	virtual void		ShowDebugInfo() = 0;
	virtual void		DebugShowAnimState(int iStartLine) = 0;

	Activity GetCurrentMainActivity() { return m_eCurrentMainSequenceActivity; }

	bool m_bForceAimYaw;

	virtual void Init(CBaseEntity *pPlayer, MultiPlayerMovementData_t &movementData) = 0;
	CBaseEntity *GetBasePlayer() { return m_pPlayer; }

	virtual int SelectWeightedSequence(Activity activity) = 0;
	virtual void RestartMainSequence() = 0;
	virtual void GetOuterAbsVelocity(Vec3 &vel) = 0;
	virtual bool HandleJumping(Activity &idealActivity) = 0;
	virtual bool HandleDucking(Activity &idealActivity) = 0;
	virtual bool HandleMoving(Activity &idealActivity) = 0;
	virtual bool HandleSwimming(Activity &idealActivity) = 0;
	virtual bool HandleDying(Activity &idealActivity) = 0;

	CUtlVector<GestureSlot_t> m_aGestureSlots;

	virtual void	RestartGesture(int iGestureSlot, Activity iGestureActivity, bool bAutoKill = true) = 0;
	virtual float	GetGesturePlaybackRate() = 0;
	virtual void	PlayFlinchGesture(Activity iActivity) = 0;
	virtual float	CalcMovementSpeed(bool *bIsMoving) = 0;
	virtual float	CalcMovementPlaybackRate(bool *bIsMoving) = 0;
	virtual void	ComputePoseParam_MoveYaw(CStudioHdr *pStudioHdr) = 0;
	virtual void	ComputePoseParam_AimPitch(CStudioHdr *pStudioHdr) = 0;
	virtual void	ComputePoseParam_AimYaw(CStudioHdr *pStudioHdr) = 0;
	virtual void	EstimateYaw() = 0;
	virtual float	GetCurrentMaxGroundSpeed() = 0;
	virtual void	ComputeSequences(CStudioHdr *pStudioHdr) = 0;
	virtual bool	ShouldUpdateAnimState() = 0;

	CBaseEntity *m_pPlayer;
	Vec3 m_angRender;

	bool					m_bPoseParameterInit;
	MultiPlayerPoseData_t	m_PoseParameterData;
	DebugPlayerAnimData_t	m_DebugAnimData;

	bool m_bCurrentFeetYawInitialized;
	float m_flLastAnimationStateClearTime;

	float m_flEyeYaw;
	float m_flEyePitch;
	float m_flGoalFeetYaw;
	float m_flCurrentFeetYaw;
	float m_flLastAimTurnTime;

	MultiPlayerMovementData_t m_MovementData;

	bool	m_bJumping;
	float	m_flJumpStartTime;
	bool	m_bFirstJumpFrame;
	bool	m_bInSwim;
	bool	m_bFirstSwimFrame;
	bool	m_bDying;
	bool	m_bFirstDyingFrame;

	Activity m_eCurrentMainSequenceActivity;
	int m_nSpecificMainSequence;

	CBaseCombatWeapon *m_hActiveWeapon;

	float m_flLastGroundSpeedUpdateTime;
	float m_iv_flMaxGroundSpeed;
	float m_flMaxGroundSpeed;

	int m_nMovementSequence;
	LegAnimType_t m_LegAnimType;
};