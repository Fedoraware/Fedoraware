#include "Visuals.h"
#include "../Vars.h"
#include "../Menu/Menu.h"

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

bool bWorldIsModulated = false;

void ApplyModulation(const Color_t &clr)
{
	for (MaterialHandle_t h = g_Interfaces.MatSystem->First(); h != g_Interfaces.MatSystem->Invalid(); h = g_Interfaces.MatSystem->Next(h))
	{
		if (const auto &pMaterial = g_Interfaces.MatSystem->Get(h))
		{
			if (pMaterial->IsErrorMaterial() || !pMaterial->IsPrecached())
				continue;

			std::string_view group(pMaterial->GetTextureGroupName());

			if (group.find(_(TEXTURE_GROUP_WORLD)) != group.npos || group.find(_(TEXTURE_GROUP_SKYBOX)) != group.npos)
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

void CVisuals::ModulateWorld()
{
	if (!Vars::Visuals::WorldModulation.m_Var)
		return;

	ApplyModulation(Colors::WorldModulation);
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

	ApplyModulation({ 255, 255, 255, 255 });
	bWorldIsModulated = false;
}