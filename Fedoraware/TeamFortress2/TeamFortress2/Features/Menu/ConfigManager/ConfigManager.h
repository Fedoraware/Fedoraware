#pragma once

#include "../../../Utils/Color/Color.h"

#include <fstream>
#include <filesystem>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class CConfigManager
{
private:
	std::wofstream m_Write;
	std::wifstream m_Read;
	boost::property_tree::ptree WriteTree;
	boost::property_tree::ptree ReadTree;

private:
	boost::property_tree::ptree ColorToTree(Color_t color);
	bool Find(const wchar_t* name, std::wstring& output);

	void Save(const wchar_t* name, bool val);
	void SaveJson(const char* name, bool val);
	void Save(const wchar_t* name, std::string val);
	void SaveJson(const char* name, std::string val);
	void Save(const wchar_t* name, const char* val);
	void Save(const wchar_t* name, int val);
	void SaveJson(const char* name, int val);
	void Save(const wchar_t* name, float val);
	void SaveJson(const char* name, float val);
	void Save(const wchar_t* name, Color_t val);
	void SaveJson(const char* name, Color_t val);
	void Save(const wchar_t* name, Gradient_t val);
	void SaveJson(const char* name, Gradient_t val);
	void Save(const wchar_t* name, Vec3 val);
	void SaveJson(const char* name, Vec3 val);
	void Save(const wchar_t* name, Chams_t val);
	void SaveJson(const char* name, Chams_t val);

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
	std::string ConfigPath;

	CConfigManager();
	bool SaveJson(const std::string& configName);
	void Save(const wchar_t* name);
	bool LoadJson(const std::string& configName);
	void Load(const wchar_t* name);
	void Remove(const wchar_t* name);
};

inline CConfigManager g_CFG;