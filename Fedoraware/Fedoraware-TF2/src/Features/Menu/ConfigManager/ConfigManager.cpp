#include "ConfigManager.h"

#include <string>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "../../Vars.h"
#include "../../../SDK/SDK.h"
#include "../../Misc/Notifications/Notifications.h"

#define SAVE_VAR(x) SaveJson(#x, x.Value)
#define LOAD_VAR(x) LoadJson(#x, x.Value)

#define SAVE_STRING(x) SaveJson(#x, x)
#define LOAD_STRING(x) LoadJson(#x, x)

#define SAVE_OTHER(x) SaveJson(#x, x)
#define LOAD_OTHER(x) LoadJson(#x, x)

boost::property_tree::ptree WriteTree;
boost::property_tree::ptree ReadTree;

boost::property_tree::ptree ColorToTree(const Color_t& color)
{
	boost::property_tree::ptree colorTree;
	colorTree.put("r", color.r);
	colorTree.put("g", color.g);
	colorTree.put("b", color.b);
	colorTree.put("a", color.a);

	return colorTree;
}

void TreeToColor(const boost::property_tree::ptree& tree, Color_t& out)
{
	if (auto v = tree.get_optional<byte>("r")) { out.r = *v; }
	if (auto v = tree.get_optional<byte>("g")) { out.g = *v; }
	if (auto v = tree.get_optional<byte>("b")) { out.b = *v; }
	if (auto v = tree.get_optional<byte>("a")) { out.a = *v; }
}

boost::property_tree::ptree VecToTree(const Vec3& vec)
{
	boost::property_tree::ptree vecTree;
	vecTree.put("x", vec.x);
	vecTree.put("y", vec.y);
	vecTree.put("z", vec.z);

	return vecTree;
}

void TreeToVec(const boost::property_tree::ptree& tree, Vec3& out)
{
	if (auto v = tree.get_optional<float>("x")) { out.x = *v; }
	if (auto v = tree.get_optional<float>("y")) { out.y = *v; }
	if (auto v = tree.get_optional<float>("z")) { out.z = *v; }
}

void CConfigManager::SaveJson(const char* name, bool val)
{
	WriteTree.put(name, val);
}

void CConfigManager::SaveJson(const char* name, int val)
{
	WriteTree.put(name, val);
}

void CConfigManager::SaveJson(const char* name, float val)
{
	WriteTree.put(name, val);
}

void CConfigManager::SaveJson(const char* name, const std::string& val)
{
	WriteTree.put(name, val);
}

void CConfigManager::SaveJson(const char* name, const Color_t& val)
{
	WriteTree.put_child(name, ColorToTree(val));
}

void CConfigManager::SaveJson(const char* name, const Gradient_t& val)
{
	boost::property_tree::ptree gradientTree;
	gradientTree.put_child("startColour", ColorToTree(val.startColour));
	gradientTree.put_child("endColour", ColorToTree(val.endColour));

	WriteTree.put_child(name, gradientTree);
}

void CConfigManager::SaveJson(const char* name, const Vec3& val)
{
	WriteTree.put_child(name, VecToTree(val));
}

void CConfigManager::SaveJson(const char* name, const Chams_t& val)
{
	boost::property_tree::ptree chamTree;
	chamTree.put("showObstructed", val.showObstructed);
	chamTree.put("drawMaterial", val.drawMaterial);
	chamTree.put("overlayType", val.overlayType);
	chamTree.put("chamsActive", val.chamsActive);
	chamTree.put("rainbow", val.rainbow);
	chamTree.put("overlayRainbow", val.overlayRainbow);
	chamTree.put("overlayPulse", val.overlayPulse);
	chamTree.put("overlayIntensity", val.overlayIntensity);
	chamTree.put_child("fresnelBase", ColorToTree(val.fresnelBase));
	chamTree.put_child("colour", ColorToTree(val.colour));
	chamTree.put_child("overlayColour", ColorToTree(val.overlayColour));
	chamTree.put("customMaterial", val.customMaterial);

	WriteTree.put_child(name, chamTree);
}

void CConfigManager::SaveJson(const char* name, const DragBox_t& val)
{
	boost::property_tree::ptree dragBoxTree;
	dragBoxTree.put("x", val.x);
	dragBoxTree.put("y", val.y);
	dragBoxTree.put("w", val.w);
	dragBoxTree.put("h", val.h);
	dragBoxTree.put("c", val.c);

	WriteTree.put_child(name, dragBoxTree);
}

