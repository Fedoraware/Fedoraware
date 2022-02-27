#pragma once
#include "../../SDK/SDK.h"

class CCameraWindow {
public:
	IMaterial* CameraMat;
	ITexture* CameraTex;
	Vec3 CameraOrigin;
	Vec3 CameraAngles;
	bool CanDraw = false;
	Rect_t ViewRect = { 50, 50, 800, 400 };

	void Init();
	void Draw();
	void Update();
	void RenderView(void* ecx, const CViewSetup& pViewSetup);
	void RenderCustomView(void* ecx, const CViewSetup& pViewSetup, ITexture* pTexture);
};

inline CCameraWindow g_CameraWindow;