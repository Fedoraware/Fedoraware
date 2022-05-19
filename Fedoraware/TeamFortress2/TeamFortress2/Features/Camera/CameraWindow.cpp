#include "CameraWindow.h"
#include "../ESP/ESP.h"
#include "../../Hooks/HookManager.h"
#include "../../Hooks/Hooks.h"

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

		case 3:
		case 4:
			{
				// Teleporter camera
				const auto& buildings = g_EntityCache.GetGroup(EGroupType::BUILDINGS_ALL);
				
				for (const auto& pBuilding : buildings)
				{
					if (!pBuilding->IsAlive()) { continue; }

					const auto& building = reinterpret_cast<CBaseObject*>(pBuilding);
					const auto nType = static_cast<EBuildingType>(building->GetType());

					const float distance = pLocal->GetAbsOrigin().DistTo(building->GetAbsOrigin());
					if (distance > 260.f) { continue; }

					if (nType == EBuildingType::TELEPORTER && building->GetObjectMode() == 0)
					{
						Vec3 tpExit;
						if (Utils::GetTeleporterExit(building->GetOwner()->GetIndex(), &tpExit))
						{
							CameraAngles = g_Interfaces.Engine->GetViewAngles();
							CameraOrigin = Vec3(tpExit.x, tpExit.y, tpExit.z + 70);
							CanDraw = true;

							if (Vars::Visuals::CameraMode.m_Var == 4)
							{
								Vec3 vScreen;
								if (Utils::W2S(building->GetAbsOrigin(), vScreen))
								{
									const float height = Math::RemapValClamped(100.f, 0.f, distance, 100.f, 600.f);
									const float width = Math::RemapValClamped(100.f, 0.f, distance, 80.f, 400.f);
									ViewRect.w = width;
									ViewRect.h = height;
									ViewRect.x = vScreen.x - (width * 0.5);
									ViewRect.y = vScreen.y - height;
								} else { CanDraw = true; }
							}

							return;
						}
					}
				}

				CanDraw = false;
			}
		}
	}
}

// Renders another view onto a texture
void CCameraWindow::RenderView(void* ecx, const CViewSetup& pViewSetup)
{
	if (!CameraTex || Vars::Visuals::CameraMode.m_Var == 0 ||
		(Vars::Visuals::CameraMode.m_Var > 1 && !CanDraw)) {
		return;
	}

	CViewSetup mirrorView = pViewSetup;
	mirrorView.x = 0;
	mirrorView.y = 0;
	if (Vars::Visuals::CameraMode.m_Var != 1) {
		// Custom origin & angles
		mirrorView.origin = CameraOrigin;
		mirrorView.angles = CameraAngles;
	}
	else {
		// Mirror cam
		const Vec3 viewAngles = g_Interfaces.Engine->GetViewAngles();
		const Vec3 camAngles = { 0.f, viewAngles.y + 180.f, viewAngles.z };
		mirrorView.angles = camAngles;
	}
	mirrorView.width = ViewRect.w;
	mirrorView.height = ViewRect.h;
	mirrorView.fov = Vars::Visuals::CameraFOV.m_Var;
	mirrorView.m_flAspectRatio = static_cast<float>(mirrorView.width) / static_cast<float>(mirrorView.height);

	RenderCustomView(ecx, mirrorView, CameraTex);
}

void CCameraWindow::RenderCustomView(void* ecx, const CViewSetup& pViewSetup, ITexture* pTexture) {
	const auto renderCtx = g_Interfaces.MatSystem->GetRenderContext();

	renderCtx->PushRenderTargetAndViewport();
	renderCtx->SetRenderTarget(pTexture);

	if (const auto renderViewHook = g_HookManager.GetMapHooks()["ViewRender_RenderView"])
	{
		renderViewHook->Original<void(__thiscall*)(void*, const CViewSetup&, int, int)>()(ecx, pViewSetup, VIEW_CLEAR_COLOR | VIEW_CLEAR_DEPTH | VIEW_CLEAR_STENCIL, RENDERVIEW_UNSPECIFIED);
	}

	renderCtx->PopRenderTargetAndViewport();
	renderCtx->Release();
}