#pragma once
#include "../../SDK/SDK.h"

using DMEFN = void(__stdcall*)(CModelRender*, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld);

class CDMEChams
{
public:
	IMaterial* m_pMatShaded;
	IMaterial* m_pMatShiny;
	IMaterial* m_pMatFlat;
	IMaterial* m_pMatFresnel;
	IMaterial* m_pMatBrick;
	IMaterial* m_pMatOverlay;
	IMaterial* m_pMatWFShaded;
	IMaterial* m_pMatWFShiny;
	IMaterial* m_pMatWFFlat;
	IMaterial* m_pMatProxy;
private:
	bool ShouldRun();
	IMaterial* GetChamMaterial(int nIndex);
	IMaterial* GetProxyMaterial(int nIndex);

public:
	void Init();
	bool Render(const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld);
	bool m_bRendering;
};

ADD_FEATURE(CDMEChams, DMEChams)