#pragma once
#include <mutex>
#include <lua.hpp>
//#include <LuaBridge.h>
#include <sol/sol.hpp>
#include "../../SDK/SDK.h"

class CLuaEngine {
public:
	void ExecuteFile(std::string file);
	void ExecuteString(std::string expression);
	
	void Init();
	void Reset();
};

ADD_FEATURE(CLuaEngine, LuaEngine)