#pragma once
#include "../../Utils/Utils.h"
#include "Const.h"

struct Rect_t { int x, y, w, h; };
typedef unsigned long model_t;
class IClientRenderable;
typedef unsigned short ModelInstanceHandle_t;
struct StudioData_t;
struct StudioDecalHandle_t;

struct mstudiobbox_t
{
	int					bone;
	int					group;
	Vec3				bbmin;
	Vec3				bbmax;
	int					szhitboxnameindex;
	Vec3 angle;
	float m_radius;
	uint8_t _padding[0x10];
};


struct mstudiohitboxset_t
{
	int sznameindex;
	inline char* const name(void) const { return ((char*)this) + sznameindex; }
	int numhitboxes;
	int hitboxindex;
	mstudiobbox_t* hitbox(int i) const { return (mstudiobbox_t*)(((BYTE*)this) + hitboxindex) + i; };
};

struct mstudiobone_t
{
	int					sznameindex;
	inline char *const pszName(void) const { return ((char *)this) + sznameindex; }
	int		 			parent;
	int					bonecontroller[6];
	Vec3				pos;
	float				quat[4];
	Vec3				rot;
	Vec3				posscale;
	Vec3				rotscale;
	float				poseToBone[3][4];
	float				qAlignment[4];
	int					flags;
	int					proctype;
	int					procindex;
	mutable int			physicsbone;
	inline void *pProcedure() const { if (procindex == 0) return NULL; else return  (void *)(((byte *)this) + procindex); };
	int					surfacepropidx;
	inline char *const pszSurfaceProp(void) const { return ((char *)this) + surfacepropidx; }
	int					contents;
	int					unused[8];

	mstudiobone_t() {}
private:
	// No copy constructors allowed
	mstudiobone_t(const mstudiobone_t &vOther);
};

struct studiohdr_t
{
	int id;
	int version;
	int checksum;
	char name[64];
	int length;
	Vec3 eyeposition;
	Vec3 illumposition;
	Vec3 hull_min;
	Vec3 hull_max;
	Vec3 view_bbmin;
	Vec3 view_bbmax;
	int flags;
	int numbones;
	int boneindex;
	inline mstudiobone_t *GetBone(int i) const { return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };

	int numbonecontrollers;
	int bonecontrollerindex;
	int numhitboxsets;
	int hitboxsetindex;

	mstudiohitboxset_t *GetHitboxSet(int i) const
	{
		return (mstudiohitboxset_t *)(((byte *)this) + hitboxsetindex) + i;
	}

	inline mstudiobbox_t *GetHitbox(int i, int set) const
	{
		mstudiohitboxset_t const *s = GetHitboxSet(set);

		if (!s)
			return NULL;

		return s->hitbox(i);
	}

	inline int GetHitboxCount(int set) const
	{
		mstudiohitboxset_t const *s = GetHitboxSet(set);

		if (!s)
			return 0;

		return s->numhitboxes;
	}

	int numlocalanim;
	int localanimindex;
	int numlocalseq;
	int localseqindex;
	mutable int activitylistversion;
	mutable int eventsindexed;
	int numtextures;
	int textureindex;
	int numcdtextures;
	int cdtextureindex;
	int numskinref;
	int numskinfamilies;
	int skinindex;
	int numbodyparts;
	int bodypartindex;
	int numlocalattachments;
	int localattachmentindex;
	int numlocalnodes;
	int localnodeindex;
	int localnodenameindex;
	int numflexdesc;
	int flexdescindex;
	int numflexcontrollers;
	int flexcontrollerindex;
	int numflexrules;
	int flexruleindex;
	int numikchains;
	int ikchainindex;
	int nummouths;
	int mouthindex;
	int numlocalposeparameters;
	int localposeparamindex;
	int surfacepropindex;
	int keyvalueindex;
	int keyvaluesize;
	int numlocalikautoplaylocks;
	int localikautoplaylockindex;
	float mass;
	int contents;
	int numincludemodels;
	int includemodelindex;
	mutable void *virtualModel;
	int szanimblocknameindex;
	int numanimblocks;
	int animblockindex;
	mutable void *animblockModel;
	int bonetablebynameindex;
	void *pVertexBase;
	void *pIndexBase;
	byte constdirectionallightdot;
	byte rootLOD;
	byte numAllowedRootLODs;
	byte unused[1];
	int unused4;
	int numflexcontrollerui;
	int	flexcontrolleruiindex;
	float flVertAnimFixedPointScale;
	int	unused3[1];
	int	studiohdr2index;
	int	unused2[1];
};

