#pragma once
#include <lua.hpp>
#include <mutex>
#include "../../SDK/SDK.h"

#define LOCKLUA() std::lock_guard<std::mutex> lock(F::LuaEngine.LuaMutex)

class CLuaEngine {
	lua_State* LuaState;
	void PrintError();

public:
	CLuaEngine() : LuaState(luaL_newstate()) { luaL_openlibs(LuaState); }
	~CLuaEngine() { lua_close(LuaState); }
	
	void ExecuteFile(const char* file);
	void ExecuteString(const char* expression);
	void Reset() {
		if (LuaState) { lua_close(LuaState); }
		LuaState = luaL_newstate();
		luaL_openlibs(LuaState);
	}
	void Init();

	std::mutex LuaMutex;
};

ADD_FEATURE(CLuaEngine, LuaEngine)