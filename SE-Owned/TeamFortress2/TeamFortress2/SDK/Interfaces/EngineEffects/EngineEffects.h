#pragma once

#include "../../Includes/Includes.h"

#define	MAX_DLIGHTS		32

struct ColorRGBExp32
{
	byte r, g, b;
	signed char m_Exponent;
};

struct DLight_t
{
	int				 m_fFlags;
	Vec3			 m_vOrigin;
	float			 m_flRadius;
	ColorRGBExp32    m_Color;
	float			 m_flDie;
	float			 m_flDecay;
	float			 m_flMinLight;
	int				 m_nKey;
	int				 m_nStyle;

	Vec3			 m_vDirection;
	float			 m_flInnerAngle;
	float			 m_flOuterAngle;

	float GetRadius() const {
		return m_flRadius;
	}
	float GetRadiusSquared() const {
		return m_flRadius * m_flRadius;
	}
	float IsRadiusGreaterThanZero() const {
		return m_flRadius > 0.0f;
	}
};

class IMaterial;
class IVEngineEffects
{
public:
	// Retrieve decal texture index from decal by name
	virtual	int				Draw_DecalIndexFromName(char* name) = 0;

	// Apply decal
	virtual	void			DecalShoot(int textureIndex, int entity,
		const model_t* model, const Vec3& model_origin, const Vec3& model_angles,
		const Vec3& position, const Vec3* saxis, int flags) = 0;

	// Apply colored decal
	virtual	void			DecalColorShoot(int textureIndex, int entity,
		const model_t* model, const Vec3& model_origin, const Vec3& model_angles,
		const Vec3& position, const Vec3* saxis, int flags, const Color_t& rgbaColor) = 0;

	virtual void			PlayerDecalShoot(IMaterial* material, void* userdata, int entity, const model_t* model,
		const Vec3& model_origin, const Vec3& model_angles,
		const Vec3& position, const Vec3* saxis, int flags, const Color_t& rgbaColor) = 0;

	// Allocate a dynamic world light ( key is the entity to whom it is associated )
	virtual	DLight_t* CL_AllocDlight(int key) = 0;

	// Allocate a dynamic entity light ( key is the entity to whom it is associated )
	virtual	DLight_t* CL_AllocElight(int key) = 0;

	// Get a list of the currently-active dynamic lights.
	virtual int CL_GetActiveDLights(DLight_t* pList[MAX_DLIGHTS]) = 0;

	// Retrieve decal texture name from decal by index
	virtual	const char* Draw_DecalNameFromIndex(int nIndex) = 0;

	// Given an elight key, find it. Does not search ordinary dlights. May return NULL.
	virtual DLight_t* GetElightByKey(int key) = 0;
};

#define VENGINE_EFFECTS_INTERFACE_VERSION "VEngineEffects001"