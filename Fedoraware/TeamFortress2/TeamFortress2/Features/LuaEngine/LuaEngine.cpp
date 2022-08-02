#include "LuaEngine.h"
#include <LuaBridge.h>
#include <boost/algorithm/string/join.hpp>
#include "../Commands/Commands.h"

/* Prints the last lua error */
void CLuaEngine::PrintError()
{
	std::string errorMessage = "LUA Error: ";
	errorMessage.append(lua_tostring(LuaState, -1));
	errorMessage.append("\n");

	I::Cvar->ConsoleColorPrintf({ 235, 59, 90, 255 }, "%s\n", errorMessage.c_str());
	lua_pop(LuaState, -1);
}

/* Executes the given file */
void CLuaEngine::ExecuteFile(const char* file) {
	if (!file || !LuaState) { return; }

	if (luaL_dofile(LuaState, file))
	{
		PrintError();
	}
}

/* Executes the given expression */
void CLuaEngine::ExecuteString(const char* expression) {
	if (!expression || !LuaState) {
		I::Cvar->ConsoleColorPrintf({ 235, 59, 90, 255 }, _("Lua Error: null expression passed to script engine\n"));
		return;
	}

	if (luaL_dostring(LuaState, expression))
	{
		PrintError();
	}
}

void Print(const char* msg)
{
	I::Cvar->ConsolePrintf("%s\n", msg);
}

void CLuaEngine::Init()
{
	LOCKLUA();

	using namespace luabridge;
	getGlobalNamespace(LuaState)
		.addFunction("print", Print)

		// Client
		.beginNamespace("Client")
		.endNamespace();

	/* Register commands */

	F::Commands.Register("lua_load", [&](const std::deque<std::string>& args)
	{
		if (args.empty())
		{
			F::Commands.Error("Usage: lua_load <path>");
			return;
		}

		ExecuteFile(args.front().c_str());
	});

	F::Commands.Register("lua_do", [&](const std::deque<std::string>& args)
	{
		if (args.empty())
		{
			F::Commands.Error("Usage: lua_do <expression>");
			return;
		}

		const std::string expr = boost::algorithm::join(args, " ");
		ExecuteString(expr.c_str());
	});
}
