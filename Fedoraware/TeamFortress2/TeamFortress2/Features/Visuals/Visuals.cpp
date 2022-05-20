#include "Visuals.h"
#include "../Vars.h"

void CVisuals::DrawHitboxMatrix(CBaseEntity* pEntity, Color_t colourface, Color_t colouredge, float time)
{
	//I::DebugOverlay->ClearAllOverlays();

	const model_t* model = pEntity->GetModel();
	const studiohdr_t* hdr = I::ModelInfo->GetStudioModel(model);
	const mstudiohitboxset_t* set = hdr->GetHitboxSet(pEntity->GetHitboxSet());

	for (int i{}; i < set->numhitboxes; ++i)
	{
		const mstudiobbox_t* bbox = set->hitbox(i);
		if (!bbox)
		{
			continue;
		}
		
		/*if (bbox->m_radius <= 0.f) {*/
		matrix3x4 rotMatrix;
		Math::AngleMatrix(bbox->angle, rotMatrix);

		matrix3x4 matrix;
		matrix3x4 boneees[128];
		pEntity->SetupBones(boneees, 128, BONE_USED_BY_ANYTHING, I::GlobalVars->curtime);
		Math::ConcatTransforms(boneees[bbox->bone], rotMatrix, matrix);

		Vec3 bboxAngle;
		Math::MatrixAngles(matrix, bboxAngle);

		Vec3 matrixOrigin;
		Math::GetMatrixOrigin(matrix, matrixOrigin);

		I::DebugOverlay->AddBoxOverlay2(matrixOrigin, bbox->bbmin, bbox->bbmax, bboxAngle, colourface, colouredge, time);
	}
}

void CVisuals::ScopeLines()
{
	const int centerX = g_ScreenSize.w / 2;
	const int centerY = g_ScreenSize.h / 2;
	const Color_t line1 = {Colors::NoscopeLines1.r, Colors::NoscopeLines1.g, Colors::NoscopeLines1.b, 255};
	const Color_t line2 = {Colors::NoscopeLines2.r, Colors::NoscopeLines2.g, Colors::NoscopeLines2.b, 255};

	g_Draw.GradientRect(g_ScreenSize.w / 2, centerY - 1, g_ScreenSize.w, centerY + 1, line1, line2, true);
	g_Draw.GradientRect(0, centerY - 1, centerX, centerY + 1, line2, line1, true);
	g_Draw.GradientRect(centerX - 1, 0, centerX + 1, centerY, line2, line1, false);
	g_Draw.GradientRect(centerX - 1, centerY, centerX + 1, g_ScreenSize.h, line1, line2, false);
}

void CVisuals::SkyboxChanger()
{
	using LoadNamedSkysFn = bool(_cdecl*)(const char*);
	static auto LoadSkys = (LoadNamedSkysFn)g_Pattern.Find(
		_(L"engine.dll"), _(L"55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 56 57 8B 01 C7 45"));

	const char* skybNames[] = {
		"Custom",
		"sky_tf2_04",
		"sky_upward",
		"sky_dustbowl_01",
		"sky_goldrush_01",
		"sky_granary_01",
		"sky_well_01",
		"sky_gravel_01",
		"sky_badlands_01",
		"sky_hydro_01",
		"sky_night_01",
		"sky_nightfall_01",
		"sky_trainyard_01",
		"sky_stormfront_01",
		"sky_morningsnow_01",
		"sky_alpinestorm_01",
		"sky_harvest_01",
		"sky_harvest_night_01",
		"sky_halloween",
		"sky_halloween_night_01",
		"sky_halloween_night2014_01",
		"sky_island_01",
		"sky_rainbow_01"
	};
	if (Vars::Visuals::SkyboxChanger.m_Var)
	{
		if (Vars::Skybox::SkyboxNum == 0)
		{
			if (Vars::Misc::BypassPure.m_Var)
			{
				LoadSkys(Vars::Skybox::SkyboxName.c_str());
			}
			else
			{
				LoadSkys(I::CVars->FindVar("sv_skyname")->GetString());
			}
		}
		else
		{
			LoadSkys(skybNames[Vars::Skybox::SkyboxNum]);
		}
	}
	else
	{
		LoadSkys(I::CVars->FindVar("sv_skyname")->GetString());
	}
}

