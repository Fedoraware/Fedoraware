#pragma once
#include <mutex>
#include "../../SDK/SDK.h"
#include "sol/sol.hpp"

class CLuaEngine
{
public:
	void ExecuteFile(const std::string& file);
	void ExecuteString(const std::string& expression);
	void HandleError(const sol::protected_function_result& result);

	void Init();
	void Reset();
};

ADD_FEATURE(CLuaEngine, LuaEngine)