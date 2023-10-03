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

inline Color_t FloatRGBAtoColor(float r, float g, float b, float a)
{
	return Color_t(
		(unsigned char)std::clamp(r + .5f, 0.0f, 255.0f),
		(unsigned char)std::clamp(g + .5f, 0.0f, 255.0f),
		(unsigned char)std::clamp(b + .5f, 0.0f, 255.0f),
		(unsigned char)std::clamp(a + .5f, 0.0f, 255.0f)
	);
}

inline float LerpFloat(float a1, float a2, float t)
{
	return a1 + (a2 - a1) * t;
}

inline Color_t LerpColor(const Color_t& a1, const Color_t& a2, float t)
{
	if (t <= 0.0f) return a1;
	if (t >= 1.0f) return a2;
	return FloatRGBAtoColor(
		LerpFloat(static_cast<float>(a1.r), static_cast<float>(a2.r), t),
		LerpFloat(static_cast<float>(a1.g), static_cast<float>(a2.g), t),
		LerpFloat(static_cast<float>(a1.b), static_cast<float>(a2.b), t),
		LerpFloat(static_cast<float>(a1.a), static_cast<float>(a2.a), t)
	);
}

void DrawBT(void* ecx, void* edx, CBaseEntity* pEntity, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo,
			matrix3x4* pBoneToWorld)
{
	if (!Vars::Backtrack::Enabled.Value)
		return;

	if (!Vars::Backtrack::BtChams::Enabled.Value)
		return;

	auto OriginalFn = Hooks::ModelRender_DrawModelExecute::Hook.Original<Hooks::ModelRender_DrawModelExecute::FN>();

	if (G::CurWeaponType == EWeaponType::PROJECTILE) return;

	if (!pEntity)
		return;

	if (pEntity->GetClassID() != ETFClassID::CTFPlayer)
		return;

	if (!pEntity->IsAlive())
		return;

	if (F::Glow.m_bRendering)
		return;

	if (F::Chams.m_bRendering)
		return;

	const auto& vRecords = F::Backtrack.GetRecords(pEntity);

	if (!vRecords)
		return;

	if (vRecords->empty())
		return;

	if (Vars::Backtrack::BtChams::EnemyOnly.Value && g_EntityCache.GetLocal() && pEntity->GetTeamNum() ==
		g_EntityCache.GetLocal()->GetTeamNum())
		return;

	IMaterial* chosenMat = F::DMEChams.v_MatList.at(Vars::Backtrack::BtChams::Material.Value) ? F::DMEChams.v_MatList.at(Vars::Backtrack::BtChams::Material.Value) : nullptr;

	I::ModelRender->ForcedMaterialOverride(chosenMat);

	if (chosenMat)
	{
		I::RenderView->SetColorModulation(
			Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.r),
			Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.g),
			Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.b));
	}

	const auto& pRenderContext = I::MaterialSystem->GetRenderContext();

	if (Vars::Backtrack::BtChams::IgnoreZ.Value && pRenderContext)
	{
		pRenderContext->DepthRange(0.0f, 0.2f);
	}

	I::RenderView->SetBlend(Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.a));

	if (Vars::Backtrack::BtChams::LastOnly.Value)
	{
		std::optional<TickRecord> vLastRec = F::Backtrack.GetLastRecord(pEntity);

		if (vLastRec)
		{
			float colour[3] = {
				vLastRec->bOnShot ? 1.f : Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.r),
				vLastRec->bOnShot ? 0.f : Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.g),
				vLastRec->bOnShot ? 0.f : Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.b)
			};

			I::RenderView->SetColorModulation(colour);

			OriginalFn(ecx, edx, pState, pInfo, (matrix3x4*)(&vLastRec->BoneMatrix));
		}
	}
	else
	{
		for (size_t i = 0; i < vRecords->size(); i++)
		{
			const TickRecord& pRecord = vRecords->at(i);

			if (!F::Backtrack.WithinRewind(pRecord)) { continue; }

			Color_t btColour = Vars::Backtrack::BtChams::BacktrackColor;

			if (Vars::Backtrack::BtChams::Gradient.Value)
			{
				btColour = LerpColor(Vars::Backtrack::BtChams::BacktrackColor, Vars::Backtrack::BtChams::BacktrackColor2, (float)i / (float)vRecords->size());
			}

			if (pRecord.bOnShot)
			{
				btColour = Color_t(255, 0, 0, 255);
			}

			float colour[3] = {
				Color::TOFLOAT(btColour.r),
				Color::TOFLOAT(btColour.g),
				Color::TOFLOAT(btColour.b)
			};

			I::RenderView->SetColorModulation(colour);

			OriginalFn(ecx, edx, pState, pInfo, (matrix3x4*)(&pRecord.BoneMatrix));
		}
	}

	I::ModelRender->ForcedMaterialOverride(nullptr);
	I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
	I::RenderView->SetBlend(1.0f);

	if (Vars::Backtrack::BtChams::IgnoreZ.Value && pRenderContext)
	{
		pRenderContext->DepthRange(0.0f, 1.0f);
	}
}

