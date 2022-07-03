#pragma once
#include "../../SDK/SDK.h"

using DMEFN = void(__stdcall*)(CModelRender*, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld);

class CDMEChams
{
private:
	bool ShouldRun();
	IMaterial* GetChamMaterial(const Chams_t& chams);
	IMaterial* GetProxyMaterial(int nIndex);
public:
	void Init();
	IMaterial* CreateNRef(char const* szName, void* pKV);
	void ValidateMaterial(IMaterial* mTarget);
	bool Render(const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld);
	bool m_bRendering;
	std::vector<IMaterial*> v_MatList;
	std::unordered_map<IMaterial*, std::unordered_map<int, const char*>> backupInformation;
};

ADD_FEATURE(CDMEChams, DMEChams)