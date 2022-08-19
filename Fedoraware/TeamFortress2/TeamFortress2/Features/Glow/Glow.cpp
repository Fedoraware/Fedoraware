#include "Glow.h"
#include "../Vars.h"
#include "../Chams/Chams.h"
#include "../Chams/DMEChams.h"
#include "../Visuals/Visuals.h"

void CGlowEffect::DrawModel(CBaseEntity* pEntity, int nFlags, bool bIsDrawingModels)
{
	m_bRendering = true;

	if (!bIsDrawingModels)
		m_bDrawingGlow = true;

	pEntity->DrawModel(nFlags);

	if (bIsDrawingModels)
		m_DrawnEntities[pEntity] = true;

	if (!bIsDrawingModels)
		m_bDrawingGlow = false;

	m_bRendering = false;
}

void CGlowEffect::SetScale(int nScale)
{
	static IMaterialVar* pVar = nullptr;
	static bool bFound = false;


	if (!bFound && m_pMatBlurY)
	{
		pVar = m_pMatBlurY->FindVar("$bloomamount", &bFound);
	}
	else if (pVar)
	{
		pVar->SetIntValue(nScale);
	}
}

void CGlowEffect::Init()
{
	m_pMatGlowColor = I::MaterialSystem->Find("dev/glow_color", TEXTURE_GROUP_OTHER);
	m_pRtFullFrame = I::MaterialSystem->FindTexture("_rt_FullFrameFB", TEXTURE_GROUP_RENDER_TARGET);
	m_pRtQuarterSize1 = I::MaterialSystem->FindTexture("_rt_SmallFB1", TEXTURE_GROUP_RENDER_TARGET);

	F::DMEChams.v_MatListGlobal.push_back(m_pMatGlowColor);

	m_pMatGlowColor->IncrementReferenceCount();
	m_pRtFullFrame->IncrementReferenceCount();
	m_pRtQuarterSize1->IncrementReferenceCount();

	m_pRenderBuffer1 = I::MaterialSystem->CreateNamedRenderTargetTextureEx(
		"glow_buffer_1", m_pRtFullFrame->GetActualWidth(), m_pRtFullFrame->GetActualHeight(),
		RT_SIZE_LITERAL, IMAGE_FORMAT_RGB888, MATERIAL_RT_DEPTH_SHARED,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA, CREATERENDERTARGETFLAGS_HDR);
	m_pRenderBuffer1->IncrementReferenceCount();
	m_pRenderBuffer2 = I::MaterialSystem->CreateNamedRenderTargetTextureEx(
		"glow_buffer_2", m_pRtFullFrame->GetActualWidth(), m_pRtFullFrame->GetActualHeight(),
		RT_SIZE_LITERAL, IMAGE_FORMAT_RGB888, MATERIAL_RT_DEPTH_SHARED,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA, CREATERENDERTARGETFLAGS_HDR);
	m_pRenderBuffer2->IncrementReferenceCount();

	static KeyValues* m_pMatBlurXkv = new KeyValues("BlurFilterX");
	static KeyValues* m_pMatBlurXwfkv = new KeyValues("BlurFilterX");
	static KeyValues* m_pMatBlurYkv = new KeyValues("BlurFilterY");
	static KeyValues* m_pMatBlurYwfkv = new KeyValues("BlurFilterY");
	static KeyValues* m_pMatHaloAddToScreenkv = new KeyValues("UnlitGeneric");

	m_pMatBlurXkv->SetString("$basetexture", "glow_buffer_1");
	m_pMatBlurXwfkv->SetString("$basetexture", "glow_buffer_1");
	m_pMatBlurXwfkv->SetString("$wireframe", "1");
	
	m_pMatBlurYkv->SetString("$basetexture", "glow_buffer_2");
	m_pMatBlurYwfkv->SetString("$basetexture", "glow_buffer_2");
	m_pMatBlurYwfkv->SetString("$wireframe", "1");


	m_pMatHaloAddToScreenkv->SetString("$basetexture", "glow_buffer_1");
	m_pMatHaloAddToScreenkv->SetString("$additive", "1");

	m_pMatBlurX = F::DMEChams.CreateNRef("m_pMatBlurX", m_pMatBlurXkv);
	m_pMatBlurXwf = F::DMEChams.CreateNRef("m_pMatBlurXwf", m_pMatBlurXwfkv);
	m_pMatBlurY = F::DMEChams.CreateNRef("m_pMatBlurY", m_pMatBlurYkv);
	m_pMatBlurYwf = F::DMEChams.CreateNRef("m_pMatBlurYwf", m_pMatBlurYwfkv);
	m_pMatHaloAddToScreen = F::DMEChams.CreateNRef("m_pMatHaloAddToScreen", m_pMatHaloAddToScreenkv);
}

