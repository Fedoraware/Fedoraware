#pragma once
#include "../../../SDK/SDK.h"

class CLuaMenu
{
	void LoadScripts();
	void MainWindow();

	std::string LuaFolder;
	std::string CurrentScript;

public:
	void Render();
	void Init();

	bool IsOpen = false;
	std::map<std::string, std::string> ScriptMap;
};

ADD_FEATURE(CLuaMenu, LuaMenu)