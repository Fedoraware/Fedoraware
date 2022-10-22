#include "../Hooks.h"

#include "../../Features/Chams/Chams.h"
#include "../../Features/Chams/DMEChams.h"
#include "../../Features/Glow/Glow.h"
#include "../../Features/Backtrack/Backtrack.h"
#include "../../Features/Visuals/FakeAngleManager/FakeAng.h"

void DrawBT(void* ecx, void* edx, CBaseEntity* pEntity, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld);

MAKE_HOOK(ModelRender_DrawModelExecute, Utils::GetVFuncPtr(I::ModelRender, 19), void, __fastcall,
		  void* ecx, void* edx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld)
{
	if (I::EngineClient->IsTakingScreenshot() && Vars::Visuals::CleanScreenshots.Value) { return Hook.Original<FN>()(ecx, edx, pState, pInfo, pBoneToWorld); }

	CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(pInfo.m_nEntIndex);

	DrawBT(ecx, edx, pEntity, pState, pInfo, pBoneToWorld);


	if (!F::Glow.m_bRendering)
	{
		if (F::DMEChams.Render(pState, pInfo, pBoneToWorld)) { return; }
	}

	Hook.Original<FN>()(ecx, edx, pState, pInfo, pBoneToWorld);
}

void DrawBT(void* ecx, void* edx, CBaseEntity* pEntity, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo,
			matrix3x4* pBoneToWorld)
{
	auto OriginalFn = Hooks::ModelRender_DrawModelExecute::Hook.Original<Hooks::ModelRender_DrawModelExecute::FN>();

	if (G::CurWeaponType == EWeaponType::PROJECTILE) return;

	if (Vars::Backtrack::Enabled.Value && Vars::Backtrack::BtChams::Enabled.Value)
	{
		if (pEntity && pEntity->GetClassID() == ETFClassID::CTFPlayer)
		{
			if (!pEntity->IsAlive())
			{
				return;
			}
			if (!F::Glow.m_bRendering && !F::Chams.m_bRendering)
			{
				if (Vars::Backtrack::BtChams::EnemyOnly.Value && g_EntityCache.GetLocal() && pEntity->GetTeamNum() ==
					g_EntityCache.GetLocal()->GetTeamNum())
				{
					return;
				}

				IMaterial* chosenMat = F::DMEChams.v_MatList.at(Vars::Backtrack::BtChams::Material.Value) ? F::DMEChams.v_MatList.at(Vars::Backtrack::BtChams::Material.Value) : nullptr;

				I::ModelRender->ForcedMaterialOverride(chosenMat);

				if (chosenMat)
				{
					I::RenderView->SetColorModulation(
						Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.r),
						Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.g),
						Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.b));
				}


				if (const auto& pRenderContext = I::MaterialSystem->GetRenderContext())
				{
					if (Vars::Backtrack::BtChams::IgnoreZ.Value)
						pRenderContext->DepthRange(0.0f, 0.2f);
				}

				I::RenderView->SetBlend(Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.a));

				const auto& vRecords = F::Backtrack.GetRecords(pEntity);
				if (Vars::Backtrack::BtChams::LastOnly.Value)
				{
					std::optional<TickRecord> vLastRec = F::Backtrack.GetLastRecord(pEntity);
					if (vLastRec)
					{
						OriginalFn(ecx, edx, pState, pInfo, (matrix3x4*)(&vLastRec->BoneMatrix));
					}
				}
				else
				{
					if (vRecords)
					{
						for (auto& record : *vRecords)
						{
							I::RenderView->SetColorModulation(record.bOnShot ? 1 : Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.r), record.bOnShot ? 0 : Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.g), record.bOnShot ? 0 : Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.b));
							if (F::Backtrack.WithinRewind(record)) { OriginalFn(ecx, edx, pState, pInfo, (matrix3x4*)(&record.BoneMatrix)); }
						}
					}
				}

				I::ModelRender->ForcedMaterialOverride(nullptr);
				I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				I::RenderView->SetBlend(1.0f);

				if (const auto& pRenderContext = I::MaterialSystem->GetRenderContext())
				{
					if (Vars::Backtrack::BtChams::IgnoreZ.Value)
						pRenderContext->DepthRange(0.0f, 1.0f);
				}
			}
		}
	}
}

