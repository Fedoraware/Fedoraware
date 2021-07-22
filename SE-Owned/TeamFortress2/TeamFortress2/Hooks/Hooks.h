#pragma once

#include "ClientHook/ClientHook.h"
#include "ClientModeHook/ClientModeHook.h"
#include "PredictionHook/PredictionHook.h"
#include "SurfaceHook/SurfaceHook.h"
#include "PanelHook/PanelHook.h"
#include "EngineVGuiHook/EngineVGuiHook.h"
#include "EngineClientHook/EngineClientHook.h"
#include "ModelRenderHook/ModelRenderHook.h"
#include "WndProcHook/WndProcHook.h"
#include "UniformRandomStreamHook/UniformRandomStreamHook.h"
#include "GetDrawPositionHook/GetDrawPositionHook.h"
#include "PerformScreenSpaceEffectsHook/PerformScreenSpaceEffectsHook.h"
#include "InCondHook/InCondHook.h"
#include "DrawStaticPropsHook/DrawStaticPropsHook.h"
#include "EngineHook/EngineHook.h"

class CHooks
{
private:
	HWND m_hwWindow = 0;

public:
	void Init();
	void Release();
};

inline CHooks g_Hooks;