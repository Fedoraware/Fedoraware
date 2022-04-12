#include "Visuals.h"
#include "../Vars.h"

void CVisuals::DrawHitboxMatrix(CBaseEntity* pEntity, Color_t colourface, Color_t colouredge, float time)
{
	//I::DebugOverlay->ClearAllOverlays();

	const model_t* model = pEntity->GetModel();
	studiohdr_t* hdr = g_Interfaces.ModelInfo->GetStudioModel(model);
	mstudiohitboxset_t* set = hdr->GetHitboxSet(pEntity->GetHitboxSet());

	for (int i{}; i < set->numhitboxes; ++i)
	{
		mstudiobbox_t* bbox = set->hitbox(i);
		if (!bbox)
			continue;

		//nigga balls
		/*if (bbox->m_radius <= 0.f) {*/
		matrix3x4 rot_matrix;
		Math::AngleMatrix(bbox->angle, rot_matrix);

		matrix3x4 matrix;
		matrix3x4 boneees[128];
		pEntity->SetupBones(boneees, 128, BONE_USED_BY_ANYTHING, g_Interfaces.GlobalVars->curtime);
		Math::ConcatTransforms(boneees[bbox->bone], rot_matrix, matrix);

		Vec3 bbox_angle;
		Math::MatrixAngles(matrix, bbox_angle);

		Vec3 matrix_origin;
		Math::GetMatrixOrigin(matrix, matrix_origin);

		g_Interfaces.DebugOverlay->AddBoxOverlay2(matrix_origin, bbox->bbmin, bbox->bbmax, bbox_angle, colourface, colouredge, time);
	}
}

void CVisuals::ScopeLines()
{
	const int centerX = g_ScreenSize.w / 2;
	const int centerY = g_ScreenSize.h / 2;
	Color_t line1 = {Colors::NoscopeLines1.r, Colors::NoscopeLines1.g, Colors::NoscopeLines1.b, 255};
	Color_t line2 = {Colors::NoscopeLines2.r, Colors::NoscopeLines2.g, Colors::NoscopeLines2.b, 255};

	g_Draw.GradientRect(g_ScreenSize.w / 2, centerY - 1, g_ScreenSize.w, centerY + 1, line1, line2, true);
	g_Draw.GradientRect(0, centerY - 1, centerX, centerY + 1, line2, line1, true);
	g_Draw.GradientRect(centerX - 1, 0, centerX + 1, centerY, line2, line1, false);
	g_Draw.GradientRect(centerX - 1, centerY, centerX + 1, g_ScreenSize.h, line1, line2, false);
}

void CVisuals::SkyboxChanger()
{
	using LoadNamedSkysFn = bool(_cdecl*)(const char*);
	static LoadNamedSkysFn LoadSkys = (LoadNamedSkysFn)g_Pattern.Find(
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
				LoadSkys(g_Interfaces.CVars->FindVar("sv_skyname")->GetString());
			}
		}
		else
		{
			LoadSkys(skybNames[Vars::Skybox::SkyboxNum]);
		}
	}
	else
	{
		LoadSkys(g_Interfaces.CVars->FindVar("sv_skyname")->GetString());
	}
}

bool CVisuals::RemoveScope(int nPanel)
{
	if (!m_nHudZoom && Hash::IsHudScope(g_Interfaces.Panel->GetName(nPanel)))
		m_nHudZoom = nPanel;

	return (Vars::Visuals::RemoveScope.m_Var && nPanel == m_nHudZoom);
}

bool CVisuals::RemoveMOTD(int nPanel)
{
	if (!m_nHudMotd && Hash::IsHudMotd(g_Interfaces.Panel->GetName(nPanel)))
	{
		m_nHudMotd = nPanel;
	}
	return (Vars::Visuals::RemoveMOTD.m_Var && nPanel == m_nHudMotd);
}


void CVisuals::FOV(CViewSetup* pView)
{
	CBaseEntity* pLocal = g_EntityCache.m_pLocal;

	if (pLocal && pView)
	{
		if (pLocal->GetObserverMode() == OBS_MODE_FIRSTPERSON || (pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.
			m_Var))
			return;

		pView->fov = Vars::Visuals::FieldOfView.m_Var;

		if (pLocal->IsAlive()) {
			pLocal->SetFov(Vars::Visuals::FieldOfView.m_Var);
			pLocal->m_iDefaultFOV() = Vars::Visuals::FieldOfView.m_Var;
		}
	}
}

