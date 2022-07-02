#pragma once
#include "../../SDK/SDK.h"

using DMEFN = void(__stdcall*)(CModelRender*, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld);

struct ChamInfo {
	int $flags = 0;
	int $flags_defined = 0;
	int $flags2 = 0;
	int $flags_defined2 = 0;
};

class CDMEChams
{
private:
	bool ShouldRun();
	IMaterial* GetChamMaterial(const Chams_t& chams);
	IMaterial* GetProxyMaterial(int nIndex);
public:
	void Init();
	IMaterial* CreateNRef(char const* szName, void* pKV);
	void ValidateMaterial(IMaterial* mTarget, ChamInfo backupInfo);
	bool Render(const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld);
	bool m_bRendering;
	std::vector<IMaterial*> v_MatList;
	std::unordered_map<IMaterial*, ChamInfo> backupInformation;
};

ADD_FEATURE(CDMEChams, DMEChams)