#include "Glow.h"
#include "../Vars.h"
#include "../Chams/Chams.h"

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


	if (!bFound)
	{
		pVar = m_pMatBlurY->FindVar(_("$bloomamount"), &bFound);
	}
	else if (pVar)
	{
		pVar->SetIntValue(nScale);
	}
}

void CGlowEffect::Init()
{
	m_pMatGlowColor = I::MatSystem->Find(_("dev/glow_color"), TEXTURE_GROUP_OTHER);
	m_pRtFullFrame = I::MatSystem->FindTexture(_("_rt_FullFrameFB"), TEXTURE_GROUP_RENDER_TARGET);

	m_pRenderBuffer1 = I::MatSystem->CreateNamedRenderTargetTextureEx(
		_("glow_buffer_1"), m_pRtFullFrame->GetActualWidth(), m_pRtFullFrame->GetActualHeight(),
		RT_SIZE_LITERAL, IMAGE_FORMAT_RGB888, MATERIAL_RT_DEPTH_SHARED,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA, CREATERENDERTARGETFLAGS_HDR);
	m_pRenderBuffer1->IncrementReferenceCount();
	// This can cause a crash on inject for some reason
	m_pRenderBuffer2 = I::MatSystem->CreateNamedRenderTargetTextureEx(
		_("glow_buffer_2"), m_pRtFullFrame->GetActualWidth(), m_pRtFullFrame->GetActualHeight(),
		RT_SIZE_LITERAL, IMAGE_FORMAT_RGB888, MATERIAL_RT_DEPTH_SHARED,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA, CREATERENDERTARGETFLAGS_HDR);
	m_pRenderBuffer2->IncrementReferenceCount();

	m_pMatBlurX = Utils::CreateMaterial
	({
		_("\"BlurFilterX\"\
		\n{\
		\n\t\"$basetexture\" \"glow_buffer_1\"\
		\n}\n")
	});

	m_pMatBlurXwf = Utils::CreateMaterial
	({
		_("\"BlurFilterX\"\
		\n{\
		\n\t\"$basetexture\" \"glow_buffer_1\"\
		\n\t\"$wireframe\" \"1\"\
		\n}\n")
	});

	m_pMatBlurY = Utils::CreateMaterial
	({
		_("\"BlurFilterY\"\
		\n{\
		\n\t\"$basetexture\" \"glow_buffer_2\"\
		\n}\n")
	});

	m_pMatBlurYwf = Utils::CreateMaterial
	({
		_("\"BlurFilterY\"\
		\n{\
		\n\t\"$basetexture\" \"glow_buffer_2\"\
		\n\t\"$wireframe\" \"1\"\
		\n}\n")
	});


	m_pMatHaloAddToScreen = Utils::CreateMaterial
	({
		_("\"UnlitGeneric\"\
		\n{\
		\n\t\"$basetexture\" \"glow_buffer_1\"\
		\n\t\"$additive\" \"1\"\
		\n}\n")
	});
}

