#include "Chams.h"
#include "../Vars.h"

bool CChams::ShouldRun()
{
	return !g_Interfaces.EngineVGui->IsGameUIVisible();
}

void CChams::DrawModel(CBaseEntity* pEntity)
{
	m_bRendering = true;
	pEntity->DrawModel(STUDIO_RENDER);
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
		m_pMatFresnel = g_Interfaces.MatSystem->Create("m_pMatFresnel", kv);
	}

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
		m_pMatBrick = g_Interfaces.MatSystem->Create("m_pMatBrick", kv);
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
		{
			kv->SetString("$basetexture", "models/player/shared/ice_player");
			kv->SetString("$bumpmap", "models/player/shared/shared_normal");
			kv->SetString("$phong", "1");
			kv->SetString("$phongexponent", "10");
			kv->SetString("$phongboost", "1");
			kv->SetString("$phongfresnelranges", "[0 0 0]");
			kv->SetString("$basemapalphaphongmask", "1");
			kv->SetString("$phongwarptexture", "models/player/shared/ice_player_warp");
			m_pMatPlastic = g_Interfaces.MatSystem->Create("m_pMatPlastic", kv);
		}
	}

	m_pMatBlur = g_Interfaces.MatSystem->Find("models/effects/muzzleflash/blurmuzzle", "Model textures");
}

void CChams::Render()
{
	if (!m_DrawnEntities.empty())
		m_DrawnEntities.clear();

	m_bHasSetStencil = false;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!ShouldRun())
			return;

		if (const auto& pRenderContext = g_Interfaces.MatSystem->GetRenderContext())
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

			RenderPlayers(pLocal, pRenderContext);
			RenderBuildings(pLocal, pRenderContext);
			RenderWorld(pLocal, pRenderContext);
		}
	}
}

Chams_t fetchChams(CBaseEntity* pEntity) {
	PlayerInfo_t info{}; g_Interfaces.Engine->GetPlayerInfo(pEntity->GetIndex(), &info);

	if (pEntity->GetIndex() == g_GlobalInfo.m_nCurrentTargetIdx)
		return Vars::Chams::Players::Target;
	if (pEntity == g_EntityCache.m_pLocal)
		return Vars::Chams::Players::Local;
	if (g_EntityCache.Friends[pEntity->GetIndex()] || pEntity == g_EntityCache.m_pLocal)
		return Vars::Chams::Players::Friend;
	if (pEntity->GetTeamNum() != g_EntityCache.m_pLocal->GetTeamNum())
		return Vars::Chams::Players::Enemy;
	if (pEntity->GetTeamNum() == g_EntityCache.m_pLocal->GetTeamNum())
		return Vars::Chams::Players::Team;
}

IMaterial* CChams::fetchMaterial(Chams_t chams)
{
	switch (chams.drawMaterial)
	{
	case 1:
	{
		return m_pMatShaded;
	}
	case 2:
	{
		return m_pMatShiny;
	}
	case 3:
	{
		return m_pMatFlat;
	}
	case 4:
	{
		return m_pMatBrick;
	}
	case 5:
	{
		return m_pMatBlur;
	}
	case 6:
	{
		return m_pMatFresnel;
	}
	case 7:
	{
		return m_pMatPlastic;
	}
	default: return nullptr;
	}
}