void CVisuals::ThirdPerson(CViewSetup* pView)
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (Vars::Visuals::ThirdPersonKey.m_Var)
		{
			if (!g_Interfaces.EngineVGui->IsGameUIVisible() && !g_Interfaces.Surface->IsCursorVisible())
			{
				static float flPressedTime = g_Interfaces.Engine->Time();
				float flElapsed = g_Interfaces.Engine->Time() - flPressedTime;

				if ((GetAsyncKeyState(Vars::Visuals::ThirdPersonKey.m_Var) & 0x8000) && !GetAsyncKeyState(Vars::Visuals::ThirdpersonArrowOffsetKey.m_Var) && flElapsed > 0.2f)
				{
					Vars::Visuals::ThirdPerson.m_Var = !Vars::Visuals::ThirdPerson.m_Var;
					flPressedTime = g_Interfaces.Engine->Time();
				}
			}
		}

		bool bIsInThirdPerson = g_Interfaces.Input->CAM_IsThirdPerson();

		if (!Vars::Visuals::ThirdPerson.m_Var
			|| ((!Vars::Visuals::RemoveScope.m_Var || !Vars::Visuals::RemoveZoom.m_Var) && pLocal->IsScoped()))
		{
			if (bIsInThirdPerson)
				pLocal->ForceTauntCam(0);

			return;
		}

		if (!bIsInThirdPerson)
			pLocal->ForceTauntCam(1);

		if (bIsInThirdPerson && Vars::Visuals::ThirdPersonSilentAngles.m_Var)
		{
			g_Interfaces.Prediction->SetLocalViewAngles(g_GlobalInfo.m_vRealViewAngles);
			if (Vars::Visuals::ThirdPersonInstantYaw.m_Var)
			{
				if (const auto& pAnimState = pLocal->GetAnimState())
					pAnimState->m_flCurrentFeetYaw = g_GlobalInfo.m_vRealViewAngles.y;
			}
		}



		if (bIsInThirdPerson && Vars::Visuals::ThirdpersonOffset.m_Var) {
			
			const Vec3 viewangles = g_Interfaces.Engine->GetViewAngles(); // Use engine view angles so anti aim doesn't make your camera go crazy mode
			Vec3 vForward, vRight, vUp;
			Math::AngleVectors(viewangles, &vForward, &vRight, &vUp);
			if (Vars::Visuals::ThirdpersonOffsetWithArrows.m_Var) {
				if (GetAsyncKeyState(Vars::Visuals::ThirdpersonArrowOffsetKey.m_Var) && GetAsyncKeyState(VK_UP)) {
					arrowUp += 1.5f;
				}

				if (GetAsyncKeyState(Vars::Visuals::ThirdpersonArrowOffsetKey.m_Var) && GetAsyncKeyState(VK_DOWN)) {
					arrowUp -= 1.5f;
				}

				if (GetAsyncKeyState(Vars::Visuals::ThirdpersonArrowOffsetKey.m_Var) && GetAsyncKeyState(VK_RIGHT)) {
					arrowRight += 1.5f;
				}

				if (GetAsyncKeyState(Vars::Visuals::ThirdpersonArrowOffsetKey.m_Var) && GetAsyncKeyState(VK_LEFT)) {
					arrowRight -= 1.5f;
				}

				pView->origin += vRight * arrowRight;
				pView->origin += vUp * arrowUp;
				pView->origin += vForward * Vars::Visuals::ThirdpersonDist.m_Var;
			} else {
				pView->origin += vRight * Vars::Visuals::ThirdpersonRight.m_Var;
				pView->origin += vUp * Vars::Visuals::ThirdpersonUp.m_Var;
				pView->origin += vForward * Vars::Visuals::ThirdpersonDist.m_Var;
			}
		}
	}
}


void CVisuals::BulletTrace(CBaseEntity* pEntity, Color_t color)
{
	Vector src3D, dst3D, forward, src, dst;
	CGameTrace tr;
	Ray_t ray;
	CTraceFilterHitscan filter;

	Math::AngleVectors(pEntity->GetEyeAngles(), &forward);
	filter.pSkip = pEntity;
	src3D = pEntity->GetBonePos(6) - Vector(0, 0, 0);
	dst3D = src3D + (forward * 1000);

	ray.Init(src3D, dst3D);

	g_Interfaces.EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	if (!Utils::W2S(src3D, src) || !Utils::W2S(tr.vEndPos, dst))
	{
		return;
	}

	//g_Interfaces.Surface->DrawLine(src.x, src.y, dst.x, dst.y);
	g_Draw.Line(src.x, src.y, dst.x, dst.y, color);
}



