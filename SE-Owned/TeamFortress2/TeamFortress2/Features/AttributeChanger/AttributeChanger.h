#pragma once

#include "../../SDK/SDK.h"

#include <fstream>
#include <filesystem>

enum AttributeID
{
	IsAustraliumItem = 2027,
	LootRarity = 2022,
	ItemStyleOverride = 542,
	AncientPowers = 150,
	IsFestive = 2053,
	Sheen = 2014,
	UnusualEffect = 134,
	ParticleEffect = 370
};

struct AttributeInfo_t
{
	int m_nItemDefIndex;
	int m_nEffect;
	int m_nParticle;
	int m_nSheen;
	bool m_bAncient;
	bool m_bStyleOverride;
};

class CAttributChanger
{
public:
	void Run();

	bool m_bSave = false, m_bLoad = true, m_bSet = false;

private:
	void SetAttribute();
	void SaveConfig();
	void LoadConfig();

	std::map<int, AttributeInfo_t> m_mapAttributes;

	std::wifstream m_Read;
	std::wofstream m_Write;
	std::wstring m_szAttributePath = L"";

private:
	__inline void SaveInt(const wchar_t* szSection, const wchar_t* szItem, int value) {
		std::wstring szToSave = L"";
		szToSave += std::to_wstring(value);
		WritePrivateProfileStringW(szSection, szItem, szToSave.c_str(), m_szAttributePath.c_str());
	}

	__inline void SaveBool(const wchar_t* szSection, const wchar_t* szItem, bool value) {
		std::wstring szToSave = L"";
		szToSave += value ? L"true" : L"false";
		WritePrivateProfileStringW(szSection, szItem, szToSave.c_str(), m_szAttributePath.c_str());
	}

	__inline int LoadInt(const wchar_t* szSection, const wchar_t* szItem) {
		wchar_t szReturn[69];
		GetPrivateProfileStringW(szSection, szItem, L"0", szReturn, 69, m_szAttributePath.c_str());
		return std::stoi(szReturn);
	}

	__inline bool LoadBool(const wchar_t* szSection, const wchar_t* szItem) {
		wchar_t szReturn[69];
		GetPrivateProfileStringW(szSection, szItem, L"false", szReturn, 69, m_szAttributePath.c_str());
		return (wcscmp(szReturn, L"true") == 0) ? true : false;
	}
};

inline CAttributChanger g_AttributeChanger;