void CConfigManager::LoadJson(const char* name, bool& val)
{
	if (auto getValue = ReadTree.get_optional<bool>(name))
	{
		val = *getValue;
	}
}

void CConfigManager::LoadJson(const char* name, int& val)
{
	if (auto getValue = ReadTree.get_optional<int>(name))
	{
		val = *getValue;
	}
}

void CConfigManager::LoadJson(const char* name, float& val)
{
	if (auto getValue = ReadTree.get_optional<float>(name))
	{
		val = *getValue;
	}
}

void CConfigManager::LoadJson(const char* name, std::string& val)
{
	if (auto getValue = ReadTree.get_optional<std::string>(name))
	{
		val = *getValue;
	}
}

void CConfigManager::LoadJson(const char* name, Color_t& val)
{
	if (const auto getChild = ReadTree.get_child_optional(name))
	{
		TreeToColor(*getChild, val);
	}
}

void CConfigManager::LoadJson(const char* name, Gradient_t& val)
{
	if (const auto getChild = ReadTree.get_child_optional(name))
	{
		if (const auto getStartColor = getChild->get_child_optional("startColour"))
		{
			TreeToColor(*getStartColor, val.startColour);
		}
		if (const auto endColor = getChild->get_child_optional("endColour"))
		{
			TreeToColor(*endColor, val.endColour);
		}
	}
}

void CConfigManager::LoadJson(const char* name, Vec3& val)
{
	if (const auto getChild = ReadTree.get_child_optional(name))
	{
		TreeToVec(*getChild, val);
	}
}

void CConfigManager::LoadJson(const char* name, Chams_t& val)
{
	if (const auto getChild = ReadTree.get_child_optional(name))
	{
		if (auto getValue = getChild->get_optional<bool>("showObstructed")) { val.showObstructed = *getValue; }
		if (auto getValue = getChild->get_optional<int>("drawMaterial")) { val.drawMaterial = *getValue; }
		if (auto getValue = getChild->get_optional<int>("overlayType")) { val.overlayType = *getValue; }
		if (auto getValue = getChild->get_optional<bool>("chamsActive")) { val.chamsActive = *getValue; }
		if (auto getValue = getChild->get_optional<bool>("rainbow")) { val.rainbow = *getValue; }
		if (auto getValue = getChild->get_optional<bool>("overlayRainbow")) { val.overlayRainbow = *getValue; }
		if (auto getValue = getChild->get_optional<bool>("overlayPulse")) { val.overlayPulse = *getValue; }
		if (auto getValue = getChild->get_optional<float>("overlayIntensity")) { val.overlayIntensity = *getValue; }
		if (const auto getChildColor = getChild->get_child_optional("fresnelBase")) { TreeToColor(*getChildColor, val.fresnelBase); }
		if (const auto getChildColor = getChild->get_child_optional("colour")) { TreeToColor(*getChildColor, val.colour); }
		if (const auto getChildColor = getChild->get_child_optional("overlayColour")) { TreeToColor(*getChildColor, val.overlayColour); }
		if (auto getValue = getChild->get_optional<std::string>("customMaterial")) { val.customMaterial = *getValue; }
	}
}

void CConfigManager::LoadJson(const char* name, DragBox_t& val)
{
	if (const auto getChild = ReadTree.get_child_optional(name))
	{
		if (auto getValue = getChild->get_optional<int>("x")) { val.x = *getValue; }
		if (auto getValue = getChild->get_optional<int>("y")) { val.y = *getValue; }
		if (auto getValue = getChild->get_optional<int>("w")) { val.w = *getValue; }
		if (auto getValue = getChild->get_optional<int>("h")) { val.h = *getValue; }
		if (auto getValue = getChild->get_optional<int>("c")) { val.c = *getValue; }
		val.update = true;
	}
}