void CVisuals::StoreMaterialHandles()
{
	for (MaterialHandle_t h = g_Interfaces.MatSystem->First(); h != g_Interfaces.MatSystem->Invalid(); h = g_Interfaces.MatSystem->Next(h))
	{
		if (const auto& pMaterial = g_Interfaces.MatSystem->Get(h))
		{
			if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
				continue;
			
			MaterialHandleData data;
			
			data.Handle = h;

			data.Material = pMaterial;

			auto sGroup = std::string_view(pMaterial->GetTextureGroupName());

			data.Group = sGroup;

			auto sName = std::string_view(pMaterial->GetName());

			data.Name = sName;

			if (sGroup._Starts_with("SkyBox"))
			{
				data.GroupType = CVisuals::MaterialHandleData::EMatGroupType::GROUP_SKY;
			}
			else if (sGroup._Starts_with("World"))
			{
				data.GroupType = CVisuals::MaterialHandleData::EMatGroupType::GROUP_WORLD;
			}
			else
			{
				data.GroupType = CVisuals::MaterialHandleData::EMatGroupType::GROUP_OTHER;
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
	static auto oldW = Colors::WorldModulation; static auto oldS = Colors::SkyModulation;
	auto curW = Colors::WorldModulation; auto curS = Colors::SkyModulation;

	if (curW.r != oldW.r || curW.g != oldW.g || curW.b != oldW.b || curS.r != oldS.r || curS.g != oldS.g || curS.b != oldS.b) { oldW = curW; oldS = curS; return true; }
	return false;
}

bool ModSetChanged() // check if modulation has been switched
{
	static auto oldS = Vars::Visuals::SkyModulation.m_Var; static auto oldW = Vars::Visuals::WorldModulation.m_Var;
	auto curS = Vars::Visuals::SkyModulation.m_Var; auto curW = Vars::Visuals::WorldModulation.m_Var;

	if (curS != oldS || curW != oldW) { oldW = curW; oldS = curS; return true; }
	return false;
}

void ApplyModulation(const Color_t& clr)
{
	//for (MaterialHandle_t h = g_Interfaces.MatSystem->First(); h != g_Interfaces.MatSystem->Invalid(); h = g_Interfaces.
	//	MatSystem->Next(h))
	//{
	//	if (const auto& pMaterial = g_Interfaces.MatSystem->Get(h))
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
	if (g_Visuals.MaterialHandleDatas.size() < 1)
	{
		return;
	}

	for (auto& Material : g_Visuals.MaterialHandleDatas)
	{
		if (Material.Material)
		{
			if (Material.GroupType != CVisuals::MaterialHandleData::EMatGroupType::GROUP_WORLD)
			{
				continue;
			}
			if (Material.Material->IsErrorMaterial() || !Material.Material->IsPrecached())
			{
				continue;
			}
			Material.Material->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
		}
	}

}

void ApplySkyboxModulation(const Color_t& clr)
{
	//for (MaterialHandle_t h = g_Interfaces.MatSystem->First(); h != g_Interfaces.MatSystem->Invalid(); h = g_Interfaces.
	//	MatSystem->Next(h))
	//{
	//	const auto& pMaterial = g_Interfaces.MatSystem->Get(h);

	//	if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
	//		continue;

	//	std::string_view group(pMaterial->GetTextureGroupName());

	//	if (group._Starts_with("SkyBox"))
	//	{
	//		pMaterial->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
	//	}
	//}
	if (g_Visuals.MaterialHandleDatas.size() < 1)
	{
		return;
	}

	for (auto& Material : g_Visuals.MaterialHandleDatas)
	{
		if (Material.Material)
		{
			if (Material.GroupType != CVisuals::MaterialHandleData::EMatGroupType::GROUP_SKY)
			{
				continue;
			}
			if (Material.Material->IsErrorMaterial() || !Material.Material->IsPrecached())
			{
				continue;
			}
			Material.Material->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
		}
	}

}

void CVisuals::ModulateWorld()
{
	static bool oconnectionstate = (g_Interfaces.Engine->IsConnected() && g_Interfaces.Engine->IsInGame());
	bool connectionstate = (g_Interfaces.Engine->IsConnected() && g_Interfaces.Engine->IsInGame());
	bool isunchanged = connectionstate == oconnectionstate;
	static bool a = false;
	if (ModColChanged() || ModSetChanged() || !isunchanged) {
		Vars::Visuals::WorldModulation.m_Var ? ApplyModulation(Colors::WorldModulation) : ApplyModulation({ 255, 255, 255, 255 });
		Vars::Visuals::SkyModulation.m_Var ? ApplySkyboxModulation(Colors::SkyModulation) : ApplySkyboxModulation({ 255, 255, 255, 255 });
		oconnectionstate = connectionstate;
		a = false;
	}
	else if (!Vars::Visuals::WorldModulation.m_Var) { 
		
		if (!a)
		{
			ApplyModulation({ 255, 255, 255, 255 });
			a = true;
		}
	} // i don't know why i need to do this
}

void CVisuals::RestoreWorldModulation() // keep this because its mentioned in @DLLMain.cpp if you find a better way to do this, remove it ig.
{
	ApplyModulation({ 255, 255, 255, 255 });
	ApplySkyboxModulation({ 255, 255, 255, 255 });
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
		for (auto &data : MaterialHandleDatas)
		{
			if (data.Material == nullptr)
				continue;

			if (data.Material->IsTranslucent() || data.Material->IsSpriteCard() || data.GroupType != CVisuals::MaterialHandleData::EMatGroupType::GROUP_WORLD)
				continue;

			if (!data.ShouldOverrideTextures)
				continue;

			data.Material->SetShaderAndParams(kv);
		}
	}
}

void CVisuals::PickupTimers()
{
	if (!Vars::Visuals::PickupTimers.m_Var) { return; }

	for (auto pickupData = PickupDatas.begin(); pickupData != PickupDatas.end(); )
	{
		const float timeDiff = g_Interfaces.Engine->Time() - pickupData->Time;
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
		for (auto pClass = g_Interfaces.Client->GetAllClasses(); pClass; pClass = pClass->m_pNext)
		{
			if (pClass->m_ClassID == (int)ETFClassID::CPrecipitation) {
				pReturn = pClass;
				break;
			}
		}
	}

	return pReturn;
}

void CVisuals::CPrecipitation::Run()
{
	const auto PRECIPITATION_INDEX = (MAX_EDICTS - 1);

	auto* pRainEntity = g_Interfaces.EntityList->GetClientEntity(PRECIPITATION_INDEX);

	if (!pRainEntity) {
		auto pClass = GetPrecipitationClass();

		if (!pClass || !pClass->m_pCreateFn)
			return;

		RainNetworkable = reinterpret_cast<IClientNetworkable * (__cdecl*)(int, int)>(pClass->m_pCreateFn)(PRECIPITATION_INDEX, 0);

		if (!RainNetworkable)
			return;

		RainEntity = g_Interfaces.EntityList->GetClientEntity(PRECIPITATION_INDEX);

		if (!RainEntity)
			return;

		static auto dwOff = GetNetVar("CPrecipitation", "m_nPrecipType");

		*reinterpret_cast<int*>(RainEntity + dwOff) = Vars::Visuals::Rain.m_Var - 1;

		RainEntity->Networkable()->PreDataUpdate(DATA_UPDATE_CREATED);
		RainEntity->Networkable()->OnPreDataChanged(DATA_UPDATE_CREATED);

		RainEntity->m_vecMins() = Vec3(-32767.0f, -32767.0f, -32767.0f);
		RainEntity->m_vecMaxs() = Vec3(32767.0f, 32767.0f, 32767.0f);

		RainEntity->Networkable()->OnDataChanged(DataUpdateType_t::DATA_UPDATE_CREATED);
		RainEntity->Networkable()->PostDataUpdate(DataUpdateType_t::DATA_UPDATE_CREATED);
	}
}

void CVisuals::CPrecipitation::Cleanup()
{
	//if (RainEntity) {
	//	RainEntity->Networkable()->Release();
	//} // This doesn't seem to do anything anyway

	RainEntity = nullptr;
	RainNetworkable = nullptr;
}
