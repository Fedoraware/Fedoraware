#pragma once
#include <mutex>
#include "../../SDK/SDK.h"

class CLuaEngine {
public:
	void ExecuteFile(const std::string& file);
	void ExecuteString(const std::string& expression);
	
	void Init();
	void Reset();
};

ADD_FEATURE(CLuaEngine, LuaEngine)