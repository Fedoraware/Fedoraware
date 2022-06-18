#include "Chams.h"
#include "../Vars.h"
#include "../Menu/MaterialEditor/MaterialEditor.h"

bool CChams::ShouldRun()
{
	return !I::EngineVGui->IsGameUIVisible();
}

void CChams::DrawModel(CBaseEntity* pEntity)
{
	m_bRendering = true;
	pEntity->DrawModel(STUDIO_RENDER | STUDIO_NOSHADOWS);
	m_DrawnEntities[pEntity] = true;
	m_bRendering = false;
}

void CChams::Init()
{
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
		m_pMatFresnel = I::MatSystem->Create("m_pMatFresnel", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatShaded = I::MatSystem->Create("m_pMatShaded", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "brick/brickwall031b");
		kv->SetString("$additive", "1");
		kv->SetString("$phong", "1");
		kv->SetString("$phongfresnelrangse", "[0 0.5 10]");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$selfillum", "1");
		kv->SetString("$rimlight", "1");
		kv->SetString("$rimlightboost", "100");
		kv->SetString("$envmapfresnelminmaxexp", "[0 1 2]");
		m_pMatBrick = I::MatSystem->Create("m_pMatBrick", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatShiny = I::MatSystem->Create("m_pMatShiny", kv);
	}


	{
		auto kv = new KeyValues("UnlitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		m_pMatFlat = I::MatSystem->Create("m_pMatFlat", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		{
			kv->SetString("$basetexture", "models/player/shared/ice_player");
			kv->SetString("$bumpmap", "models/player/shared/shared_normal");
			kv->SetString("$phong", "1");
			kv->SetString("$phongexponent", "10");
			kv->SetString("$phongboost", "1");
			kv->SetString("$phongfresnelranges", "[0 0 0]");
			kv->SetString("$basemapalphaphongmask", "1");
			kv->SetString("$phongwarptexture", "models/player/shared/ice_player_warp");
			m_pMatPlastic = I::MatSystem->Create("m_pMatPlastic", kv);
		}
	}

	m_pMatBlur = I::MatSystem->Find("models/effects/muzzleflash/blurmuzzle", "Model textures");
}

void CChams::Render()
{
	if (!m_DrawnEntities.empty())
		m_DrawnEntities.clear();

	m_bHasSetStencil = false;

	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		if (!ShouldRun())
			return;

		if (const auto& pRenderContext = I::MatSystem->GetRenderContext())
		{
			//Let's do this in advance if Glow is enabled.
			/*if (Vars::Glow::Main::Active.m_Var)
			{*/
			ShaderStencilState_t StencilState = {};
			StencilState.m_bEnable = true;
			StencilState.m_nReferenceValue = 1;
			StencilState.m_CompareFunc = STENCILCOMPARISONFUNCTION_ALWAYS;
			StencilState.m_PassOp = STENCILOPERATION_REPLACE;
			StencilState.m_FailOp = STENCILOPERATION_KEEP;
			StencilState.m_ZFailOp = STENCILOPERATION_REPLACE;
			StencilState.SetStencilState(pRenderContext);
			m_bHasSetStencil = true;
			//}

			RenderEnts(pLocal, pRenderContext);
		}
	}
}

void CChams::RenderEnts(CBaseEntity* pLocal, IMatRenderContext* pRenderContext)
{
	if (!Vars::Chams::Main::Active.Value)
		return;

	std::vector<CBaseEntity*> Entities = g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL);
	
	for (const auto& Entity : g_EntityCache.GetGroup(EGroupType::BUILDINGS_ALL)) {
		Entities.push_back(Entity);
	}
	for (const auto& Entity : g_EntityCache.GetGroup(EGroupType::WORLD_HEALTH)) {
		Entities.push_back(Entity);
	}
	for (const auto& Entity : g_EntityCache.GetGroup(EGroupType::WORLD_AMMO)) {
		Entities.push_back(Entity);
	}
	for (const auto& Entity : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES)) {
		Entities.push_back(Entity);
	}

	if (Entities.empty())
		return;

	for (const auto& Entity : Entities)
	{
		const bool isPlayer = Entity->IsPlayer();
		if (isPlayer && (!Entity->IsAlive() || Entity->IsAGhost()))
			continue;

		if (!Utils::IsOnScreen(pLocal, Entity))
			continue;

		DrawModel(Entity);

		if (isPlayer) {
			if (Vars::Chams::Players::Wearables.Value)
			{
				CBaseEntity* pAttachment = Entity->FirstMoveChild();

				for (int n = 0; n < 32; n++)
				{
					if (!pAttachment)
						continue;

					if (pAttachment->IsWearable())
						DrawModel(pAttachment);

					pAttachment = pAttachment->NextMovePeer();
				}
			}

			if (Vars::Chams::Players::Weapons.Value)
			{
				if (const auto& pWeapon = Entity->GetActiveWeapon())
					DrawModel(pWeapon);
			}
		}

		I::ModelRender->ForcedMaterialOverride(nullptr);
		I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);

		I::RenderView->SetBlend(1.0f);

		pRenderContext->DepthRange(0.0f, 1.0f);
	}
}