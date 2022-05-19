#include "../Hooks.h"

#include "../../Features/Chams/Chams.h"
#include "../../Features/Chams/DMEChams.h"
#include "../../Features/Glow/Glow.h"
#include "../../Features/Backtrack/Backtrack.h"
#include "../../Features/Visuals/FakeAngleManager/FakeAng.h"

void DrawBT(void* ecx, void* edx, CBaseEntity* pEntity, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld);
void DrawFakeAngles(void* ecx, void* edx, CBaseEntity* pEntity, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo);

MAKE_HOOK(ModelRender_DrawModelExecute, Utils::GetVFuncPtr(g_Interfaces.ModelRender, 19), void, __fastcall,
		  void* ecx, void* edx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld)
{
	CBaseEntity* pEntity = g_Interfaces.EntityList->GetClientEntity(pInfo.m_nEntIndex);

	DrawBT(ecx, edx, pEntity, pState, pInfo, pBoneToWorld);
	DrawFakeAngles(ecx, edx, pEntity, pState, pInfo);

	if ((g_Chams.HasDrawn(pEntity) || g_Glow.HasDrawn(pEntity)) && !g_Glow.m_bDrawingGlow) { return; }
	if (g_DMEChams.Render(pState, pInfo, pBoneToWorld)) { return; }

	Hook.Original<FN>()(ecx, edx, pState, pInfo, pBoneToWorld);
}

void DrawBT(void* ecx, void* edx, CBaseEntity* pEntity, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo,
			matrix3x4* pBoneToWorld)
{
	auto OriginalFn = Hooks::ModelRender_DrawModelExecute::Hook.Original<Hooks::ModelRender_DrawModelExecute::FN>();

	if (Vars::Backtrack::Enabled.m_Var && Vars::Backtrack::BtChams::Enabled.m_Var)
	{
		if (pEntity && pEntity->GetClassID() == ETFClassID::CTFPlayer)
		{
			if (!g_Glow.m_bRendering && !g_Chams.m_bRendering)
			{
				bool bMatWasForced = false;

				if (Vars::Backtrack::BtChams::EnemyOnly.m_Var && g_EntityCache.m_pLocal && pEntity->GetTeamNum() ==
					g_EntityCache.m_pLocal->GetTeamNum())
				{
					return;
				}

				g_Interfaces.ModelRender->ForcedMaterialOverride([&]() -> IMaterial*
																 {
																	 switch (Vars::Backtrack::BtChams::Material.m_Var)
																	 {
																		 case 0:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatShaded;
																		 }
																		 case 1:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatShiny;
																		 }
																		 case 2:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatFlat;
																		 }
																		 case 3:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatWFShaded;
																		 }
																		 case 4:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatWFShiny;
																		 }
																		 case 5:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatWFFlat;
																		 }
																		 case 6:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatScuffed;
																		 }
																		 case 7:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatBrick;
																		 }
																		 default: return nullptr;
																	 }
																 }());
				if (bMatWasForced)
				{
					g_Interfaces.RenderView->SetColorModulation(
						Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.r),
						Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.g),
						Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.b));
				}


				if (const auto& pRenderContext = g_Interfaces.MatSystem->GetRenderContext())
				{
					if (Vars::Backtrack::BtChams::IgnoreZ.m_Var)
						pRenderContext->DepthRange(0.0f, 0.2f);
				}

				g_Interfaces.RenderView->SetBlend(Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.a));



				if (Vars::Backtrack::BtChams::LastOnly.m_Var)
				{
					if (!g_Backtrack.Record[pEntity->GetIndex()].empty())
					{
						OriginalFn(ecx, edx, pState, pInfo, reinterpret_cast<matrix3x4*>(&g_Backtrack.Record[pEntity->GetIndex()].back().BoneMatrix));
					}
				}
				else
				{
					if (!g_Backtrack.Record[pEntity->GetIndex()].empty())
					{
						for (size_t t = 0; t < g_Backtrack.Record[pEntity->GetIndex()].size(); t++)
						{
							if (g_Backtrack.IsGoodTick(t)) { continue; }
							OriginalFn(ecx, edx, pState, pInfo, reinterpret_cast<matrix3x4*>(&g_Backtrack.Record[pEntity->GetIndex()].at(t).BoneMatrix));
						}
					}
				}


				bMatWasForced = true;

				if (bMatWasForced)
				{
					g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
					g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				}

				g_Interfaces.RenderView->SetBlend(1.0f);

				if (const auto& pRenderContext = g_Interfaces.MatSystem->GetRenderContext())
				{
					if (Vars::Backtrack::BtChams::IgnoreZ.m_Var)
						pRenderContext->DepthRange(0.0f, 1.0f);
				}
			}
		}
	}
}

void DrawFakeAngles(void* ecx, void* edx, CBaseEntity* pEntity, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo)
{
	auto OriginalFn = Hooks::ModelRender_DrawModelExecute::Hook.Original<Hooks::ModelRender_DrawModelExecute::FN>();

	if (Vars::Misc::CL_Move::Fakelag.m_Var && Vars::Misc::CL_Move::FakelagIndicator.m_Var && g_FakeAng.DrawChams)
	{
		if (pEntity && pEntity == g_EntityCache.m_pLocal)
		{
			if (!g_Glow.m_bRendering && !g_Chams.m_bRendering)
			{
				bool bMatWasForced = false;

				g_Interfaces.ModelRender->ForcedMaterialOverride([&]() -> IMaterial*
																 {
																	 switch (Vars::Misc::CL_Move::FLGChams::Material.m_Var)
																	 {
																		 case 0:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatShaded;
																		 }
																		 case 1:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatShiny;
																		 }
																		 case 2:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatFlat;
																		 }
																		 case 3:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatWFShaded;
																		 }
																		 case 4:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatWFShiny;
																		 }
																		 case 5:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatWFFlat;
																		 }
																		 case 6:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatScuffed;
																		 }
																		 case 7:
																		 {
																			 bMatWasForced = true;
																			 return g_DMEChams.m_pMatBrick;
																		 }
																		 default: return nullptr;
																	 }
																 }());
				if (bMatWasForced)
				{
					g_Interfaces.RenderView->SetColorModulation(
						Color::TOFLOAT(Vars::Misc::CL_Move::FLGChams::FakelagColor.r),
						Color::TOFLOAT(Vars::Misc::CL_Move::FLGChams::FakelagColor.g),
						Color::TOFLOAT(Vars::Misc::CL_Move::FLGChams::FakelagColor.b));
				}

				g_Interfaces.RenderView->SetBlend(Color::TOFLOAT(Vars::Misc::CL_Move::FLGChams::FakelagColor.a)); // this is so much better than having a seperate alpha slider lmao
				OriginalFn(ecx, edx, pState, pInfo, reinterpret_cast<matrix3x4*>(&g_FakeAng.BoneMatrix));

				bMatWasForced = true;

				if (bMatWasForced)
				{
					g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
					g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				}

				g_Interfaces.RenderView->SetBlend(1.0f);
			}
		}
	}
}
