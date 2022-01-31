#include "Visuals.h"
#include "../Vars.h"
#include "../Menu/Menu.h"

typedef bool(_cdecl* LoadNamedSkysFn)(const char*);
static LoadNamedSkysFn LoadSkys = (LoadNamedSkysFn)g_Pattern.Find(_(L"engine.dll"), _(L"55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 56 57 8B 01 C7 45"));


void CVisuals::DrawHitboxMatrix(CBaseEntity* pEntity, Color_t colour, float time) {
	//I::DebugOverlay->ClearAllOverlays();

	const model_t* model = pEntity->GetModel();
	studiohdr_t* hdr = g_Interfaces.ModelInfo->GetStudioModel(model);
	mstudiohitboxset_t* set = hdr->GetHitboxSet(pEntity->GetHitboxSet());

	for (int i{}; i < set->numhitboxes; ++i) {
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

		g_Interfaces.DebugOverlay->AddBoxOverlay(matrix_origin, bbox->bbmin, bbox->bbmax, bbox_angle, colour.r, colour.g, colour.b, colour.a, time);
	}
}

void CVisuals::ScopeLines()
{
	const int centerX = g_ScreenSize.w / 2;
	const int centerY = g_ScreenSize.h / 2;
	Color_t line1 = { Colors::NoscopeLines1.r, Colors::NoscopeLines1.g, Colors::NoscopeLines1.b, 255 };
	Color_t line2 = { Colors::NoscopeLines2.r, Colors::NoscopeLines2.g, Colors::NoscopeLines2.b, 255 };
	
	g_Draw.GradientRect(g_ScreenSize.w / 2, centerY - 1, g_ScreenSize.w, centerY + 1, line1, line2, true);
	g_Draw.GradientRect(0, centerY - 1, centerX, centerY + 1, line2, line1, true);
	g_Draw.GradientRect(centerX - 1, 0, centerX + 1, centerY, line2, line1, false);
	g_Draw.GradientRect(centerX - 1, centerY, centerX + 1, g_ScreenSize.h, line1, line2, false);
}

void CVisuals::SkyboxChanger() {
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
	if (Vars::Visuals::SkyboxChanger.m_Var) {
		if (Vars::Skybox::SkyboxNum == 0) {
			if (Vars::Misc::BypassPure.m_Var) {
				LoadSkys(Vars::Skybox::SkyboxName.c_str());
			}
			else {
				LoadSkys(g_Interfaces.CVars->FindVar("sv_skyname")->GetString());
			}
		}
		else {
			LoadSkys(skybNames[Vars::Skybox::SkyboxNum]);
		}
	}
	else {
		LoadSkys(g_Interfaces.CVars->FindVar("sv_skyname")->GetString());
	}
}

void CVisuals::BigHeads(float headSize, float torsoSize, float handSize)
{
	if (Vars::ESP::Players::Funnybodypartslol.m_Var) {
		for (auto& Player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL)) {
			float* headScale = Player->GetHeadScale();
			float* torsoScale = Player->GetTorsoScale();
			float* handScale = Player->GetHandScale();
			*headScale = headSize;
			*torsoScale = torsoSize;
			*handScale = handSize;
		}
	}
}

bool CVisuals::RemoveScope(int nPanel)
{
	if (!m_nHudZoom && Hash::IsHudScope(g_Interfaces.Panel->GetName(nPanel)))
		m_nHudZoom = nPanel;

	return (Vars::Visuals::RemoveScope.m_Var && nPanel == m_nHudZoom);
}


void CVisuals::FOV(CViewSetup *pView)
{
	CBaseEntity *pLocal = g_EntityCache.m_pLocal;

	if (pLocal && pView)
	{
		if (pLocal->GetObserverMode() == OBS_MODE_FIRSTPERSON || (pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.m_Var))
			return;

		pView->fov = Vars::Visuals::FieldOfView.m_Var;

		if (pLocal->IsAlive())
			pLocal->SetFov(Vars::Visuals::FieldOfView.m_Var);
	}
}

