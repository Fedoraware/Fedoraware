#pragma once

#include "../../../Utils/Color/Color.h"

#include <filesystem>

class CConfigManager
{
	void SaveJson(const char* name, bool val);
	void SaveJson(const char* name, const std::string& val);
	void SaveJson(const char* name, int val);
	void SaveJson(const char* name, float val);
	void SaveJson(const char* name, Color_t val);
	void SaveJson(const char* name, Gradient_t val);
	void SaveJson(const char* name, const Vec3& val);
	void SaveJson(const char* name, const Chams_t& val);
	void SaveJson(const char* name, const DragBox_t& val);

	void LoadJson(const char* name, std::string& val);
	void LoadJson(const char* name, bool& val);
	void LoadJson(const char* name, int& val);
	void LoadJson(const char* name, float& val);
	void LoadJson(const char* name, Color_t& val);
	void LoadJson(const char* name, Gradient_t& val);
	void LoadJson(const char* name, Vec3& val);
	void LoadJson(const char* name, Chams_t& val);
	void LoadJson(const char* name, DragBox_t& val);

	std::string CurrentConfig = "default";
	std::string CurrentVisuals = "default";
	std::string ConfigPath;

public:
	const std::string ConfigExtension = ".fw";

	CConfigManager();
	bool SaveConfig(const std::string& configName);
	bool LoadConfig(const std::string& configName);
	bool SaveVisual(const std::string& configName);
	bool LoadVisual(const std::string& configName);
	void RemoveConfig(const std::string& configName);
	void RemoveVisual(const std::string& configName);

	std::string GetCurrentConfig() { return CurrentConfig; }
	std::string GetCurrentVisuals() { return CurrentVisuals; }
	std::string GetConfigPath() { return ConfigPath; }
	std::string GetVisualsPath() { return ConfigPath + "\\Visuals"; }
};

inline CConfigManager g_CFG;