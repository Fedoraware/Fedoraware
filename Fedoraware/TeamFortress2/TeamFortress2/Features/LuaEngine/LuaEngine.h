#pragma once
#include <lua.hpp>
#include <mutex>

#define LOCKLUA() std::lock_guard<std::mutex> lock(g_pLuaEngine->m)

class LuaEngine {
	lua_State* m_L;
	void PrintError();

public:
	LuaEngine() : m_L(luaL_newstate()) { luaL_openlibs(m_L); }
	~LuaEngine() { lua_close(m_L); }

	lua_State* L();
	void ExecuteFile(const char* file);
	void ExecuteString(const char* expression);
	void Reset() {
		if (m_L) { lua_close(m_L); }
		m_L = luaL_newstate();
		luaL_openlibs(m_L);
	}
	void Initialize();

	std::mutex m;
};

extern LuaEngine* g_pLuaEngine;