#pragma once

#include "../../../Utils/Color/Color.h"

#include <fstream>
#include <filesystem>

class CConfigManager
{
private:
	std::wofstream m_Write;
	std::wifstream m_Read;

private:
	bool Find(const wchar_t* name, std::wstring& output);

	void Save(const wchar_t* name, bool val);
	void Save(const wchar_t* name, std::string val);
	void Save(const wchar_t* name, const char* val);
	void Save(const wchar_t* name, int val);
	void Save(const wchar_t* name, float val);
	void Save(const wchar_t* name, Color_t val);
	void Save(const wchar_t* name, Gradient_t val);
	void Save(const wchar_t* name, Vec3 val);
	void Save(const wchar_t* name, Chams_t val);

	void Load(const char* name, std::string& val);

	void Load(const wchar_t* name, std::string& val);

	void Load(const wchar_t* name, bool& val);
	void Load(const wchar_t* name, int& val);
	void Load(const wchar_t* name, float& val);
	void Load(const wchar_t* name, Color_t& val);
	void Load(const wchar_t* name, Gradient_t& val);
	void Load(const wchar_t* name, Vec3& val);
	void Load(const wchar_t* name, Chams_t& val);

public:
	std::wstring m_sConfigPath;
	CConfigManager();
	void Save(const wchar_t* name);
	void Load(const wchar_t* name);
	void Remove(const wchar_t* name);
};

inline CConfigManager g_CFG;