#include "AttributeChanger.h"

#include "../Menu/ConfigManager/ConfigManager.h"
#include "../Vars.h"

constexpr int MAX_BUFFER = (MAX_PATH * 3);

void CAttributChanger::Run()
{
	if (!Vars::Visuals::Skins::Enabled.m_Var|| !g_Interfaces.Engine->IsInGame())
		return;

	static auto dwItemDefOff = g_NetVars.get_offset(_("DT_EconEntity"), _("m_AttributeManager"), _("m_Item"), _("m_iItemDefinitionIndex"));

	if (m_bSet) {
		SetAttribute();
		m_bSet = false;
	}

	if (m_bSave) {
		SaveConfig();
		m_bSave = false;
	}

	if (m_bLoad) {
		LoadConfig();
		m_bLoad = false;
	}

	if (m_mapAttributes.empty())
		return;

	//Needed here, runs 2 stages before cache is filled
	if (const auto& pLocal = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer()))
	{
		if (!pLocal->IsAlive())
			return;

		auto MyWeapons = reinterpret_cast<size_t*>(pLocal + 0xCF8);

		int n;
		for (n = 0; MyWeapons[n]; n++)
		{
			if (const auto& pWeapon = reinterpret_cast<CBaseCombatWeapon*>(g_Interfaces.EntityList->GetClientEntityFromHandle(MyWeapons[n])))
			{
				auto pList = reinterpret_cast<CAttributeList*>(pWeapon + 0x9C0);

				if (!pList || pList->m_Attributes.Count() > 0)
					continue;

				auto nIndex = reinterpret_cast<int*>(pWeapon + dwItemDefOff);

				if (m_mapAttributes.find(*nIndex) != m_mapAttributes.end())
				{			
					if (m_mapAttributes[*nIndex].m_nEffect)
						pList->Add(AttributeID::UnusualEffect, m_mapAttributes[*nIndex].m_nEffect);

					if (m_mapAttributes[*nIndex].m_nParticle)
						pList->Add(AttributeID::ParticleEffect, m_mapAttributes[*nIndex].m_nParticle);

					if (m_mapAttributes[*nIndex].m_bAncient)
						pList->Add(AttributeID::AncientPowers, true);

					if (m_mapAttributes[*nIndex].m_bStyleOverride)
						pList->Add(AttributeID::ItemStyleOverride, true);

					if (m_mapAttributes[*nIndex].m_nSheen)
						pList->Add(AttributeID::Sheen, m_mapAttributes[*nIndex].m_nSheen);
				}
			}
		}
	}
}

void CAttributChanger::SaveConfig()
{
	if (m_szAttributePath.empty())
		m_szAttributePath = (g_CFG.m_sConfigPath + _(L"\\SEOCore\\ATTRIBUTES.seo"));

	m_Write = std::wofstream(m_szAttributePath, std::ios::out);

	if (m_Write.is_open())
	{
		for (auto& Attribute : m_mapAttributes)
		{
			wchar_t szKey[85];
			wsprintfW(szKey, L"%i",           Attribute.second.m_nItemDefIndex);

			SaveInt(szKey, L"Index",          Attribute.second.m_nItemDefIndex);
			SaveInt(szKey, L"Effect",         Attribute.second.m_nEffect);
			SaveInt(szKey, L"Particle",       Attribute.second.m_nParticle);
			SaveInt(szKey, L"Sheen",          Attribute.second.m_nSheen);
			SaveBool(szKey, L"Ancient",       Attribute.second.m_bAncient);
			SaveBool(szKey, L"StyleOverride", Attribute.second.m_bStyleOverride);
		}

		m_Write.close();
	}
}

void CAttributChanger::LoadConfig()
{
	if (m_szAttributePath.empty())
		m_szAttributePath = (g_CFG.m_sConfigPath + _(L"\\SEOCore\\ATTRIBUTES.seo"));

	m_Read = std::wifstream(m_szAttributePath);

	if (m_Read.is_open())
	{
		wchar_t* szSection, szSections[MAX_BUFFER];
		GetPrivateProfileSectionNamesW(szSections, MAX_BUFFER, m_szAttributePath.c_str());
		szSection = szSections;

		while (*szSection != NULL)
		{
			const int nIndex = LoadInt(szSection, L"Index");
			m_mapAttributes[nIndex] = { nIndex, LoadInt(szSection, L"Effect"),
										LoadInt(szSection, L"Particle"), LoadInt(szSection, L"Sheen"),
										LoadBool(szSection, L"Ancient"), LoadBool(szSection, L"StyleOverride") };

			szSection += (wcslen(szSection) + 1);
		}

		m_Read.close();
	}
}

void CAttributChanger::SetAttribute()
{
	typedef void(__thiscall* fn)(CClientState*);
	fn ForceFullUpdate = (fn)g_Pattern.Find(_(L"engine.dll"), _(L"56 8B F1 83 BE ? ? ? ? ? 74 1D"));

	//Needed here, runs 2 stages before cache is filled
	if (const auto& pLocal = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer()))
	{
		if (const auto& pWeapon = pLocal->GetActiveWeapon())
		{
			const int nIndex = pWeapon->GetItemDefIndex();
			m_mapAttributes[nIndex] = { nIndex, Vars::Visuals::Skins::Effect.m_Var, 
										Vars::Visuals::Skins::Particle.m_Var, Vars::Visuals::Skins::Sheen.m_Var, 
										Vars::Visuals::Skins::Acient.m_Var,   Vars::Visuals::Skins::Override.m_Var };

			ForceFullUpdate(g_Interfaces.ClientState);
		}
	}
}