void CGlowEffect::Render()
{
	if (!m_vecGlowEntities.empty())
		m_vecGlowEntities.clear();

	if (!m_DrawnEntities.empty())
		m_DrawnEntities.clear();

	if (!Vars::Glow::Main::Active.m_Var)
		return;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		int w = g_ScreenSize.w;
		int h = g_ScreenSize.h;

		if (/*!Vars::Glow::Main::Active.m_Var ||*/ w < 1 || h < 1 || w > 4096 || h > 2160)
			return;

		if (I::EngineVGui->IsGameUIVisible())
			return;

		IMatRenderContext* pRenderContext = I::MatSystem->GetRenderContext();

		if (!pRenderContext)
			return;

		SetScale(Vars::Glow::Main::Scale.m_Var);

		ShaderStencilState_t StencilStateDisable = {};
		StencilStateDisable.m_bEnable = false;

		float flOriginalColor[3] = {};
		I::RenderView->GetColorModulation(flOriginalColor);
		float flOriginalBlend = I::RenderView->GetBlend();

		if (!g_Chams.m_bHasSetStencil)
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

		if (Vars::Glow::Players::Active.m_Var)
		{
			for (const auto& Player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
			{
				if (!Player->IsAlive() || Player->IsAGhost())
					continue;

				bool bIsLocal = Player->GetIndex() == I::Engine->GetLocalPlayer();

				if (!bIsLocal)
				{
					switch (Vars::Glow::Players::IgnoreTeammates.m_Var)
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
					if (!Vars::Glow::Players::ShowLocal.m_Var)
						continue;
				}

				if (!Utils::IsOnScreen(pLocal, Player))
					continue;

				Color_t DrawColor = {};

				if (Vars::Glow::Players::Color.m_Var == 0)
				{
					if (Vars::Glow::Players::LocalRainbow.m_Var)
					{
						if (bIsLocal)
						{
							DrawColor = Utils::Rainbow();
						}
						else
						{
							DrawColor = Utils::GetEntityDrawColor(Player, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
						}
					}
					else
					{
						DrawColor = Utils::GetEntityDrawColor(Player, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);
					}
				}
				else
				{
					DrawColor = Utils::GetHealthColor(Player->GetHealth(), Player->GetMaxHealth());
				}

				m_vecGlowEntities.push_back({Player, DrawColor, Vars::Glow::Players::Alpha.m_Var});

				if (!g_Chams.HasDrawn(Player))
					DrawModel(Player, STUDIO_RENDER, true);

				if (Vars::Glow::Players::Wearables.m_Var)
				{
					CBaseEntity* pAttachment = Player->FirstMoveChild();

					for (int n = 0; n < 32; n++)
					{
						if (!pAttachment)
							continue;

						if (pAttachment->IsWearable())
						{
							m_vecGlowEntities.push_back({pAttachment, DrawColor, Vars::Glow::Players::Alpha.m_Var});

							if (!g_Chams.HasDrawn(pAttachment))
								DrawModel(pAttachment, STUDIO_RENDER, true);
						}

						pAttachment = pAttachment->NextMovePeer();
					}
				}

				if (Vars::Glow::Players::Weapons.m_Var)
				{
					if (const auto& pWeapon = Player->GetActiveWeapon())
					{
						m_vecGlowEntities.push_back({pWeapon, DrawColor, Vars::Glow::Players::Alpha.m_Var});

						if (!g_Chams.HasDrawn(pWeapon))
							DrawModel(pWeapon, STUDIO_RENDER, true);
					}
				}
			}
		}

		if (Vars::Glow::Buildings::Active.m_Var)
		{
			for (const auto& Building : g_EntityCache.GetGroup(EGroupType::BUILDINGS_ALL))
			{
				if (!Building->IsAlive())
					continue;

				if (Vars::Glow::Buildings::IgnoreTeammates.m_Var && Building->GetTeamNum() == pLocal->GetTeamNum())
					continue;

				if (!Utils::IsOnScreen(pLocal, Building))
					continue;

				Color_t DrawColor = {};

				if (Vars::Glow::Buildings::Color.m_Var == 0)
					DrawColor = Utils::GetEntityDrawColor(Building, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);

				else DrawColor = Utils::GetHealthColor(Building->GetHealth(), Building->GetMaxHealth());

				m_vecGlowEntities.push_back({Building, DrawColor, Vars::Glow::Buildings::Alpha.m_Var});

				if (!g_Chams.HasDrawn(Building))
					DrawModel(Building, STUDIO_RENDER, true);
			}
		}

		if (Vars::Glow::World::Active.m_Var)
		{
			if (Vars::Glow::World::Health.m_Var)
			{
				for (const auto& Health : g_EntityCache.GetGroup(EGroupType::WORLD_HEALTH))
				{
					if (!Utils::IsOnScreen(pLocal, Health))
						continue;

					m_vecGlowEntities.push_back({Health, Colors::Health, Vars::Glow::World::Alpha.m_Var});

					if (!g_Chams.HasDrawn(Health))
						DrawModel(Health, STUDIO_RENDER, true);
				}
			}

			if (Vars::Glow::World::Ammo.m_Var)
			{
				for (const auto& Ammo : g_EntityCache.GetGroup(EGroupType::WORLD_AMMO))
				{
					if (!Utils::IsOnScreen(pLocal, Ammo))
						continue;

					m_vecGlowEntities.push_back({Ammo, Colors::Ammo, Vars::Glow::World::Alpha.m_Var});

					if (!g_Chams.HasDrawn(Ammo))
						DrawModel(Ammo, STUDIO_RENDER, true);
				}
			}

			if (Vars::Glow::World::Projectiles.m_Var)
			{
				for (const auto& Projectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
				{
					if (*reinterpret_cast<byte*>(Projectile + 0x7C) & EF_NODRAW)
						continue;

					int nTeam = Projectile->GetTeamNum();

					if (Vars::Glow::World::Projectiles.m_Var == 2 && nTeam == pLocal->GetTeamNum())
						continue;

					if (!Utils::IsOnScreen(pLocal, Projectile))
						continue;

					m_vecGlowEntities.push_back({
						Projectile, Utils::GetTeamColor(nTeam, Vars::ESP::Main::EnableTeamEnemyColors.m_Var),
						Vars::Glow::World::Alpha.m_Var
					});

					if (!g_Chams.HasDrawn(Projectile))
						DrawModel(Projectile, STUDIO_RENDER, true);
				}
			}
		}

		StencilStateDisable.SetStencilState(pRenderContext);

		if (m_vecGlowEntities.empty())
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

			StencilStateDisable.SetStencilState(pRenderContext);
		}
		pRenderContext->PopRenderTargetAndViewport();

		if (!Vars::Glow::Main::Stencil.m_Var) {
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

		if (Vars::Glow::Main::Stencil.m_Var)
		{
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, -1, -1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, -1, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, 0, -1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, 0, 1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, 1, 1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, 1, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, 1, -1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, -1, 1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
		}
		else {
			pRenderContext->DrawScreenSpaceRectangle(m_pMatHaloAddToScreen, 0, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h);
		}
		StencilStateDisable.SetStencilState(pRenderContext);

		I::ModelRender->ForcedMaterialOverride(nullptr);
		I::RenderView->SetColorModulation(flOriginalColor);
		I::RenderView->SetBlend(flOriginalBlend);
	}
}
