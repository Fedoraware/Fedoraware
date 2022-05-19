#include "AttributeChanger.h"

#include "../Menu/ConfigManager/ConfigManager.h"
#include "../Vars.h"

constexpr int MAX_BUFFER = (MAX_PATH * 3);

std::array<int, 46> australium_table{
	4, 7, 13, 14, 15, 16, 18, 19, 20, 21, 29, 36, 38, 45, 61, 132, 141, 194, 197, 200, 201, 202, 203, 205, 206, 207,
	208, 211, 228, 424, 654, 658, 659, 662, 663, 664, 665, 669, 1000, 1004, 1006, 1007, 1078, 1082, 1085, 1149
};

std::array<std::pair<int, int>, 12> redirects{
	std::pair{264, 1071}, std::pair{18, 205}, std::pair{13, 200}, std::pair{21, 208}, std::pair{19, 206},
	std::pair{20, 207}, std::pair{15, 202}, std::pair{7, 197}, std::pair{29, 211}, std::pair{14, 201},
	std::pair{16, 203}, std::pair{4, 194}
};

void CAttributChanger::Run()
{
	if (!I::Engine->IsInGame())
	{
		return;
	}

	static auto dwItemDefOff = g_NetVars.get_offset(_("DT_EconEntity"), _("m_AttributeManager"), _("m_Item"), _("m_iItemDefinitionIndex"));

	if (ShouldSet)
	{
		SetAttribute();
		ShouldSet = false;
	}

	if (ShouldSave)
	{
		SaveConfig();
		ShouldSave = false;
	}

	if (ShouldLoad)
	{
		LoadConfig();
		ShouldLoad = false;
	}

	if (AttributeMap.empty())
	{
		return;
	}

	//Needed here, runs 2 stages before cache is filled
	if (const auto& pLocal = I::EntityList->GetClientEntity(I::Engine->GetLocalPlayer()))
	{
		if (!pLocal->IsAlive())
		{
			return;
		}

		const auto myWeapons = reinterpret_cast<size_t*>(pLocal + 0xCF8);

		for (int n = 0; myWeapons[n]; n++)
		{
			if (const auto& pWeapon = reinterpret_cast<CBaseCombatWeapon*>(I::EntityList->GetClientEntityFromHandle(myWeapons[n])))
			{
				const auto pList = reinterpret_cast<CAttributeList*>(pWeapon + 0x9C4);
				if (!pList || pList->m_Attributes.Count() > 0)
				{
					continue;
				}

				const auto nIndex = reinterpret_cast<int*>(pWeapon + dwItemDefOff);
				if (AttributeMap.find(*nIndex) != AttributeMap.end())
				{
					switch (*nIndex)
					{
					case 264:
						*nIndex = 1071;
						break;
					case 18:
						*nIndex = 205;
						break;
					case 13:
						*nIndex = 200;
						break;
					case 21:
						*nIndex = 208;
						break;
					case 19:
						*nIndex = 206;
						break;
					case 20:
						*nIndex = 207;
						break;
					case 15:
						*nIndex = 202;
						break;
					case 7:
						*nIndex = 197;
						break;
					case 29:
						*nIndex = 211;
						break;
					case 14:
						*nIndex = 201;
						break;
					case 16:
						*nIndex = 203;
						break;
					case 4:
						*nIndex = 194;
						break;
					}

					if (AttributeMap[*nIndex].m_bStyleOverride)
					{
						pList->Add(ItemStyleOverride, true);
					}

					if (AttributeMap[*nIndex].m_nEffect)
					{
						pList->Add(UnusualEffect, AttributeMap[*nIndex].m_nEffect);
					}

					if (AttributeMap[*nIndex].m_nParticle)
					{
						pList->Add(ParticleEffect, AttributeMap[*nIndex].m_nParticle);
					}

					if (AttributeMap[*nIndex].m_bAncient)
					{
						pList->Add(AncientPowers, true);
					}

					if (AttributeMap[*nIndex].m_nSheen)
					{
						pList->Add(Sheen, AttributeMap[*nIndex].m_nSheen);
					}
				}
			}
		}
	}
}

void CAttributChanger::SaveConfig()
{
	if (AttributePath.empty())
	{
		AttributePath = (g_CFG.GetConfigPath() + _("\\Core\\ATTRIBUTES.ini"));
	}

	WriteStream = std::ofstream(AttributePath, std::ios::out);

	if (WriteStream.is_open())
	{
		for (const auto& attribute : AttributeMap)
		{
			char szKey[85];
			sprintf(szKey, "%i", attribute.second.m_nItemDefIndex);

			SaveInt(szKey, "Index", attribute.second.m_nItemDefIndex);
			SaveInt(szKey, "Effect", attribute.second.m_nEffect);
			SaveInt(szKey, "Particle", attribute.second.m_nParticle);
			SaveInt(szKey, "Sheen", attribute.second.m_nSheen);
			SaveBool(szKey, "Ancient", attribute.second.m_bAncient);
			SaveBool(szKey, "StyleOverride", attribute.second.m_bStyleOverride);
		}

		WriteStream.close();
	}
}

void CAttributChanger::LoadConfig()
{
	if (AttributePath.empty())
	{
		AttributePath = (g_CFG.GetConfigPath() + _("\\Core\\ATTRIBUTES.ini"));
	}

	ReadStream = std::ifstream(AttributePath);

	if (ReadStream.is_open())
	{
		char szSections[MAX_BUFFER];
		GetPrivateProfileSectionNamesA(szSections, MAX_BUFFER, AttributePath.c_str());
		char* szSection = szSections;

		while (*szSection != NULL)
		{
			const int nIndex = LoadInt(szSection, "Index");
			AttributeMap[nIndex] = {
				nIndex, LoadInt(szSection, "Effect"),
				LoadInt(szSection, "Particle"), LoadInt(szSection, "Sheen"),
				LoadBool(szSection, "Ancient"), LoadBool(szSection, "StyleOverride")
			};

			szSection += (strlen(szSection) + 1);
		}

		ReadStream.close();
	}
}

void CAttributChanger::SetAttribute()
{
	using FN = void(__thiscall*)(CClientState*);
	auto ForceFullUpdate = reinterpret_cast<FN>(g_Pattern.Find(_(L"engine.dll"), _(L"56 8B F1 83 BE ? ? ? ? ? 74 1D")));

	//Needed here, runs 2 stages before cache is filled
	if (const auto& pLocal = I::EntityList->GetClientEntity(I::Engine->GetLocalPlayer()))
	{
		if (const auto& pWeapon = pLocal->GetActiveWeapon())
		{
			const int nIndex = pWeapon->GetItemDefIndex();
			AttributeMap[nIndex] = {
				nIndex, Vars::Visuals::Skins::Effect.m_Var,
				Vars::Visuals::Skins::Particle.m_Var, Vars::Visuals::Skins::Sheen.m_Var,
				Vars::Visuals::Skins::Acient.m_Var, Vars::Visuals::Skins::Override.m_Var
			};

			ForceFullUpdate(I::ClientState);
		}
	}
}