struct Ray_t
{
	VectorAligned vaStartP;
	VectorAligned vaDelta;
	VectorAligned vaStartOffset;
	VectorAligned vaExtents;

	bool IsRay;
	bool IsSwept;

	void Init(const Vec3 &source, const Vec3 &destination)
	{
		vaDelta = destination - source;

		IsSwept = (vaDelta.LengthSqr() != 0);

		vaExtents.Set();
		IsRay = true;

		vaStartOffset.Set();
		vaStartP = source;
	}

	void Init(const Vec3& source, const Vec3& destination, const Vec3& min, const Vec3& max)
	{
		vaDelta = destination - source;

		IsSwept = (vaDelta.LengthSqr() != 0);

		vaExtents = max - min;
		vaExtents *= 0.5f;
		IsRay = (vaExtents.LengthSqr() < 1e-6);

		vaStartOffset = min + max;

		vaStartOffset *= 0.5f;
		vaStartP = source - vaStartOffset;
		vaStartOffset *= -1.0f;
	}
};



struct csurface_t
{
	const char* name;
	short surface_props;
	unsigned short flags;
};

struct PlayerInfo_t
{
	// scoreboard information
	char            name[MAX_PLAYER_NAME_LENGTH];
	// local server user ID, unique while server is running
	int				userID;
	// global unique player identifer
	char			guid[SIGNED_GUID_LEN + 1];
	// friends identification number
	uint32_t		friendsID;
	// friends name
	char			friendsName[MAX_PLAYER_NAME_LENGTH];
	// true, if player is a bot controlled by game.dll
	bool			fakeplayer;
	// true if player is the HLTV proxy
	bool			ishltv;
	// custom files CRC for this player
	unsigned long	customFiles[MAX_CUSTOM_FILES];
	// this counter increases each time the server downloaded a new file
	unsigned char	filesDownloaded;
};

struct DrawModelState_t
{
	studiohdr_t* m_pStudioHdr;
	StudioData_t* m_pStudioData;
	IClientRenderable* m_pRenderable;
	const matrix3x4* m_pModelToWorld;
	StudioDecalHandle_t* m_hDecals;
	int					 m_DrawFlags;
	int					 m_Lod;
};

struct ModelRenderInfo_t
{
	Vec3			      m_vOrigin;
	Vec3			      m_vAngles;
	IClientRenderable*    m_pRenderable;
	const model_t*		  m_pModel;
	const matrix3x4*      m_pModelToWorld;
	const matrix3x4*      m_pLightingOffset;
	const Vec3*           m_pLightingOrigin;
	int				      m_nFlags;
	int				      m_nEntIndex;
	int				      m_nSkin;
	int					  m_nBody;
	int				      m_nHitboxSet;
	ModelInstanceHandle_t m_hMdlInstance;

	ModelRenderInfo_t()
	{
		m_pModelToWorld = NULL;
		m_pLightingOffset = NULL;
		m_pLightingOrigin = NULL;
	}
};

struct StaticPropRenderInfo_t
{
	const matrix3x4*        m_pModelToWorld;
	const model_t*          m_pModel;
	IClientRenderable*      m_pRenderable;
	Vec3*                   m_pLightOrigin;
	short					m_sSkin;
	ModelInstanceHandle_t	m_hMdlInstance;
};