void CGlowEffect::Render()
{
	if (!m_vecGlowEntities.empty())
		m_vecGlowEntities.clear();

	if (!m_DrawnEntities.empty())
		m_DrawnEntities.clear();

	if (!Vars::Glow::Main::Active.Value)
	{
		return;
	}
	
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		int w = g_ScreenSize.w;
		int h = g_ScreenSize.h;

		if (/*!Vars::Glow::Main::Active.m_Var ||*/ w < 1 || h < 1 || w > 4096 || h > 2160)
			return;

		if (I::EngineVGui->IsGameUIVisible())
			return;

		IMatRenderContext* pRenderContext = I::MaterialSystem->GetRenderContext();

		if (!pRenderContext)
			return;

		SetScale(Vars::Glow::Main::Scale.Value);

		ShaderStencilState_t StencilStateDisable = {};
		StencilStateDisable.m_bEnable = false;

		float flOriginalColor[3] = {};
		I::RenderView->GetColorModulation(flOriginalColor);
		float flOriginalBlend = I::RenderView->GetBlend();

		if (!F::Chams.m_bHasSetStencil)
		{
			ShaderStencilState_t StencilState = {};
			StencilState.m_bEnable = true;
			StencilState.m_nReferenceValue = 1;
			StencilState.m_CompareFunc = STENCILCOMPARISONFUNCTION_ALWAYS;
			StencilState.m_PassOp = STENCILOPERATION_REPLACE;
			StencilState.m_FailOp = STENCILOPERATION_KEEP;
			StencilState.m_ZFailOp = STENCILOPERATION_REPLACE;
			StencilState.SetStencilState(pRenderContext);
		}

		I::RenderView->SetBlend(1.0f);
		I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);



		if (Vars::Glow::Players::Active.Value)
		{
			for (const auto& Player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
			{
				if (Player->GetDormant())
					continue;

				if (!Player->IsAlive() || Player->IsAGhost())
					continue;

				bool bIsLocal = Player->GetIndex() == I::EngineClient->GetLocalPlayer();

				if (!bIsLocal)
				{
					switch (Vars::Glow::Players::IgnoreTeammates.Value)
					{
					case 0: break;
					case 1:
						{
							if (Player->GetTeamNum() == pLocal->GetTeamNum()) { continue; }
							break;
						}
					case 2:
						{
							if (Player->GetTeamNum() == pLocal->GetTeamNum() && !g_EntityCache.IsFriend(Player->GetIndex())) { continue; }
							break;
						}
					}
				}

				else
				{
					if (!Vars::Glow::Players::ShowLocal.Value)
						continue;
				}

				if (!Utils::IsOnScreen(pLocal, Player))
					continue;

				Color_t DrawColor = {};

				if (Vars::Glow::Players::Color.Value == 0)
				{
					if (Vars::Glow::Players::LocalRainbow.Value)
					{
						if (bIsLocal)
						{
							DrawColor = Utils::Rainbow();
						}
						else
						{
							DrawColor = Utils::GetEntityDrawColor(Player, Vars::ESP::Main::EnableTeamEnemyColors.Value);
						}
					}
					else
					{
						DrawColor = Utils::GetEntityDrawColor(Player, Vars::ESP::Main::EnableTeamEnemyColors.Value);
					}
				}
				else
				{
					DrawColor = Utils::GetHealthColor(Player->GetHealth(), Player->GetMaxHealth());
				}

				m_vecGlowEntities.push_back({Player, DrawColor, Vars::Glow::Players::Alpha.Value});

				if (!F::Chams.HasDrawn(Player))
					DrawModel(Player, STUDIO_RENDER, true);

				if (Vars::Glow::Players::Wearables.Value && !Player->IsUbered())
				{
					CBaseEntity* pAttachment = Player->FirstMoveChild();

					for (int n = 0; n < 32; n++)
					{
						if (!pAttachment)
							continue;

						if (pAttachment->IsWearable())
						{
							m_vecGlowEntities.push_back({pAttachment, DrawColor, Vars::Glow::Players::Alpha.Value});

							if (!F::Chams.HasDrawn(pAttachment))
								DrawModel(pAttachment, STUDIO_RENDER, true);
						}

						pAttachment = pAttachment->NextMovePeer();
					}
				}

				if (Vars::Glow::Players::Weapons.Value && !Player->IsUbered())
				{
					if (const auto& pWeapon = Player->GetActiveWeapon())
					{
						m_vecGlowEntities.push_back({pWeapon, DrawColor, Vars::Glow::Players::Alpha.Value});

						if (!F::Chams.HasDrawn(pWeapon))
							DrawModel(pWeapon, STUDIO_RENDER, true);
					}
				}
			}
		}

		if (Vars::Glow::Misc::MovementSimLine.Value)
		{
			F::Visuals.DrawMovesimLine();
		}
		if (Vars::Glow::Misc::Sightlines.Value)
		{
			F::Visuals.DrawSightlines();
		}
		if (Vars::Glow::Misc::BulletTracers.Value)
		{
			F::Visuals.DrawBulletTracers();
		}
		if (Vars::Glow::Buildings::Active.Value)
		{
			for (const auto& Building : g_EntityCache.GetGroup(EGroupType::BUILDINGS_ALL))
			{
				const auto& pBuilding = reinterpret_cast<CBaseObject*>(Building);
				if (!Building->IsAlive())
					continue;

				if (Vars::Glow::Buildings::IgnoreTeammates.Value && Building->GetTeamNum() == pLocal->GetTeamNum())
					continue;

				if (!Utils::IsOnScreen(pLocal, Building))
					continue;

				Color_t DrawColor = {};

				switch (Vars::Glow::Buildings::Color.Value) {
				case 0: {
					DrawColor = Utils::GetEntityDrawColor(Building, Vars::ESP::Main::EnableTeamEnemyColors.Value);
					break;
				}
				case 1: {
					DrawColor = Utils::GetHealthColor(pBuilding->GetHealth(), pBuilding->GetMaxHealth());
					break;
				}
				}

				m_vecGlowEntities.push_back({Building, DrawColor, Vars::Glow::Buildings::Alpha.Value});

				if (!F::Chams.HasDrawn(Building))
					DrawModel(Building, STUDIO_RENDER, true);
			}
		}

		if (Vars::Glow::World::Active.Value)
		{
			if (Vars::Glow::World::Health.Value)
			{
				for (const auto& Health : g_EntityCache.GetGroup(EGroupType::WORLD_HEALTH))
				{
					if (!Utils::IsOnScreen(pLocal, Health))
						continue;

					m_vecGlowEntities.push_back({Health, Colors::Health, Vars::Glow::World::Alpha.Value});

					if (!F::Chams.HasDrawn(Health))
						DrawModel(Health, STUDIO_RENDER, true);
				}
			}

			if (Vars::Glow::World::Ammo.Value)
			{
				for (const auto& Ammo : g_EntityCache.GetGroup(EGroupType::WORLD_AMMO))
				{
					if (!Utils::IsOnScreen(pLocal, Ammo))
						continue;

					m_vecGlowEntities.push_back({Ammo, Colors::Ammo, Vars::Glow::World::Alpha.Value});

					if (!F::Chams.HasDrawn(Ammo))
						DrawModel(Ammo, STUDIO_RENDER, true);
				}
			}

			if (Vars::Glow::World::Projectiles.Value)
			{
				for (const auto& Projectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
				{
					if (*reinterpret_cast<byte*>(Projectile + 0x7C) & EF_NODRAW)
						continue;

					int nTeam = Projectile->GetTeamNum();

					if (Vars::Glow::World::Projectiles.Value == 2 && nTeam == pLocal->GetTeamNum())
						continue;

					if (!Utils::IsOnScreen(pLocal, Projectile))
						continue;

					m_vecGlowEntities.push_back({
						Projectile, Utils::GetTeamColor(nTeam, Vars::ESP::Main::EnableTeamEnemyColors.Value),
						Vars::Glow::World::Alpha.Value
					});

					if (!F::Chams.HasDrawn(Projectile))
						DrawModel(Projectile, STUDIO_RENDER, true);
				}
			}

			if (Vars::Glow::World::NPCs.Value)
			{
				for (const auto& NPC : g_EntityCache.GetGroup(EGroupType::WORLD_NPC))
				{
					if (!Utils::IsOnScreen(pLocal, NPC))
						continue;

					m_vecGlowEntities.push_back({ NPC, Utils::GetEntityDrawColor(NPC, false), Vars::Glow::World::Alpha.Value});

					if (!F::Chams.HasDrawn(NPC))
						DrawModel(NPC, STUDIO_RENDER, true);
				}
			}

			if (Vars::Glow::World::Bombs.Value)
			{
				for (const auto& Bombs : g_EntityCache.GetGroup(EGroupType::WORLD_BOMBS))
				{
					if (!Utils::IsOnScreen(pLocal, Bombs))
						continue;

					m_vecGlowEntities.push_back({ Bombs, Utils::GetEntityDrawColor(Bombs, false), Vars::Glow::World::Alpha.Value });

					if (!F::Chams.HasDrawn(Bombs))
						DrawModel(Bombs, STUDIO_RENDER, true);
				}
			}
		}

		StencilStateDisable.SetStencilState(pRenderContext);

		if (m_vecGlowEntities.empty() && F::Visuals.m_SightLines.empty() && F::Visuals.m_vecBulletTracers.empty() && G::PredictionLines.empty())
			return;

		I::ModelRender->ForcedMaterialOverride(m_pMatGlowColor);

		pRenderContext->PushRenderTargetAndViewport();
		{
			pRenderContext->SetRenderTarget(m_pRenderBuffer1);
			pRenderContext->Viewport(0, 0, w, h);
			pRenderContext->ClearColor4ub(0, 0, 0, 0);
			pRenderContext->ClearBuffers(true, false, false);

			for (const auto& GlowEntity : m_vecGlowEntities)
			{
				I::RenderView->SetBlend(GlowEntity.m_flAlpha);
				I::RenderView->SetColorModulation(Color::TOFLOAT(GlowEntity.m_Color.r),
				                                            Color::TOFLOAT(GlowEntity.m_Color.g),
				                                            Color::TOFLOAT(GlowEntity.m_Color.b));
				DrawModel(GlowEntity.m_pEntity, STUDIO_RENDER | STUDIO_NOSHADOWS, false);
			}

			if (Vars::Glow::Misc::MovementSimLine.Value)
			{
				F::Visuals.DrawMovesimLine();
			}
			if (Vars::Glow::Misc::Sightlines.Value)
			{
				F::Visuals.DrawSightlines();
			}
			if (Vars::Glow::Misc::BulletTracers.Value)
			{
				F::Visuals.DrawBulletTracers();
			}

			StencilStateDisable.SetStencilState(pRenderContext);
		}
		pRenderContext->PopRenderTargetAndViewport();

		if (Vars::Glow::Main::Type.Value == 0) {
			pRenderContext->PushRenderTargetAndViewport();
			{
				pRenderContext->Viewport(0, 0, w, h);

				pRenderContext->SetRenderTarget(m_pRenderBuffer2);
				pRenderContext->DrawScreenSpaceRectangle(m_pMatBlurX, 0,
					0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
				//pRenderContext->DrawScreenSpaceRectangle(m_pMatBlurX, 0, 0, w, h, 0.0f, 0.0f, w, h, w, h);

				pRenderContext->SetRenderTarget(m_pRenderBuffer1);
				pRenderContext->DrawScreenSpaceRectangle(m_pMatBlurY, 0,
					0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
				//pRenderContext->DrawScreenSpaceRectangle(m_pMatBlurY, 0, 0, w, h, 0.0f, 0.0f, w, h, w, h);
			}
			pRenderContext->PopRenderTargetAndViewport();
		}

		ShaderStencilState_t StencilState = {};
		StencilState.m_bEnable = true;
		StencilState.m_nWriteMask = 0x0;
		StencilState.m_nTestMask = 0xFF;
		StencilState.m_nReferenceValue = 0;
		StencilState.m_CompareFunc = STENCILCOMPARISONFUNCTION_EQUAL;
		StencilState.m_PassOp = STENCILOPERATION_KEEP;
		StencilState.m_FailOp = STENCILOPERATION_KEEP;
		StencilState.m_ZFailOp = STENCILOPERATION_KEEP;
		StencilState.SetStencilState(pRenderContext);

		int nViewportX, nViewportY, nViewportWidth, nViewportHeight;
		pRenderContext->GetViewport( nViewportX, nViewportY, nViewportWidth, nViewportHeight );

		switch (Vars::Glow::Main::Type.Value){
		case 0:{
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, 0, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			break;
		}
		case 1:{
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, -1, -1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, -1, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, 0, -1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, 0, 1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, 1, 1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, 1, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, 1, -1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, -1, 1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			break;
		}
		case 2:{
			const float flScale = (Vars::Glow::Main::Scale.Value * 0.05f);
			pRenderContext->DrawScreenSpaceRectangle( m_pMatHaloAddToScreen, 0, 0, w, h, +flScale, +flScale, (float)nViewportWidth / 4 - 1, (float)nViewportHeight/ 4 - 1, m_pRtQuarterSize1->GetActualWidth(), m_pRtQuarterSize1->GetActualHeight() );
			pRenderContext->DrawScreenSpaceRectangle( m_pMatHaloAddToScreen, 0, 0, w, h, -flScale, -flScale, (float)nViewportWidth / 4 - 1, (float)nViewportHeight/ 4 - 1, m_pRtQuarterSize1->GetActualWidth(), m_pRtQuarterSize1->GetActualHeight() );
			break;
		}
		}
		StencilStateDisable.SetStencilState(pRenderContext);

		I::ModelRender->ForcedMaterialOverride(nullptr);
		I::RenderView->SetColorModulation(flOriginalColor);
		I::RenderView->SetBlend(flOriginalBlend);
	}
}