#include "DMEChams.h"

#include "../Vars.h"
#include "../../Hooks/ModelRenderHook/ModelRenderHook.h"
#include "Chams.h"
#include "../Glow/Glow.h"

bool CDMEChams::ShouldRun()
{
	if (!Vars::Chams::DME::Active.m_Var || g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	return true;
}

void CDMEChams::Init()
{
	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatShaded = g_Interfaces.MatSystem->Create("m_pMatShaded", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatShiny = g_Interfaces.MatSystem->Create("m_pMatShiny", kv);
	}

	{
		auto kv = new KeyValues("UnlitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		m_pMatFlat = g_Interfaces.MatSystem->Create("m_pMatFlat", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "models/player/shared/shared_normal");
		kv->SetString("$envmap", "skybox/sky_dustbowl_01");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$phong", "1");
		kv->SetString("$phongfresnelranges", "[0 1.5 2]");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[0.5 0.5 0]");
		kv->SetString("$selfillumtint", "[0 0 0]");
		kv->SetString("$envmaptint", "[0 1 0]");
		m_pMatFresnelHDR0 = g_Interfaces.MatSystem->Create("m_pMatFresnelHDR0", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "models/player/shared/shared_normal");
		kv->SetString("$envmap", "skybox/sky_dustbowl_01");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$phong", "1");
		kv->SetString("$phongfresnelranges", "[0 0.05 0.1]");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[0.5 0.5 0]");
		kv->SetString("$selfillumtint", "[0 0 0]");
		kv->SetString("$envmaptint", "[0 1 0]");
		m_pMatFresnelHDR1 = g_Interfaces.MatSystem->Create("m_pMatFresnelHDR1", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "brick/brickwall031b");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$color2", "[10 10 10]");
		kv->SetString("$additive", "1");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$envmaptint", "[4 4 0]");
		kv->SetString("$selfillum", "1");
		kv->SetString("$rimlight", "1");
		kv->SetString("$rimlightboost", "10");
		m_pMatBrick = g_Interfaces.MatSystem->Create("m_pMatBrick", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "models/player/shared/ice_player");
		kv->SetString("$bumpmap", "models/player/shared/shared_normal");
		kv->SetString("$additive", "1");
		kv->SetString("$phong", "1");
		kv->SetString("$phongfresnelranges", "[0 0.5 10]");
		kv->SetString("$phongtint", "[0 0 0]");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$envmaptint", "[0 0 0]");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumtint", "[0 0 0]");
		kv->SetString("$rimlight", "1");
		kv->SetString("$rimlightboost", "10");
		m_pMatScuffed = g_Interfaces.MatSystem->Create("m_pMatScuffed", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$wireframe", "1");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatWFShaded = g_Interfaces.MatSystem->Create("m_pMatWFShaded", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$wireframe", "1");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatWFShiny = g_Interfaces.MatSystem->Create("m_pMatWFShiny", kv);
	}

	{
		auto kv = new KeyValues("UnlitGeneric");
		kv->SetString("$wireframe", "1");
		kv->SetString("$basetexture", "vgui/white_additive");
		m_pMatWFFlat = g_Interfaces.MatSystem->Create("m_pMatWFFlat", kv);
	}
}

bool CDMEChams::Render(const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld)
{
	m_bRendering = false;

	if (ShouldRun())
	{
		m_bRendering = true;

		CBaseEntity* pEntity = g_Interfaces.EntityList->GetClientEntity(pInfo.m_nEntIndex);

		if (pEntity && pEntity->GetClassID() == ETFClassID::CTFViewModel)
		{
			bool bMatWasForced = false;

			if (Vars::Chams::DME::Hands.m_Var)
			{
				g_Interfaces.ModelRender->ForcedMaterialOverride([&]() -> IMaterial*
					{
						switch (Vars::Chams::DME::Hands.m_Var) {

						case 1: { bMatWasForced = true; return m_pMatShaded; }
						case 2: { bMatWasForced = true; return m_pMatShiny; }
						case 3: { bMatWasForced = true; return m_pMatFlat; }
						case 4: { bMatWasForced = true; return m_pMatWFShaded; }
						case 5: { bMatWasForced = true; return m_pMatWFShiny; }
						case 6: { bMatWasForced = true; return m_pMatWFFlat; }
						case 7: bMatWasForced = true;
							if (g_Interfaces.CVars->FindVar("mat_hdr_level")->GetInt() > 1) {
								m_pMatFresnel = m_pMatFresnelHDR1;
								return m_pMatFresnel;
							}
							else {
								m_pMatFresnel = m_pMatFresnelHDR0;
								return m_pMatFresnel;
							}
						case 8: { bMatWasForced = true; return m_pMatBrick; }
						default: return nullptr;
						}
					}());

			}

			if (bMatWasForced) {
				g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(Colors::Hands.r), Color::TOFLOAT(Colors::Hands.g), Color::TOFLOAT(Colors::Hands.b));
			}

			if (Vars::Chams::DME::HandsAlpha.m_Var < 1.0f)
				g_Interfaces.RenderView->SetBlend(Vars::Chams::DME::HandsAlpha.m_Var);



			ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(ModelRenderHook::DrawModelExecute::index)
				(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
			bMatWasForced = true;

			if (Vars::Chams::DME::HandsGlowOverlay.m_Var && bMatWasForced) { // Overlay
				IMaterial* pMaterial = m_pMatScuffed;
				bool found = false;
				bool found2 = false;
				bool found3 = false;
				IMaterialVar* pVar = pMaterial->FindVar(_("$phongtint"), &found);
				if (found) {
					pVar->SetVecValue(Color::TOFLOAT(Colors::HandsOverlay.r) * 4, Color::TOFLOAT(Colors::HandsOverlay.g) * 4, Color::TOFLOAT(Colors::HandsOverlay.b) * 4);
				}
				IMaterialVar* pVar2 = pMaterial->FindVar(_("$envmaptint"), &found2);
				if (found2) {
					pVar2->SetVecValue(Color::TOFLOAT(Colors::HandsOverlay.r) * 4, Color::TOFLOAT(Colors::HandsOverlay.g) * 4, Color::TOFLOAT(Colors::HandsOverlay.b) * 4);
				}
				IMaterialVar* pVar3 = pMaterial->FindVar(_("$rimlightboost"), &found3);
				if (found3) {
					pVar3->SetIntValue(Vars::Chams::DME::HandsRimMultiplier.m_Var);
				}

				g_Interfaces.ModelRender->ForcedMaterialOverride(pMaterial);

				ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(ModelRenderHook::DrawModelExecute::index)
					(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
			}


			if (bMatWasForced) {
				g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
				g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);

			}

			if (Vars::Chams::DME::HandsAlpha.m_Var < 1.0f)
				g_Interfaces.RenderView->SetBlend(1.0f);

			return true;
		}

		if (!pEntity && pInfo.m_pModel)
		{
			std::string_view szModelName(g_Interfaces.ModelInfo->GetModelName(pInfo.m_pModel));

			if (szModelName.find(_("weapon")) != std::string_view::npos
				&& szModelName.find(_("arrow")) == std::string_view::npos
				&& szModelName.find(_("w_syringe")) == std::string_view::npos
				&& szModelName.find(_("nail")) == std::string_view::npos
				&& szModelName.find(_("shell")) == std::string_view::npos
				&& szModelName.find(_("parachute")) == std::string_view::npos
				&& szModelName.find(_("buffbanner")) == std::string_view::npos
				&& szModelName.find(_("shogun_warbanner")) == std::string_view::npos
				&& szModelName.find(_("targe")) == std::string_view::npos //same as world model, can't filter
				&& szModelName.find(_("shield")) == std::string_view::npos //same as world model, can't filter
				&& szModelName.find(_("repair_claw")) == std::string_view::npos)
			{
				//g_Interfaces.DebugOverlay->AddTextOverlay(pInfo.m_vOrigin, 0.003f, "%hs", szModelName);

				bool bMatWasForced = false;

				int _case = 0;

				if (Vars::Chams::DME::Weapon.m_Var)
				{
					g_Interfaces.ModelRender->ForcedMaterialOverride([&]() -> IMaterial*
						{
							switch (Vars::Chams::DME::Weapon.m_Var) {
								//case 0: { bMatWasForced = true; _case = 9; return nullptr; }
							case 1: { bMatWasForced = true; _case = 1; return m_pMatShaded; }
							case 2: { bMatWasForced = true; _case = 2; return m_pMatShiny; }
							case 3: { bMatWasForced = true; _case = 3; return m_pMatFlat; }
							case 4: { bMatWasForced = true; _case = 4; return m_pMatWFShaded; }
							case 5: { bMatWasForced = true; _case = 5; return m_pMatWFShiny; }
							case 6: { bMatWasForced = true; _case = 6; return m_pMatWFFlat; }
							case 7: bMatWasForced = true;
								if (g_Interfaces.CVars->FindVar("mat_hdr_level")->GetInt() > 1) {
									m_pMatFresnel = m_pMatFresnelHDR1;
									return m_pMatFresnel;
								}
								else {
									m_pMatFresnel = m_pMatFresnelHDR0;
									return m_pMatFresnel;
								}
							case 8: { bMatWasForced = true; _case = 8; return m_pMatBrick; }
							default: { return nullptr; }
								   //default: { bMatWasForced = true; _case = 100;  return nullptr; }
							}
						}());
				}

				if (bMatWasForced) {
					g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(Colors::Weapon.r), Color::TOFLOAT(Colors::Weapon.g), Color::TOFLOAT(Colors::Weapon.b));
				}

				if (Vars::Chams::DME::WeaponAlpha.m_Var < 1.0f)
					g_Interfaces.RenderView->SetBlend(Vars::Chams::DME::WeaponAlpha.m_Var);

				ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(ModelRenderHook::DrawModelExecute::index) // base
					(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
				bMatWasForced = true;


				// the following code is bad & pasted og @ Chams.cpp
				bool foundselfillumtint = false;
				if (Vars::Chams::DME::Hands.m_Var == 7) {
					IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar(_("$selfillumtint"), &foundselfillumtint);
					if (foundselfillumtint) {
						fresnelSelfillumtint->SetVecValue(Color::TOFLOAT(Colors::FresnelBase.r), Color::TOFLOAT(Colors::FresnelBase.g), Color::TOFLOAT(Colors::FresnelBase.b));
					}
				}

				foundselfillumtint = false;
				if (Vars::Chams::DME::Weapon.m_Var == 7) {
					IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar(_("$selfillumtint"), &foundselfillumtint);
					if (foundselfillumtint) {
						fresnelSelfillumtint->SetVecValue(Color::TOFLOAT(Colors::FresnelBase.r), Color::TOFLOAT(Colors::FresnelBase.g), Color::TOFLOAT(Colors::FresnelBase.b));
					}
				}

				if (Vars::Chams::DME::WeaponGlowOverlay.m_Var && bMatWasForced) {
					IMaterial* pMaterial = m_pMatScuffed;
					bool found = false;
					bool found2 = false;
					bool found3 = false;
					IMaterialVar* pVar = pMaterial->FindVar(_("$phongtint"), &found);
					if (found) {
						pVar->SetVecValue(Color::TOFLOAT(Colors::WeaponOverlay.r) * 4, Color::TOFLOAT(Colors::WeaponOverlay.g) * 4, Color::TOFLOAT(Colors::WeaponOverlay.b) * 4);
					}
					IMaterialVar* pVar2 = pMaterial->FindVar(_("$envmaptint"), &found2);
					if (found2) {
						pVar2->SetVecValue(Color::TOFLOAT(Colors::WeaponOverlay.r) * 4, Color::TOFLOAT(Colors::WeaponOverlay.g) * 4, Color::TOFLOAT(Colors::WeaponOverlay.b) * 4);
					}
					IMaterialVar* pVar3 = pMaterial->FindVar(_("$rimlightboost"), &found3);
					if (found3) {
						pVar3->SetIntValue(Vars::Chams::DME::WeaponRimMultiplier.m_Var);
					}

					g_Interfaces.ModelRender->ForcedMaterialOverride(pMaterial);

					ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(ModelRenderHook::DrawModelExecute::index) //overlay
						(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
				}

				if (bMatWasForced) {

					g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
					g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				}

				if (Vars::Chams::DME::WeaponAlpha.m_Var < 1.0f)
					g_Interfaces.RenderView->SetBlend(1.0f);

				return true;
			}
		}

		m_bRendering = false;
	}

	return false;
}