void CChams::RenderPlayers(CBaseEntity* pLocal, IMatRenderContext* pRenderContext)
{
	if (!Vars::Chams::Players::Active.m_Var || !Vars::Chams::Main::Active.m_Var)
		return;

	const auto& Players = g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL);

	if (Players.empty())
		return;

	bool foundselfillumtint = false;
	bool foundenvmaptint = false;
	IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar(_("$selfillumtint"), &foundselfillumtint);
	IMaterialVar* fresnelEnvmaptint = m_pMatFresnel->FindVar(_("$envmaptint"), &foundenvmaptint);


	for (const auto& Player : Players)
	{
		if (!Player->IsAlive() || Player->IsAGhost())
			continue;
		g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
		auto chams = fetchChams(Player);
		auto chamsMaterial = fetchMaterial(chams);
		bool bIsLocal = Player->GetIndex() == g_Interfaces.Engine->GetLocalPlayer();

		//skip if disabled or null material
		if (!chams.chamsActive || !chams.drawMaterial) 
			continue;

		if (chams.showObstructed)
			pRenderContext->DepthRange(0.0f, 0.2f);

		g_Interfaces.ModelRender->ForcedMaterialOverride(chamsMaterial);

		if (!Utils::IsOnScreen(pLocal, Player))
			continue;

		Color_t DrawColor = Utils::GetEntityDrawColor(Player, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
			

		g_Interfaces.RenderView->SetBlend(Color::TOFLOAT(DrawColor.a));
		if (chams.drawMaterial != 6)
		{
			g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(DrawColor.r), Color::TOFLOAT(DrawColor.g),
				Color::TOFLOAT(DrawColor.b));
		}
		else if (chams.drawMaterial == 6)
		{
			if (foundselfillumtint)
			{
				fresnelSelfillumtint->SetVecValue(Color::TOFLOAT(Colors::FresnelBase.r),
					Color::TOFLOAT(Colors::FresnelBase.g),
					Color::TOFLOAT(Colors::FresnelBase.b));
			}
			if (foundenvmaptint)
			{
				if (bIsLocal && Vars::Glow::Players::LocalRainbow.m_Var)
				{
					fresnelEnvmaptint->SetVecValue(Color::TOFLOAT(Utils::Rainbow().r),
						Color::TOFLOAT(Utils::Rainbow().g),
						Color::TOFLOAT(Utils::Rainbow().b));
				}
				else
				{
					fresnelEnvmaptint->SetVecValue(Color::TOFLOAT(DrawColor.r), Color::TOFLOAT(DrawColor.g),
						Color::TOFLOAT(DrawColor.b));
				}
			}
		}

		DrawModel(Player);

		if (Vars::Chams::Players::Wearables.m_Var)
		{
			CBaseEntity* pAttachment = Player->FirstMoveChild();

			for (int n = 0; n < 32; n++)
			{
				if (!pAttachment)
					continue;

				if (pAttachment->IsWearable())
					DrawModel(pAttachment);

				pAttachment = pAttachment->NextMovePeer();
			}
		}

		if (Vars::Chams::Players::Weapons.m_Var)
		{
			if (const auto& pWeapon = Player->GetActiveWeapon())
				DrawModel(pWeapon);
		}

		g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
		g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);

		g_Interfaces.RenderView->SetBlend(1.0f);

		pRenderContext->DepthRange(0.0f, 1.0f);
	}
}

