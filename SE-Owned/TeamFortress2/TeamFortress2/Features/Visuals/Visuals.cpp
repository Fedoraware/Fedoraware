#include "Visuals.h"
#include "../Vars.h"
#include "../Menu/Menu.h"

typedef bool(_cdecl* LoadNamedSkysFn)(const char*);
static LoadNamedSkysFn LoadSkys = (LoadNamedSkysFn)g_Pattern.Find(_(L"engine.dll"), _(L"55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 56 57 8B 01 C7 45"));

void CVisuals::SkyboxChanger() {
	if (Vars::Visuals::SkyboxChanger.m_Var) {
		LoadSkys(Vars::Skybox::SkyboxName.c_str());
	}
	else {
		LoadSkys(g_Interfaces.CVars->FindVar("sv_skyname")->GetString());
	}
}

void CVisuals::Init() {

}

/*
CVisualsm_pMatDev = Utils::CreateMaterial({
	_("\"LightmappedGeneric\"\
			\n{\
			\n\t\"$basetexture\" \"dev/dev_measuregeneric01b\"\
			\n}\n")
	}
);*/

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
			if (!g_Interfaces.EngineVGui->IsGameUIVisible() && !g_Interfaces.Surface->IsCursorVisible() && !g_Menu.m_bTyping)
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

			if (group.find(_(TEXTURE_GROUP_WORLD)) != group.npos /*|| group.find(_(TEXTURE_GROUP_SKYBOX)) != group.npos*/)
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
		if (const auto& pMaterial = g_Interfaces.MatSystem->Get(h))
		{
			//bool bFound2 = false;
			//IMaterialVar* rain = pMaterial->FindVar(_("env_global"), &bFound2);

			if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
				continue;

			std::string_view group(pMaterial->GetTextureGroupName());

			if (/*group.find(_(TEXTURE_GROUP_WORLD)) != group.npos ||*/ group.find(_(TEXTURE_GROUP_SKYBOX)) != group.npos)
			{
				bool bFound = false;
				IMaterialVar* pVar = pMaterial->FindVar(_("$color2"), &bFound);

				if (bFound && pVar)
					pVar->SetVecValue(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));

				else pMaterial->ColorModulate(Color::TOFLOAT(clr.r), Color::TOFLOAT(clr.g), Color::TOFLOAT(clr.b));
			}
		}
	}
	bSkyIsModulated = true;
}


void CVisuals::ModulateWorld()
{
	if (!Vars::Visuals::WorldModulation.m_Var)
		return;

	ConVar* sv_cheats = g_Interfaces.CVars->FindVar("sv_cheats");
	if (sv_cheats->GetInt() == 0) sv_cheats->SetValue(1);

	ApplyModulation(Colors::WorldModulation);
	ApplySkyboxModulation(Colors::SkyModulation);
}



void CVisuals::OverrideWorldTextures()
{
	void* kv = nullptr;
	if (!kv) {
		kv = Utils::CreateKeyVals({
		_("\"LightmappedGeneric\"\
			\n{\
			\n\t\"$basetexture\" \"dev/dev_measuregeneric01b\"\
			\n}\n") 
		});
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

			//pMaterial->SetShaderAndParams(pKeyValues);
 			pMaterial->SetShaderAndParams(kv);
		}
	}
	else {
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

			//pMaterial->SetShaderAndParams(pKeyValues);
			pMaterial->Refresh();
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