#pragma once
#include <mutex>
#include <lua.hpp>
#include <LuaBridge.h>
#include "../../SDK/SDK.h"

#define LOCKLUA() std::lock_guard<std::mutex> lock(F::LuaEngine.LuaMutex)

class CLuaEngine {
	void PrintError();

	lua_State* LuaState;
	std::mutex LuaMutex;

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
};

ADD_FEATURE(CLuaEngine, LuaEngine)