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
						default: return nullptr;
					}
				}());
			}

			if (bMatWasForced)
				g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(Colors::Hands.r), Color::TOFLOAT(Colors::Hands.g), Color::TOFLOAT(Colors::Hands.b));

			if (Vars::Chams::DME::HandsAlpha.m_Var < 1.0f)
				g_Interfaces.RenderView->SetBlend(Vars::Chams::DME::HandsAlpha.m_Var);

			ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(ModelRenderHook::DrawModelExecute::index)
				(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);

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

				if (Vars::Chams::DME::Weapon.m_Var)
				{
					g_Interfaces.ModelRender->ForcedMaterialOverride([&]() -> IMaterial *
					{
						switch (Vars::Chams::DME::Weapon.m_Var) {
							case 1: { bMatWasForced = true; return m_pMatShaded; }
							case 2: { bMatWasForced = true; return m_pMatShiny; }
							case 3: { bMatWasForced = true; return m_pMatFlat; }
							case 4: { bMatWasForced = true; return m_pMatWFShaded; }
							case 5: { bMatWasForced = true; return m_pMatWFShiny; }
							case 6: { bMatWasForced = true; return m_pMatWFFlat; }
							default: return nullptr;
						}
					}());
				}

				if (bMatWasForced)
					g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(Colors::Weapon.r), Color::TOFLOAT(Colors::Weapon.g), Color::TOFLOAT(Colors::Weapon.b));

				if (Vars::Chams::DME::WeaponAlpha.m_Var < 1.0f)
					g_Interfaces.RenderView->SetBlend(Vars::Chams::DME::WeaponAlpha.m_Var);

				ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(ModelRenderHook::DrawModelExecute::index)
					(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);

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