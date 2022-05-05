#pragma once

#include "../../../Utils/Color/Color.h"

#include <fstream>
#include <filesystem>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class CConfigManager
{
private:
	boost::property_tree::ptree WriteTree;
	boost::property_tree::ptree ReadTree;

	void SaveJson(const char* name, bool val);
	void SaveJson(const char* name, std::string val);
	void SaveJson(const char* name, int val);
	void SaveJson(const char* name, float val);
	void SaveJson(const char* name, Color_t val);
	void SaveJson(const char* name, Gradient_t val);
	void SaveJson(const char* name, Vec3 val);
	void SaveJson(const char* name, Chams_t val);
	
	void LoadJson(const char* name, std::string& val);
	void LoadJson(const char* name, bool& val);
	void LoadJson(const char* name, int& val);
	void LoadJson(const char* name, float& val);
	void LoadJson(const char* name, Color_t& val);
	void LoadJson(const char* name, Gradient_t& val);
	void LoadJson(const char* name, Vec3& val);
	void LoadJson(const char* name, Chams_t& val);

public:
	std::wstring m_sConfigPath;
	std::string ConfigPath;
	const std::string ConfigExtension = ".fw";

	CConfigManager();
	bool SaveConfig(const std::string& configName);
	bool LoadConfig(const std::string& configName);
	void RemoveConfig(const std::string& configName);
};

inline CConfigManager g_CFG;