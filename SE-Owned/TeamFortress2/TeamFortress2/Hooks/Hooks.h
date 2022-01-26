#pragma once

#include "ClientHook/ClientHook.h"
#include "ClientModeHook/ClientModeHook.h"
#include "PredictionHook/PredictionHook.h"
#include "SurfaceHook/SurfaceHook.h"
#include "PanelHook/PanelHook.h"
#include "EngineVGuiHook/EngineVGuiHook.h"
#include "EngineClientHook/EngineClientHook.h"
#include "ModelRenderHook/ModelRenderHook.h"
#include "UniformRandomStreamHook/UniformRandomStreamHook.h"
#include "GetDrawPositionHook/GetDrawPositionHook.h"
#include "PerformScreenSpaceEffectsHook/PerformScreenSpaceEffectsHook.h"
#include "InCondHook/InCondHook.h"
#include "DrawStaticPropsHook/DrawStaticPropsHook.h"
#include "EngineHook/EngineHook.h"
#include "MenuHook/MenuHook.h"
#include "Scoreboard/Scoreboard.h"
#include "InputHook/InputHook.h"
#include "GameMovementHook/GameMovementHook.h"
#include "FireBullets/FireBullets.h"
#include "Viewmodel/Viewmodel.h"
#include "InventoryExpander/InventoryExpander.h"
#include "IsLocalPlayerUsingVisionFilterFlags/IsLocalPlayerUsingVisionFilterFlags.h"

class CHooks
{
private:
	HWND m_hwWindow = 0;
	void* m_pOriginal;

public:
	void Init();
	void Release();
	void Create(void* pSrc, void* pDst)
	{
		if (MH_CreateHook(pSrc, pDst, &m_pOriginal) != MH_STATUS::MH_OK)
			return;
	}

	template <typename FN>
	inline FN CallOriginal()
	{
		return reinterpret_cast<FN>(m_pOriginal);
	}
};

inline CHooks g_Hooks;