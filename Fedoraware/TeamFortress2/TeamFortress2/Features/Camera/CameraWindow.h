#pragma once
#include "../../SDK/SDK.h"

class CCameraWindow {
public:
	IMaterial* CameraMat;
	ITexture* CameraTex;
	Rect_t ViewRect = { 50, 50, 800, 400 };

	void Init();
	void Draw();
	void RenderView(void* ecx, const CViewSetup& pViewSetup);
	void RenderCustomView(void* ecx, const CViewSetup& pViewSetup, ITexture* pTexture);
};

inline CCameraWindow g_CameraWindow;