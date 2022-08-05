#include "LuaMenu.h"

#include <filesystem>
#include <fstream>

#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_stdlib.h"
#include "../ConfigManager/ConfigManager.h"
#include "../../LuaEngine/LuaEngine.h"

void CLuaMenu::LoadScripts()
{
	ScriptMap.clear();

	// Load Lua files
	for (const auto& entry : std::filesystem::directory_iterator(LuaFolder))
	{
		// Ignore all non-Lua files
		if (std::filesystem::path(entry).filename().string().find(".lua") == std::string_view::npos)
		{
			continue;
		}

		// Get the script name
		std::string scriptName = entry.path().filename().string();
		scriptName.erase(scriptName.end() - 4, scriptName.end());

		ScriptMap[scriptName] = entry.path().string();
	}
}

void CLuaMenu::MainWindow()
{
	if (!IsOpen) { return; }

	using namespace ImGui;
	SetNextWindowSize(ImVec2(400, 380), ImGuiCond_Once);
	if (Begin("Lua Scripts", &IsOpen, ImGuiWindowFlags_NoCollapse))
	{
		const auto& currentPath = ScriptMap[CurrentScript];

		// Toolbar
		{
			if (Button("Execute") && !currentPath.empty())
			{
				F::LuaEngine.ExecuteFile(currentPath);
			}

			if (Button("Refresh"))
			{
				LoadScripts();
			}

			SameLine();
			if (Button("Remove") && !currentPath.empty())
			{
				std::filesystem::remove(currentPath);
				CurrentScript = "";
				LoadScripts();
			}

			SameLine();
			if (Button("Open Folder"))
			{
				ShellExecuteA(nullptr, "open", LuaFolder.c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
			}
		}

		PushItemWidth(GetWindowSize().x - 2 * GetStyle().WindowPadding.x);

		// Script list
		if (BeginChild("ListChild###Lua"))
		{
			if (ListBoxHeader("###ScriptList", { GetWindowWidth(), GetWindowHeight() }))
			{
				for (auto const& [name, path] : ScriptMap)
				{
					if (name.empty()) { continue; }

					if (Selectable(name.c_str(), CurrentScript == name))
					{
						CurrentScript = name;
					}
				}

				ListBoxFooter();
			}
		}
		EndChild();

		PopItemWidth();
		End();
	}
}

void CLuaMenu::Render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(10, 10));
	MainWindow();
	ImGui::PopStyleVar(2);
}

void CLuaMenu::Init()
{
	LuaFolder = g_CFG.GetConfigPath() + "\\Lua";
	LoadScripts();
}
