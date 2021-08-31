#pragma once
#include "../../SDK/SDK.h"

class CDMEChams
{
private:
	IMaterial *m_pMatShaded;
	IMaterial *m_pMatShiny;
	IMaterial *m_pMatFlat;
	IMaterial* m_pMatFlat2;
	IMaterial* m_pMatGlow;
	IMaterial* m_pMatNone;
	IMaterial* m_pMatScuffed;
	IMaterial *m_pMatWFShaded;
	IMaterial *m_pMatWFShiny;
	IMaterial *m_pMatWFFlat;

private:
	bool ShouldRun();

public:
	void Init();
	bool Render(const DrawModelState_t &pState, const ModelRenderInfo_t &pInfo, matrix3x4 *pBoneToWorld);
	bool m_bRendering;
};

inline CDMEChams g_DMEChams;