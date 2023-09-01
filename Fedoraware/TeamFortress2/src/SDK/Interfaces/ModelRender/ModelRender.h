#pragma once
#include "../../Includes/Includes.h"


class CModelRender
{
public:
	void ForcedMaterialOverride(IMaterial* mat, EOverrideType type = EOverrideType::OVERRIDE_NORMAL)
	{
		typedef void(__thiscall* FN)(PVOID, IMaterial*, EOverrideType);
		return GetVFunc<FN>(this, 1)(this, mat, type);
	}

	void DrawModelExecute(const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld)
	{
		typedef void(__thiscall* FN)(PVOID, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*);
		return GetVFunc<FN>(this, 19)(this, pState, pInfo, pBoneToWorld);
	}

	void GetMaterialOverride(IMaterial** ppOutForcedMaterial, EOverrideType* pOutOverrideType)
	{
		typedef void(__thiscall* FN)(PVOID, IMaterial**, EOverrideType*);
		return GetVFunc<FN>(this, 25)(this, ppOutForcedMaterial, pOutOverrideType);
	}

};

#define VENGINE_MODELRENDER_INTERFACE "VEngineModel016"