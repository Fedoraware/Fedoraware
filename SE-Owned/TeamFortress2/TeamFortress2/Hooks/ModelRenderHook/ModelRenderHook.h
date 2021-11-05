#pragma once

#include "../../SDK/SDK.h"

namespace ModelRenderHook
{
	inline SEOHook::VTable Table;

	namespace DrawModelExecute
	{
		using fn = void(__thiscall*)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*);
		constexpr int index = 19;
		void __stdcall Hook(const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld);
	}

	namespace ForcedMaterialOverride
	{
		using fn = void(__thiscall*)(CModelRender*, IMaterial*, EOverrideType);
		constexpr int index = 1;
		void __stdcall Hook(IMaterial* mat, EOverrideType type);
	}
}