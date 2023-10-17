#include "../Hooks.h"

#include "../../Features/Camera/CameraWindow.h"
#include "../../Features/Glow/Glow.h"
#include "../../Features/Chams/Chams.h"
#include "../../Features/Chams/DMEChams.h"
#include "../../Features/Items/AttributeChanger/AttributeChanger.h"

#include <mutex>

MAKE_HOOK(ViewRender_RenderView, Utils::GetVFuncPtr(I::ViewRender, 6), void, __fastcall,
		  void* ecx, void* edx, const CViewSetup& view, ClearFlags_t nClearFlags, RenderViewInfo_t whatToDraw)
{
	static std::once_flag onceFlag;
	std::call_once(onceFlag, []
				   {
					   F::Glow.Init();
					   F::Chams.Init();
					   F::DMEChams.Init();
					   F::CameraWindow.Init();
					   F::AttributeChanger.Init();
				   });

	const bool bProjectileCam = Vars::Visuals::ProjectileCameraKey.Value && GetAsyncKeyState(Vars::Visuals::ProjectileCameraKey.Value) & 0x8000 && !g_EntityCache.GetGroup(EGroupType::LOCAL_PROJECTILES).empty();
	const bool bFreeCam = G::FreecamActive && Vars::Visuals::FreecamKey.Value && GetAsyncKeyState(Vars::Visuals::FreecamKey.Value) & 0x8000;

	if (bFreeCam || bProjectileCam) {
		CViewSetup tCustomView{};
		memcpy(&tCustomView, &view, sizeof(CViewSetup));

		//	Projectile Camera (:vomit:)
		if (bProjectileCam) {
			CBaseEntity* pLocal = g_EntityCache.GetLocal();
			CBaseEntity* pFurthest = nullptr;
			for (CBaseEntity* pEntity : g_EntityCache.GetGroup(EGroupType::LOCAL_PROJECTILES)) {
				if (!pFurthest || pEntity->GetAbsOrigin().DistTo(pLocal->GetAbsOrigin()) > pFurthest->GetAbsOrigin().DistTo(pLocal->GetAbsOrigin())) {
					pFurthest = pEntity;
					continue;
				}
			}
			tCustomView.origin = pFurthest->GetAbsOrigin();
		}
		// Handle freecam position
		else if (bFreeCam) {
			tCustomView.origin = G::FreecamPos;
		}

		Hook.Original<void(__thiscall*)(void*, const CViewSetup&, int, int)>()(ecx, tCustomView, nClearFlags, whatToDraw);
		if (!(I::EngineClient->IsTakingScreenshot() && Vars::Visuals::CleanScreenshots.Value)) { F::CameraWindow.RenderView(ecx, view); }
		return;
	}
	Hook.Original<void(__thiscall*)(void*, const CViewSetup&, int, int)>()(ecx, view, nClearFlags, whatToDraw);
	if (!(I::EngineClient->IsTakingScreenshot() && Vars::Visuals::CleanScreenshots.Value)) { F::CameraWindow.RenderView(ecx, view); }
}