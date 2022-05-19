#pragma once

#include "../../SDK/SDK.h"

#include <fstream>
#include <filesystem>

enum AttributeID {
	IsAustraliumItem = 2027,
	LootRarity = 2022,
	ItemStyleOverride = 542,
	AncientPowers = 150,
	IsFestive = 2053,
	Sheen = 2014,
	UnusualEffect = 134,
	ParticleEffect = 370
};

struct AttributeInfo_t {
	int m_nItemDefIndex;
	int m_nEffect;
	int m_nParticle;
	int m_nSheen;
	bool m_bAncient;
	bool m_bStyleOverride;
};

class CAttributChanger {
public:
	void Run();

	bool ShouldSave = false, ShouldLoad = true, ShouldSet = false;

private:
	void SetAttribute();
	void SaveConfig();
	void LoadConfig();

	std::map<int, AttributeInfo_t> AttributeMap;

	std::ifstream ReadStream;
	std::ofstream WriteStream;
	std::string AttributePath;

private:
	__inline void SaveInt(const char* szSection, const char* szItem, int value)
	{
		const std::string szToSave = std::to_string(value);
		WritePrivateProfileStringA(szSection, szItem, szToSave.c_str(), AttributePath.c_str());
	}

	__inline void SaveBool(const char* szSection, const char* szItem, bool value)
	{
		const std::string szToSave = value ? "true" : "false";
		WritePrivateProfileStringA(szSection, szItem, szToSave.c_str(), AttributePath.c_str());
	}

	__inline int LoadInt(const char* szSection, const char* szItem)
	{
		char szReturn[69];
		GetPrivateProfileStringA(szSection, szItem, "0", szReturn, 69, AttributePath.c_str());
		return std::stoi(szReturn);
	}

	__inline bool LoadBool(const char* szSection, const char* szItem)
	{
		char szReturn[69];
		GetPrivateProfileStringA(szSection, szItem, "false", szReturn, 69, AttributePath.c_str());
		return (strcmp(szReturn, "true") == 0) ? true : false;
	}
};

inline CAttributChanger g_AttributeChanger;
