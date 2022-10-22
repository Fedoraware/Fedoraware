#pragma once
#include "../../Utils/Utils.h"

#include "Enums.h"
#include "Structs.h"

#include "dt_common.h"
#include "dt_recv.h"

typedef unsigned short MaterialVarSym_t;
class ITexture;
class IMaterial;

class MaterialPropertyTypes_t;
class VertexFormat_t;
using CBaseHandle = unsigned long;

class IHandleEntity
{
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle(const CBaseHandle& handle) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
};

struct Vertex_t
{
	Vec2	m_Position;
	Vec2	m_TexCoord;
	Vertex_t() {}
	Vertex_t(const Vec2& pos, const Vec2& coord = Vec2(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init(const Vec2& pos, const Vec2& coord = Vec2(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
};

class VMatrix
{
private:
	Vec3 m[4][4];

public:
	const matrix3x4& As3x4() const
	{
		return *((const matrix3x4*)this);
	}
};

class CAttribute
{
public:
	void* _pad0;
	std::uint16_t m_nAttributeDefIndex;
	float m_flValue;
	unsigned int _pad1;

	__inline CAttribute(std::uint16_t nAttributeDefIndex, float flVal)
	{
		m_nAttributeDefIndex = nAttributeDefIndex;
		m_flValue = flVal;
	}
};

class CAttributeList
{
public:
	void* _pad0;
	CUtlVector<CAttribute> m_Attributes;

	__inline void Add(int iIndex, float flValue)
	{
		if (m_Attributes.Count() > 14) return;

		CAttribute Attr(iIndex, flValue);

		m_Attributes.AddToTail(Attr);
	}
};

class CViewSetup
{
public:
	// left side of view window
	int			x;
	int			m_nUnscaledX;
	// top side of view window
	int			y;
	int			m_nUnscaledY;
	// width of view window
	int			width;
	int			m_nUnscaledWidth;
	// height of view window
	int			height;
	// which eye are we rendering?
	EStereoEye m_eStereoEye;
	int			m_nUnscaledHeight;

	// the rest are only used by 3D views

		// Orthographic projection?
	bool		m_bOrtho;
	// View-space rectangle for ortho projection.
	float		m_OrthoLeft;
	float		m_OrthoTop;
	float		m_OrthoRight;
	float		m_OrthoBottom;

	// horizontal FOV in degrees
	float		fov;
	// horizontal FOV in degrees for in-view model
	float		fovViewmodel;

	// 3D origin of camera
	Vec3		origin;

	// heading of camera (pitch, yaw, roll)
	Vec3		angles;
	// local Z coordinate of near plane of camera
	float		zNear;
	// local Z coordinate of far plane of camera
	float		zFar;

	// local Z coordinate of near plane of camera ( when rendering view model )
	float		zNearViewmodel;
	// local Z coordinate of far plane of camera ( when rendering view model )
	float		zFarViewmodel;

	// set to true if this is to draw into a subrect of the larger screen
	// this really is a hack, but no more than the rest of the way this class is used
	bool		m_bRenderToSubrectOfLargerScreen;

	// The aspect ratio to use for computing the perspective projection matrix
	// (0.0f means use the viewport)
	float		m_flAspectRatio;

	// Controls for off-center projection (needed for poster rendering)
	bool		m_bOffCenter;
	float		m_flOffCenterTop;
	float		m_flOffCenterBottom;
	float		m_flOffCenterLeft;
	float		m_flOffCenterRight;

	// Control that the SFM needs to tell the engine not to do certain post-processing steps
	bool		m_bDoBloomAndToneMapping;

	// Cached mode for certain full-scene per-frame varying state such as sun entity coverage
	bool		m_bCacheFullSceneState;

	// If using VR, the headset calibration will feed you a projection matrix per-eye.
	// This does NOT override the Z range - that will be set up as normal (i.e. the values in this matrix will be ignored).
	bool        m_bViewToProjectionOverride;
	VMatrix     m_ViewToProjection;
};

using EntityHandle_t = CBaseHandle;

class CMoveData
{
public:
	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;
	EntityHandle_t m_nPlayerHandle;
	int m_nImpulseCommand;
	Vec3 m_vecViewAngles;
	Vec3 m_vecAbsViewAngles;
	int m_nButtons;
	int m_nOldButtons;
	float m_flForwardMove;
	float m_flOldForwardMove;
	float m_flSideMove;
	float m_flUpMove;
	float m_flMaxSpeed;
	float m_flClientMaxSpeed;
	Vec3 m_vecVelocity;
	Vec3 m_vecAngles;
	Vec3 m_vecOldAngles;
	float m_outStepHeight;
	Vec3 m_outWishVel;
	Vec3 m_outJumpVel;
	Vec3 m_vecConstraintCenter;
	float m_flConstraintRadius;
	float m_flConstraintWidth;
	float m_flConstraintSpeedFactor;
	Vec3 m_vecAbsOrigin;
};

class CEntityGroundContact
{
public:
	int		entindex;
	float	minheight;
	float	maxheight;
};

class CUserCmd
{
public:
	virtual ~CUserCmd() {};

	// For matching server and client commands for debugging
	int		command_number;

	// the tick the client created this command
	int		tick_count;

	// Player instantaneous view angles.
	Vec3	viewangles;
	// Intended velocities
	//	forward velocity.
	float	forwardmove;
	//  sideways velocity.
	float	sidemove;
	//  upward velocity.
	float	upmove;
	// Attack button states
	int		buttons;
	// Impulse command issued.
	byte    impulse;
	// Current weapon id
	int		weaponselect;
	int		weaponsubtype;

	int		random_seed;	// For shared random functions

	//int		server_random_seed; // Only the server populates this seed

	short	mousedx;		// mouse accum in x from create move
	short	mousedy;		// mouse accum in y from create move

	// Client only, tracks whether we've predicted this command at least once
	bool	hasbeenpredicted;

	CRC32_t GetChecksum(void) const
	{
		CRC32_t crc;

		CRC32_Init(&crc);
		CRC32_ProcessBuffer(&crc, &command_number, sizeof(command_number));
		CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
		CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
		CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
		CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
		CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
		CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
		CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
		CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
		CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
		CRC32_ProcessBuffer(&crc, &random_seed, sizeof(random_seed));
		CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
		CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));
		CRC32_Final(&crc);

		return crc;
	}
};

class CVerifiedUserCmd
{
public:
	CUserCmd m_cmd;
	CRC32_t m_crc;
};

typedef void* (*CreateClientClassFn)(int entnum, int serialNum);
typedef void* (*CreateEventFn)();

class CClientClass
{
public:
	const char* GetName()
	{
		return m_pNetworkName;
	}
public:
	CreateClientClassFn		m_pCreateFn;
	CreateEventFn			m_pCreateEventFn;	// Only called for event objects.
	char* m_pNetworkName;
	RecvTable* m_pRecvTable;
	CClientClass* m_pNext;
	int m_ClassID;
};

class CBaseEntity; //forward declare

class CTraceFilter
{
public:
	virtual bool ShouldHitEntity(void* entity, int contents_mask) = 0;
	virtual ETraceType GetTraceType() const = 0;
};

class CBaseTrace
{
public:
	bool IsDispSurface(void) { return ((DispFlags & ESurfaceFlags::DISPSURF_FLAG_SURFACE) != 0); }
	bool IsDispSurfaceWalkable(void) { return ((DispFlags & ESurfaceFlags::DISPSURF_FLAG_WALKABLE) != 0); }
	bool IsDispSurfaceBuildable(void) { return ((DispFlags & ESurfaceFlags::DISPSURF_FLAG_BUILDABLE) != 0); }
	bool IsDispSurfaceSurfProp1(void) { return ((DispFlags & ESurfaceFlags::DISPSURF_FLAG_SURFPROP1) != 0); }
	bool IsDispSurfaceSurfProp2(void) { return ((DispFlags & ESurfaceFlags::DISPSURF_FLAG_SURFPROP2) != 0); }

public:
	Vec3			vStartPos = Vec3();
	Vec3			vEndPos = Vec3();
	cplane_t		Plane = {};
	float			flFraction = 0.0f;
	int				iContents = 0;
	unsigned short	DispFlags = 0;
	bool			bAllSolid = false;
	bool			bStartSolid = false;

	CBaseTrace()
	{

	}

private:
	CBaseTrace(const CBaseTrace& othr);
};

class CGameTrace : public CBaseTrace
{
public:
	bool DidHitWorld() const;
	bool DidHitNonWorldEntity() const;
	int  GetEntityIndex() const;

	bool DidHit() const
	{
		return (flFraction < 1 || bAllSolid || bStartSolid);
	}

public:
	float			fraction_left_solid;
	csurface_t		surface;
	int				hit_group;
	short			physics_bone;
	CBaseEntity* entity;
	int				hitbox;

	CGameTrace() {}
	CGameTrace(const CGameTrace& othr);
};

// basic rectangle struct used for drawing
typedef struct wrect_s
{
	int	left; //[0]
	int right; //[1]
	int top; //[2]
	int bottom; //[3]
} wrect_t;


class CHudTexture
{
public:
	CHudTexture();
	CHudTexture& operator =(const CHudTexture& src);
	virtual ~CHudTexture();

	int Width() const
	{
		return rc.right - rc.left;
	}

	int Height() const
	{
		return rc.bottom - rc.top;
	}

	// causes the font manager to generate the glyph, prevents run time hitches on platforms that have slow font managers
	void Precache(void);

	// returns width & height of icon with scale applied (scale is ignored if font is used to render)
	int EffectiveWidth(float flScale) const;
	int EffectiveHeight(float flScale) const;

	void DrawSelf(int x, int y, const Color_t& clr) const;
	void DrawSelf(int x, int y, int w, int h, const Color_t& clr) const;
	void DrawSelfCropped(int x, int y, int cropx, int cropy, int cropw, int croph, Color_t clr) const;
	// new version to scale the texture over a finalWidth and finalHeight passed in
	void DrawSelfCropped(int x, int y, int cropx, int cropy, int cropw, int croph, int finalWidth, int finalHeight, Color_t clr) const;

	char		szShortName[64];
	char		szTextureFile[64];

	bool		bRenderUsingFont;
	bool		bPrecached;
	char		cCharacterInFont;
	unsigned long		hFont;

	// vgui texture Id assigned to this item
	int			textureId;
	// s0, t0, s1, t1
	float		texCoords[4];

	// Original bounds
	wrect_t		rc;
};

class CFileWeaponInfo
{
public:

	CFileWeaponInfo();

	// Each game can override this to get whatever values it wants from the script.
	virtual void Parse(void* pKeyValuesData, const char* szWeaponName);


public:
	bool					bParsedScript;
	bool					bLoadedHudElements;

	// SHARED
	char					szClassName[MAX_WEAPON_STRING];
	char					szPrintName[MAX_WEAPON_STRING];			// Name for showing in HUD, etc.

	char					szViewModel[MAX_WEAPON_STRING];			// View model of this weapon
	char					szWorldModel[MAX_WEAPON_STRING];		// Model of this weapon seen carried by the player
	char					szAnimationPrefix[MAX_WEAPON_PREFIX];	// Prefix of the animations that should be used by the player carrying this weapon
	int						iSlot;									// inventory slot.
	int						iPosition;								// position in the inventory slot.
	int						iMaxClip1;								// max primary clip size (-1 if no clip)
	int						iMaxClip2;								// max secondary clip size (-1 if no clip)
	int						iDefaultClip1;							// amount of primary ammo in the gun when it's created
	int						iDefaultClip2;							// amount of secondary ammo in the gun when it's created
	int						iWeight;								// this value used to determine this weapon's importance in autoselection.
	int						iRumbleEffect;							// Which rumble effect to use when fired? (xbox)
	bool					bAutoSwitchTo;							// whether this weapon should be considered for autoswitching to
	bool					bAutoSwitchFrom;						// whether this weapon can be autoswitched away from when picking up another weapon or ammo
	int						iFlags;									// miscellaneous weapon flags
	char					szAmmo1[MAX_WEAPON_AMMO_NAME];			// "primary" ammo type
	char					szAmmo2[MAX_WEAPON_AMMO_NAME];			// "secondary" ammo type

	// Sound blocks
	char					aShootSounds[NUM_SHOOT_SOUND_TYPES][MAX_WEAPON_STRING];

	int						iAmmoType;
	int						iAmmo2Type;
	bool					m_bMeleeWeapon;		// Melee weapons can always "fire" regardless of ammo.

	// This tells if the weapon was built right-handed (defaults to true).
	// This helps cl_righthand make the decision about whether to flip the model or not.
	bool					m_bBuiltRightHanded;
	bool					m_bAllowFlipping;	// False to disallow flipping the model, regardless of whether
												// it is built left or right handed.

// CLIENT DLL
	// Sprite data, read from the data file
	int						iSpriteCount;
	CHudTexture* iconActive;
	CHudTexture* iconInactive;
	CHudTexture* iconAmmo;
	CHudTexture* iconAmmo2;
	CHudTexture* iconCrosshair;
	CHudTexture* iconAutoaim;
	CHudTexture* iconZoomedCrosshair;
	CHudTexture* iconZoomedAutoaim;
	CHudTexture* iconSmall;

	// TF2 specific
	bool					bShowUsageHint;							// if true, then when you receive the weapon, show a hint about it

// SERVER DLL

};

struct WeaponData_t
{
	int		m_nDamage;
	int		m_nBulletsPerShot;
	float	m_flRange;
	float	m_flSpread;
	float	m_flPunchAngle;
	float	m_flTimeFireDelay;				// Time to delay between firing
	float	m_flTimeIdle;					// Time to idle after firing
	float	m_flTimeIdleEmpty;				// Time to idle after firing last bullet in clip
	float	m_flTimeReloadStart;			// Time to start into a reload (ie. shotgun)
	float	m_flTimeReload;					// Time to reload
	bool	m_bDrawCrosshair;				// Should the weapon draw a crosshair
	int		m_iProjectile;					// The type of projectile this mode fires
	int		m_iAmmoPerShot;					// How much ammo each shot consumes
	float	m_flProjectileSpeed;			// Start speed for projectiles (nail, etc.); NOTE: union with something non-projectile
	float	m_flSmackDelay;					// how long after swing should damage happen for melee weapons
	bool	m_bUseRapidFireCrits;

	void Init(void)
	{
		m_nDamage = 0;
		m_nBulletsPerShot = 0;
		m_flRange = 0.0f;
		m_flSpread = 0.0f;
		m_flPunchAngle = 0.0f;
		m_flTimeFireDelay = 0.0f;
		m_flTimeIdle = 0.0f;
		m_flTimeIdleEmpty = 0.0f;
		m_flTimeReloadStart = 0.0f;
		m_flTimeReload = 0.0f;
		m_iProjectile = 0;
		m_iAmmoPerShot = 0;
		m_flProjectileSpeed = 0.0f;
		m_flSmackDelay = 0.0f;
		m_bUseRapidFireCrits = false;
	};
};

class CTFWeaponInfo : public CFileWeaponInfo
{
public:

	CTFWeaponInfo();
	~CTFWeaponInfo();

	virtual void Parse(void* pKeyValuesData, const char* szWeaponName);

	WeaponData_t const& GetWeaponData(int iWeapon) const { return m_WeaponData[iWeapon]; }


public:

	WeaponData_t	m_WeaponData[2];

	int		m_iWeaponType;

	// Grenade.
	bool	m_bGrenade;
	float	m_flDamageRadius;
	float	m_flPrimerTime;
	bool	m_bLowerWeapon;
	bool	m_bSuppressGrenTimer;

	// Skins
	bool	m_bHasTeamSkins_Viewmodel;
	bool	m_bHasTeamSkins_Worldmodel;

	// Muzzle flash
	char	m_szMuzzleFlashModel[128];
	float	m_flMuzzleFlashModelDuration;
	char	m_szMuzzleFlashParticleEffect[128];

	// Tracer
	char	m_szTracerEffect[128];

	// Eject Brass
	bool	m_bDoInstantEjectBrass;
	char	m_szBrassModel[128];

	// Explosion Effect
	char	m_szExplosionSound[128];
	char	m_szExplosionEffect[128];
	char	m_szExplosionPlayerEffect[128];
	char	m_szExplosionWaterEffect[128];

	bool	m_bDontDrop;
};

class IMaterialVar;

class IMaterial
{
public:
	virtual const char* GetName() const = 0;
	virtual const char* GetTextureGroupName() const = 0;

	virtual EPreviewImageRetVal GetPreviewImageProperties(int* width, int* height,
														  EImageFormat* imageFormat, bool* isTranslucent) const = 0;

	virtual EPreviewImageRetVal GetPreviewImage(unsigned char* data,
												int width, int height,
												EImageFormat imageFormat) const = 0;

	virtual int				GetMappingWidth() = 0;
	virtual int				GetMappingHeight() = 0;
	virtual int				GetNumAnimationFrames() = 0;
	virtual bool			InMaterialPage(void) = 0;
	virtual	void			GetMaterialOffset(float* pOffset) = 0;
	virtual void			GetMaterialScale(float* pScale) = 0;
	virtual IMaterial* GetMaterialPage(void) = 0;
	virtual IMaterialVar* FindVar(const char* varName, bool* found, bool complain = true) = 0;
	virtual void			IncrementReferenceCount(void) = 0;
	virtual void			DecrementReferenceCount(void) = 0;
	virtual int 			GetEnumerationID(void) const = 0;
	virtual void			GetLowResColorSample(float s, float t, float* color) const = 0;
	virtual void			RecomputeStateSnapshots() = 0;
	virtual bool			IsTranslucent() = 0;
	virtual bool			IsAlphaTested() = 0;
	virtual bool			IsVertexLit() = 0;
	virtual VertexFormat_t	GetVertexFormat() const = 0;
	virtual bool			HasProxy(void) const = 0;
	virtual bool			UsesEnvCubemap(void) = 0;
	virtual bool			NeedsTangentSpace(void) = 0;
	virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool			NeedsSoftwareSkinning(void) = 0;
	virtual void			AlphaModulate(float alpha) = 0;
	virtual void			ColorModulate(float r, float g, float b) = 0;
	virtual void			SetMaterialVarFlag(EMaterialVarFlags flag, bool on) = 0;
	virtual bool			GetMaterialVarFlag(EMaterialVarFlags flag) const = 0;
	virtual void			GetReflectivity(Vec3& reflect) = 0;
	virtual bool			GetPropertyFlag(MaterialPropertyTypes_t type) = 0;
	virtual bool			IsTwoSided() = 0;
	virtual void			SetShader(const char* pShaderName) = 0;
	virtual int				GetNumPasses(void) = 0;
	virtual int				GetTextureMemoryBytes(void) = 0;
	virtual void			Refresh() = 0;
	virtual bool			NeedsLightmapBlendAlpha(void) = 0;
	virtual bool			NeedsSoftwareLighting(void) = 0;
	virtual int				ShaderParamCount() const = 0;
	virtual IMaterialVar** GetShaderParams(void) = 0;
	virtual bool			IsErrorMaterial() const = 0;
	virtual void			SetUseFixedFunctionBakedLighting(bool bEnable) = 0;
	virtual float			GetAlphaModulation() = 0;
	virtual void			GetColorModulation(float* r, float* g, float* b) = 0;
	virtual MorphFormat_t	GetMorphFormat() const = 0;
	virtual IMaterialVar* FindVarFast(char const* pVarName, unsigned int* pToken) = 0;
	virtual void			SetShaderAndParams(void* pKeyValues) = 0;
	virtual const char* GetShaderName() const = 0;
	virtual void			DeleteIfUnreferenced() = 0;
	virtual bool			IsSpriteCard() = 0;
	virtual void			CallBindProxy(void* proxyData) = 0;
	virtual IMaterial* CheckProxyReplacement(void* proxyData) = 0;
	virtual void			RefreshPreservingMaterialVars() = 0;
	virtual bool			WasReloadedFromWhitelist() = 0;
	virtual bool			IsPrecached() const = 0;
};

class IMaterialVar
{
public:
	virtual ITexture* GetTextureValue(void) = 0;

	virtual char const* GetName(void) const = 0;
	virtual MaterialVarSym_t	GetNameAsSymbol() const = 0;

	virtual void			SetFloatValue(float val) = 0;

	virtual void			SetIntValue(int val) = 0;

	virtual void			SetStringValue(char const* val) = 0;
	virtual char const* GetStringValue(void) const = 0;

	// Use FourCC values to pass app-defined data structures between
	// the proxy and the shader. The shader should ignore the data if
	// its FourCC type not correct.	
	virtual void			SetFourCCValue(void* type, void* pData) = 0;
	virtual void			GetFourCCValue(void* type, void** ppData) = 0;

	// Vec (dim 2-4)
	virtual void			SetVecValue(float const* val, int numcomps) = 0;
	virtual void			SetVecValue(float x, float y) = 0;
	virtual void			SetVecValue(float x, float y, float z) = 0;
	virtual void			SetVecValue(float x, float y, float z, float w) = 0;
	virtual void			GetLinearVecValue(float* val, int numcomps) const = 0;

	// revisit: is this a good interface for textures?
	virtual void			SetTextureValue(ITexture*) = 0;

	virtual IMaterial* GetMaterialValue(void) = 0;
	virtual void			SetMaterialValue(IMaterial*) = 0;

	virtual bool			IsDefined() const = 0;
	virtual void			SetUndefined() = 0;

	// Matrix
	virtual void			SetMatrixValue(VMatrix const& matrix) = 0;
	virtual const VMatrix& GetMatrixValue() = 0;
	virtual bool			MatrixIsIdentity() const = 0;

	// Copy....
	virtual void			CopyFrom(IMaterialVar* pMaterialVar) = 0;

	virtual void			SetValueAutodetectType(char const* val) = 0;

	virtual IMaterial* GetOwningMaterial() = 0;

	//set just 1 component
	virtual void			SetVecComponentValue(float fVal, int nComponent) = 0;

protected:
	virtual int				GetIntValueInternal(void) const = 0;
	virtual float			GetFloatValueInternal(void) const = 0;
	virtual float const* GetVecValueInternal() const = 0;
	virtual void			GetVecValueInternal(float* val, int numcomps) const = 0;
	virtual int				VectorSizeInternal() const = 0;
};

enum ParticleAttachment_t
{
	PATTACH_ABSORIGIN = 0,			// Create at absorigin, but don't follow
	PATTACH_ABSORIGIN_FOLLOW,		// Create at absorigin, and update to follow the entity
	PATTACH_CUSTOMORIGIN,			// Create at a custom origin, but don't follow
	PATTACH_POINT,					// Create on attachment point, but don't follow
	PATTACH_POINT_FOLLOW,			// Create on attachment point, and update to follow the entity

	PATTACH_WORLDORIGIN,			// Used for control points that don't attach to an entity

	PATTACH_ROOTBONE_FOLLOW,		// Create at the root bone of the entity, and update to follow

	MAX_PATTACH_TYPES,
};

struct te_tf_particle_effects_colors_t
{
	Vec3 m_vecColor1;
	Vec3 m_vecColor2;
};

struct te_tf_particle_effects_control_point_t
{
	ParticleAttachment_t m_eParticleAttachment;
	Vec3 m_vecOffset;
};

class CEffectData
{
public:
	Vec3									m_vOrigin;
	Vec3									m_vStart;
	Vec3									m_vNormal;
	Vec3									m_vAngles;
	int										m_fFlags;
	int										m_nEntIndex;
	float									m_flScale;
	float									m_flMagnitude;
	float									m_flRadius;
	int										m_nAttachmentIndex;
	short									m_nSurfaceProp;
	int										m_nMaterial;
	int										m_nDamageType;
	int										m_nHitBox;
	unsigned char							m_nColor;
	bool									m_bCustomColors;
	te_tf_particle_effects_colors_t			m_CustomColors;
	bool									m_bControlPoint1;
	te_tf_particle_effects_control_point_t	m_ControlPoint1;
	int										m_iEffectName;
};

class ICollideable
{
public:
	virtual void* GetEntityHandle() = 0;
	virtual const Vec3& OBBMinsPreScaled() const = 0;
	virtual const Vec3& OBBMaxsPreScaled() const = 0;
	virtual const Vec3& OBBMins() const = 0;
	virtual const Vec3& OBBMaxs() const = 0;
	virtual void			WorldSpaceTriggerBounds(Vec3* pVecWorldMins, Vec3* pVecWorldMaxs) const = 0;
	virtual bool			TestCollision(const Ray_t& ray, unsigned int fContentsMask, CGameTrace& tr) = 0;
	virtual bool			TestHitboxes(const Ray_t& ray, unsigned int fContentsMask, CGameTrace& tr) = 0;
	virtual int				GetCollisionModelIndex() = 0;
	virtual const model_t* GetCollisionModel() = 0;
	virtual const Vec3& GetCollisionOrigin() const = 0;
	virtual const Vec3& GetCollisionAngles() const = 0;
	virtual const matrix3x4& CollisionToWorldTransform() const = 0;
	virtual SolidType_t		GetSolid() const = 0;
	virtual int				GetSolidFlags() const = 0;
	virtual void* GetIClientUnknown() = 0;
	virtual int				GetCollisionGroup() const = 0;
	virtual void			WorldSpaceSurroundingBounds(Vec3* pVecMins, Vec3* pVecMaxs) = 0;
	virtual bool			ShouldTouchTrigger(int triggerSolidFlags) const = 0;
	virtual const matrix3x4* GetRootParentToWorldTransform() const = 0;
};

class CCollisionProperty : public ICollideable
{
public:
	__inline void SetCollisionBounds(const Vec3& mins, const Vec3& maxs)
	{
		static auto FN = reinterpret_cast<void(__thiscall*)(CCollisionProperty*, const Vec3&, const Vec3&)>(g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 28 53 8B 5D 08 56 8B 75 0C 57 8B 03"));
		FN(this, mins, maxs);
	}
};

struct StartSoundParams_t
{
	bool			staticsound;
	int				userdata;
	int				soundsource;
	int				entchannel;
	void* pSfx; //CSfxTable
	Vec3			origin;
	Vec3			direction;
	bool			bUpdatePositions;
	float			fvol;
	soundlevel_t	soundlevel;
	int				flags;
	int				pitch;
	int				specialdsp;
	bool			fromserver;
	float			delay;
	int				speakerentity;
	bool			suppressrecording;
	int				initialStreamPosition;
};

class IRecipientFilter
{
public:
	virtual			~IRecipientFilter() {}

	virtual bool	IsReliable(void) const = 0;
	virtual bool	IsInitMessage(void) const = 0;

	virtual int		GetRecipientCount(void) const = 0;
	virtual int		GetRecipientIndex(int slot) const = 0;
};