void CChams::RenderBuildings(CBaseEntity* pLocal, IMatRenderContext* pRenderContext)
{
	if (!Vars::Chams::Buildings::Active.m_Var || !Vars::Chams::Main::Active.m_Var)
		return;

	const auto& Buildings = g_EntityCache.GetGroup(EGroupType::BUILDINGS_ALL);

	if (Buildings.empty())
		return;

	bool bMatWasForced = false;

	if (Vars::Chams::Buildings::Material.m_Var)
	{
		g_Interfaces.ModelRender->ForcedMaterialOverride([&]() -> IMaterial*
		{
				bMatWasForced = true;
				switch (Vars::Chams::Buildings::Material.m_Var)
				{
				case 1:
				{
					return m_pMatShaded;
				}
				case 2:
				{
					return m_pMatShiny;
				}
				case 3:
				{
					return m_pMatFlat;
				}
				case 4:
				{
					return m_pMatBrick;
				}
				case 5:
				{
					return m_pMatBlur;
				}
				case 6:
				{
					return m_pMatFresnel;
				}
				case 7:
				{
					return m_pMatPlastic;
				}
				default: bMatWasForced = false; return nullptr;
				}
		}());
	}

	g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);

	if (Vars::Chams::Buildings::IgnoreZ.m_Var)
		pRenderContext->DepthRange(0.0f, 0.2f);

	for (const auto& Building : Buildings)
	{
		if (!Building->IsAlive())
			continue;

		if (Vars::Chams::Buildings::IgnoreTeammates.m_Var && Building->GetTeamNum() == pLocal->GetTeamNum())
			continue;

		if (!Utils::IsOnScreen(pLocal, Building))
			continue;

		if (bMatWasForced)
		{
			Color_t DrawColor = Utils::GetEntityDrawColor(Building, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
			bool foundselfillumtint = false;
			bool foundenvmaptint = false;
			g_Interfaces.RenderView->SetBlend(Color::TOFLOAT(DrawColor.a));
			if (Vars::Chams::Buildings::Material.m_Var != 6)
			{
				g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(DrawColor.r), Color::TOFLOAT(DrawColor.g),
					Color::TOFLOAT(DrawColor.b));
			}
			else if (Vars::Chams::Players::Material.m_Var == 6)
			{
				IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar(_("$selfillumtint"), &foundselfillumtint);
				if (foundselfillumtint)
				{
					fresnelSelfillumtint->SetVecValue(Color::TOFLOAT(Colors::FresnelBase.r),
						Color::TOFLOAT(Colors::FresnelBase.g),
						Color::TOFLOAT(Colors::FresnelBase.b));
				}
				IMaterialVar* fresnelEnvmaptint = m_pMatFresnel->FindVar(_("$envmaptint"), &foundenvmaptint);
				if (foundenvmaptint)
				{
					fresnelEnvmaptint->SetVecValue(Color::TOFLOAT(DrawColor.r) /* * 4 */,
						Color::TOFLOAT(DrawColor.g) /* * 4 */,
						Color::TOFLOAT(DrawColor.b) /* * 4 */);
				}
			}
		}

		DrawModel(Building);
	}

	if (bMatWasForced)
	{
		g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
		g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
	}

	g_Interfaces.RenderView->SetBlend(1.0f);

	if (Vars::Chams::Buildings::IgnoreZ.m_Var)
		pRenderContext->DepthRange(0.0f, 1.0f);
}

