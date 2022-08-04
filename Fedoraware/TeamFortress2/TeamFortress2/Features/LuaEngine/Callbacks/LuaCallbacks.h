#pragma once
#include <lua.hpp>
#include <sol/sol.hpp>

#include "../../SDK/SDK.h"

class CLuaCallbacks {
	std::unordered_map<std::string, std::unordered_map<std::string, sol::function>> Callbacks;

public:
	void Register(const char* type, const char* name, const sol::function& callback);
	void Unregister(const char* type, const char* name);
	void Reset();

	void ByType(const char* type);
	void OnCreateMove(CUserCmd* pCmd, bool* pSendPacket);
	void OnFireGameEvent(CGameEvent* pEvent);
};

ADD_FEATURE(CLuaCallbacks, LuaCallbacks)