bool CVisuals::RemoveScope(int nPanel)
{
	if (!m_nHudZoom && Hash::IsHudScope(I::Panel->GetName(nPanel)))
	{
		m_nHudZoom = nPanel;
	}

	return (Vars::Visuals::RemoveScope.m_Var && nPanel == m_nHudZoom);
}

void CVisuals::FOV(CViewSetup* pView)
{
	CBaseEntity* pLocal = g_EntityCache.m_pLocal;

	if (pLocal && pView)
	{
		if (pLocal->GetObserverMode() == OBS_MODE_FIRSTPERSON || (pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.m_Var))
		{
			return;
		}

		pView->fov = static_cast<float>(Vars::Visuals::FieldOfView.m_Var);

		if (pLocal->IsAlive())
		{
			pLocal->SetFov(Vars::Visuals::FieldOfView.m_Var);
			pLocal->m_iDefaultFOV() = Vars::Visuals::FieldOfView.m_Var;
		}
	}
}

void CVisuals::ThirdPerson(CViewSetup* pView)
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		// Toggle key
		if (Vars::Visuals::ThirdPersonKey.m_Var)
		{
			if (!I::EngineVGui->IsGameUIVisible() && !I::Surface->IsCursorVisible())
			{
				static KeyHelper tpKey{&Vars::Visuals::ThirdPersonKey.m_Var};
				if (tpKey.Pressed())
				{
					Vars::Visuals::ThirdPerson.m_Var = !Vars::Visuals::ThirdPerson.m_Var;
				}
			}
		}

		const bool bIsInThirdPerson = I::Input->CAM_IsThirdPerson();

		if (!Vars::Visuals::ThirdPerson.m_Var
			|| ((!Vars::Visuals::RemoveScope.m_Var || !Vars::Visuals::RemoveZoom.m_Var) && pLocal->IsScoped()))
		{
			if (bIsInThirdPerson)
			{
				pLocal->ForceTauntCam(0);
			}

			return;
		}

		if (!bIsInThirdPerson)
		{
			pLocal->ForceTauntCam(1);
		}

		// Thirdperson angles
		if (bIsInThirdPerson && Vars::Visuals::ThirdPersonSilentAngles.m_Var)
		{
			I::Prediction->SetLocalViewAngles(g_GlobalInfo.m_vRealViewAngles);
			if (Vars::Visuals::ThirdPersonInstantYaw.m_Var)
			{
				if (const auto& pAnimState = pLocal->GetAnimState())
				{
					pAnimState->m_flCurrentFeetYaw = g_GlobalInfo.m_vRealViewAngles.y;
				}
			}
		}

		// Thirdperson offset
		if (bIsInThirdPerson && Vars::Visuals::ThirdpersonOffset.m_Var)
		{
			const Vec3 viewangles = I::Engine->GetViewAngles(); // Use engine view angles so anti aim doesn't make your camera go crazy mode
			Vec3 vForward, vRight, vUp;
			Math::AngleVectors(viewangles, &vForward, &vRight, &vUp);
			static KeyHelper offsetKey{ &Vars::Visuals::ThirdpersonArrowOffsetKey.m_Var };

			if (Vars::Visuals::ThirdpersonOffsetWithArrows.m_Var)
			{
				if (offsetKey.Down())
				{
					if (GetAsyncKeyState(VK_UP) & 0x8000)
					{
						arrowUp += 1.5f;
					}

					if (GetAsyncKeyState(VK_DOWN) & 0x8000)
					{
						arrowUp -= 1.5f;
					}

					if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
					{
						arrowRight += 1.5f;
					}

					if (GetAsyncKeyState(VK_LEFT) & 0x8000)
					{
						arrowRight -= 1.5f;
					}
				}

				pView->origin += vRight * arrowRight;
				pView->origin += vUp * arrowUp;
				pView->origin += vForward * Vars::Visuals::ThirdpersonDist.m_Var;
			}
			else
			{
				pView->origin += vRight * Vars::Visuals::ThirdpersonRight.m_Var;
				pView->origin += vUp * Vars::Visuals::ThirdpersonUp.m_Var;
				pView->origin += vForward * Vars::Visuals::ThirdpersonDist.m_Var;
			}
		}
	}
}


