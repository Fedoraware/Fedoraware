#include "LuaEngine.h"
#include <LuaBridge.h>

/* Prints the last lua error */
void CLuaEngine::PrintError()
{
	std::string errorMessage = "LUA Error: ";
	errorMessage.append(lua_tostring(LuaState, -1));
	errorMessage.append("\n");

	I::Cvar->ConsoleColorPrintf({ 235, 59, 90, 255 }, errorMessage.c_str());
	lua_pop(LuaState, -1);
}

/* Executes the given file */
void CLuaEngine::ExecuteFile(const char* file) {
	if (!file || !LuaState) { return; }
	if (luaL_dofile(LuaState, file)) {
		PrintError();
	}
}

/* Executes the given expression */
void CLuaEngine::ExecuteString(const char* expression) {
	if (!expression || !LuaState) {
		I::Cvar->ConsoleColorPrintf({ 235, 59, 90, 255 }, _("Lua Error: null expression passed to script engine\n"));
		return;
	}

	if (luaL_dostring(LuaState, expression)) {
		PrintError();
	}
}

void CLuaEngine::Init()
{
	LOCKLUA();

	using namespace luabridge;
	getGlobalNamespace(LuaState)
		.beginNamespace("Game")
		.endNamespace();
}
