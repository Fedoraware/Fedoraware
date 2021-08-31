#include "DMEChams.h"

#include "../Vars.h"
#include "../../Hooks/ModelRenderHook/ModelRenderHook.h"

bool CDMEChams::ShouldRun()
{
	if (!Vars::Chams::DME::Active.m_Var || g_Interfaces.EngineVGui->IsGameUIVisible())
		return false;

	return true;
}

void CDMEChams::Init()
{
	m_pMatFlat2 = Utils::CreateMaterial({
	_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$bumpmap\" \"vgui/white_additive\"\
		\n\t\"$color2\" \"[10 10 10]\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$selfillumfresnel\" \"1\"\
		\n\t\"$selfillumfresnelminmaxexp\" \"[0.1 0.2 0.3]\"\
		\n\t\"$selfillumtint\" \"[0 0 0]\"\
		\n}\n")
	});

	m_pMatGlow = Utils::CreateMaterial({
	_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$basetexture\" \"brick/brickwall031b\"\
		\n\t\"$bumpmap\" \"vgui/white_additive\"\
		\n\t\"$color2\" \"[10 10 10]\"\
		\n\t\"$additive\" \"1\"\
		\n\t\"$envmap\" \"cubemaps/cubemap_sheen001\"\
		\n\t\"$envmapfresnel\" \"1\"\
		\n\t\"$envmaptint\" \"[4 4 0]\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$rimlight\" \"1\"\
		\n\t\"$rimlightboost\" \"10\"\
		\n")
	});

	m_pMatNone = Utils::CreateMaterial({
	_("")
	});

	m_pMatScuffed = Utils::CreateMaterial({
	_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$basetexture\" \"models/player/shared/ice_player\"\
		\n\t\"$bumpmap\" \"models/player/shared/shared_normal\"\
		\n\t\"$additive\" \"1\"\
		\n\t\"$phong\" \"1\"\
		\n\t\"$phongfresnelranges\" \"[0 0.5 10]\"\
		\n\t\"$phongtint\" \"[0 0 0]\"\
		\n\t\"$envmap\" \"cubemaps/cubemap_sheen001\"\
		\n\t\"$envmapfresnel\" \"1\"\
		\n\t\"$envmaptint\" \"[0 0 0]\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$selfillumtint\" \"[0 0 0]\"\
		\n\t\"$rimlight\" \"1\"\
		\n\t\"$rimlightboost\" \"10\"\
		\n")
	});

	m_pMatShaded = Utils::CreateMaterial({
	_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$bumpmap\" \"vgui/white_additive\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$selfillumfresnel\" \"1\"\
		\n\t\"$selfillumfresnelminmaxexp\" \"[-0.25 1 1]\"\
		\n}\n")
	});

	m_pMatShiny = Utils::CreateMaterial({
	_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$bumpmap\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"cubemaps/cubemap_sheen001\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$selfillumfresnel\" \"1\"\
		\n\t\"$selfillumfresnelminmaxexp\" \"[-0.25 1 1]\"\
		\n}\n")
	});

	m_pMatFlat = Utils::CreateMaterial({
		_("\"UnlitGeneric\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n}\n")
	});

	m_pMatWFShaded = Utils::CreateMaterial({
	_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$wireframe\" \"1\"\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$bumpmap\" \"vgui/white_additive\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$selfillumfresnel\" \"1\"\
		\n\t\"$selfillumfresnelminmaxexp\" \"[-0.25 1 1]\"\
		\n}\n")
	});

	m_pMatWFShiny = Utils::CreateMaterial({
	_("\"VertexLitGeneric\"\
		\n{\
		\n\t\"$wireframe\" \"1\"\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$bumpmap\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"cubemaps/cubemap_sheen001\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$selfillumfresnel\" \"1\"\
		\n\t\"$selfillumfresnelminmaxexp\" \"[-0.25 1 1]\"\
		\n}\n")
	});

	m_pMatWFFlat = Utils::CreateMaterial({
	_("\"UnlitGeneric\"\
		\n{\
		\n\t\"$wireframe\" \"1\"\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n}\n")
	});
}

bool CDMEChams::Render(const DrawModelState_t &pState, const ModelRenderInfo_t &pInfo, matrix3x4 *pBoneToWorld)
{
	m_bRendering = false;

	if (ShouldRun())
	{
		m_bRendering = true;

		CBaseEntity *pEntity = g_Interfaces.EntityList->GetClientEntity(pInfo.m_nEntIndex);

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
						case 7: { bMatWasForced = true; return m_pMatFlat2; }
						case 8: { bMatWasForced = true; return m_pMatGlow; }
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
				if (found2) {
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
					g_Interfaces.ModelRender->ForcedMaterialOverride([&]() -> IMaterial *
					{
						switch (Vars::Chams::DME::Weapon.m_Var) {
							//case 0: { bMatWasForced = true; _case = 9; return nullptr; }
							case 1: { bMatWasForced = true; _case = 1; return m_pMatShaded; }
							case 2: { bMatWasForced = true; _case = 2; return m_pMatShiny; }
							case 3: { bMatWasForced = true; _case = 3; return m_pMatFlat; }
							case 4: { bMatWasForced = true; _case = 4; return m_pMatWFShaded; }
							case 5: { bMatWasForced = true; _case = 5; return m_pMatWFShiny; }
							case 6: { bMatWasForced = true; _case = 6; return m_pMatWFFlat; }
							case 7: { bMatWasForced = true; _case = 7; return m_pMatFlat2; }
							case 8: { bMatWasForced = true; _case = 8; return m_pMatGlow; }
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
					if (found2) {
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