void CVisuals::BulletTrace(CBaseEntity* pEntity, Color_t color)
{
	Vector forward, src, dst;
	CGameTrace tr;
	Ray_t ray;
	CTraceFilterHitscan filter;

	Math::AngleVectors(pEntity->GetEyeAngles(), &forward);
	filter.pSkip = pEntity;
	const Vector src3D = pEntity->GetBonePos(6) - Vector(0, 0, 0);
	const Vector dst3D = src3D + (forward * 1000);

	ray.Init(src3D, dst3D);

	I::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	if (!Utils::W2S(src3D, src) || !Utils::W2S(tr.vEndPos, dst))
	{
		return;
	}

	//I::Surface->DrawLine(src.x, src.y, dst.x, dst.y);
	g_Draw.Line(src.x, src.y, dst.x, dst.y, color);
}


void CVisuals::StoreMaterialHandles()
{
	for (MaterialHandle_t h = I::MatSystem->First(); h != I::MatSystem->Invalid(); h = I::MatSystem->Next(h))
	{
		if (const auto& pMaterial = I::MatSystem->Get(h))
		{
			if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
			{
				continue;
			}

			MaterialHandleData data;

			data.Handle = h;

			data.Material = pMaterial;

			auto sGroup = std::string_view(pMaterial->GetTextureGroupName());

			data.Group = sGroup;

			auto sName = std::string_view(pMaterial->GetName());

			data.Name = sName;

			if (sGroup._Starts_with("SkyBox"))
			{
				data.GroupType = MaterialHandleData::EMatGroupType::GROUP_SKY;
			}
			else if (sGroup._Starts_with("World"))
			{
				data.GroupType = MaterialHandleData::EMatGroupType::GROUP_WORLD;
			}
			else
			{
				data.GroupType = MaterialHandleData::EMatGroupType::GROUP_OTHER;
			}

			if (sName.find("water") != std::string_view::npos || sName.find("glass") != std::string_view::npos
				|| sName.find("door") != std::string_view::npos || sName.find("tools") != std::string_view::npos
				|| sName.find("player") != std::string_view::npos || sName.find("chicken") != std::string_view::npos
				|| sName.find("wall28") != std::string_view::npos || sName.find("wall26") != std::string_view::npos
				|| sName.find("decal") != std::string_view::npos || sName.find("overlay") != std::string_view::npos
				|| sName.find("hay") != std::string_view::npos)
			{
				data.ShouldOverrideTextures = false;
			}
			else
			{
				data.ShouldOverrideTextures = true;
			}

			MaterialHandleDatas.push_back(data);
		}
	}
}

void CVisuals::ClearMaterialHandles()
{
	MaterialHandleDatas.clear();
}

// this whole section below is for world modulation
bool ModColChanged() // check if colours have been changed
{
	static auto oldW = Colors::WorldModulation;
	static auto oldS = Colors::SkyModulation;
	const auto curW = Colors::WorldModulation;
	const auto curS = Colors::SkyModulation;

	if (curW.r != oldW.r || curW.g != oldW.g || curW.b != oldW.b || curS.r != oldS.r || curS.g != oldS.g || curS.b != oldS.b)
	{
		oldW = curW;
		oldS = curS;
		return true;
	}
	return false;
}