CConfigManager::CConfigManager()
{
	ConfigPath = std::filesystem::current_path().string() + "\\FedFigs";
	VisualsPath = ConfigPath + "\\Visuals";

	// Create 'FedFigs' folder
	if (!std::filesystem::exists(ConfigPath))
	{
		std::filesystem::create_directory(ConfigPath);
	}

	// Create 'Core' folder for Attribute-Changer & Playerlist
	if (!std::filesystem::exists(ConfigPath + "\\Core"))
	{
		std::filesystem::create_directory(ConfigPath + "\\Core");
	}

	// Create 'Materials' folder for custom materials
	if (!std::filesystem::exists(ConfigPath + "\\Materials"))
	{
		std::filesystem::create_directory(ConfigPath + "\\Materials");
	}

	if (!std::filesystem::exists(VisualsPath))
	{
		std::filesystem::create_directory(VisualsPath);
	}

	if (!std::filesystem::exists(ConfigPath + "\\Killsays"))
	{
		std::filesystem::create_directory(ConfigPath + "\\Killsays");
	}
}

#define IsType(type) var->m_iType == typeid(type).hash_code()
#define SaveType(type) SaveJson(var->m_sName.c_str(), var->GetVar<type>()->Value)
#define SaveT(type) if (IsType(type)) SaveType(type);
#define LoadType(type) LoadJson(var->m_sName.c_str(), var->GetVar<type>()->Value)
#define LoadT(type) if (IsType(type)) LoadType(type);

bool CConfigManager::SaveConfig(const std::string& configName)
{
	try
	{
		WriteTree.clear();

		for (const auto& var : g_Vars)
		{
			if (var->m_bVisual)
				continue;

			SaveT(bool)
			else SaveT(int)
			else SaveT(float)
			else SaveT(std::string)
			else SaveT(Color_t)
			else SaveT(Gradient_t)
			else SaveT(Vec3)
			else SaveT(Chams_t)
			else SaveT(DragBox_t);
		}

		write_json(ConfigPath + "\\" + configName + ConfigExtension, WriteTree);
		F::Notifications.Add("Config " + configName + " saved");
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool CConfigManager::LoadConfig(const std::string& configName)
{
	// Check if the visual config exists
	if (!std::filesystem::exists(g_CFG.GetConfigPath() + "\\" + configName + ConfigExtension))
	{
		return false;
	}

	// Read ptree from json
	try
	{
		ReadTree.clear();
		read_json(ConfigPath + "\\" + configName + ConfigExtension, ReadTree);

		for (auto& var : g_Vars)
		{
			if (var->m_bVisual)
				continue;

			LoadT(bool)
			else LoadT(int)
			else LoadT(float)
			else LoadT(std::string)
			else LoadT(Color_t)
			else LoadT(Gradient_t)
			else LoadT(Vec3)
			else LoadT(Chams_t)
			else LoadT(DragBox_t);
		}


		CurrentConfig = configName;
		F::Notifications.Add("Config " + configName + " loaded");
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool CConfigManager::SaveVisual(const std::string& configName)
{
	try
	{
		WriteTree.clear();

		for (const auto& var : g_Vars)
		{
			if (!var->m_bVisual)
				continue;

			SaveT(bool)
			else SaveT(int)
			else SaveT(float)
			else SaveT(std::string)
			else SaveT(Color_t)
			else SaveT(Gradient_t)
			else SaveT(Vec3)
			else SaveT(Chams_t)
			else SaveT(DragBox_t);
		}

		write_json(ConfigPath + "\\Visuals\\" + configName + ConfigExtension, WriteTree);
		F::Notifications.Add("Visual config " + configName + " saved");
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool CConfigManager::LoadVisual(const std::string& configName)
{
	// Check if the visual config exists
	if (!std::filesystem::exists(g_CFG.GetVisualsPath() + "\\" + configName + ConfigExtension))
	{
		return false;
	}

	try
	{
		ReadTree.clear();
		read_json(ConfigPath + "\\Visuals\\" + configName + ConfigExtension, ReadTree);

		for (auto& var : g_Vars)
		{
			if (!var->m_bVisual)
				continue;

			LoadT(bool)
			else LoadT(int)
			else LoadT(float)
			else LoadT(std::string)
			else LoadT(Color_t)
			else LoadT(Gradient_t)
			else LoadT(Vec3)
			else LoadT(Chams_t)
			else LoadT(DragBox_t);
		}

		g_Draw.RemakeFonts();

		CurrentVisuals = configName;
	}
	catch (...)
	{
		return false;
	}
	return true;
}

void CConfigManager::RemoveConfig(const std::string& configName)
{
	std::filesystem::remove(ConfigPath + "\\" + configName + ConfigExtension);
}

void CConfigManager::RemoveVisual(const std::string& configName)
{
	std::filesystem::remove(VisualsPath + "\\" + configName + ConfigExtension);
}
