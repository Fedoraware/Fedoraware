#include "CameraWindow.h"
#include "../../Hooks/ViewRenderHook/ViewRenderHook.h"

void CCameraWindow::Init()
{
	// Create camera texture
	CameraTex = g_Interfaces.MatSystem->CreateFullFrameRenderTarget("mirrorcam_rt");

	// Create camera material
	const auto kv = new KeyValues("UnlitGeneric");
	kv->SetString("$basetexture", "mirrorcam_rt");
	CameraMat = g_Interfaces.MatSystem->Create("m_cameraMat", kv);
}

// Draws camera to the screen
void CCameraWindow::Draw()
{
	if (!CameraMat || !g_Interfaces.Engine->IsInGame() ||
		Vars::Visuals::CameraMode.m_Var == 0 ||
		(Vars::Visuals::CameraMode.m_Var > 1 && !CanDraw)) {
		return;
	}

	// Draw to screen
	const auto renderCtx = g_Interfaces.MatSystem->GetRenderContext();
	renderCtx->DrawScreenSpaceRectangle(
		CameraMat,
		ViewRect.x, ViewRect.y, ViewRect.w + 1, ViewRect.h + 1,
		0, 0, static_cast<float>(ViewRect.w), static_cast<float>(ViewRect.h),
		CameraTex->GetActualWidth(), CameraTex->GetActualHeight(),
		nullptr, 1, 1
	);
	renderCtx->Release();
}

// Updates camera data (Origin and Angles)
void CCameraWindow::Update()
{
	if (Vars::Visuals::CameraMode.m_Var <= 1) { return; }
	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		switch (Vars::Visuals::CameraMode.m_Var) {
		case 2:
			{
				// Spy camera
				for (const auto& player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES)) {
					if (!player->IsAlive())
						continue;

					if (player->GetClassNum() != CLASS_SPY)
						continue;

					if (pLocal->GetAbsOrigin().DistTo(player->GetAbsOrigin()) <= 350.f) {
						CameraOrigin = player->GetHitboxPos(HITBOX_HEAD);
						CameraAngles = player->GetEyeAngles();
						CanDraw = true;
						return;
					}
				}
				CanDraw = false;
				break;
			}
		}
	}
}

// Renders another view onto a texture
void CCameraWindow::RenderView(void* ecx, const CViewSetup& pViewSetup)
{
	if (!CameraTex || Vars::Visuals::CameraMode.m_Var == 0 ||
		(Vars::Visuals::CameraMode.m_Var > 1 && !CanDraw)) { return; }

	CViewSetup mirrorView = pViewSetup;
	mirrorView.x = 0;
	mirrorView.y = 0;
	if (Vars::Visuals::CameraMode.m_Var != 1) {
		// Custom origin & angles
		mirrorView.origin = CameraOrigin;
		mirrorView.angles = CameraAngles;
	} else {
		// Mirror cam
		const Vec3 viewAngles = g_Interfaces.Engine->GetViewAngles();
		const Vec3 camAngles = { -viewAngles.x, viewAngles.y + 180.f, viewAngles.z };
		mirrorView.angles = camAngles;
	}
	mirrorView.width = ViewRect.w;
	mirrorView.height = ViewRect.h;
	mirrorView.fov = Vars::Visuals::CameraFOV.m_Var;
	mirrorView.m_flAspectRatio = static_cast<float>(mirrorView.width) / static_cast<float>(mirrorView.height);

	RenderCustomView(ecx, mirrorView, CameraTex);
}

void CCameraWindow::RenderCustomView(void* ecx, const CViewSetup& pViewSetup, ITexture* pTexture) {
	using namespace ViewRenderHook;
	using namespace RenderView;

	const auto renderCtx = g_Interfaces.MatSystem->GetRenderContext();

	renderCtx->PushRenderTargetAndViewport();
	renderCtx->SetRenderTarget(pTexture);

	Func.Original<fn>()(ecx, pViewSetup, VIEW_CLEAR_COLOR | VIEW_CLEAR_DEPTH | VIEW_CLEAR_STENCIL, RENDERVIEW_UNSPECIFIED);

	renderCtx->PopRenderTargetAndViewport();
	renderCtx->Release();
}