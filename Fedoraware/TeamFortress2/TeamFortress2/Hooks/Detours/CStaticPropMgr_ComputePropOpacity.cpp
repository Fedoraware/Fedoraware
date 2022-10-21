#include "../Hooks.h"

typedef unsigned short SpatialPartitionHandle_t;
typedef unsigned short ClientRenderHandle_t;

class CStaticProp
{
public:
	byte pad[11];
	Vector						m_Origin;
	QAngle						m_Angles;
	model_t* m_pModel;
	SpatialPartitionHandle_t	m_Partition;
	ModelInstanceHandle_t		m_ModelInstance;
	unsigned char				m_Alpha;
	unsigned char				m_nSolidType;
	unsigned char				m_Skin;
	unsigned char				m_Flags;
	unsigned short				m_FirstLeaf;
	unsigned short				m_LeafCount;
	CBaseHandle					m_EntHandle;	// FIXME: Do I need client + server handles?
	ClientRenderHandle_t		m_RenderHandle;
	unsigned short				m_FadeIndex;	// Index into the m_StaticPropFade dictionary
	float						m_flForcedFadeScale;

	// bbox is the same for both GetBounds and GetRenderBounds since static props never move.
	// GetRenderBounds is interpolated data, and GetBounds is last networked.
	Vector					m_RenderBBoxMin;
	Vector					m_RenderBBoxMax;
	matrix3x4				m_ModelToWorld;
	float					m_flRadius;

	Vector					m_WorldRenderBBoxMin;
	Vector					m_WorldRenderBBoxMax;

	// FIXME: This sucks. Need to store the lighting origin off
	// because the time at which the static props are unserialized
	// doesn't necessarily match the time at which we can initialize the light cache
	Vector					m_LightingOrigin;
};

MAKE_HOOK(CStaticPropMgr_ComputePropOpacity, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC 0C A1 ? ? ? ? 53 56 57 8B 7D 08 8B D9 8B 30 8D 4F 04 8B 01"), void, __fastcall,
		  void* ecx, void* edx, CStaticProp* pProp)
{
	if (pProp && Vars::Visuals::NoStaticPropFade.Value)
	{
		pProp->m_Alpha = 255;
	}
	else
	{
		Hook.Original<FN>()(ecx, edx, pProp);
	}
}