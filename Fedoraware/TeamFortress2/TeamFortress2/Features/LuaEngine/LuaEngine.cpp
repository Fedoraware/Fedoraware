#include "LuaEngine.h"
#include "../../SDK/SDK.h"
#include <iostream>

LuaEngine* g_pLuaEngine = new LuaEngine();

lua_State* LuaEngine::L()
{
	return m_L;
}

void LuaEngine::PrintError()
{
	std::string errorMessage = "LUA Error: ";
	errorMessage.append(lua_tostring(m_L, -1));
	errorMessage.append("\n");

	I::Cvar->ConsoleColorPrintf({ 235, 59, 90, 255 }, errorMessage.c_str());
	lua_pop(m_L, -1);
}

void LuaEngine::ExecuteFile(const char* file) {
	if (!file || !m_L) { return; }
	if (luaL_dofile(m_L, file)) {
		PrintError();
	}
}

void LuaEngine::ExecuteString(const char* expression) {
	if (!expression || !m_L) {
		I::Cvar->ConsoleColorPrintf({ 235, 59, 90, 255 }, _("Lua Error: null expression passed to script engine\n"));
		return;
	}

	if (luaL_dostring(m_L, expression)) {
		PrintError();
	}
}