bool ModSetChanged() // check if modulation has been switched
{
	static auto oldS = Vars::Visuals::SkyModulation.m_Var;
	static auto oldW = Vars::Visuals::WorldModulation.m_Var;
	const auto curS = Vars::Visuals::SkyModulation.m_Var;
	const auto curW = Vars::Visuals::WorldModulation.m_Var;

	if (curS != oldS || curW != oldW)
	{
		oldW = curW;
		oldS = curS;
		return true;
	}
	return false;
}

void ApplyModulation(const Color_t& clr)
{
	//for (MaterialHandle_t h = I::MatSystem->First(); h != I::MatSystem->Invalid(); h = I::
	//	MatSystem->Next(h))
	//{
	//	if (const auto& pMaterial = I::MatSystem->Get(h))
	//	{
	//		if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
	//			continue;

	//		std::string_view group(pMaterial->GetTextureGroupName());

	//		if (group.find(_(TEXTURE_GROUP_WORLD)) != group.npos)
	//		{
	//			pMaterial->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
	//		}
	//	}
	//}
	if (g_Visuals.MaterialHandleDatas.empty())
	{
		return;
	}

	for (const auto& material : g_Visuals.MaterialHandleDatas)
	{
		if (material.Material)
		{
			if (material.GroupType != CVisuals::MaterialHandleData::EMatGroupType::GROUP_WORLD)
			{
				continue;
			}
			if (material.Material->IsErrorMaterial() || !material.Material->IsPrecached())
			{
				continue;
			}
			material.Material->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
		}
	}
}

void ApplySkyboxModulation(const Color_t& clr)
{
	//for (MaterialHandle_t h = I::MatSystem->First(); h != I::MatSystem->Invalid(); h = I::
	//	MatSystem->Next(h))
	//{
	//	const auto& pMaterial = I::MatSystem->Get(h);

	//	if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
	//		continue;

	//	std::string_view group(pMaterial->GetTextureGroupName());

	//	if (group._Starts_with("SkyBox"))
	//	{
	//		pMaterial->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
	//	}
	//}
	if (g_Visuals.MaterialHandleDatas.empty())
	{
		return;
	}

	for (const auto& material : g_Visuals.MaterialHandleDatas)
	{
		if (material.Material)
		{
			if (material.GroupType != CVisuals::MaterialHandleData::EMatGroupType::GROUP_SKY)
			{
				continue;
			}
			if (material.Material->IsErrorMaterial() || !material.Material->IsPrecached())
			{
				continue;
			}
			material.Material->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
		}
	}
}

void CVisuals::ModulateWorld()
{
	static bool oConnectionState = (I::Engine->IsConnected() && I::Engine->IsInGame());
	const bool connectionState = (I::Engine->IsConnected() && I::Engine->IsInGame());
	const bool isUnchanged = connectionState == oConnectionState;
	static bool shouldModulate = false;

	if (ModColChanged() || ModSetChanged() || !isUnchanged)
	{
		Vars::Visuals::WorldModulation.m_Var ? ApplyModulation(Colors::WorldModulation) : ApplyModulation({255, 255, 255, 255});
		Vars::Visuals::SkyModulation.m_Var ? ApplySkyboxModulation(Colors::SkyModulation) : ApplySkyboxModulation({255, 255, 255, 255});
		oConnectionState = connectionState;
		shouldModulate = false;
	}
	else if (!Vars::Visuals::WorldModulation.m_Var)
	{
		if (!shouldModulate)
		{
			ApplyModulation({255, 255, 255, 255});
			shouldModulate = true;
		}
	} // i don't know why i need to do this
}

void CVisuals::RestoreWorldModulation() // keep this because its mentioned in @DLLMain.cpp if you find a better way to do this, remove it ig.
{
	ApplyModulation({255, 255, 255, 255});
	ApplySkyboxModulation({255, 255, 255, 255});
}

// all world mod stuff above

