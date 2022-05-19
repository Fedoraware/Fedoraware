#pragma once
#include "../../SDK/SDK.h"

using RenderViewFN = void(__fastcall*)(void* ecx, void* edx, const CViewSetup& view, ClearFlags_t nClearFlags, RenderViewInfo_t whatToDraw);

class CCameraWindow {
public:
	IMaterial* CameraMat;
	ITexture* CameraTex;
	Vec3 CameraOrigin;
	Vec3 CameraAngles;
	bool CanDraw = false;
	Rect_t ViewRect = { 50, 50, 800, 400 };
	RenderViewFN originalFn = nullptr;
	void Init();
	void Draw();
	void Update();
	void RenderView(void* ecx, const CViewSetup& pViewSetup);
	void RenderCustomView(void* ecx, const CViewSetup& pViewSetup, ITexture* pTexture);
};

inline CCameraWindow g_CameraWindow;