void CChams::RenderWorld(CBaseEntity* pLocal, IMatRenderContext* pRenderContext)
{
	if (!Vars::Chams::World::Active.m_Var || !Vars::Chams::Main::Active.m_Var)
		return;

	bool bMatWasForced = false;

	if (Vars::Chams::World::Material.m_Var)
	{
		g_Interfaces.ModelRender->ForcedMaterialOverride([&]() -> IMaterial*
			{
				bMatWasForced = true;
				switch (Vars::Chams::World::Material.m_Var)
				{
				case 1:
				{
					return m_pMatShaded;
				}
				case 2:
				{
					return m_pMatShiny;
				}
				case 3:
				{
					return m_pMatFlat;
				}
				case 4:
				{
					return m_pMatBrick;
				}
				case 5:
				{
					return m_pMatBlur;
				}
				case 6:
				{
					return m_pMatFresnel;
				}
				case 7:
				{
					return m_pMatPlastic;
				}
				default: bMatWasForced = false; return nullptr;
				}
			}());
	}

	g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);

	if (Vars::Chams::World::IgnoreZ.m_Var)
		pRenderContext->DepthRange(0.0f, 0.2f);

	if (Vars::Chams::World::Health.m_Var)
	{
		for (const auto& Health : g_EntityCache.GetGroup(EGroupType::WORLD_HEALTH))
		{
			if (!Utils::IsOnScreen(pLocal, Health))
				continue;

			if (bMatWasForced)
			{
				Color_t DrawColor = Colors::Health;
				bool foundselfillumtint = false;
				bool foundenvmaptint = false;
				g_Interfaces.RenderView->SetBlend(Color::TOFLOAT(DrawColor.a));
				if (Vars::Chams::World::Material.m_Var != 6)
				{
					g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(DrawColor.r),
						Color::TOFLOAT(DrawColor.g),
						Color::TOFLOAT(DrawColor.b));
				}
				else if (Vars::Chams::World::Material.m_Var == 6)
				{
					IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar(
						_("$selfillumtint"), &foundselfillumtint);
					if (foundselfillumtint)
					{
						fresnelSelfillumtint->SetVecValue(Color::TOFLOAT(Colors::FresnelBase.r),
							Color::TOFLOAT(Colors::FresnelBase.g),
							Color::TOFLOAT(Colors::FresnelBase.b));
					}
					IMaterialVar* fresnelEnvmaptint = m_pMatFresnel->FindVar(_("$envmaptint"), &foundenvmaptint);
					if (foundenvmaptint)
					{
						fresnelEnvmaptint->SetVecValue(Color::TOFLOAT(DrawColor.r), Color::TOFLOAT(DrawColor.g),
							Color::TOFLOAT(DrawColor.b));
					}
				}
			}

			DrawModel(Health);
		}
	}

	if (Vars::Chams::World::Ammo.m_Var)
	{
		for (const auto& Ammo : g_EntityCache.GetGroup(EGroupType::WORLD_AMMO))
		{
			if (!Utils::IsOnScreen(pLocal, Ammo))
				continue;

			if (bMatWasForced)
			{
				Color_t DrawColor = Colors::Ammo;
				g_Interfaces.RenderView->SetBlend(Color::TOFLOAT(DrawColor.a));
				if (Vars::Chams::World::Material.m_Var != 6)
				{
					g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(DrawColor.r),
						Color::TOFLOAT(DrawColor.g),
						Color::TOFLOAT(DrawColor.b));
				}
				else
				{
					g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				}
				bool foundselfillumtint = false;
				bool foundenvmaptint = false;
				if (Vars::Chams::World::Material.m_Var == 6)
				{
					IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar(
						_("$selfillumtint"), &foundselfillumtint);
					if (foundselfillumtint)
					{
						fresnelSelfillumtint->SetVecValue(Color::TOFLOAT(Colors::FresnelBase.r),
							Color::TOFLOAT(Colors::FresnelBase.g),
							Color::TOFLOAT(Colors::FresnelBase.b));
					}
					IMaterialVar* fresnelEnvmaptint = m_pMatFresnel->FindVar(_("$envmaptint"), &foundenvmaptint);
					if (foundenvmaptint)
					{
						fresnelEnvmaptint->SetVecValue(Color::TOFLOAT(DrawColor.r), Color::TOFLOAT(DrawColor.g),
							Color::TOFLOAT(DrawColor.b));
					}
				}
			}

			DrawModel(Ammo);
		}
	}

	if (Vars::Chams::World::Projectiles.m_Var)
	{
		for (const auto& Projectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
		{
			if (*reinterpret_cast<byte*>(Projectile + 0x7C) & EF_NODRAW)
				continue;

			int nTeam = Projectile->GetTeamNum();

			if (Vars::Chams::World::Projectiles.m_Var == 2 && nTeam == pLocal->GetTeamNum())
				continue;

			if (!Utils::IsOnScreen(pLocal, Projectile))
				continue;

			if (bMatWasForced)
			{
				Color_t DrawColor = Utils::GetTeamColor(nTeam, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
				g_Interfaces.RenderView->SetBlend(Color::TOFLOAT(DrawColor.a));
				if (Vars::Chams::World::Material.m_Var != 6)
				{
					g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(DrawColor.r),
						Color::TOFLOAT(DrawColor.g),
						Color::TOFLOAT(DrawColor.b));
				}
				else
				{
					g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				}
				bool foundselfillumtint = false;
				bool foundenvmaptint = false;
				if (Vars::Chams::World::Material.m_Var == 6)
				{
					IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar(
						_("$selfillumtint"), &foundselfillumtint);
					if (foundselfillumtint)
					{
						fresnelSelfillumtint->SetVecValue(Color::TOFLOAT(Colors::FresnelBase.r),
							Color::TOFLOAT(Colors::FresnelBase.g),
							Color::TOFLOAT(Colors::FresnelBase.b));
					}
					IMaterialVar* fresnelEnvmaptint = m_pMatFresnel->FindVar(_("$envmaptint"), &foundenvmaptint);
					if (foundenvmaptint)
					{
						fresnelEnvmaptint->SetVecValue(Color::TOFLOAT(DrawColor.r), Color::TOFLOAT(DrawColor.g),
							Color::TOFLOAT(DrawColor.b));
					}
				}
			}

			DrawModel(Projectile);
		}
	}

	if (bMatWasForced)
	{
		g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
		g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
	}

	g_Interfaces.RenderView->SetBlend(1.0f);

	if (Vars::Chams::World::IgnoreZ.m_Var)
		pRenderContext->DepthRange(0.0f, 1.0f);
}