void CVisuals::OverrideWorldTextures()
{
	static KeyValues* kv = nullptr;
	if (!kv)
	{
		kv = new KeyValues("LightmappedGeneric");
		//kv->SetString("$basetexture", "dev/dev_measuregeneric01b"); //Nitro (dev texture)
		kv->SetString("$basetexture", "vgui/white_additive"); //flat 
		kv->SetString("$color2", "[0.12 0.12 0.15]"); //grey
	}

	if (Vars::Visuals::OverrideWorldTextures.m_Var)
	{
		for (const auto& data : MaterialHandleDatas)
		{
			if (data.Material == nullptr)
			{
				continue;
			}

			if (data.Material->IsTranslucent() || data.Material->IsSpriteCard() || data.GroupType != MaterialHandleData::EMatGroupType::GROUP_WORLD)
			{
				continue;
			}

			if (!data.ShouldOverrideTextures)
			{
				continue;
			}

			data.Material->SetShaderAndParams(kv);
		}
	}
}

void CVisuals::PickupTimers()
{
	if (!Vars::Visuals::PickupTimers.m_Var) { return; }

	for (auto pickupData = PickupDatas.begin(); pickupData != PickupDatas.end();)
	{
		const float timeDiff = I::Engine->Time() - pickupData->Time;
		if (timeDiff > 10.f)
		{
			pickupData = PickupDatas.erase(pickupData);
			continue;
		}

		auto timerText = tfm::format("%.1f", 10.f - timeDiff);
		auto color = pickupData->Type ? Colors::Health : Colors::Ammo;

		Vec3 vScreen;
		if (Utils::W2S(pickupData->Location, vScreen))
		{
			g_Draw.String(FONT_ESP_PICKUPS, vScreen.x, vScreen.y, color, ALIGN_CENTER, timerText.c_str());
		}

		++pickupData;
	}
}

CClientClass* CVisuals::CPrecipitation::GetPrecipitationClass()
{
	static CClientClass* pReturn = nullptr;

	if (!pReturn)
	{
		for (auto pClass = I::Client->GetAllClasses(); pClass; pClass = pClass->m_pNext)
		{
			if (pClass->m_ClassID == static_cast<int>(ETFClassID::CPrecipitation))
			{
				pReturn = pClass;
				break;
			}
		}
	}

	return pReturn;
}

void CVisuals::CPrecipitation::Run()
{
	constexpr auto PRECIPITATION_INDEX = (MAX_EDICTS - 1);

	const auto* pRainEntity = I::EntityList->GetClientEntity(PRECIPITATION_INDEX);

	if (!pRainEntity)
	{
		const auto pClass = GetPrecipitationClass();

		if (!pClass || !pClass->m_pCreateFn)
		{
			return;
		}

		RainNetworkable = reinterpret_cast<IClientNetworkable * (__cdecl*)(int, int)>(pClass->m_pCreateFn)(PRECIPITATION_INDEX, 0);

		if (!RainNetworkable)
		{
			return;
		}

		RainEntity = I::EntityList->GetClientEntity(PRECIPITATION_INDEX);

		if (!RainEntity)
		{
			return;
		}

		static auto dwOff = GetNetVar("CPrecipitation", "m_nPrecipType");

		*reinterpret_cast<int*>(RainEntity + dwOff) = Vars::Visuals::Rain.m_Var - 1;

		RainEntity->Networkable()->PreDataUpdate(DATA_UPDATE_CREATED);
		RainEntity->Networkable()->OnPreDataChanged(DATA_UPDATE_CREATED);

		RainEntity->m_vecMins() = Vec3(-32767.0f, -32767.0f, -32767.0f);
		RainEntity->m_vecMaxs() = Vec3(32767.0f, 32767.0f, 32767.0f);

		RainEntity->Networkable()->OnDataChanged(DATA_UPDATE_CREATED);
		RainEntity->Networkable()->PostDataUpdate(DATA_UPDATE_CREATED);
	}
}

void CVisuals::CPrecipitation::Cleanup()
{
	RainEntity = nullptr;
	RainNetworkable = nullptr;
}
