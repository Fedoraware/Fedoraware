#pragma once

#include "../../SDK/SDK.h"
#include "../Hooks.h"
#include "../../Utils/MinHook/hook.h"

namespace EndSceneHook
{
	inline CHook Hook;
	using fn = HRESULT(__stdcall*)(IDirect3DDevice9*);
	HRESULT __stdcall Func(IDirect3DDevice9* pDevice);
	void Init();
}