void CVisuals::ThirdPerson()
{
	if (const auto &pLocal = g_EntityCache.m_pLocal)
	{
		if (Vars::Visuals::ThirdPersonKey.m_Var)
		{
			if (!g_Interfaces.EngineVGui->IsGameUIVisible() && !g_Interfaces.Surface->IsCursorVisible())
			{
				static float flPressedTime = g_Interfaces.Engine->Time();
				float flElapsed = g_Interfaces.Engine->Time() - flPressedTime;

				if ((GetAsyncKeyState(Vars::Visuals::ThirdPersonKey.m_Var) & 0x8000) && flElapsed > 0.2f) {
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
				if (const auto &pAnimState = pLocal->GetAnimState())
					pAnimState->m_flCurrentFeetYaw = g_GlobalInfo.m_vRealViewAngles.y;
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

bool bWorldIsModulated = false;
bool bSkyIsModulated = false;



void ApplyModulation(const Color_t &clr)
{
	for (MaterialHandle_t h = g_Interfaces.MatSystem->First(); h != g_Interfaces.MatSystem->Invalid(); h = g_Interfaces.MatSystem->Next(h))
	{
		if (const auto &pMaterial = g_Interfaces.MatSystem->Get(h))
		{
			//bool bFound2 = false;
			//IMaterialVar* rain = pMaterial->FindVar(_("env_global"), &bFound2);

			if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
				continue;

			std::string_view group(pMaterial->GetTextureGroupName());

			if (group.find(_(TEXTURE_GROUP_WORLD)) != group.npos/* || group.find(_(TEXTURE_GROUP_SKYBOX)) != group.npos*/)
			{
				bool bFound = false;
				IMaterialVar *pVar = pMaterial->FindVar(_("$color2"), &bFound);

				if (bFound && pVar)
					pVar->SetVecValue(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));

				else pMaterial->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
			}


		}
	}

	bWorldIsModulated = true;
}

void ApplySkyboxModulation(const Color_t& clr)
{
	for (MaterialHandle_t h = g_Interfaces.MatSystem->First(); h != g_Interfaces.MatSystem->Invalid(); h = g_Interfaces.MatSystem->Next(h))
	{
		const auto& pMaterial = g_Interfaces.MatSystem->Get(h);

		if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
			continue;

		std::string_view group(pMaterial->GetTextureGroupName());

		if (group._Starts_with("SkyBox"))
		{
			pMaterial->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
		}
	}
	bSkyIsModulated = true;
}


void CVisuals::ModulateWorld()
{
	if (!Vars::Visuals::WorldModulation.m_Var)
		return;

	ApplyModulation(Colors::WorldModulation);
	ApplySkyboxModulation(Colors::SkyModulation);
}



void CVisuals::OverrideWorldTextures()
{
		static KeyValues *kv = nullptr;
		if (!kv) {
			kv = new KeyValues("LightmappedGeneric");
			//kv->SetString("$basetexture", "dev/dev_measuregeneric01b"); //Nitro (dev texture)
			kv->SetString("$basetexture", "vgui/white_additive"); //flat 
			kv->SetString("$color2", "[0.12 0.12 0.15]"); //grey
		}

		if (Vars::Visuals::OverrideWorldTextures.m_Var) {
			for (auto h = g_Interfaces.MatSystem->First(); h != g_Interfaces.MatSystem->Invalid(); h = g_Interfaces.MatSystem->Next(h)) {
				IMaterial* pMaterial = g_Interfaces.MatSystem->Get(h);

				if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached()
					|| pMaterial->IsTranslucent() || pMaterial->IsSpriteCard()
					|| std::string_view(pMaterial->GetTextureGroupName()).find("World") == std::string_view::npos)
					continue;

				std::string_view sName = std::string_view(pMaterial->GetName());

				if (sName.find("water") != std::string_view::npos || sName.find("glass") != std::string_view::npos
					|| sName.find("door") != std::string_view::npos || sName.find("tools") != std::string_view::npos
					|| sName.find("player") != std::string_view::npos || sName.find("chicken") != std::string_view::npos
					|| sName.find("wall28") != std::string_view::npos || sName.find("wall26") != std::string_view::npos
					|| sName.find("decal") != std::string_view::npos || sName.find("overlay") != std::string_view::npos
					|| sName.find("hay") != std::string_view::npos)
					continue;

				pMaterial->SetShaderAndParams(kv);
			}
		}
}

void CVisuals::UpdateWorldModulation()
{
	if (!Vars::Visuals::WorldModulation.m_Var) {
		RestoreWorldModulation();
		return;
	}

	auto ColorChanged = [&]() -> bool
	{
		static Color_t old = Colors::WorldModulation;
		Color_t cur = Colors::WorldModulation;

		if (cur.r != old.r || cur.g != old.g || cur.b != old.b) {
			old = cur;
			return true;
		}

		return false;
	};

	if (!bWorldIsModulated || ColorChanged())
		ApplyModulation(Colors::WorldModulation);
}

void CVisuals::UpdateSkyModulation()
{
	if (!Vars::Visuals::SkyModulation.m_Var || Vars::Visuals::Vision.m_Var == 1) {
		RestoreWorldModulation();
		return;
	}

	auto ColorChanged = [&]() -> bool
	{
		static Color_t old = Colors::SkyModulation;
		Color_t cur = Colors::SkyModulation;

		if (cur.r != old.r || cur.g != old.g || cur.b != old.b) {
			old = cur;
			return true;
		}

		return false;
	};

	if (!bWorldIsModulated || ColorChanged())
		ApplySkyboxModulation(Colors::SkyModulation);
}

void CVisuals::RestoreWorldModulation()
{
	if (!bWorldIsModulated)
		return;

	if (!bSkyIsModulated)
		return;

	ApplyModulation({ 255, 255, 255, 255 });
	ApplySkyboxModulation({ 255, 255, 255, 255 });
	bWorldIsModulated = false;
	bSkyIsModulated = false;
}