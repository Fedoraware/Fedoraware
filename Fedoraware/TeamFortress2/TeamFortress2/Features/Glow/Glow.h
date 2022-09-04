#pragma once

#include "../../SDK/SDK.h"

class CGlowEffect
{
private:
	IMaterial* m_pMatGlowColor;
	ITexture* m_pRtFullFrame;
	ITexture* m_pRtQuarterSize1;
	ITexture* m_pRenderBuffer1;
	ITexture* m_pRenderBuffer2;
	IMaterial* m_pMatBlurXwf;
	IMaterial* m_pMatBlurX;
	IMaterial* m_pMatBlurYwf;
	IMaterial* m_pMatBlurY;
	IMaterial* m_pMatHaloAddToScreen;

	struct GlowEnt_t
	{
		CBaseEntity* m_pEntity;
		Color_t m_Color;
		float m_flAlpha;
	};

	std::vector<GlowEnt_t> m_vecGlowEntities;
	std::unordered_map<CBaseEntity*, bool> m_DrawnEntities;

private:
	void DrawModel(CBaseEntity* pEntity, int nFlags, bool bIsDrawingModels);
	void SetScale(int nScale, bool bReset);
	void DeleteMaterials();

public:
	void Init();
	void Render();
	void CreateMaterials();

	bool HasDrawn(CBaseEntity* pEntity)
	{
		return m_DrawnEntities.find(pEntity) != m_DrawnEntities.end();
	}

	bool IsGlowMaterial(IMaterial* pMat)
	{
		return pMat == m_pMatGlowColor;
	}

public:
	bool m_bDrawingGlow;
	bool m_bRendering;
};

ADD_FEATURE(CGlowEffect, Glow)